from time import clock, time
import numpy as np
import re
#from xfel.cxi.display_spots import run_one_index_core

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

  # get libtbx.python modules path
  try:
    pymodpathidx = [a.find("libtbx.python.modules")==0 for a in args].index(True)
  except ValueError:
    pymodpath = ""
  else:
    pymodpath = args.pop(pymodpathidx).split("=")[1]
  try:
    npyfilediridx = [a.find("output.dir")==0 for a in args].index(True)
  except ValueError:
    npyfiledir=""
  else:
    npyfiledir = args.pop(npyfilediridx).split("=")[1]
  # processed diffuse diffraction image to be converted
  try:
    diffimgnameidx = [a.find("input.fname")==0 for a in args].index(True)
  except ValueError:
    diffimgname = ""
  else:
    diffimgname = args.pop(diffimgnameidx).split("=")[1]

  import os

  if (pymodpath == ""):
    from dxtbx.format.FormatSMVADSCNoDateStamp import FormatSMVADSCNoDateStamp
  else:	
    import imp
    FormatSMVADSCNoDateStamp = imp.load_module("dxtbx.format.FormatSMVADSCNoDateStamp",None,os.path.join(pymodpath,"/cctbx_project/dxtbx/format"),('','',5))

  img = FormatSMVADSCNoDateStamp(diffimgname)
  detector = img.get_detector()
  if len(detector)>1:
      raise Exception("Multi-panel detector, aborting.")
  DATA = img.get_raw_data()
  DATAnp = np.int16(np.asarray(np.frombuffer(DATA.copy_to_byte_str(),dtype='int32')))
  if (not os.path.isdir(npyfiledir)):
      print("Error npyfiledir={0}\n".format(npyfiledir))
      raise Exception("Target directory does not exist.")
  else:
      np.save(npyfiledir+"/DATA.npy",DATAnp)

