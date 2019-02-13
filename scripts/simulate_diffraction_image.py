from time import clock, time
import numpy as np
import re
#from xfel.cxi.display_spots import run_one_index_core
from cctbx.array_family import flex
#from labelit.command_line.imagefiles import QuickImage
from multiprocessing import Pool
try:
    import cPickle as pickle
except:
    import pickle
    
def pixmap(Isize1,Isize2,this_frame_phi_deg,pixel_size,size1,size2,spot_convention,procid):
  # returms a list of data points in a chunk of each image with info to calculate the h,k,l
  from spotfinder.applications.xfel import cxi_phil
  from iotbx.detectors.context.spot_xy_convention import spot_xy_convention
  SXYC = spot_xy_convention(pixel_size*size1,pixel_size*size2)
  from spotfinder.math_support import pixels_to_mmPos
  # for parallel running; calculate the range that will be worked on by this process
  chunksize = int(Isize1/nproc)
  if (Isize1 % nproc !=  0):
    chunksize += 1
  x1 = procid*chunksize
  x2 = x1 + chunksize
  if (x2>Isize1):
    x2=Isize1
  # now walk through the pixels and create the list of data points
  raw_spot_input = flex.vec3_double()
  for x in xrange(x1,x2): # slow dimension
    for y in xrange(Isize2): # fast dimension
      mmPos = pixels_to_mmPos(x,y,pixel_size)
      rawspot = (mmPos[0],mmPos[1],this_frame_phi_deg)
      transpot = SXYC.select(rawspot,spot_convention)
      raw_spot_input.append(transpot)
  return raw_spot_input

def pixmapstar(args):
  # wrapper that can be called for parallel processing using pool
  return pixmap(*args)

def procimg(Isize1,Isize2,scale,mask_tag,A_matrix,rvec,D,procid):
  # returns a 3D lattice with integrated data from a chunk of data points
  from scitbx.matrix import col
  # define the lattice indices at which h,k,l = 0,0,0

  # calculate the range of data points to be integrated in this process
  chunksize = int(Isize2/nproc)
  if (Isize2 % nproc !=  0):
    chunksize += 1
  y1 = procid*chunksize
  y2 = y1 + chunksize
  if (y2>Isize2):
    y2=Isize2
  # walk through the data points and accumulate the integrated data in the lattice

  ip=np.zeros(y2-y1+1,Isize1)
  for x in xrange(Isize1): # slow dimension
    for y in xrange(y1,y2): # fast dimension
      # calculate index into the data points
      z = x*Isize2 + y
      tmid = clock()
      # calculate h,k,l for this data point
      H = A_matrix * col(rvec[z])
      # Calculate fractional index into diffuse intensity
      isz = len(D)
      jsz = len(D[0])
      ksz = len(D[0][0])
      fi = H[0]+len(D)/2-1
      fj = H[1]+len(D[0])/2-1
      fk = H[2]+len(D[0][0])/2-1
      # calculate reference point integer index into diffuse intensity
      i = int(fi)
      j = int(fj)
      k = int(fk)
      # calculate the displacement of this data point from the nearest Miller index
      di = fi - i
      dj = fj - j
      dk = fk - k
# labelit values
#      val = int(DATA[(x,y)])
# 
      if (i>=0 and i<(isz-1) and j>=0 and j<(jsz-1) and k>=0 and k<(ksz-1)):
          ndat = 0
          for ii in range(2):
              wi = (1-di)*(1-ii)+di*ii
              for jj in range(2):
                  wj = (1-dj)*(1-jj)+dj*jj
                  for kk in range(2):
                      wk = (1-dk)*(1-kk)+dk*kk
                      if (D[i+ii][j+jj][k+kk] != ignore_tag):
                          ip[y][x]+=D[i+ii][j+jj][k+kk]*wi*wj*wk
                          ndat = ndat+1
          ip[y][x] *= 8/ndat
  return ip

def procimg_single(Isize1,Isize2,scale,lattice_mask_tag,A_matrix,rvec,D):
  # returns a 3D lattice with integrated data from a chunk of data points
  from scitbx.matrix import col
  # define the lattice indices at which h,k,l = 0,0,0
  global image_mask_tag,pphkl
  imp=np.zeros((Isize2,Isize1))
  for x in xrange(Isize1): # slow dimension
    for y in xrange(Isize2): # fast dimension
      # calculate index into the data points
      z = x*Isize2 + y
      tmid = clock()
      # calculate h,k,l for this data point
      H = A_matrix * col(rvec[z]) * pphkl
      # Calculate fractional index into diffuse intensity
      isz = len(D)
      jsz = len(D[0])
      ksz = len(D[0][0])
      fi = H[0]+len(D)/2-1
      fj = H[1]+len(D[0])/2-1
      fk = H[2]+len(D[0][0])/2-1
      # calculate reference point integer index into diffuse intensity
      i = int(fi)
      j = int(fj)
      k = int(fk)
      # calculate the displacement of this data point from the nearest Miller index
      di = fi - i
      dj = fj - j
      dk = fk - k
# labelit values
#      val = int(DATA[(x,y)])
# 
      if (i>=0 and i<(isz-1) and j>=0 and j<(jsz-1) and k>=0 and k<(ksz-1)):
          ndat = 0
          for ii in range(2):
              wi = (1-di)*(1-ii)+di*ii
              for jj in range(2):
                  wj = (1-dj)*(1-jj)+dj*jj
                  for kk in range(2):
                      wk = (1-dk)*(1-kk)+dk*kk
                      if (D[i+ii][j+jj][k+kk] != lattice_mask_tag):
                          imp[y][x]+=D[i+ii][j+jj][k+kk]*wi*wj*wk
                          ndat = ndat+1
#          if (ndat>0):
#              imp[y][x] *= 8/ndat
#          else:
          if (ndat<8):
              imp[y][x] = image_mask_tag
      else:
          imp[y][x] = image_mask_tag
  return imp

def procimgstar(args):
  # wrapper for procimg to be used for parallel processing using pool
  return procimg(*args)

if __name__=="__main__":
  import sys

  args = sys.argv[1:] # normally the user puts these things on command line, not in quotes, no commas
  usage = ["indexing.data=/net/sunbird/raid1/sauter/rawdata/pilatus/ribosome/images/colD55A_13_1_00001.cbf",
          # user can input any number of indexing.data image file names
          # if more than two, maxcell (unit cell upper bound in Angstroms) must be given
          # using abutting images works but slows things down
          "indexing.data=/net/sunbird/raid1/sauter/rawdata/pilatus/ribosome/images/colD55A_13_1_00401.cbf",
          "codecamp.maxcell=800",
          "index_only=True",
          "analyze.image=201"] #image number to be used for pixel analysis.
                               # but it doesn't have to be one of the images used to index.

 # Read command line arguments

 # number of processors available for pool
  try:
    nprocidx = [a.find("np")==0 for a in args].index(True)
  except ValueError:
    nproc = 1
  else:
    nproc = int(args.pop(nprocidx).split("=")[1])
  # unit cell a
  # target cell for indexing
  try:
      targetcellidx = [a.find("target_cell")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target cell target_cell must be specified"
  else:
      target_cell = args.pop(targetcellidx).split("=")[1]
      celllist = target_cell.split(",")
      cella=float(celllist[0])
      cellb=float(celllist[1])
      cellc=float(celllist[2])
  # spacegroup for indexing
  try:
      targetsgidx = [a.find("target_sg")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target space group target_sg must be specified"
  else:
      target_sg = args.pop(targetsgidx).split("=")[1]
  # spacegroup for indexing
  try:
    residx = [a.find("diffuse.lattice.resolution")==0 for a in args].index(True)
  except ValueError:
    print "diffuse.lattice.resolution not specified, looking for explicit lattice dimensions"
    residx = -1
  else:
    res = float(args.pop(residx).split("=")[1])
    latxdim = (int(cella/res)+1)*2
    latydim = (int(cellb/res)+1)*2
    latzdim = (int(cellc/res)+1)*2
  # input file with list of diffuse images and scale factors
  try:
    ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
  except ValueError:
    ifname = "genlat.input"
  else:
    ifname = args.pop(ifnameidx).split("=")[1]
  # frame number
  try:
    framenumidx = [a.find("framenum")==0 for a in args].index(True)
  except ValueError:
    framenum=-1
  else:
    framenum=int(args.pop(framenumidx).split("=")[1])
    print "integrating frame ",framenum
  # output diffuse lattice file
  try:
    ofnameidx = [a.find("diffuse.lattice.fname")==0 for a in args].index(True)
  except ValueError:
    ofname = "diffuse.vtk"
  else:
    ofname = args.pop(ofnameidx).split("=")[1]
  # output diffuse lattice file
  try:
    cfnameidx = [a.find("counts.lattice.fname")==0 for a in args].index(True)
  except ValueError:
    cfname = "counts.vtk"
  else:
    cfname = args.pop(cfnameidx).split("=")[1]
  # type of lattice output. "sum" = sum, "mean" = mean
  try:
    lattypeidx = [a.find("diffuse.lattice.type")==0 for a in args].index(True)
  except ValueError:
    lattype = "mean"
  else:
    lattype = args.pop(lattypeidx).split("=")[1]
    if not((lattype == "sum") or (lattype == "mean")):
      raise Exception,"Lattice type must be ""sum"" or ""mean"""
  # size of diffuse lattice in x direction
  # specify path to proc directory
  try:
    procpathidx = [a.find("path.to.proc")==0 for a in args].index(True)
  except ValueError:
    procpath="."
  else:
    procpath = args.pop(procpathidx).split("=")[1]
  # read indexing info from a file instead of calculating (not implemented)
  try:
    readindexidx = [a.find("readindex")==0 for a in args].index(True)
  except ValueError:
    readindex = False
  else:
    readindex = bool(args.pop(readindexidx).split("=")[1])
  try:
    scaleidx = [a.find("scale.factor")==0 for a in args].index(True)
  except ValueError:
    scale = 1.
  else:
    scale = float(args.pop(scaleidx).split("=")[1])
  try:
    pphklidx = [a.find("pphkl")==0 for a in args].index(True)
  except ValueError:
    pphkl = 1.
  else:
    pphkl = float(args.pop(pphklidx).split("=")[1])
  try:
    hklidx = [a.find("input.hkl")==0 for a in args].index(True)
  except ValueError:
    diffusein = "diffuse.hkl"
  else:
    diffusein = (args.pop(hklidx).split("=")[1])
  try:
    rawidx = [a.find("output.raw")==0 for a in args].index(True)
  except ValueError:
    rawout = "diffuse.raw"
  else:
    rawout = (args.pop(rawidx).split("=")[1])
  try:
    imgidx = [a.find("input.img")==0 for a in args].index(True)
  except ValueError:
    imgname = "diffimg.img"
  else:
    imgname = (args.pop(imgidx).split("=")[1])

  lattice_mask_tag=-32768

  import os

  # read input file with list of diffraction images and scale factors
  f = open(ifname,"r")
  lines = []
  linenum = 1
  for line in f:
    if ((line.strip()!="") and (line[0] != '.')):
      if ((framenum == -1) or (framenum == linenum)):
        lines.append(line)
      linenum = linenum + 1
  f.close()

  #########################################################################
  # new dials
  from iotbx.phil import parse
  from dxtbx.datablock import DataBlockFactory
  from dials.array_family import flex
  #from dials.algorithms.indexing.fft1d import indexer_fft1d as indexer
  from dials.algorithms.indexing.fft3d import indexer_fft3d as indexer
  #from dials.algorithms.indexing.real_space_grid_search import indexer_real_space_grid_search as indexer
  import copy, os

  print target_cell,target_sg

  phil_scope_str='''
     include scope dials.algorithms.peak_finding.spotfinder_factory.phil_scope
     include scope dials.algorithms.indexing.indexer.index_only_phil_scope
     include scope dials.algorithms.refinement.refiner.phil_scope
     indexing.known_symmetry.unit_cell={0}
       .type = unit_cell
     indexing.known_symmetry.space_group={1}
       .type = space_group
   '''
  phil_scope = parse(phil_scope_str.format(target_cell,target_sg), process_includes=True)
  params = phil_scope.extract()
  params.refinement.parameterisation.crystal.scan_varying = False
  params.indexing.scan_range = []
#params.refinement.parameterisation.crystal.unit_cell.restraints.tie_to_target = []
#  params.spotfinder.filter.min_spot_size=3
  
  filenames = []
  for arg in args:
     if "indexing.data" in arg:
       path = arg.split('=')[1]
       if os.path.isdir(path):
         for subfile in os.listdir(path):
 	  subpath = os.path.join(path, subfile)
 	  if os.path.isfile(subpath):
             filenames.append(subpath)
       else:
         filenames.append(path)

  print filenames

  datablock = DataBlockFactory.from_filenames(filenames)[0]
 
  observed = flex.reflection_table.from_observations(datablock, params)
  observed.as_pickle("strong.pickle")
  print "Number of observed reflections:", len(observed)
 
  working_params = copy.deepcopy(params)
  imagesets = datablock.extract_imagesets()

# old labelit
#  from spotfinder.applications.xfel import cxi_phil
#  horizons_phil = cxi_phil.cxi_versioned_extract(args)


  print "indexing..."
  t0 = clock()
# new dials
  idxr = indexer(observed, imagesets, params=working_params)
  print "...done"
# old labelit
#  results = run_one_index_core(horizons_phil)
  tel = clock()-t0
  print "done indexing (",tel," sec)"

# new dials
  indexed = idxr.refined_reflections
  experiments = idxr.refined_experiments
  print experiments.crystals()[0]

# old labelit
  #from spotfinder.applications.xfel import cxi_phil
  #horizons_phil = cxi_phil.cxi_versioned_extract(args)

  #print "indexing..."
  #t0 = clock()
  # indexing can be slow. would be good to save indexing info and read it back in
  #results = run_one_index_core(horizons_phil)
  #tel = clock()-t0
  #print "done indexing (",tel," sec)"



  #name_of_interest = results.organizer.Files.filenames.FN[0]
# old labelit
#  AI = results.indexing_ai

  # Read the diffuse lattice in .hkl format
  DhklI = np.loadtxt(diffusein)
  mx = np.zeros(3)
  for i in range(3):
      mx[i]=int(max(abs(DhklI[:,i])))
  D = np.zeros((mx[0]*2,mx[1]*2,mx[2]*2))
  D[:,:,:] = lattice_mask_tag
  for i in range(len(DhklI)):
      hh,kk,ll=DhklI[i][:3]
      D[int(hh)+mx[0]-1][int(kk)+mx[1]-1][int(ll)+mx[2]-1]=float(DhklI[i][3])
#  fileidx = 0

  #Create parallel processing pool

  pool = Pool(processes=nproc)

  for line in lines:

    # parse the input file line into a diffuse image file name and scale factor
    words = line.split()

#    imgname = os.path.join(procpath+"/"+words[1])
#    scale = float(words[2])

    print "processing file %s with scale factor %f"%(imgname,scale)
#    I = QuickImage(imgname)
#    I.read()
#    DATA = I.linearintdata
    import dxtbx
    img = dxtbx.load(imgname)
    detector = img.get_detector()
    beam = img.get_beam()
    scan = img.get_scan()
    gonio = img.get_goniometer()
 
    print "transform pixel numbers to mm positions and rotational degrees"
#    from iotbx.detectors.context.spot_xy_convention import spot_xy_convention
#    SF = results.spotfinder_results
#    SXYC = spot_xy_convention(SF.pixel_size*SF.size1,SF.pixel_size*SF.size2)
#    from spotfinder.math_support import pixels_to_mmPos
#    this_frame_phi_deg = I.deltaphi/2.0+I.osc_start

    print "Creating pixel map..."
    t0 = clock()
    """
    raw_spot_input_all = flex.vec3_double()
    Isize1 = I.size1
    Isize2 = I.size2
    pixel_size = SF.pixel_size
    # prepare the list of arguments to run pixmap in parallel
    pixmap_tasks = [(Isize1,Isize2,this_frame_phi_deg,SF.pixel_size,SF.size1,SF.size2,results.horizons_phil.spot_convention,procid) for procid in range(nproc)]
    # run pixmap in parallel and collect results
    raw_spot_input_it = pool.map(pixmapstar,pixmap_tasks)
    # gather pixmap results into a single collection of data points
    for raw_spot_input_this in raw_spot_input_it:
      raw_spot_input_all.extend(raw_spot_input_this)
      #      for j in raw_spot_input_it[i]:
      # raw_spot_input_all.append(j)
      # print "len(raw_spot_input_all) = ",len(raw_spot_input_all),"; I.size1*I.size2 = ",I.size1*I.size2
    """
    lab_coordinates = flex.vec3_double()
    for panel in detector: 
      pixels = flex.vec2_double(panel.get_image_size())
      #for j in xrange(panel.get_image_size()[1]):
      #  for i in xrange(panel.get_image_size()[0]):
#	  pixels.append((i,j))
      mms = panel.pixel_to_millimeter(pixels)
      lab_coordinates.extend(panel.get_lab_coord(mms))

    # generate s1 vectors
    s1_vectors = lab_coordinates.each_normalize() * (1/beam.get_wavelength())
    # Generate x vectors
    x_vectors = s1_vectors - beam.get_s0()

    print "there are ",x_vectors.size()," elements in x_vectors"
    tel = clock()-t0
    print "done creating pixel map (",tel," sec)"
    

    print "transform to laboratory axis reciprocal space coordinates"
#    AI.setData(raw_spot_input_all)
#    f = AI.film_to_camera()
#    rvec = AI.camera_to_xyz()
  
    print "transform to fractional miller indices and populate diffuse lattice"
    from scitbx.matrix import sqr,col
#    A_matrix = sqr(AI.getOrientation().direct_matrix())

    crystal = copy.deepcopy(experiments.crystals()[0])
    axis = gonio.get_rotation_axis()
    start_angle, delta_angle = scan.get_oscillation()
    crystal.rotate_around_origin(axis, start_angle + (delta_angle/2), deg=True)
    A_matrix = crystal.get_A().inverse()

#    print "Integrating diffuse scattering in parallel using ",nproc," processors..."
    telmatmul=0
    t0 = clock()
    latit = None
    for panel_id, panel in enumerate(detector):
      #    z = 0
      #Isize1 = I.size1
      #Isize2 = I.size2
      Is1, Is2 = panel.get_image_size()
      Isize1 = int(Is1)
      Isize2 = int(Is2)
      print "Isize1 = ",Isize1,", Isize2 = ",Isize2
      print "there are ",Isize1*Isize2," pixels in this diffraction image"
      if len(detector) > 1:
        DATA = img.get_raw_data(panel_id)
      else:
        DATA = img.get_raw_data()
      # prepare list of arguments to run procimg in parallel
      #tasks = [(Isize2,Isize2,scale,mask_tag,A_matrix,rvec,DATA,latxdim,latydim,latzdim,procid) for procid in range(nproc)]
#      tasks = [(Isize1,Isize2,scale,lattice_mask_tag,A_matrix,x_vectors,D,procid) for procid in range(nproc)]
#      tasks = [(Isize1,Isize1,scale,mask_tag,A_matrix,x_vectors,DATA,latxdim,latydim,latzdim,procid) for procid in range(nproc)]
# Serial procimg
      image_mask_tag=32767.
      diffim = procimg_single(Isize1,Isize2,scale,lattice_mask_tag,A_matrix,x_vectors,D)
      # run procimg in parallel and collect results
#      diffim = pool.map(procimgstar,tasks)
    tel = clock()-t0
    print "done integrating diffuse scattering (",tel," sec wall clock time)"
    t0 = clock()

# Scale pixel values

  dmin=np.amin(diffim)

#  s = 256./(dmax-dmin)

  for i in range(len(diffim)):
      for j in range(len(diffim[i])):
          if (diffim[i][j] != image_mask_tag):
              diffim[i][j] = (diffim[i][j]-dmin)*scale

  # write image to output file
#  diffim.astype('int8').tofile("diffuse.raw")
  diffim.astype('int16').tofile(rawout)
