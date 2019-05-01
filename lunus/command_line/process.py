#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME lunus.process

from time import clock, time
import numpy as np
import glob, subprocess, shlex
import lunus

def get_experiment_params(experiments):

  beam = experiments[0].beam
  detector = experiments[0].detector

  for panel in detector: 
    pixel_size_mm = panel.get_pixel_size()[0]
    beam_mm_x = panel.get_beam_centre(beam.get_s0())[0]
    beam_mm_y = panel.get_beam_centre(beam.get_s0())[1]
    distance_mm = panel.get_distance()

  experiment_params = "\npixel_size_mm={0}\nbeam_mm_x={1}\nbeam_mm_y={2}\ndistance_mm={3}\n".format(pixel_size_mm,beam_mm_x,beam_mm_y,distance_mm)

  return(experiment_params)

def get_experiment_xvectors(experiments):

  beam = experiments[0].beam
  detector = experiments[0].detector

  lab_coordinates = flex.vec3_double()

  for panel in detector: 
    pixels = flex.vec2_double(panel.get_image_size())
    mms = panel.pixel_to_millimeter(pixels)
    lab_coordinates.extend(panel.get_lab_coord(mms))

    # generate s1 vectors
  s1 = lab_coordinates.each_normalize() * (1/beam.get_wavelength())
    # Generate x vectors
  x = (s1 - beam.get_s0()).as_double()

  return(x)

def process_one_glob():

    global fresh_lattice

    from dxtbx.imageset import ImageSetFactory
    from dxtbx.model.experiment_list import Experiment, ExperimentList
    from dxtbx.serialize import dump

    imnum=1

    filelist=glob.glob(image_glob)
    filelist.sort()

    if (subtract_background_images==True):
      bkglist=glob.glob(bkg_glob)
      bkglist.sort()
      if (len(filelist) != len(bkglist) and len(bkglist) != 1):
        raise ValueError,"Must either be one background or as many as there are images."
      if (len(bkglist) == 1):
        bkg = dxtbx.load(bkglist[0])
        bkg_data = bkg.get_raw_data()

    for i in range(len(filelist)):
      print "{0}...".format(i),
      sys.stdout.flush()
      imgname=filelist[i]
      img = dxtbx.load(imgname)
      data = img.get_raw_data()

#      print "min of data = ",flex.min(data)
#      print "max of data = ",flex.max(data)

      p.set_image(data)
      
      if (subtract_background_images==True):
        if (len(bkglist) != 1):
          bkg = dxtbx.load(bkglist[i])
          bkg_data = bkg.get_raw_data()
        p.set_background(bkg_data)
        p.LunusBkgsubim()

      scan = img.get_scan()
      start_angle, delta_angle = scan.get_oscillation()      

      gonio = img.get_goniometer()
      axis = gonio.get_rotation_axis()

      crystal = copy.deepcopy(experiments.crystals()[0])
      crystal.rotate_around_origin(axis, start_angle + (delta_angle/2), deg=True)

      from scitbx import matrix
      A_matrix = matrix.sqr(crystal.get_A()).inverse()
      At = np.asarray(A_matrix.transpose()).reshape((3,3))
      At_flex = A_matrix.transpose().as_flex_double_matrix()

      p.set_amatrix(At_flex)

      if (fresh_lattice==True):
        p.set_xvectors(x)
        p.set_image_ref()
        p.LunusProcimlt(0)
        fresh_lattice = False

      p.LunusProcimlt(1)

      imnum = imnum +1

    print

if __name__=="__main__":
  import sys

  args = sys.argv[1:] 
  usage = ["experiments=<experiments.json file, for metrology info>",
           "images=<glob for images, used for rotation series>"]

 # Output .vtk file
  try:
    idx = [a.find("vtk")==0 for a in args].index(True)
  except ValueError:
    vtk_file = "diffuse.vtk"
  else:
    vtk_file = args.pop(idx).split("=")[1]

 # Output .hkl file
  try:
    idx = [a.find("hkl")==0 for a in args].index(True)
  except ValueError:
    hkl_file = "diffuse.hkl"
  else:
    hkl_file = args.pop(idx).split("=")[1]

 # Output .cube file
  try:
    idx = [a.find("cube")==0 for a in args].index(True)
  except ValueError:
    cube_file = None
  else:
    cube_file = args.pop(idx).split("=")[1]

 # Output .lat file
  try:
    idx = [a.find("lat")==0 for a in args].index(True)
  except ValueError:
    lat_file = "diffuse.lat"
  else:
    lat_file = args.pop(idx).split("=")[1]

 # Input lunus input deck
  try:
    idx = [a.find("params")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Processing parameters must be specified using params="
  else:
    deck_file = args.pop(idx).split("=")[1]

 # Input json
  keep_going = True
  metro_list = []
  while keep_going:
    try:
      metroidx = [a.find("experiments")==0 for a in args].index(True)
    except ValueError:
      keep_going = False
    else:
      metro_list.append(args.pop(metroidx).split("=")[1])
  if (len(metro_list) == 0):
    raise ValueError,"Experiments .json file must be specified using experiments="
  
  metro = metro_list[0]

 # Image input glob
  keep_going = True
  image_glob_list = []
  rotation_series = False
  while (keep_going):
    try:
      imageglobidx = [a.find("images")==0 for a in args].index(True)
    except ValueError:
      keep_going = False
    else:
      image_glob_list.append(args.pop(imageglobidx).split("=")[1])
      rotation_series = True
  if (rotation_series):
    image_glob = image_glob_list[0]
  else:
    raise ValueError,"Must specify images (for rotation series)."

 # Background image input glob
  keep_going = True
  bkg_glob_list = []
  subtract_background_images = False
  while (keep_going):
    try:
      bkgglobidx = [a.find("backgrounds")==0 for a in args].index(True)
    except ValueError:
      keep_going = False
    else:
      bkg_glob_list.append(args.pop(bkgglobidx).split("=")[1])
      subtract_background_images = True

  if (subtract_background_images):
    bkg_glob = bkg_glob_list[0]


  if (len(metro_list) != len(image_glob_list) or (subtract_background_images and len(metro_list) != len(bkg_glob_list))):
    raise ValueError,"Must specify same number of experiments, images, and backgrounds"

  import copy, os

  import dxtbx
  from dxtbx.model.experiment_list import ExperimentListFactory
  from dials.array_family import flex

  p = lunus.Process()

  with open(deck_file) as f:
    deck = f.read()

  experiments = ExperimentListFactory.from_json_file(metro, check_format=False)
  experiment_params = get_experiment_params(experiments)

  deck_and_extras = deck+experiment_params

  p.LunusSetparamslt(deck_and_extras)

  fresh_lattice = True

  if rotation_series:
    for i in range(len(metro_list)):

      print "Image set ",i+1,":",

      metro = metro_list[i]

      experiments = ExperimentListFactory.from_json_file(metro, check_format=False)
      experiment_params = get_experiment_params(experiments)
      x = get_experiment_xvectors(experiments)

      deck_and_extras = deck+experiment_params

      p.LunusSetparamsim(deck_and_extras)

      image_glob = image_glob_list[i]
      if (subtract_background_images):
        bkg_glob = bkg_glob_list[i]

      process_one_glob()
    
    p.divide_by_counts()

    if (not vtk_file is None):
        p.write_as_vtk(vtk_file)
    if (not hkl_file is None):
        p.write_as_hkl(hkl_file)
    if (not cube_file is None):
        p.write_as_vtk(cube_file)
    if (not lat_file is None):
        p.write_as_lat(lat_file)
    

