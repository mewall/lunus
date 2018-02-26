#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME sematura.stills_process_mpi

from __future__ import absolute_import, division

from lunus.command_line import stills_process as lunus_stills_process
# has the side effect of injecting the sematura phil string into dials.stills_process

from dials.command_line.stills_process_mpi import Script
from dials.command_line import stills_process_mpi as dials_stills_process_mpi
dials_stills_process_mpi.Processor = lunus_stills_process.Processor

if __name__ == '__main__':
    from mpi4py import MPI
    comm = MPI.COMM_WORLD
    script = Script(comm)
    script.assign_work()
    comm.barrier()
    script.run()
