#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME lunus.stills_process

from __future__ import absolute_import, division, print_function

help_message = '''

See dials.stills_process. This version adds lunus processing in the integration step

'''

from dials.util import show_mail_on_error
from libtbx.phil import parse
from scitbx import matrix
from dials.array_family import flex
import numpy as np
import os
import lunus

def mpi_enabled():
  return 'MPI_LOCALRANKID' in os.environ.keys()
#  return False

def mpi_init():
  global mpi_comm
  global MPI
  from mpi4py import MPI as MPI
  mpi_comm = MPI.COMM_WORLD

def get_mpi_rank():
  return mpi_comm.Get_rank() if mpi_enabled() else 0


def get_mpi_size():
  return mpi_comm.Get_size() if mpi_enabled() else 1

def mpi_bcast(d):
  if mpi_enabled():
    db = mpi_comm.bcast(d,root=0)
  else:
    db = d
    
  return db

def mpi_barrier():
  if mpi_enabled():
    mpi_comm.Barrier()

def mpi_reduce_p(p, root=0):

  if mpi_enabled():
#    if get_mpi_rank() == 0:
#      print("LUNUS.PROCESS: Convertinf flex arrays to numpy arrays")
#      sys.stdout.flush()

    l = p.get_lattice().as_numpy_array()
    c = p.get_counts().as_numpy_array()

    if get_mpi_rank() == root:
      lt = np.zeros_like(l)
      ct = np.zeros_like(c)
    else: 
      lt = None
      ct = None


    mpi_comm.Reduce(l,lt,op=MPI.SUM,root=root)
    mpi_comm.Reduce(c,ct,op=MPI.SUM,root=root)
    
    if get_mpi_rank() == root:
      print("LUNUS.PROCESS: Converting numpy arrays to flex arrays")
      p.set_lattice(flex.double(lt))
      p.set_counts(flex.int(ct))

  return p

control_phil_str = '''
lunus {
  deck_file = None
    .type = path
}
'''

from dials.command_line.stills_process import dials_phil_str, program_defaults_phil_str, Script as DialsScript, control_phil_str as dials_control_phil_str, Processor as DialsProcessor

program_defaults_phil_str += """
input.cache_reference_image = True
"""

phil_scope = parse(dials_control_phil_str + control_phil_str + dials_phil_str,  process_includes=True).fetch(parse(program_defaults_phil_str))

#from libtbx.mpi4py import MPI
#comm = MPI.COMM_WORLD
#rank = comm.Get_rank()  # each process in MPI has a unique id, 0-indexed
#size = comm.Get_size()  # size: number of processes running in this job

if mpi_enabled():
  mpi_init()
  
from lunus.command_line.process import get_experiment_params, get_experiment_xvectors

class LunusProcessor(DialsProcessor):
  def integrate(self, experiments, indexed):
    integrated = super(LunusProcessor, self).integrate(experiments, indexed)

    if not hasattr(self, 'reference_experiment_params'):
      self.reference_experiment_params = get_experiment_params(self.reference_experiments)
      self.lunus_processor = lunus.Process(len(self.reference_experiment_params))
      with open(self.params.lunus.deck_file) as f:
        self.deck = f.read()

      deck_and_extras = self.deck+self.reference_experiment_params[0]
      self.lunus_processor.LunusSetparamslt(deck_and_extras)

      self.lunus_integrate(self.reference_experiments, is_reference = True)

    if len(experiments) > 1:
      print("Skipping lunus integration: more than 1 lattice was indexed")
    else:
      self.lunus_integrate(experiments)

    return integrated

  def lunus_integrate(self, experiments, is_reference = False):
    assert len(experiments) == 1

    experiment_params = get_experiment_params(experiments)
    p = self.lunus_processor

    data = self.reference_experiments[0].imageset[0]
    if not isinstance(data, tuple):
      data = data,
    for panel_idx, panel in enumerate(data):
      self.lunus_processor.set_image(panel_idx, panel)

    for pidx in range(len(experiment_params)):
      deck_and_extras = self.deck+experiment_params[pidx]
      p.LunusSetparamsim(pidx,deck_and_extras)

    print("LUNUS: Processing image")

    if is_reference:
      x = get_experiment_xvectors(experiments)
      for pidx in range(len(x)):
        p.set_xvectors(pidx,x[pidx])

   # We need an amatrix for the next call, to set up the lattice size
      print("LUNUS: Entering lprocimlt()")
      p.LunusProcimlt(0)
      print("LUNUS: Done with lprocimlt()")
    else:
      crystal = experiments[0].crystal
      A_matrix = matrix.sqr(crystal.get_A()).inverse()
      At = np.asarray(A_matrix.transpose()).reshape((3,3))
      At_flex = A_matrix.transpose().as_flex_double_matrix()

      p.set_amatrix(At_flex)

      p.LunusProcimlt(1)

  def finalize(self):
    mpi_barrier() # Need to synchronize at this point so that all the server/client ranks finish

    print("STARTING FINALIZE, rank %d, size %d"%(get_mpi_rank(), get_mpi_size()))

    if get_mpi_size() > 2:
      if get_mpi_rank() > 0:
        p = self.lunus_processor
        mpi_reduce_p(p, root=1)

        if get_mpi_rank() == 1:
          p.divide_by_counts()
          p.write_as_hkl('results.hkl')
    else:
      p = self.lunus_processor
      mpi_reduce_p(p, root=0)

      if get_mpi_rank() == 0:
        p.divide_by_counts()
        p.write_as_hkl('results.hkl')

class Script(DialsScript):
  '''A class for running the script.'''
  def __init__(self):
    '''Initialise the script.'''
    from dials.util.options import OptionParser
    import libtbx.load_env

    # The script usage
    usage = "usage: %s [options] [param.phil] filenames" % libtbx.env.dispatcher_name

    self.tag = None
    self.reference_detector = None

    # Create the parser
    self.parser = OptionParser(
      usage=usage,
      phil=phil_scope,
      epilog=help_message
      )

if __name__ == '__main__':
  import dials.command_line.stills_process
  dials.command_line.stills_process.Processor = LunusProcessor

  with show_mail_on_error():
    script = Script()
    script.run()
