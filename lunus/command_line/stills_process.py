#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME sematura.stills_process

from __future__ import absolute_import, division
from dials.command_line.stills_process import Script
from dials.command_line.stills_process import Processor as SP_Processor

from dials.command_line import stills_process
from libtbx.phil import parse
sematura_phil_str = '''
  lunus {
    d_min = 2.5
      .type = float
      .help = Limiting resolution of diffuse intensity map.
  }
'''
sematura_defaults = """
  mp {
    method = mpi
      .type = choice
  }
"""


stills_process.phil_scope = parse(stills_process.control_phil_str +
                                  stills_process.dials_phil_str +
                                  sematura_phil_str,
                                  process_includes=True).fetch(
                            parse(stills_process.program_defaults_phil_str)).fetch(
                            parse(sematura_defaults))

class Processor(SP_Processor):

  def process_datablock(self, tag, datablock):

    if not self.params.output.composite_output:
      self.setup_filenames(tag)
    self.tag = tag

    if self.params.output.datablock_filename:
      from dxtbx.datablock import DataBlockDumper
      dump = DataBlockDumper(datablock)
      dump.as_json(self.params.output.datablock_filename)

    # Do the processing
    try:
      observed = self.find_spots(datablock)
    except Exception as e:
      print "Error spotfinding", tag, str(e)
      if not self.params.dispatch.squash_errors: raise
      return
    try:
      experiments, indexed = self.index(datablock, observed)
    except Exception as e:
      print "Couldn't index", tag, str(e)
      if not self.params.dispatch.squash_errors: raise
      return
    try:
      experiments, indexed = self.refine(experiments, indexed)
    except Exception as e:
      print "Error refining", tag, str(e)
      if not self.params.dispatch.squash_errors: raise
      return

    from diffuse_scattering.sematura import DiffuseExperiment, DiffuseImage
    def from_experiments(self,experiments):
        exp_xtal = experiments.crystals()[0]

        ### define useful attributes
        self.crystal = exp_xtal
        uc = self.crystal.get_unit_cell()
        uc_nospace = str(uc).replace(" ", "")
        uc_nospace_noparen = uc_nospace[1:-1]
        self.unit_cell = uc_nospace_noparen
        self.space_group = self.crystal.get_space_group()
        self.laue_group = self.space_group.laue_group_type()
        # self.a_matrix = crystal.get_A()
        self.experiments = experiments
    DiffuseExperiment.from_experiments = from_experiments

    test_exp = DiffuseExperiment()
    test_exp.from_experiments(experiments)

    img_set = test_exp.experiments.imagesets()
    imgs = img_set[0]
    file_list = imgs.paths()
    img_file = file_list[0]

    test_img = DiffuseImage(img_file)
    test_img.set_general_variables()
    test_img.remove_bragg_peaks(radial=True)
    test_img.scale_factor()
    test_img.crystal_geometry(test_exp.crystal)

    test_img.setup_diffuse_lattice(self.params.lunus.d_min)
    test_img.integrate_diffuse()


stills_process.Processor = Processor


if __name__ == '__main__':
  from dials.util import halraiser
  try:
    script = Script()
    script.run()
  except Exception as e:
    halraiser(e)
