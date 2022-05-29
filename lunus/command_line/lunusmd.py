#!/usr/bin/env python
# lunusmd Prototype MD code
# Author: Michael E. Wall, Los Alamos National Laboratory
#
# LIBTBX_SET_DISPATCHER_NAME lunus.md

import argparse
import os.path
from os import path
import sys
import mdtraj as md
import numpy as np
import io
import torch as tc
from lunus.md.energy import *
from lunus.md.maxwell import *
from lunus.md.integrate import *
from lunus.md.parse import *
import lunus.md.units as u

def xyz_to_mdtraj(xyz,cell):
    xyzmd = np.zeros((xyz.shape[1],3))
    xyzmd[:,0] = (xyz[0,:] % cell[0]) * 0.1
    xyzmd[:,1] = (xyz[1,:] % cell[1]) * 0.1
    xyzmd[:,2] = (xyz[2,:] % cell[2]) * 0.1

    return xyzmd


if __name__=="__main__":

# Parse command line arguments

    parser = argparse.ArgumentParser(description='Test MD with charge equilibration for free')

    parser.add_argument("--topin",help="Input topology file (.pdb with coords)",required=True)

    parser.add_argument("--topout",help="Output topology file (.pdb with coords)",required=False)

    parser.add_argument("--mdsteps",help="Number of MD time steps",required=False,type=int,default=0)

    parser.add_argument("--minsteps",help="Number of minimization steps",required=False,type=int,default=0)

    parser.add_argument("--trajin",help="Input trajectory file",required=False)

    parser.add_argument("--trajout",help="Output trajectory file",required=False,default="trajout.pdb")

    parser.add_argument("--log",help="Log file",required=False)

    parser.add_argument("--params",help="Parameters file (output from Tinker analyze)",required=True)

    parser.add_argument("--debug",help="Debug output",required=False,action="store_true")

    parser.add_argument("--outfreq",help="Trajectory output frequency",required=False,default=10,type=int)

    parser.add_argument("--dt",help="Time step",required=False,default=0.001,type=np.float64)
    
    parser.add_argument("--temperature",help="Temperature (K)",required=False,default=300.,type=np.float64)

    parser.add_argument("--alpha",help="Ewald summation alpha",required=False,default=None,type=np.float64)

    parser.add_argument("--elec_method",help="Electrostatics method (dumb, ewald)",required=False,default="ewald")

    parser.add_argument("--enable-cuda",help="Enable CUDA",required=False,action="store_true")
    
    parser.add_argument("--disable-vdw",help="Disable VDW potential",required=False,action="store_true")

    parser.add_argument("--disable-elec",help="Disable ELEC potential",required=False,action="store_true")

    parser.add_argument("--disable-bond",help="Disable BOND potential",required=False,action="store_true")

    parser.add_argument("--disable-angle",help="Disable ANGLE potential",required=False,action="store_true")

    parser.add_argument("--check-elec",help="Check consistency of electrostatics using finite differences",required=False,action="store_true")

    args=parser.parse_args()
    if args.enable_cuda and tc.cuda.is_available():
        print("Using GPU")
        args.device = tc.device('cuda')
    else:
        print("Using CPU")
        args.device = tc.device('cpu')

# Read structure

    t = md.load(args.topin)

# Read the parameters file

    with open(args.params,"r") as f:
        plines = f.readlines()

    atom_table = get_atom_table(plines)
    bond_table = get_bond_table(plines)
    angle_table = get_angle_table(plines)
    urey_bradley_table = get_urey_bradley_table(plines)
    vdw_table = get_vdw_table(plines)
    multipole_table = get_multipole_table(plines)


# Get the masses

    m = np.array(atom_table['Mass'])

    natoms = len(m)

# Get the charges

    q = np.array(multipole_table['Charge'])
    print("Total charge = ",np.sum(q))
    qq = np.zeros((natoms,natoms))
    for i in range(q.shape[0]):
        qq[i,:] = q[i]*q[:]

# Get the VDW parameters

    vdwatom = np.array([vdw_table['Size'],vdw_table['Epsilon'],vdw_table['Reduction']])
    vdwatom[2][np.isnan(vdwatom[2])] = 1.
    vdw = np.zeros((2,natoms,natoms))
    for i in range(vdwatom.shape[1]):
        vdw[0,i,:] = 0.5 * (vdwatom[0,i] + vdwatom[0,:])
        vdw[1,i,:] = np.sqrt(vdwatom[1,i] * vdwatom[1,:])

# Get the Bond parameters

    bond = [np.array(bond_table['Atom1']-1),
            np.array(bond_table['Atom2']-1),
            np.array(bond_table['KS']),
            np.array(bond_table['Bond'])]

# Get the Angle parameters

    angle = np.array([np.array(angle_table['Atom1']-1),
             np.array(angle_table['Atom2']-1),
             np.array(angle_table['Atom3']-1),
             np.array(angle_table['KB']),
             np.array(angle_table['Angle'])*np.pi/180.])

    if args.debug:
        print(atom_table)
        print(bond_table)
        print(angle_table)
        print(urey_bradley_table)
        print(vdw_table)
        print(multipole_table)
        print(m)
        print(q)
        print(vdw)
        print(bond)
        print(angle)

# Get the initial coordinate arrays and unit cell (multiply by 10 to get Angstroms)

    cell = (t.unitcell_lengths[0] * 10.).astype(np.float64)

    x = (t.xyz[0,:,0] % cell[0]) * 10.
    y = (t.xyz[0,:,1] % cell[1]) * 10.
    z = (t.xyz[0,:,2] % cell[2]) * 10.

    xyz = np.array([x,y,z],dtype=np.float64)

# Define the first order connection matrix

    atom1 = bond[0]
    atom2 = bond[1]

    C1 = np.zeros((natoms,natoms),dtype=bool)

    C1[atom1[:],atom2[:]] = True
    C1[atom2[:],atom1[:]] = True

    np.fill_diagonal(C1,True)

# Compute the second order connection matrix

    C2 = np.einsum('ij,jk',C1,C1)

# Initialize output trajectory

    txyz = np.zeros((1,natoms,3))
    xyzmd = np.zeros((natoms,3))

    xyzmd = xyz_to_mdtraj(xyz,cell)

    txyz[0] = xyzmd

# Set up device copies of arrays

    xyz_d = tc.tensor(xyz,device=args.device)
    vdw_d = tc.tensor(vdw,device=args.device)
    q_d = tc.tensor(q,device=args.device)
    qq_d = tc.tensor(qq,device=args.device)
    bond_d = tc.tensor(np.array(bond),device=args.device)
    angle_d = tc.tensor(angle,device=args.device)
    cell_d = tc.tensor(cell,device=args.device)
    m_d = tc.tensor(m,device=args.device)
    C2_d = tc.tensor(C2,device=args.device)

#    get_nonbond_terms.initialize = True

# Check electrostatic consistency using finite differences

    if args.check_elec:
        Evdw,Fvdw,Eelecref,Felecref = get_nonbond_terms(vdw_d,q_d,qq_d,C2_d,xyz_d,cell_d,elec_method=args.elec_method,alpha=args.alpha)
        dc = 0.00001
        xyz_d[0][0] += dc
        Evdw,Fvdw,Eelec0,Felec = get_nonbond_terms(vdw_d,q_d,qq_d,C2_d,xyz_d,cell_d,elec_method=args.elec_method,alpha=args.alpha)
        xyz_d[0][0] -= dc
        xyz_d[1][0] += dc
        Evdw,Fvdw,Eelec1,Felec = get_nonbond_terms(vdw_d,q_d,qq_d,C2_d,xyz_d,cell_d,elec_method=args.elec_method,alpha=args.alpha)
        xyz_d[1][0] -= dc
        xyz_d[2][0] += dc
        Evdw,Fvdw,Eelec2,Felec = get_nonbond_terms(vdw_d,q_d,qq_d,C2_d,xyz_d,cell_d,elec_method=args.elec_method,alpha=args.alpha)
        xyz_d[2][0] -= dc
        Fx = -u.ekcal*(Eelec0-Eelecref)/dc
        Fy = -u.ekcal*(Eelec1-Eelecref)/dc
        Fz = -u.ekcal*(Eelec2-Eelecref)/dc
        print([Felecref[0,0].item(),Felecref[1,0].item(),Felecref[2,0].item()],[Fx.item(),Fy.item(),Fz.item()])
        exit(0)

    # Evdw,Fvdw,Eelec,Felec,Eelec_r,Eelec_k = get_nonbond_terms(vdw_d,q_d,qq_d,C2_d,xyz_d,cell_d,elec_method=args.elec_method,alpha=args.alpha)
    # print("Before shift: Eelec_r = %0.2f, Eelec_k = %0.2f, Eelec = %0.2f" % (Eelec_r,Eelec_k,Eelec))
    # xyz_d[0][0] += cell_d[0]
    # Evdw,Fvdw,Eelec,Felec,Eelec_r,Eelec_k = get_nonbond_terms(vdw_d,q_d,qq_d,C2_d,xyz_d,cell_d,elec_method=args.elec_method,alpha=args.alpha)
    # print("After shift: Eelec_r = %0.2f, Eelec_k = %0.2f, Eelec = %0.2f" % (Eelec_r,Eelec_k,Eelec))
    # exit(0)

# Perform minimization

    if args.minsteps > 0:
        print("Energy minimize for ",args.minsteps,"steps")
        v_d = tc.zeros([3,natoms],dtype=tc.float64,device=args.device)
        ct = 0
        outct = 0
        for xyz_step,v_step in vverlet(xyz_d,cell_d,v_d,m_d,
                                       bond=bond_d,angle=angle_d,
                                       vdw=vdw_d,q=q_d,qq=qq_d,
                                       C2=C2_d,nsteps=args.minsteps,
                                       minimize=True,outfreq=10,
                                       elec_method=args.elec_method,
                                       alpha=args.alpha,
                                       disable_bond=args.disable_bond,
                                       disable_angle=args.disable_angle,
                                       disable_vdw=args.disable_vdw,
                                       disable_elec=args.disable_elec):
            ct = ct + 1

        xyz_d = xyz_step

# Ramp up temperature for MD
            
    if args.mdsteps > 0:
        print("Ramping up temperature to ",args.temperature," in 5 stages")

        for i in range(5):
            T_stage = args.temperature/5.*(i+1)

# Get the initial velocities

            print("T = ",T_stage," stage")
            v = get_random_velocities(m,T_stage)
            v_d = tc.tensor(v,device=args.device)
            ct = 0
            for xyz_step,v_step in vverlet(xyz_d,cell_d,v_d,m_d,
                                           dt=args.dt,bond=bond_d,
                                           angle=angle_d,vdw=vdw_d,
                                           q=q_d,qq=qq_d,C2=C2_d,
                                           nsteps=100,minimize=False,
                                           outfreq=10,
                                           alpha=args.alpha,
                                           elec_method=args.elec_method,
                                           disable_bond=args.disable_bond,
                                           disable_angle=args.disable_angle,
                                           disable_vdw=args.disable_vdw,
                                           disable_elec=args.disable_elec):
                ct = ct + 1

            xyz_d = xyz_step

# Production MD at final temperature

        print("Production MD at T = ",args.temperature)

# Get the initial velocities

        v = get_random_velocities(m,args.temperature)
        v_d = tc.tensor(v,device=args.device)
        ct = 0
        outct = 0
        for xyz_step,v_step in vverlet(xyz_d,cell_d,v_d,m_d,
                                       dt=args.dt,bond=bond_d,
                                       angle=angle_d,vdw=vdw_d,
                                       q=q_d,qq=qq_d,C2=C2_d,
                                       nsteps=args.mdsteps,minimize=False,
                                       outfreq=args.outfreq,
                                       alpha=args.alpha,
                                       elec_method=args.elec_method,
                                       disable_bond=args.disable_bond,
                                       disable_angle=args.disable_angle,
                                       disable_vdw=args.disable_vdw,
                                       disable_elec=args.disable_elec):
            ct = ct + 1
            xyzmd = xyz_to_mdtraj(xyz_step.cpu().numpy(),cell)
            txyz = np.append(txyz,[xyzmd],axis=0)
        xyz_d = xyz_step
        v_d = v_step

    tout = md.Trajectory(txyz,t.topology)

    tout.save(args.trajout)
