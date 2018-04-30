from time import clock, time
import glob
import copy
import dxtbx
from iotbx.phil import parse
from dxtbx.datablock import DataBlockFactory
from dials.array_family import flex
from dials.algorithms.indexing.indexer import indexer_base
from dials.util.options import OptionParser


if __name__=="__main__":
  import sys

  args = sys.argv[1:] 

  # target cell for indexing
  try:
      targetcellidx = [a.find("unit_cell")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target cell target_cell must be specified"
  else:
      target_cell = args.pop(targetcellidx).split("=")[1]
  # target spacegroup for indexing
  try:
      targetsgidx = [a.find("space_group")==0 for a in args].index(True)
  except ValueError:
      raise ValueError,"Target space group target_sg must be specified"
  else:
      target_sg = args.pop(targetsgidx).split("=")[1]
 # Image input glob
  try:
    imageglobidx = [a.find("image_glob")==0 for a in args].index(True)
  except ValueError:
    raise ValueError,"Must supply image_glob"
  else:
    image_glob = args.pop(imageglobidx).split("=")[1]

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
#  from dials.util.options import OptionParser
  parser = OptionParser(phil=phil_scope)
  params, options = parser.parse_args(args=[], show_diff_phil=True)
  
  params.refinement.parameterisation.scan_varying = False
  params.refinement.reflections.outlier.algorithm = 'sauter_poon'
  params.indexing.method='real_space_grid_search'
  
  filenames=glob.glob(image_glob)

  datablock = DataBlockFactory.from_filenames(filenames)[0]
 
  observed = flex.reflection_table.from_observations(datablock, params)
 
  working_params = copy.deepcopy(params)
  imagesets = datablock.extract_imagesets()

  idxr = indexer_base.from_parameters(observed, imagesets, params=params)
  idxr.index()

  indexed = idxr.refined_reflections
  experiments = idxr.refined_experiments

  crystal = copy.deepcopy(experiments.crystals()[0])
  print crystal.get_unit_cell()

  observed.as_pickle("lightweight_index_strong.pickle")
  idxr.export_as_json(experiments,file_name="lightweight_index_experiments.json")
  idxr.export_reflections(indexed,file_name="lightweight_index_reflections.pickle")

