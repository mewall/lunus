#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME lunus.process

from time import clock, time
import numpy as np
import glob, subprocess, shlex
import lunus

if __name__=="__main__":
  import sys

  args = sys.argv[1:] 
  usage = ["experiments=<experiments.json file, for metrology info>",
           "images=<glob for images, used for rotation series>"]

 # Read command line arguments

  subtract_background_images=False

 # Output .vtk file
  try:
    idx = [a.find("vtk")==0 for a in args].index(True)
  except ValueError:
    vtk_file = "diffuse.vtk"
  else:
    vtk_file = args.pop(idx).split("=")[1]
 # Input lunus input deck
  try:
    idx = [a.find("params")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Processing parameters must be specified using params="
  else:
    deck_file = args.pop(idx).split("=")[1]
 # Input json
  try:
    metroidx = [a.find("experiments")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Experiments .json file must be specified using experiments="
  else:
    metro = args.pop(metroidx).split("=")[1]
 # Image input glob
  try:
    imageglobidx = [a.find("images")==0 for a in args].index(True)
  except ValueError:
    rotation_series = False
  else:
    image_glob = args.pop(imageglobidx).split("=")[1]
    rotation_series = True
 # Background image input glob
  try:
    bkgglobidx = [a.find("backgrounds")==0 for a in args].index(True)
  except ValueError:
    subtract_background_images = False
  else:
    bkg_glob = args.pop(bkgglobidx).split("=")[1]
    subtract_background_images = True

  if (not (rotation_series)):
    raise ValueError,"Must specify images (for rotation series)."

  import copy, os

  import dxtbx
  from dxtbx.model.experiment_list import ExperimentListFactory
  from dials.array_family import flex

  experiments = ExperimentListFactory.from_json_file(metro, check_format=False)
  beam = experiments[0].beam
  detector = experiments[0].detector

  lab_coordinates = flex.vec3_double()
  for panel in detector: 
    pixels = flex.vec2_double(panel.get_image_size())
    mms = panel.pixel_to_millimeter(pixels)
    lab_coordinates.extend(panel.get_lab_coord(mms))
    pixel_size_mm = panel.get_pixel_size()[0]
    beam_mm_x = panel.get_beam_centre(beam.get_s0())[0]
    beam_mm_y = panel.get_beam_centre(beam.get_s0())[1]
    distance_mm = panel.get_distance()

  with open(deck_file) as f:
    deck = f.read()

  extra_params = "\npixel_size_mm={0}\nbeam_mm_x={1}\nbeam_mm_y={2}\ndistance_mm={3}\n".format(pixel_size_mm,beam_mm_x,beam_mm_y,distance_mm)

  deck += extra_params

  p = lunus.Process()

  p.LunusSetparamslt(deck)
  p.LunusSetparamsim(deck)

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
    if (subtract_background_images==True):
      bkglist=glob.glob(bkg_glob)
      bkglist.sort()
      if (len(filelist) != len(bkglist) and len(bkglist) != 1):
        raise ValueError,"Must either be one background or as many as there are images."
      if (len(bkglist) == 1):
        bkg = dxtbx.load(bkglist[0])
        bkg_data = bkg.get_raw_data()
    for i in range(len(filelist)):
      print i
      imgname=filelist[i]
      img = dxtbx.load(imgname)
      data = img.get_raw_data()

      p.set_image(data)
      
      if (subtract_background_images):
        if (len(bkglist) != 1):
          bkg = dxtbx.load(bkglist[i])
          bkg_data = bkg.get_raw_data()
        p.set_background(bkg_data)
        p.LunusBkgsubim()

      detector = img.get_detector()
      beam = img.get_beam()
      scan = img.get_scan()
      gonio = img.get_goniometer()

      crystal = copy.deepcopy(experiments.crystals()[0])
      axis = gonio.get_rotation_axis()
      start_angle, delta_angle = scan.get_oscillation()      
      crystal.rotate_around_origin(axis, start_angle + (delta_angle/2), deg=True)

      from scitbx import matrix
      A_matrix = matrix.sqr(crystal.get_A()).inverse()
      At = np.asarray(A_matrix.transpose()).reshape((3,3))
      At_flex = A_matrix.transpose().as_flex_double_matrix()

      p.set_amatrix(At_flex)

      if (i == 0):
        p.set_xvectors(x)
        p.set_image_ref()
        p.LunusProcimlt(0)

      p.LunusProcimlt(1)

      imnum = imnum +1

    p.divide_by_counts()
    p.write_as_vtk(vtk_file)

