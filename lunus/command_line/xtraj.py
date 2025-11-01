#!/usr/bin/env python
#
# LIBTBX_SET_DISPATCHER_NAME lunus.xtraj
#
# Read a MD trajectory and output structure factor statistics including diffuse
#
# Michael Wall, Los Alamos National Laboratory
#
# Version 0.1a, July 2018
# Version 0.2a, October 2019
#
# This script depends on CCTBX. Launch using mpirun for parallel execution.

from __future__ import print_function
from iotbx.pdb import hierarchy
from cctbx.array_family import flex
import mmtbx.utils
import mmtbx.model
from cctbx import maptbx
import copy
import mdtraj as md
import time
import numpy as np
import scipy.optimize
import os
from libtbx.utils import Keep
from cctbx import crystal
import cctbx.sgtbx
import subprocess

def mpi_enabled():
  return 'OMPI_COMM_WORLD_SIZE' in os.environ.keys()

def calc_msd(x):
  d = np.zeros(this_sites_frac.shape)
  msd = 0
  for k in range(3):
    d[:,k] = (this_sites_frac[:,k] - ref_sites_frac[:,k] + x[k] + 0.5)%1.0 - 0.5
    msd += np.sum(d[:,k] * d[:,k])
    return msd

if __name__=="__main__":
  import sys

  args = sys.argv[1:]

# selection                                                              

  try:
    idx = [a.find("selection")==0 for a in args].index(True)
  except ValueError:
    selection_text = "all"
  else:
    selection_text = args.pop(idx).split("=")[1]

# d_min

  try:
    idx = [a.find("d_min")==0 for a in args].index(True)
  except ValueError:
    d_min = 0.9
  else:
    d_min = float(args.pop(idx).split("=")[1])

# nsteps (use in lieu of "last" parameter)

  try:
    idx = [a.find("nsteps")==0 for a in args].index(True)
  except ValueError:
    nsteps = 0
  else:
    nsteps = int(args.pop(idx).split("=")[1])

# stride

  try:
    idx = [a.find("stride")==0 for a in args].index(True)
  except ValueError:
    stride = 1
  else:
    stride = int(args.pop(idx).split("=")[1])

# first frame number (numbering starts at 0)

  try:
    idx = [a.find("first")==0 for a in args].index(True)
  except ValueError:
    first = 0
  else:
    first = int(args.pop(idx).split("=")[1])

# last frame number

  try:
    idx = [a.find("last")==0 for a in args].index(True)
  except ValueError:
    last = 0
  else:
    last = int(args.pop(idx).split("=")[1])

# chunk size (number of frames) for breaking up the trajectory

  try:
    idx = [a.find("chunk")==0 for a in args].index(True)
  except ValueError:
    chunksize = None
  else:
    chunksize = int(args.pop(idx).split("=")[1])

# topology file (typically a .pdb file)

  try:
    idx = [a.find("top")==0 for a in args].index(True)
  except ValueError:
    top_file = "top.pdb"
  else:
    top_file = args.pop(idx).split("=")[1]

# trajectory file (mpirun works with .xtc but not .pdb)

  try:
    idx = [a.find("traj")==0 for a in args].index(True)
  except ValueError:
    traj_file = "traj.xtc"
  else:
    traj_file = args.pop(idx).split("=")[1]

# density_traj (does nothing right now)

  try:
    idx = [a.find("density_traj")==0 for a in args].index(True)
  except ValueError:
    dens_file = None
  else:
    dens_file = args.pop(idx).split("=")[1]

# diffuse

  try:
    idx = [a.find("diffuse")==0 for a in args].index(True)
  except ValueError:
    diffuse_file = "diffuse.hkl"
  else:
    diffuse_file = args.pop(idx).split("=")[1]

# fcalc

  try:
    idx = [a.find("fcalc")==0 for a in args].index(True)
  except ValueError:
    fcalc_file = "fcalc.mtz"
  else:
    fcalc_file = args.pop(idx).split("=")[1]

# icalc

  try:
    idx = [a.find("icalc")==0 for a in args].index(True)
  except ValueError:
    icalc_file = "icalc.mtz"
  else:
    icalc_file = args.pop(idx).split("=")[1]

# Diffuse data file

  try:
    idx = [a.find("ID_file")==0 for a in args].index(True)
  except ValueError:
    diffuse_data_file = None
  else:
    diffuse_data_file = args.pop(idx).split("=")[1]

# density map

#  try:
#    idx = [a.find("density")==0 for a in args].index(True)
#  except ValueError:
#    density_file = "density.ccp4"
#  else:
#    density_file = args.pop(idx).split("=")[1]

# partial_sum (don't divide by nsteps at the end)

  try:
    idx = [a.find("partial_sum")==0 for a in args].index(True)
  except ValueError:
    partial_sum_mode = False
  else:
    partial_sum_str = args.pop(idx).split("=")[1]
    if partial_sum_str == "True":
      partial_sum_mode = True
    else:
      partial_sum_mode = False

# translational fit (align using fractional coordinates)

  try:
    idx = [a.find("fit")==0 for a in args].index(True)
  except ValueError:
    translational_fit = False
  else:
    fit_str = args.pop(idx).split("=")[1]
    if fit_str == "True":
      translational_fit = True
    else:
      translational_fit = False

# Scattering table

  try:
    idx = [a.find("scattering_table")==0 for a in args].index(True)
  except ValueError:
    scattering_table = 'n_gaussian'
  else:
    scattering_table = args.pop(idx).split("=")[1]

    # Unit cell, replaces the one in the top file

  try:
    idx = [a.find("unit_cell")==0 for a in args].index(True)
  except ValueError:
    unit_cell_str = None
  else:
    unit_cell_str = args.pop(idx).split("=")[1]

# Space group, replaces the one in the top file

  try:
    idx = [a.find("space_group")==0 for a in args].index(True)
  except ValueError:
    space_group_str = None
  else:
    space_group_str = args.pop(idx).split("=")[1]

# Apply B factor in structure calculations, then reverse after calc

  try:
    idx = [a.find("apply_bfac")==0 for a in args].index(True)
  except ValueError:
    apply_bfac = True
  else:
    apply_bfac_str = args.pop(idx).split("=")[1]
    if apply_bfac_str == "False":
      apply_bfac = False
    else:
      apply_bfac = True

# Calculate f_000 and print it

  try:
    idx = [a.find("calc_f000")==0 for a in args].index(True)
  except ValueError:
    calc_f000 = False
  else:
    calc_f000_str = args.pop(idx).split("=")[1]
    if calc_f000_str == "False":
      calc_f000 = False
    else:
      calc_f000 = True
      
# Do optimization with respect to data

  try:
    idx = [a.find("do_opt")==0 for a in args].index(True)
  except ValueError:
    do_opt = False
  else:
    do_opt_str = args.pop(idx).split("=")[1]
    if do_opt_str == "False":
      do_opt = False
    else:
      do_opt = True
      
# Use topology file B factoris in structure calculations

  try:
    idx = [a.find("use_top_bfacs")==0 for a in args].index(True)
  except ValueError:
    use_top_bfacs = False
  else:
    use_top_bfacs_str = args.pop(idx).split("=")[1]
    if use_top_bfacs_str == "True":
      use_top_bfacs =True 
    else:
      use_top_bfacs = False

# Set nsteps if needed
  
  if (nsteps == 0):
    nsteps = last - first + 1
  elif (last != 0):
    print("Please specify nsteps or last, but not both.")
    raise ValueError()

  last = first + nsteps - 1

# Method (engine) for calculating structure factors

  try:
    idx = [a.find("engine")==0 for a in args].index(True)
  except ValueError:
    engine = "cctbx"
  else:
    engine = args.pop(idx).split("=")[1]

# Calculate difference with respect to reference (for optimization)

  try:
    idx = [a.find("diff_mode")==0 for a in args].index(True)
  except ValueError:
    diff_mode = False
  else:
    diff_str = args.pop(idx).split("=")[1]
    if diff_str == "True":
      diff_mode = True
    else:
      diff_mode = False
      
# Reference fcalc for diff mode

  try:
    idx = [a.find("fcalc_ref")==0 for a in args].index(True)
  except ValueError:
    fcalc_ref_file = "fcalc_ref.mtz"
  else:
    fcalc_ref_file = args.pop(idx).split("=")[1]
    
# Reference icalc for diff mode

  try:
    idx = [a.find("icalc_ref")==0 for a in args].index(True)
  except ValueError:
    icalc_ref_file = "icalc_ref.mtz"
  else:
    icalc_ref_file = args.pop(idx).split("=")[1]

# Number of frames in reference

  try:
    idx = [a.find("nref")==0 for a in args].index(True)
  except ValueError:
    nsteps_ref = 1
  else:
    nsteps_ref = int(args.pop(idx).split("=")[1])
    
# Initialize MPI

  if mpi_enabled():
    from mpi4py import MPI
    mpi_comm = MPI.COMM_WORLD
    mpi_rank = mpi_comm.Get_rank()
    mpi_size = mpi_comm.Get_size()
  else:
    mpi_comm = None
    mpi_rank = 0
    mpi_size = 1

# If in diff_mode, read the reference .mtz files

  if diff_mode:
    if mpi_rank == 0:      
      from iotbx.reflection_file_reader import any_reflection_file
      hkl_in = any_reflection_file(file_name=fcalc_ref_file)
      miller_arrays = hkl_in.as_miller_arrays()
      avg_fcalc_ref = miller_arrays[0]
      hkl_in = any_reflection_file(file_name=icalc_ref_file)
      miller_arrays = hkl_in.as_miller_arrays()
      avg_icalc_ref = miller_arrays[0]

# If there's a diffuse data file, read it

  if diffuse_data_file is not None:
    if mpi_rank == 0:      
      from iotbx.reflection_file_reader import any_reflection_file
      hkl_in = any_reflection_file(file_name=diffuse_data_file)
      miller_arrays = hkl_in.as_miller_arrays()
      diffuse_expt = miller_arrays[0].as_non_anomalous_array()
    else:
      diffuse_expt = None
    diffuse_expt = mpi_comm.bcast(diffuse_expt,root=0)

# read .pdb file. It's used as a template, so don't sort it.

  if mpi_rank == 0:
    pdb_in = hierarchy.input(file_name=top_file,sort_atoms=False)

# MEW use cctbx.xray.structure.customized_copy() here to change the unit cell and space group as needed
    symm = pdb_in.input.crystal_symmetry()
    if unit_cell_str is None:
      unit_cell = symm.unit_cell()
    else:
      unit_cell = unit_cell_str
    if space_group_str is None:
      space_group_info = symm.space_group_info()
    else:
      space_group_info = cctbx.sgtbx.space_group_info(symbol=space_group_str)

    xrs = pdb_in.input.xray_structure_simple(crystal_symmetry=crystal.symmetry(unit_cell=unit_cell,space_group_info=space_group_info))
  else:
    pdb_in = None
    xrs = None
    space_group_info = None
    
  if mpi_enabled():
    pdb_in = mpi_comm.bcast(pdb_in,root=0)
    xrs = mpi_comm.bcast(xrs,root=0)
    space_group_info = mpi_comm.bcast(space_group_info,root=0)

  space_group_str = str(space_group_info).replace(" ","")
  selection_cache = pdb_in.hierarchy.atom_selection_cache()
  selection = selection_cache.selection(selection_text)
  xrs.convert_to_isotropic()
  if apply_bfac:
    xrs.set_b_iso(15.0)
  else:
    if not use_top_bfacs:
      xrs.set_b_iso(0.0)
  xrs.set_occupancies(1.0)
  xrs_sel = xrs.select(selection)
  xrs_sel.scattering_type_registry(table=scattering_table)
  if (mpi_rank == 0):
#    pdbtmp = xrs_sel.as_pdb_file()
#    with open("reference.pdb","w") as fo:
#      fo.write(pdbtmp)
    if engine == "sfall":
      sfall_script = \
"""
sfall xyzin $1 hklin $2 hklout $3 <<EOF
mode sfcalc xyzin hklin
symm {space_group}
labin FP=F SIGFP=SIGF
RESOLUTION {d_min}
NOSCALE
VDWR 3.0
end
EOF
"""
      print("Writing the following as run_sfall.sh:")
      print(sfall_script.format(d_min=d_min,space_group=space_group_str))
      with open("run_sfall.sh","w") as fo:
        fo.write(sfall_script.format(d_min=d_min,space_group=space_group_str))
    if calc_f000:
      f_000 = mmtbx.utils.f_000(xray_structure=xrs_sel,mean_solvent_density=0.0)
      volume = xrs_sel.unit_cell().volume()
      print("f_000 = %g, volume = %g" % (f_000.f_000,volume))

  if engine == "sfall":
    fcalc = xrs_sel.structure_factors(d_min=d_min).f_calc()
    mtz_dataset = fcalc.as_mtz_dataset('FWT')
    famp = abs(fcalc)
    famp.set_observation_type_xray_amplitude()
    famp.set_sigmas(sigmas=flex.double(fcalc.data().size(),1))
    #famp_with_sigmas = miller_set.array(data=famp.data(),sigmas=sigmas)
    mtz_dataset.add_miller_array(famp,'F')
    mtz_dataset.mtz_object().write(file_name="reference_{rank:03d}.mtz".format(rank=mpi_rank))

# read the MD trajectory and extract coordinates

  if (chunksize is None):
    nchunks = mpi_size
    chunksize = int(nsteps/nchunks)
  else:
    nchunks = int(nsteps/chunksize)
  chunklist = np.zeros((mpi_size), dtype=int)
  nchunklist = np.zeros((mpi_size),dtype=int)
  skiplist = np.zeros((mpi_size), dtype=int)
  nchunksize = nchunks/mpi_size
  leftover = nsteps % mpi_size
  ct = 0
  for i in range(mpi_size):
    chunklist[i] = chunksize
    nchunklist[i] = nchunksize
    if (i < leftover):
      chunklist[i] += 1
    if (i == 0):
      skiplist[i] = first
    else:
      skiplist[i] = skiplist[i-1] + chunklist[i-1] * nchunklist[i-1]
    ct = ct + chunklist[i]*nchunklist[i]

  if (mpi_rank == 0):               
    stime = time.time()
    print("Will use ",ct," frames distributed over ",mpi_size," ranks")

  ct = 0
  sig_fcalc = None
  sig_icalc = None

  if (skiplist[mpi_rank] <= last):
    skip_calc = False
  else:
    skip_calc = True

  ti = md.iterload(traj_file,chunk=chunklist[mpi_rank],top=top_file,skip=skiplist[mpi_rank])

# Each MPI rank works with its own trajectory chunk t

  chunk_ct = 0
  fcalc_list = None
  
  itime = time.time()
  
  for tt in ti:
    
    mtime = time.time()
      
    t = tt
#    print "rank =",mpi_rank," skip = ",skiplist[mpi_rank]," chunk = ",chunklist[mpi_rank]," start time = ",t.time[0]," coords of first atom = ",t.xyz[0][0]

#    if mpi_enabled():
#      mpi_comm.Barrier()                                                                          
    na = len(t.xyz[0])

  # np.around() is needed here to avoid small changes in the coordinates
  #   that accumulate large errors in structure factor calculations. The factor
  #   of 10 is needed as the units from the trajectory are nm, whereas cctbx
  #   needs Angstrom units.

    tsites = np.around(np.array(t.xyz*10.,dtype=np.float64),3)

  # ***The following code needs modification to prevent the bcast here, as
  #   it will create a barrier that prevents execution when the number
  #   of steps is not equal to a multiple of the number of ranks
    
    if (translational_fit and chunk_ct == 0):

      if (mpi_rank == 0):

        # Get the fractional coords of the reference structure alpha carbons, for translational fit. 
        # MEW Note: only uses all c-alpha atoms in the structure and only does translational fit for now

        sites_frac = xrs.sites_frac().as_double().as_numpy_array().reshape((na,3))
        sel_indices = t.topology.select('name CA')
        ref_sites_frac = np.take(sites_frac,sel_indices,axis=0)

      else:
        ref_sites_frac = None
        sel_indices = None

    # Broadcast arrays used for translational fit

      if mpi_enabled():
        ref_sites_frac = mpi_comm.bcast(ref_sites_frac,root=0)
        sel_indices = mpi_comm.bcast(sel_indices,root=0)

  # calculate fcalc, diffuse intensity, and (if requested) density trajectory

    if mpi_rank == 0:
      stime = time.time()
      if chunk_ct == 0:
        print("Number of atoms in topology file = ",na)

    map_data = []
    num_elems = len(t)

    if (num_elems <= 0 or skip_calc):
      num_elems = 0
      xrs_sel = xrs.select(selection)
      if sig_fcalc is None:
        sig_fcalc = xrs_sel.structure_factors(d_min=d_min).f_calc() * 0.0
      if sig_icalc is None:
        sig_icalc = abs(sig_fcalc).set_observation_type_xray_amplitude().f_as_f_sq()
      print("WARNING: Rank ",mpi_rank," is idle")

    else:

      for i in range(num_elems):

        # overwrite crystal structure coords with trajectory coords

        tmp = flex.vec3_double(tsites[i,:,:])
        xrs.set_sites_cart(tmp)

    # perform translational fit with respect to the alpha carbons in the topology file

        if (translational_fit):
          sites_frac = xrs.sites_frac().as_double().as_numpy_array().reshape((na,3))
          x0 = [0.0,0.0,0.0]
          otime1 = time.time()
          this_sites_frac = np.take(sites_frac,sel_indices,axis=0)
          res = scipy.optimize.minimize(calc_msd,x0,method='Powell',jac=None,options={'disp': False,'maxiter': 10000})
          for j in range(3):
            sites_frac[:,j] +=res.x[j]        
            otime2 = time.time()
            xrs.set_sites_frac(flex.vec3_double(sites_frac))
    #        print ("Time to optimize = ",otime2-otime1)

    # select the atoms for the structure factor calculation

        xrs_sel = xrs.select(selection)
        if engine == "sfall":
          pdbtmp = xrs_sel.as_pdb_file()
          pdbnam_tmp = "tmp_{rank:03d}.pdb".format(rank=mpi_rank)
          fcalcnam_tmp = "tmp_{rank:03d}.mtz".format(rank=mpi_rank)
          lognam = "sfall_{rank:03d}.log".format(rank=mpi_rank)
          with open(pdbnam_tmp,"w") as fo:
            fo.write(pdbtmp)
          with open(lognam,"w") as fo:
            subprocess.run(["bash","run_sfall.sh",pdbnam_tmp,"reference_{rank:03d}.mtz".format(rank=mpi_rank),fcalcnam_tmp],stdout=fo)
          from iotbx.reflection_file_reader import any_reflection_file
          hkl_in = any_reflection_file(file_name=fcalcnam_tmp)
          miller_arrays = hkl_in.as_miller_arrays()
          fcalc = miller_arrays[1]
        else:
          xrs_sel.scattering_type_registry(table=scattering_table)
          fcalc = xrs_sel.structure_factors(d_min=d_min).f_calc()

        if do_opt:
          diffuse_expt_common,fcalc_common = diffuse_expt.common_sets(fcalc.as_non_anomalous_array())
          icalc_common = abs(fcalc_common).set_observation_type_xray_amplitude().f_as_f_sq()
          fcalc_common_data = np.array(fcalc_common.data())
          icalc_common_data = np.array(icalc_common.data())
          if fcalc_list is None:
            fcalc_list = np.empty((chunklist[mpi_rank]*nchunklist[mpi_rank],fcalc_common_data.size),dtype=fcalc_common_data.dtype)
            icalc_list = np.empty((chunklist[mpi_rank]*nchunklist[mpi_rank],icalc_common_data.size),dtype=icalc_common_data.dtype)
            sig_fcalc = fcalc_common
            sig_icalc = icalc_common
          fcalc_list[ct] = fcalc_common_data
          icalc_list[ct] = icalc_common_data
    # Commented out some density trajectory code
    #    if not (dens_file is None):
    #      this_map = fcalc.fft_map(d_min=d_min, resolution_factor = 0.5)
    #      real_map_np = this_map.real_map_unpadded().as_numpy_array()
    #      map_data.append(real_map_np)
        else:
          if sig_fcalc is None:
            sig_fcalc = fcalc
            sig_icalc = abs(fcalc).set_observation_type_xray_amplitude().f_as_f_sq()
          else:
            sig_fcalc = sig_fcalc + fcalc
            sig_icalc = sig_icalc + abs(fcalc).set_observation_type_xray_amplitude().f_as_f_sq()
        ct = ct + 1

    chunk_ct = chunk_ct + 1

    print("Rank ",mpi_rank," processed chunk ",chunk_ct," of ",nchunklist[mpi_rank]," with ",ct," frames in ",time.time()-mtime," seconds")

    if (chunk_ct >= nchunklist[mpi_rank]):
      break


    
# Commented out some density trajectory code
#  if not (dens_file is None):
#    map_grid = np.concatenate(map_data)
#    Ni = map_data[0].shape[0]
#    Nj = map_data[0].shape[1]
#    Nk = map_data[0].shape[2]
#    map_grid_3D = np.reshape(map_grid,(len(tsites),Ni,Nj,Nk))
#    np.save(dens_file,map_grid_3D)                           

  print("Rank ",mpi_rank," is done with individual calculations")
  sys.stdout.flush()

  if mpi_enabled():
    mpi_comm.Barrier()

  if (mpi_rank == 0):
    mtime = time.time()
    print("TIMING: Calculate individual statistics = ",mtime-itime)

# If optimization is on, calculate sig_fcalc and sig_icalc
  if do_opt:
    if apply_bfac:
      miller_set = sig_fcalc.set()
      dwf_array = miller_set.debye_waller_factors(b_iso=15.0)
      dwf_data_np = np.array(dwf_array.data())
      # fcalc_list /= dwf_data_np[np.newaxis,:]
      # icalc_list /= dwf_data_np[np.newaxis,:]
      for x in range(len(fcalc_list)):
        fcalc_list[x] /= dwf_data_np
        icalc_list[x] /= dwf_data_np * dwf_data_np
    #At this point fcalc_list and icalc_list can be used for optimization
    #Still need to calculate the sums across all MPI ranks, however.
    sig_fcalc_np = np.sum(fcalc_list,axis=0)
    sig_icalc_np = np.sum(icalc_list,axis=0)
  else:
    sig_fcalc_np = sig_fcalc.data().as_numpy_array()
    sig_icalc_np = sig_icalc.data().as_numpy_array()
    
# perform reduction of sig_fcalc, sig_icalc, and ct

  # if mpi_rank == 0:
  #   tot_sig_fcalc_np = np.zeros_like(sig_fcalc_np)
  #   tot_sig_icalc_np = np.zeros_like(sig_icalc_np)
  # else:
  #   tot_sig_fcalc_np = None
  #   tot_sig_icalc_np = None
  tot_sig_fcalc_np = np.zeros_like(sig_fcalc_np)
  tot_sig_icalc_np = np.zeros_like(sig_icalc_np)

  if mpi_enabled():
    mpi_comm.Barrier()                                                        

  if mpi_enabled():
    mpi_comm.Allreduce(sig_fcalc_np,tot_sig_fcalc_np,op=MPI.SUM)
    mpi_comm.Allreduce(sig_icalc_np,tot_sig_icalc_np,op=MPI.SUM)
    ct = mpi_comm.allreduce(ct,op=MPI.SUM)
  else:
    tot_sig_fcalc_np = sig_fcalc_np
    tot_sig_icalc_np = sig_icalc_np

# compute averages

  if (mpi_rank == 0):
    sig_fcalc_data = sig_fcalc.data()
    sig_icalc_data = sig_icalc.data()
    for x in range(sig_fcalc_data.size()):
      sig_fcalc_data[x] = tot_sig_fcalc_np[x]
      sig_icalc_data[x] = tot_sig_icalc_np[x]
    avg_fcalc = sig_fcalc / float(ct)
    avg_icalc = sig_icalc / float(ct)
    if apply_bfac and not do_opt:
      miller_set = avg_fcalc.set()
      dwf_array = miller_set.debye_waller_factors(b_iso=15.0)
      dwf_data = dwf_array.data()
      avg_fcalc_data = avg_fcalc.data()
      avg_icalc_data = avg_icalc.data()
      for x in range(0,avg_fcalc_data.size()):
        avg_fcalc_data[x] /= dwf_data[x]
        avg_icalc_data[x] /= dwf_data[x] * dwf_data[x]
    # Calculate difference if requested
    if diff_mode:
      avg_fcalc_ref_data = avg_fcalc_ref.data()
      avg_icalc_ref_data = avg_icalc_ref.data()
      for x in range(0,avg_fcalc_data.size()):
        avg_fcalc_data[x] = (avg_fcalc_ref_data[x] * float(nsteps_ref) - avg_fcalc_data[x] * float(ct)) / float(nsteps_ref - ct)
        avg_icalc_data[x] = (avg_icalc_ref_data[x] * float(nsteps_ref) - avg_icalc_data[x] * float(ct)) / float(nsteps_ref - ct)
    sq_avg_fcalc = abs(avg_fcalc).set_observation_type_xray_amplitude().f_as_f_sq()
    sq_avg_fcalc_data = sq_avg_fcalc.data()
    diffuse_array=avg_icalc*1.0
    diffuse_data = diffuse_array.data()
    for x in range(0,diffuse_data.size()):
      diffuse_data[x]=diffuse_data[x]-sq_avg_fcalc_data[x]
    etime = time.time()
    print("TIMING: Reduction = ",etime-mtime)
    print("TIMING: Total diffuse calculation = ",etime-stime)

# Compute the correlation with the data, if available
    if diffuse_data_file is not None:
      print("Calculating Correlation")
      if do_opt:
        #Common sets already have been extracted in this case
        diffuse_array_common = diffuse_array
      else:
        diffuse_expt_common, diffuse_array_common = diffuse_expt.common_sets(diffuse_array.as_non_anomalous_array())
      C = np.corrcoef(np.array([diffuse_expt_common.data(),diffuse_array_common.data()]))
      print("Pearson correlation between diffuse simulation and data = ",C[0,1])

    
# write fcalc

    if not partial_sum_mode:
      avg_fcalc.as_mtz_dataset('FWT').mtz_object().write(file_name=fcalc_file)
    else:
      sig_fcalc.as_mtz_dataset('FWTsum').mtz_object().write(file_name=fcalc_file)

# write density map

#    if not partial_sum_mode:
#      symmetry_flags = maptbx.use_space_group_symmetry
#      dmap = avg_fcalc.fft_map(d_min=d_min,resolution_factor=0.5,symmetry_flags=symmetry_flags)
#      dmap.apply_volume_scaling()
#      dmap = avg_fcalc.fft_map(f_000=f_000.f_000)
#      dmap.as_ccp4_map(file_name=density_file)

# write icalc

    print("Average Icalc:")
    count=0
    for hkl,intensity in avg_icalc:
      print("%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,))))
      count+=1
      if count>10: break
    if not partial_sum_mode:
      avg_icalc.as_mtz_dataset('Iavg').mtz_object().write(file_name=icalc_file)
    else:
      sig_icalc.as_mtz_dataset('Isum').mtz_object().write(file_name=icalc_file)

# write diffuse

    print("Diffuse:")
    count=0
    for hkl,intensity in diffuse_array:
      print("%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,))))
      count+=1
      if count>10: break
    if(diffuse_file.endswith(".mtz")):
      if not partial_sum_mode:
        diffuse_array.as_mtz_dataset('ID').mtz_object().write(file_name=diffuse_file)
      else:
        diffuse_array.as_mtz_dataset('IDpart').mtz_object().write(file_name=diffuse_file)
    else:
      f=open(diffuse_file,'w')
      for hkl,intensity in diffuse_array:
        print("%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,))),file=f)
      f.close()

#Perform optimization      
  if do_opt:
    if mpi_rank == 0:
      print("Doing optimization using ",ct," initial frames")
    diffuse_expt_np = np.array(diffuse_expt_common.data())
    C_ref = np.corrcoef(diffuse_expt_np,ct*tot_sig_icalc_np - (tot_sig_fcalc_np * tot_sig_fcalc_np.conjugate()).real)[0,1]
    #Initialize correlations array
    w = np.ones(ct)
    ct_nonzero = ct
    first_this = (skiplist[mpi_rank]-first)
    last_this = first_this + chunklist[mpi_rank]*nchunklist[mpi_rank]-1
    #Get the slice for the section handled by this rank
    C_all_this = np.zeros(ct)
    C_this = C_all_this[first_this:last_this+1]
    w_this = w[first_this:last_this+1]
    keep_optimizing = True
    while keep_optimizing:
      C_this[:] = 0
    #print("Rank = ",mpi_rank,"ct = ",ct,"len(C_this) = ",len(C_this),skiplist[mpi_rank]-first,chunklist[mpi_rank]*nchunklist[mpi_rank])
    #Calculation the correlations leaving out each frame
      ct_nonzero = ct_nonzero - 1
      for x in range(len(C_this)):
        if w_this[x] != 0:
          sig_fcalc_this = tot_sig_fcalc_np - fcalc_list[x]
          sig_icalc_this = tot_sig_icalc_np - icalc_list[x]
          diffuse_this = (ct_nonzero*sig_icalc_this - sig_fcalc_this * sig_fcalc_this.conjugate()).real
          C_this[x] = np.corrcoef(np.array([diffuse_expt_np,diffuse_this]))[0,1]
      C_all = np.zeros(ct)
      mpi_comm.Allreduce(C_all_this,C_all,op=MPI.SUM)
      if np.max(C_all) > C_ref:
        C_ref = np.max(C_all)
        maxind = np.argmax(C_all)
#        print("DEBUG: ",mpi_rank,maxind,first_this,last_this)
        if maxind >= first_this and maxind <= last_this:
          which_rank = mpi_rank
        else:
          which_rank = 0
        which_rank = mpi_comm.allreduce(which_rank,MPI.SUM)
#        print("which_rank = ",which_rank)
        if which_rank == mpi_rank:          
          tot_sig_fcalc_np = tot_sig_fcalc_np - fcalc_list[maxind - first_this]
          tot_sig_icalc_np = tot_sig_icalc_np - icalc_list[maxind - first_this]
        else:
          tot_sig_fcalc_np = None
          tot_sig_icalc_np = None
        tot_sig_fcalc_np = mpi_comm.bcast(tot_sig_fcalc_np,root=which_rank)
        tot_sig_icalc_np = mpi_comm.bcast(tot_sig_icalc_np,root=which_rank)
        w[maxind] = 0
        if mpi_rank == 0:
          print("Max correlation, index = ",C_ref,maxind)
      else:
        keep_optimizing = False
    if (mpi_rank == 0):
      print("Total ",ct_nonzero+1," frames remaining (see selected_frames.ndx)")
      keep_idx = np.where(w!=0)
      print(keep_idx[0])
      with open("selected_frames.ndx","w") as f:
        f.write("[ selected frames ]\n")
        for x in range(len(keep_idx[0])):
          f.write("{0}\n".format(keep_idx[0][x]+1))
      # print("Diffuse Expt:")
      # count=0
      # for hkl,intensity in diffuse_expt_common:
      #   print("%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,))))
      #   count+=1
      #   if count>10: break
      # print("Diffuse Array:")
      # count=0
      # for hkl,intensity in diffuse_array_common:
      #   print("%4d %4d %4d   %10.2f" %(hkl+tuple((intensity,))))
      #   count+=1
      #   if count>10: break
