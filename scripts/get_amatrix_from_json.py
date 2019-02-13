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
  # Output A matrix
  try:
    amatrixidx = [a.find("amatrix")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Output amatrix file must be specified."
  else:
    amatrix = args.pop(amatrixidx).split("=")[1]

  import dxtbx
  from dxtbx.model.experiment_list import ExperimentListFactory
  from dials.array_family import flex
   
  experiments = ExperimentListFactory.from_json_file(json, check_format=False)
  crystal = experiments[0].crystal
#  from scitbx.matrix import sqr
#  s = sqr(crystal.get_A())
#  A_matrix = s.inverse()
  A_direct = crystal.get_A_as_sqr()
  A_matrix = crystal.get_A_inverse_as_sqr()
#  from IPython import embed;embed()
#  A_matrix = np.linalg.inv(np.asarray(crystal.get_A()).reshape((3,3)))
  At = np.asarray(A_matrix.transpose()).reshape((3,3))

#  np.save(amatrix,At)
  At.astype('float32').tofile(amatrix)
