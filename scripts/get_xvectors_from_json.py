from time import clock, time
import numpy as np

if __name__=="__main__":
  import sys

  args = sys.argv[1:] 
  usage = ["json=<input json file>",
          "xvecs=<xvectors output file>",
          "datasize=<data size output file>"]

 # Read command line arguments

 # Input json
  try:
    jsonidx = [a.find("json")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Input json file must be specified."
  else:
    json = args.pop(jsonidx).split("=")[1]
  # Output xvectors
  try:
    xvectorsidx = [a.find("xvectors")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Output xvectors file must be specified."
  else:
    xvectors = args.pop(xvectorsidx).split("=")[1]
  # Output datasize
  try:
    datasizeidx = [a.find("datasize")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Output datasze file must be specified."
  else:
    datasize = args.pop(datasizeidx).split("=")[1]

  import copy, os

  import dxtbx
  from dxtbx.model.experiment_list import ExperimentListFactory
  from dials.array_family import flex
   
  experiments = ExperimentListFactory.from_json_file(json, check_format=False)
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
  x = np.asarray(s1 - beam.get_s0())
    
  DATAsize = np.asarray(detector[0].get_image_size())

  np.save(xvectors,x)
  np.save(datasize,DATAsize)
