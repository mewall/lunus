from time import clock, time
import numpy as np
import re
from xfel.cxi.display_spots import run_one_index_core
from cctbx.array_family import flex
from labelit.command_line.imagefiles import QuickImage
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

def procimg(Isize1,Isize2,scale,mask_tag,A_matrix,rvec,DATA,latxdim,latydim,latzdim,procid):
  # returns a 3D lattice with integrated data from a chunk of data points
  from scitbx.matrix import col
  # define the lattice indices at which h,k,l = 0,0,0
  i0=latxdim/2-1
  j0=latydim/2-1
  k0=latzdim/2-1
  # total number of voxels in the lattice
  latsize = latxdim*latydim*latzdim
  lat = np.zeros(latsize*2, dtype=np.float32).reshape((2,latsize))

  # calculate the range of data points to be integrated in this process
  chunksize = int(Isize2/nproc)
  if (Isize2 % nproc !=  0):
    chunksize += 1
  y1 = procid*chunksize
  y2 = y1 + chunksize
  if (y2>Isize2):
    y2=Isize2
  # walk through the data points and accumulate the integrated data in the lattice
  for x in xrange(Isize1): # slow dimension
    for y in xrange(y1,y2): # fast dimension
      # calculate index into the data points
      z = x*Isize2 + y
      tmid = clock()
      # calculate h,k,l for this data point
      H = A_matrix * col(rvec[z])
      if (H[0]<0):
        hh = int(H[0]-.5)
      else:
        hh = int(H[0]+.5)
      if (H[1]<0):
        kk = int(H[1]-.5)
      else:
        kk = int(H[1]+.5)
      if (H[2]<0):
        ll = int(H[2]-.5)
      else:
        ll = int(H[2]+.5)
      # calculate the displacement of this data point from the nearest Miller index
      dh = abs(H[0]-hh)
      dk = abs(H[1]-kk)
      dl = abs(H[2]-ll)
# labelit values
#      val = int(DATA[(x,y)])
#dials values
      val = int(DATA[(y,x)])
      print val
      # integrate the data only if it's not in the immediate neighborhood of a Bragg peak
      if ((val != mask_tag) and (val != 0) and not((dh < .25) and (dk < .25) and (dl < .25))):
        # i, j, and k are indices into the lattice
        i = hh+i0
        j = kk+j0
        k = ll+k0
        if ((i>=0) and (j>=0) and (k>=0) and (i<latxdim) and (j<latydim) and (k<latzdim)):
          index = k*latxdim*latydim + j*latxdim + i
          if ((val>0)  and (val < 32767)):
            # accumulate the data for this lattice point. keep track of count for averaging
            lat[0][index] += val*scale
            lat[1][index] += 1
  return lat

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
  try:
    cellaidx = [a.find("cell.a")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Lattice constant cell.a must be specified"
  else:
    cella = float(args.pop(cellaidx).split("=")[1])
  # unit cell b
  try:
    cellbidx = [a.find("cell.b")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Lattice constant cell.b must be specified"
  else:
    cellb = float(args.pop(cellbidx).split("=")[1])
  # unit cell c
  try:
    cellcidx = [a.find("cell.c")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Lattice constant cell.c must be specified"
  else:
    cellc = float(args.pop(cellcidx).split("=")[1])
  # target cell for indexing
  try:
      targetcellidx = [a.find("target_cell")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target cell target_cell must be specified"
  else:
      target_cell = args.pop(targetcellidx).split("=")[1]
  # spacegroup for indexing
  try:
      targetsgidx = [a.find("target_sg")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target space group target_sg must be specified"
  else:
      target_sg = args.pop(targetsgidx).split("=")[1]
  # maximum resolution of diffuse lattice
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
  try:
    latxdimidx = [a.find("latxdim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latxdim = -1
  else:
    latxdim = int(args.pop(latxdimidx).split("=")[1])
  # size of diffuse lattice in y direction
  try:
    latydimidx = [a.find("latydim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latydim = -1
  else:
    latydim = int(args.pop(latydimidx).split("=")[1])
  # size of diffuse lattice in z direction
  try:
    latzdimidx = [a.find("latzdim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latzdim = -1
  else:
    latzdim = int(args.pop(latzdimidx).split("=")[1])
  # specify path to proc directory
  try:
    procpathidx = [a.find("path.to.proc")==0 for a in args].index(True)
  except ValueError:
    procpath=""
  else:
    procpath = args.pop(procpathidx).split("=")[1]
  # read indexing info from a file instead of calculating (not implemented)
  try:
    readindexidx = [a.find("readindex")==0 for a in args].index(True)
  except ValueError:
    readindex = False
  else:
    readindex = bool(args.pop(readindexidx).split("=")[1])

  if (residx == -1) and ((latxdim == -1) or (latydim == -1) or (latzdim == -1)):
    raise Exception,"Must provide either diffuse.lattice.resolution or latxdim, latydim, and latzdim."

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
  #from dials.algorithms.indexing.fft3d import indexer_fft3d as indexer
  from dials.algorithms.indexing.real_space_grid_search import indexer_real_space_grid_search as indexer
  import copy, os
  # from dials.algorithms import refinement

  print target_cell,target_sg

  phil_scope_str='''
     include scope dials.algorithms.spot_finding.factory.phil_scope
     include scope dials.algorithms.indexing.indexer.index_only_phil_scope
     include scope dials.algorithms.refinement.refiner.phil_scope
     indexing.known_symmetry.unit_cell={0}
       .type = unit_cell
     indexing.known_symmetry.space_group={1}
       .type = space_group
     indexing.method=real_space_grid_search
     output.shoeboxes = False
   '''
  phil_scope = parse(phil_scope_str.format(target_cell,target_sg), process_includes=True)
  params = phil_scope.extract()
  params.refinement.parameterisation.scan_varying = False
  params.spotfinder.filter.resolution_range = []
  params.indexing.scan_range = []
  params.refinement.parameterisation.crystal.unit_cell.restraints.tie_to_target = []
  params.refinement.parameterisation.crystal.unit_cell.restraints.tie_to_group = []
#  params.indexing.max_cell=800
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
 
  datablock = DataBlockFactory.from_filenames(filenames)[0]
 
  observed = flex.reflection_table.from_observations(datablock, params)
  observed.as_pickle("strong.pickle")
  print "Number of observed reflections:", len(observed)
 
  working_params = copy.deepcopy(params)
  imagesets = datablock.extract_imagesets()

# old labelit
#  from spotfinder.applications.xfel import cxi_phil
#  horizons_phil = cxi_phil.cxi_versioned_extract(args)

  print imagesets[0].get_beam()
  print imagesets[2].get_beam()
  print imagesets[0].get_beam() == imagesets[0].get_beam()
  print imagesets[1].get_beam() == imagesets[0].get_beam()
  print imagesets[2].get_beam() == imagesets[0].get_beam()

  print "indexing..."
  t0 = clock()
# new dials
  idxr = indexer(observed, imagesets, params=working_params)
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


  latsize = latxdim*latydim*latzdim
  print "Lattice size = ",latsize
  lat = np.zeros(latsize, dtype=np.float32)
  ct = np.zeros(latsize, dtype=np.float32)

  #name_of_interest = results.organizer.Files.filenames.FN[0]
# old labelit
#  AI = results.indexing_ai
  i0=latxdim/2-1
  j0=latydim/2-1
  k0=latzdim/2-1
  mask_tag = 32767

#  fileidx = 0

  #Create parallel processing pool

  pool = Pool(processes=nproc)

  for line in lines:

    # parse the input file line into a diffuse image file name and scale factor
    words = line.split()

    imgname = os.path.join(procpath+"/"+words[1])
    scale = float(words[2])

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

    print "Integrating diffuse scattering in parallel using ",nproc," processors..."
    telmatmul=0
    t0 = clock()
    latit = None
    for panel_id, panel in enumerate(detector):
      #    z = 0
      #Isize1 = I.size1
      #Isize2 = I.size2
      Isize1, Isize2 = panel.get_image_size()
      print "Isize1 = ",Isize1,", Isize2 = ",Isize2
      print "there are ",Isize1*Isize2," pixels in this diffraction image"
      if len(detector) > 1:
        DATA = img.get_raw_data(panel_id)
      else:
        DATA = img.get_raw_data()
      # prepare list of arguments to run procimg in parallel
      #tasks = [(Isize2,Isize2,scale,mask_tag,A_matrix,rvec,DATA,latxdim,latydim,latzdim,procid) for procid in range(nproc)]
      tasks = [(Isize1,Isize2,scale,mask_tag,A_matrix,x_vectors,DATA,latxdim,latydim,latzdim,procid) for procid in range(nproc)]
#      tasks = [(Isize1,Isize1,scale,mask_tag,A_matrix,x_vectors,DATA,latxdim,latydim,latzdim,procid) for procid in range(nproc)]
      # run procimg in parallel and collect results
      tmp_latit = pool.map(procimgstar,tasks)
      if latit is None:
        latit = tmp_latit
      else:
        latit += tmp_latit
    tel = clock()-t0
    print "done integrating diffuse scattering (",tel," sec wall clock time)"
    t0 = clock()
    # accumulate integration data into a single lattice
    for l in latit:
      lat = np.add(lat,l[0])
      ct = np.add(ct,l[1])
    tel = clock()-t0
    print "Took ",tel," secs to update the lattice"

  if (lattype == "mean"):
      # compute the mean intensity at each lattice point
      for index in range(0,latsize):
          if ((ct[index] > 0) and (lat[index] != mask_tag)):
              lat[index] /= ct[index]
          else:
              lat[index] = -32768

  # write lattice to output file
  vtkfile = open(ofname,"w")

  a_recip = 1./cella
  b_recip = 1./cellb
  c_recip = 1./cellc

  print >>vtkfile,"# vtk DataFile Version 2.0"
  print >>vtkfile,"lattice_type=PR;unit_cell={0};space_group={1};".format(target_cell,target_sg)
  print >>vtkfile,"ASCII"
  print >>vtkfile,"DATASET STRUCTURED_POINTS"
  print >>vtkfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
  print >>vtkfile,"SPACING %f %f %f"%(a_recip,b_recip,c_recip)
  print >>vtkfile,"ORIGIN %f %f %f"%(-i0*a_recip,-j0*b_recip,-k0*c_recip)
  print >>vtkfile,"POINT_DATA %d"%(latsize)
  print >>vtkfile,"SCALARS volume_scalars float 1"
  print >>vtkfile,"LOOKUP_TABLE default\n"

  index = 0
  for k in range(0,latzdim):
    for j in range(0,latydim):
      for i in range(0,latxdim):
        print >>vtkfile,lat[index],
        index += 1
      print >>vtkfile,""

  vtkfile.close()

  # write counts lattice to output file, if lattice type is "sum"
  if (lattype=="sum"):
      vtkfile = open(cfname,"w")

      a_recip = 1./cella
      b_recip = 1./cellb
      c_recip = 1./cellc

      print >>vtkfile,"# vtk DataFile Version 2.0"
      print >>vtkfile,"lattice_type=PR;unit_cell={0};space_group={1};".format(target_cell,target_sg)
      print >>vtkfile,"ASCII"
      print >>vtkfile,"DATASET STRUCTURED_POINTS"
      print >>vtkfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
      print >>vtkfile,"SPACING %f %f %f"%(a_recip,b_recip,c_recip)
      print >>vtkfile,"ORIGIN %f %f %f"%(-i0*a_recip,-j0*b_recip,-k0*c_recip)
      print >>vtkfile,"POINT_DATA %d"%(latsize)
      print >>vtkfile,"SCALARS volume_scalars float 1"
      print >>vtkfile,"LOOKUP_TABLE default\n"

      index = 0
      for k in range(0,latzdim):
          for j in range(0,latydim):
              for i in range(0,latxdim):
                  print >>vtkfile,ct[index],
                  index += 1
              print >>vtkfile,""

      vtkfile.close()
