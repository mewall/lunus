from time import clock, time
import numpy as np
import re
from multiprocessing import Pool
import glob
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

 # number of processors available for pool
  try:
    nprocidx = [a.find("np")==0 for a in args].index(True)
  except ValueError:
    nproc = 1
  else:
    nproc = int(args.pop(nprocidx).split("=")[1])
  # unit cell a
  try:
    cellaidx = [a.find("cell.a")==0 for a in args].index(True)
  except ValueError:
    raise ValueError("Lattice constant cell.a must be specified")
  else:
    cella = float(args.pop(cellaidx).split("=")[1])
  # unit cell b
  try:
    cellbidx = [a.find("cell.b")==0 for a in args].index(True)
  except ValueError:
    raise ValueError("Lattice constant cell.b must be specified")
  else:
    cellb = float(args.pop(cellbidx).split("=")[1])
  # unit cell c
  try:
    cellcidx = [a.find("cell.c")==0 for a in args].index(True)
  except ValueError:
    raise ValueError("Lattice constant cell.c must be specified")
  else:
    cellc = float(args.pop(cellcidx).split("=")[1])
  # target cell for indexing
  try:
      targetcellidx = [a.find("target_cell")==0 for a in args].index(True)
  except ValueError:
      raise ValueError("Target cell target_cell must be specified")
  else:
      target_cell = args.pop(targetcellidx).split("=")[1]
  # spacegroup for indexing
  try:
      targetsgidx = [a.find("target_sg")==0 for a in args].index(True)
  except ValueError:
      raise ValueError("Target space group target_sg must be specified")
  else:
      target_sg = args.pop(targetsgidx).split("=")[1]
  # maximum resolution of diffuse lattice
  try:
    residx = [a.find("diffuse.lattice.resolution")==0 for a in args].index(True)
  except ValueError:
    print ("diffuse.lattice.resolution not specified, looking for explicit lattice dimensions")
    residx = -1
  else:
    res = float(args.pop(residx).split("=")[1])
    latxdim = (int(cella/res)+1)*2
    latydim = (int(cellb/res)+1)*2
    latzdim = (int(cellc/res)+1)*2
  # input file with list of diffuse images and scale factors
  try:
    ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
  except ValueError:
    ifname = "genlat.input"
  else:
    ifname = args.pop(ifnameidx).split("=")[1]
  # frame number
  try:
    framenumidx = [a.find("framenum")==0 for a in args].index(True)
  except ValueError:
    framenum=-1
  else:
    framenum=int(args.pop(framenumidx).split("=")[1])
    print "integrating frame ",framenum
  # output diffuse lattice file
  try:
    ofnameidx = [a.find("output.fname")==0 for a in args].index(True)
  except ValueError:
    ofname = "diffuse.vtk"
  else:
    ofname = args.pop(ofnameidx).split("=")[1]
  # input diffuse lattice glob
  try:
    latglobidx = [a.find("diffuse.lattice.glob")==0 for a in args].index(True)
  except ValueError:
    latglob = "diffuse_*.npz"
  else:
    latglob = args.pop(latglobidx).split("=")[1]
  # output diffuse lattice glob
  try:
    ctglobidx = [a.find("counts.lattice.glob")==0 for a in args].index(True)
  except ValueError:
    ctglob = "counts_*.vtk"
  else:
    ctglob = args.pop(ctglobidx).split("=")[1]
  # type of lattice output. "sum" = sum, "mean" = mean
  try:
    lattypeidx = [a.find("diffuse.lattice.type")==0 for a in args].index(True)
  except ValueError:
    lattype = "mean"
  else:
    lattype = args.pop(lattypeidx).split("=")[1]
    if not((lattype == "sum") or (lattype == "mean") or (lattype == "npz")):
      raise Exception("Lattice type must be ""sum"" or ""mean"" or ""npz""")
  # diffuse lattice points per hkl value
  try:
    pphklidx = [a.find("pphkl")==0 for a in args].index(True)
  except ValueError:
    pphkl = 1
  else:
    pphkl = int(args.pop(pphklidx).split("=")[1])
  # size of diffuse lattice in x direction
  try:
    latxdimidx = [a.find("latxdim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latxdim = -1
  else:
    latxdim = int(args.pop(latxdimidx).split("=")[1])
  # size of diffuse lattice in y direction
  try:
    latydimidx = [a.find("latydim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latydim = -1
  else:
    latydim = int(args.pop(latydimidx).split("=")[1])
  # size of diffuse lattice in z direction
  try:
    latzdimidx = [a.find("latzdim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latzdim = -1
  else:
    latzdim = int(args.pop(latzdimidx).split("=")[1])
  # specify path to proc directory
  try:
    procpathidx = [a.find("path.to.proc")==0 for a in args].index(True)
  except ValueError:
    procpath=""
  else:
    procpath = args.pop(procpathidx).split("=")[1]
  # read indexing info from a file instead of calculating (not implemented)
  try:
    readindexidx = [a.find("readindex")==0 for a in args].index(True)
  except ValueError:
    readindex = False
  else:
    readindex = bool(args.pop(readindexidx).split("=")[1])

  if (residx == -1) and ((latxdim == -1) or (latydim == -1) or (latzdim == -1)):
    raise Exception("Must provide either diffuse.lattice.resolution or latxdim, latydim, and latzdim.")

  import os

# lattice dimensions
  latxdim = (int(pphkl*cella/res)+1)*2
  latydim = (int(pphkl*cellb/res)+1)*2
  latzdim = (int(pphkl*cellc/res)+1)*2

  latsize = latxdim*latydim*latzdim
  print "Lattice size = ",latsize
  lat = np.zeros(latsize, dtype=np.float32)
  ct = np.zeros(latsize, dtype=np.float32)

  latlist = glob.glob(latglob)
  ctlist = glob.glob(ctglob)

  mask_tag = 32767

  for i in range(len(latlist)):
      thislat=np.load(latlist[i])['lat']
      select_indices = np.where(thislat != mask_tag)
      lat[select_indices] += thislat[select_indices]
      ct += np.load(ctlist[i])['ct']

  for index in range(0,latsize):
      if ((ct[index] > 0) and (lat[index] != mask_tag)):
          lat[index] /= ct[index]
      else:
          lat[index] = -32768

  i0=latxdim/2-1
  j0=latydim/2-1
  k0=latzdim/2-1

  vtkfile = open(ofname,"w")
  
  a_recip = 1./cella/float(pphkl)
  b_recip = 1./cellb/float(pphkl)
  c_recip = 1./cellc/float(pphkl)
  
  print >>vtkfile,"# vtk DataFile Version 2.0"
  print >>vtkfile,"lattice_type=PR;unit_cell={0};space_group={1};".format(target_cell,target_sg)
  print >>vtkfile,"ASCII"
  print >>vtkfile,"DATASET STRUCTURED_POINTS"
  print >>vtkfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
  print >>vtkfile,"SPACING %f %f %f"%(a_recip,b_recip,c_recip)
  print >>vtkfile,"ORIGIN %f %f %f"%(-i0*a_recip,-j0*b_recip,-k0*c_recip)
  print >>vtkfile,"POINT_DATA %d"%(latsize)
  print >>vtkfile,"SCALARS volume_scalars float 1"
  print >>vtkfile,"LOOKUP_TABLE default\n"
  
  index = 0
  for k in range(0,latzdim):
      for j in range(0,latydim):
          for i in range(0,latxdim):
              print >>vtkfile,lat[index],
              index += 1
          print >>vtkfile,""

  vtkfile.close()

