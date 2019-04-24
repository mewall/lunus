#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME lunus.integrate

from time import clock, time
import numpy as np
import glob, subprocess, shlex
import lunus

if __name__=="__main__":
  import sys

  args = sys.argv[1:] 
  usage = ["metrology=<experiments.json file, for metrology info>",
           "image_glob=<glob for images, used for rotation series>",
           "index_glob=<glob for individual .json files from stills",
           "xvectors_dir=<directory for xvectors>",
           "amatrix_dir_prefix=<directory prefix for amatrix info"]

 # Read command line arguments

  output_format="binary"
  add_background_images=False

 # Output .vtk file
  try:
    idx = [a.find("vtk")==0 for a in args].index(True)
  except ValueError:
    vtk_file = "diffuse.vtk"
  else:
    vtk_file = args.pop(idx).split("=")[1]
 # Input lunus input deck
  try:
    idx = [a.find("lunus_input_deck")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Lunus input deck must be specified using lunus_input_deck="
  else:
    deck_file = args.pop(idx).split("=")[1]
 # Input json
  try:
    metroidx = [a.find("metrology")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Metrology .json file must be specified using metrology="
  else:
    metro = args.pop(metroidx).split("=")[1]
 # Image input glob
  try:
    imageglobidx = [a.find("image_glob")==0 for a in args].index(True)
  except ValueError:
    rotation_series = False
  else:
    image_glob = args.pop(imageglobidx).split("=")[1]
    rotation_series = True
 # Background image input glob
  try:
    bkgglobidx = [a.find("bkg_glob")==0 for a in args].index(True)
  except ValueError:
    add_background_images = False
  else:
    bkg_glob = args.pop(bkgglobidx).split("=")[1]
    add_background_images = True
 # Index file input glob
  try:
    indexglobidx = [a.find("index_glob")==0 for a in args].index(True)
  except ValueError:
    stills_process = False
  else:
    index_glob = args.pop(indexglobidx).split("=")[1]
    stills_process = True
  # Output xvectors directory
  try:
    xvectorsdiridx = [a.find("xvectors_dir")==0 for a in args].index(True)
  except ValueError:
    xvectors_dir = "tmpdir_common"
  else:
    xvectors_dir = args.pop(xvectorsdiridx).split("=")[1]
  # Output json files directory
  try:
    jsondiridx = [a.find("json_dir")==0 for a in args].index(True)
  except ValueError:
    json_dir = "tmpdir_common"
  else:
    json_dir = args.pop(jsondiridx).split("=")[1]
    output_format="json"
  # Output amatrix directory
  try:
    amatrixdiridx = [a.find("amatrix_dir")==0 for a in args].index(True)
  except ValueError:
    amatrix_dir_prefix = "tmpdir_"
  else:
    amatrix_dir_prefix = args.pop(amatrixdiridx).split("=")[1]

  if (not (rotation_series or stills_process)):
    raise ValueError,"Must specify image_glob (for rotation series) or index_glob (for stills)."

  import copy, os

  import dxtbx
  from dxtbx.model.experiment_list import ExperimentListFactory
  from dials.array_family import flex

  with open(deck_file) as f:
    deck = f.read()
   
  p = lunus.Process()

  p.LunusSetparamslt(deck)
  p.LunusSetparamsim(deck)

  experiments = ExperimentListFactory.from_json_file(metro, check_format=False)
  beam = experiments[0].beam
  detector = experiments[0].detector

  print "s0 from experiment[0]: ",beam.get_s0()
  print "wavelength from experiment[0]: ",beam.get_wavelength()
 
  lab_coordinates = flex.vec3_double()
  for panel in detector: 
    pixels = flex.vec2_double(panel.get_image_size())
    mms = panel.pixel_to_millimeter(pixels)
    lab_coordinates.extend(panel.get_lab_coord(mms))

    # generate s1 vectors
  s1 = lab_coordinates.each_normalize() * (1/beam.get_wavelength())
    # Generate x vectors
  x = (s1 - beam.get_s0()).as_double()

  if rotation_series:
    from dxtbx.imageset import ImageSetFactory
    from dxtbx.model.experiment_list import Experiment, ExperimentList
    from dxtbx.serialize import dump
    imnum=1
    filelist=glob.glob(image_glob)
    filelist.sort()
    if (add_background_images==True):
      bkglist=glob.glob(bkg_glob)
      bkglist.sort()
    for i in range(len(filelist)):
      imgname=filelist[i]
      img = dxtbx.load(imgname)
      data = img.get_raw_data()

      p.set_image(data)
      
      if (i == 0):
        p.set_xvectors(x)
        p.set_imdiff_ref()
        p.LunusProcimlt(0)

      detector = img.get_detector()
      beam = img.get_beam()
      scan = img.get_scan()
      gonio = img.get_goniometer()

      print "s0 from ",imgname,": ",beam.get_s0()
      print "wavelength from ",imgname,": ",beam.get_wavelength()

      crystal = copy.deepcopy(experiments.crystals()[0])
      axis = gonio.get_rotation_axis()
      start_angle, delta_angle = scan.get_oscillation()      
      crystal.rotate_around_origin(axis, start_angle + (delta_angle/2), deg=True)

      from scitbx import matrix
      A_matrix = matrix.sqr(crystal.get_A()).inverse()
      print type(A_matrix.as_flex_double_matrix())
      At = np.asarray(A_matrix.transpose()).reshape((3,3))
      At_flex = A_matrix.transpose().as_flex_double_matrix()

      p.set_amatrix(At_flex)

      p.LunusProcimlt(1)

      imnum = imnum +1

    p.divide_by_counts()
    p.write_as_vtk(vtk_file)

  if stills_process:
    imnum=1
    filelist=glob.glob(index_glob)
    filelist.sort()
    for jsonname in filelist:
      experiments = ExperimentListFactory.from_json_file(jsonname, check_format=False)
      detector = experiments[0].detector
      beam = experiments[0].beam
      crystal = experiments.crystals()[0]

      from scitbx import matrix
      A_matrix = matrix.sqr(crystal.get_A()).inverse()
      At = np.asarray(A_matrix.transpose()).reshape((3,3))

      if (output_format != "json"):
        workdir=amatrix_dir_prefix+"{0}".format(imnum)
        if (not os.path.isdir(workdir)):
          command = 'mkdir {}'.format(workdir)
          call_params = shlex.split(command)
          subprocess.call(call_params)
#        np.save(workdir+"/At.npy",At)
        At.astype('float32').tofile(workdir+"/At.bin")
      imnum = imnum +1
