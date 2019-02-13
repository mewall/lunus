from time import clock, time
import numpy as np
import re
#from xfel.cxi.display_spots import run_one_index_core
from cctbx.array_family import flex
from multiprocessing import Pool
import subprocess, shlex

try:
    import cPickle as pickle
except:
    import pickle
    
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
  # target cell for indexing
  try:
      targetcellidx = [a.find("target_cell")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target cell target_cell must be specified"
  else:
      target_cell = args.pop(targetcellidx).split("=")[1]
  # target spacegroup for indexing
  try:
      targetsgidx = [a.find("target_sg")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target space group target_sg must be specified"
  else:
      target_sg = args.pop(targetsgidx).split("=")[1]
  try:
    procpathidx = [a.find("path.to.proc")==0 for a in args].index(True)
  except ValueError:
    procpath=""
  else:
    procpath = args.pop(procpathidx).split("=")[1]
  # input file with list of diffuse images and scale factors
  try:
    ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
  except ValueError:
    ifname = "genlat.input"
  else:
    ifname = args.pop(ifnameidx).split("=")[1]

  import os

  # new dials
  from iotbx.phil import parse
  from dxtbx.datablock import DataBlockFactory
  from dials.array_family import flex
  #from dials.algorithms.indexing.fft1d import indexer_fft1d as indexer
#  from dials.algorithms.indexing.fft3d import indexer_fft3d as indexer
  from dials.algorithms.indexing.indexer import indexer_base
  #from dials.algorithms.indexing.real_space_grid_search import indexer_real_space_grid_search as indexer
  import copy, os

#  print target_cell,target_sg

  phil_scope_str='''
    output {{
      shoeboxes = True
	.type = bool
	.help = Save the raw pixel values inside the reflection shoeboxes.
    }}
    include scope dials.algorithms.spot_finding.factory.phil_scope
    include scope dials.algorithms.indexing.indexer.index_only_phil_scope
    include scope dials.algorithms.refinement.refiner.phil_scope
    indexing.known_symmetry.unit_cell={0}
      .type = unit_cell
    indexing.known_symmetry.space_group={1}
      .type = space_group
  '''
  phil_scope = parse(phil_scope_str.format(target_cell,target_sg), process_includes=True)
  from dials.util.options import OptionParser
  parser = OptionParser(phil=phil_scope)
  params, options = parser.parse_args(args=[], show_diff_phil=True)
  
  params.refinement.parameterisation.scan_varying = False
#  params.indexing.method='real_space_grid_search'
  params.indexing.method='fft3d'
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

#  print imagesets[0].get_beam()
#  print imagesets[2].get_beam()
#  print imagesets[0].get_beam() == imagesets[0].get_beam()
#  print imagesets[1].get_beam() == imagesets[0].get_beam()
#  print imagesets[2].get_beam() == imagesets[0].get_beam()

  print "indexing..."
  t0 = time()
# new dials, fix by Aaron
  idxr = indexer_base.from_parameters(observed, imagesets, params=params)
  idxr.index()
#  idxr = indexer(observed, imagesets, params=working_params)
# old labelit
#  results = run_one_index_core(horizons_phil)
  tel = time()-t0
  print "done indexing (",tel," sec)"

# new dials
  indexed = idxr.refined_reflections
  experiments = idxr.refined_experiments
#  print experiments.crystals()[0]

  # read input file with list of diffraction images and scale factors
  f = open(ifname,"r")
  lines = []
  linenum = 1
  framenum = -1
  for line in f:
    if ((line.strip()!="") and (line[0] != '.')):
      if ((framenum == -1) or (framenum == linenum)):
        lines.append(line)
      linenum = linenum + 1
  f.close()


  imgname = filenames[0]
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

  print "Creating pixel map..."
  t0 = time()
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
  x_vectors = np.asarray(s1_vectors - beam.get_s0())

#  print "there are ",x_vectors.size()," elements in x_vectors"
  tel = time()-t0
  print "done creating pixel map (",tel," sec)"
    
  workdir="tmpdir_common"
  if (os.path.isdir(workdir)):
    command = 'rm -r {0}'.format(workdir)
    call_params = shlex.split(command)
    subprocess.call(call_params)
  print "Made it"
  command = 'mkdir {0}'.format(workdir)
  call_params = shlex.split(command)
  subprocess.call(call_params)
  command = 'lfs setstripe -c -1 {0}'.format(workdir)
  call_params = shlex.split(command)
  subprocess.call(call_params)

  DATAsize = np.asarray(detector[0].get_image_size())

# write common files
  np.save(workdir+"/x_vectors.npy",x_vectors)
  np.save(workdir+"/DATAsize.npy",DATAsize)

  imnum=0
  for line in lines:
    imnum = imnum + 1
    # parse the input file line into a diffuse image file name and scale factor
    words = line.split()

    imgname = os.path.join(procpath+"/"+words[1])
    scale = float(words[2])

#    dxtbx.print_header(imgname)

#    print "processing file %s with scale factor %f"%(imgname,scale)
#    I = QuickImage(imgname)
#    I.read()
#    DATA = I.linearintdata
#    import dxtbx
    t0 = time()
    img = dxtbx.load(imgname)
    detector = img.get_detector()
    beam = img.get_beam()
    scan = img.get_scan()
    gonio = img.get_goniometer()
#    print "Time to get image properties and data = ",time()-t0," sec"
#    AI.setData(raw_spot_input_all)
#    f = AI.film_to_camera()
#    rvec = AI.camera_to_xyz()
  
#    A_matrix = sqr(AI.getOrientation().direct_matrix())

    t0 = time()
    crystal = copy.deepcopy(experiments.crystals()[0])
    axis = gonio.get_rotation_axis()
    start_angle, delta_angle = scan.get_oscillation()
    crystal.rotate_around_origin(axis, start_angle + (delta_angle/2), deg=True)
    A_matrix = crystal.get_A().inverse()
    At = np.asarray(A_matrix.transpose()).reshape((3,3))
#    print "Time to get A matrix = ",time()-t0," sec"

#    print "Integrating diffuse scattering in parallel using ",nproc," processors..."
    t0 = time()
    if len(detector)>1:
        print "Multi-panel detector"
    for panel_id, panel in enumerate(detector):
      #    z = 0
      #Isize1 = I.size1
      #Isize2 = I.size2
#      Isize1, Isize2 = panel.get_image_size()
#      print "Isize1 = ",Isize1,", Isize2 = ",Isize2
#      print "there are ",Isize1*Isize2," pixels in this diffraction image"
      if len(detector) > 1:
        
        DATA = img.get_raw_data(panel_id)
      else:
        DATA = img.get_raw_data()
# write A matrix and diffraction image data to temporary working directory
#    print "Time to read data from image = ",time()-t0," sec"
    workdir="tmpdir_{0}".format(imnum)
    if (not os.path.isdir(workdir)):
        command = 'mkdir {}'.format(workdir)
        call_params = shlex.split(command)
        subprocess.call(call_params)
    np.save(workdir+"/At.npy",At)
    t0 = time()
#    DATA.tofile(workdir+"/DATA")
#    print DATA.size()
#    DATAnp = np.frombuffer(DATA.copy_to_byte_str(),dtype='int16').reshape((Isize2,Isize1))
#    print type(DATA)
#    print type(x_vectors)
    DATAnp = np.int16(np.asarray(np.frombuffer(DATA.copy_to_byte_str(),dtype='int32')))
    np.save(workdir+"/DATA.npy",DATAnp)
#    if (np.all(np.asarray(DATA) == DATAnp)):
#        print "Same"
#    else:
#        print "Different"
#    print len(DATAnp)
#    print type(DATAnp)
#   np.save(workdir+"DATA.npy",DATA)
#    print "Time to write ",workdir+"/DATA.npy = ",time()-t0," sec"
