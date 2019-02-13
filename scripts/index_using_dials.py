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

  import os

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
#  params.refinement.parameterisation.crystal.unit_cell.restraints.tie_to_target = []
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

