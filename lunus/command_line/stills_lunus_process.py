#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME lunus.stills_process

from __future__ import absolute_import, division, print_function

help_message = '''

See dials.stills_process. This version adds lunus processing in the integration step

'''

from dials.util import show_mail_on_error
from libtbx.phil import parse
control_phil_str = '''
'''

from dials.command_line.stills_process import dials_phil_str, program_defaults_phil_str, Script as DialsScript, control_phil_str as dials_control_phil_str, Processor as DialsProcessor

phil_scope = parse(dials_control_phil_str + control_phil_str + dials_phil_str,  process_includes=True).fetch(parse(program_defaults_phil_str))

from libtbx.mpi4py import MPI
comm = MPI.COMM_WORLD
rank = comm.Get_rank()  # each process in MPI has a unique id, 0-indexed
size = comm.Get_size()  # size: number of processes running in this job

from lunus.command_line.process import get_experiment_params, get_experiment_xvectors

class LunusProcessor(DialsProcessor):
  def integrate(self, experiments, indexed):
    integrated = super(LunusProcessor, self).integrate(experiments, indexed)

    experiment_params = get_experiment_params(experiments)
    x = get_experiment_xvectors(experiments)


    return integrated

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
