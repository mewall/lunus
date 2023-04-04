import argparse
import numpy as np
from iotbx.pdb import hierarchy
from cctbx.array_family import flex

parser = argparse.ArgumentParser(description='Calculate atom pair distances in a .pdb file')
parser.add_argument("--pdb",help="Input .pdb file",required=True)
parser.add_argument("--out",help="Output .npy distance matrix file")
args=parser.parse_args()

# Read structure file
pdb_in = hierarchy.input(file_name=args.pdb)

# Get structure
struct = pdb_in.input.xray_structure_simple()

# Get unit cell and compute fractional-to-cartesian transformation matrix
a,b,c,alpha,beta,gamma=flex.double(struct.unit_cell().parameters())
alpha *= np.pi/180.
beta *= np.pi/180.
gamma *= np.pi/180.

# From https://chemistry.stackexchange.com/questions/136836/converting-fractional-coordinates-into-cartesian-coordinates-for-crystallography
# Needs to be confirmed!
n2 = (np.cos(alpha)-np.cos(gamma)*np.cos(beta))/np.sin(gamma)
M  = np.array([[a,0,0],[b*np.cos(gamma),b*np.sin(gamma),0], 
     [c*np.cos(beta),c*n2,c*np.sqrt(np.sin(beta)**2-n2**2)]])

# Compute distances accounting for periodic boundary conditions
x=struct.sites_frac().as_numpy_array()
N = len(x)
S = np.zeros((N,N))
dc = np.zeros((N,3))
for i in range(len(x)):
  svec2 = np.zeros(N)
  for k in range(3):
    dc[:,k] = (x[i,k] - x[:,k] + 0.5)%1.0 - 0.5 
  dc = np.einsum('ij,...i->...j',M,dc)
  svec2[:] += np.einsum('...i,...i->...',dc[:],dc[:])
  svec = np.sqrt(svec2)
  S[i,:] = svec
np.save(args.out,S)
