# RigidBodyMotions.py script
# Used for covariance analysis in DW Wych, JS Fraser, DL Mobley, ME Wall. Liquid-like and 
#     rigid-body motions in molecular-dynamics simulations of a crystalline protein. 
#     LA-UR-19-30460
# Author: David Wych, UC Irvine and Los Alamos National Laboratory
# Date: August 2019

import mdtraj as md
import numpy as np
from scipy.stats import sem

class Model():
    """Rigid Body Motions model for protein c-alpha
    residue pair covariance as a function of distance"""
    def rotation(self):
        def eulerAnglesToRotationMatrix(eulers):
            """ function for converting vector of 3 euler angles
            to a rotation matrix"""
            R_x = np.array([[1,0,0],
                            [0,np.cos(eulers[0]),-np.sin(eulers[0])],
                            [0,np.sin(eulers[0]),np.cos(eulers[0])]
                            ])
            R_y = np.array([[np.cos(eulers[1]),0,np.sin(eulers[1])],
                            [0,1,0],
                            [-np.sin(eulers[1]),0,np.cos(eulers[1])]
                            ])
            R_z = np.array([[np.cos(eulers[2]),-np.sin(eulers[2]),0],
                            [np.sin(eulers[2]),np.cos(eulers[2]),0],
                            [0,0,1]
                            ])
            R = np.dot(R_z, np.dot( R_y, R_x ))
            return R

        # For every frame but the first...
        for t in range(len(self.traj)-1):
            # Generate a new vector of three euler angles and convert
            # it to a matrix
            _eulers = np.deg2rad(np.random.normal(0.0, self.rot_std, 3))
            R_mat = eulerAnglesToRotationMatrix(_eulers)
            # Apply that matrix to the coordiantes of every atom in each frame
            for n in range(self.traj.shape[1]):
                self.traj[t+1][n] = np.dot(R_mat, self.traj[t+1][n])

    def translation(self):
        # For every frame but the first...
        for t in range(len(self.traj)-1):
            # Generate a random 3-d translation vector
            _trans = np.random.normal(0, self.trans_std, 3)
            # Add that translation to each atom in the 
            self.traj[t+1] = np.add(self.traj[t+1], _trans)

    def calc_covarmat(self):
        X = self.traj[:,:,0]; Y = self.traj[:,:,1]; Z = self.traj[:,:,2]
        uX = np.mean(X, axis=0); uY = np.mean(Y, axis=0); uZ = np.mean(Z, axis=0)
        covXX = np.mean(np.array([np.outer(np.subtract(vec, uX), np.subtract(vec, uX)) for  vec in X]), axis=0)
        covYY = np.mean(np.array([np.outer(np.subtract(vec, uY), np.subtract(vec, uY)) for  vec in Y]), axis=0)
        covZZ = np.mean(np.array([np.outer(np.subtract(vec, uZ), np.subtract(vec, uZ)) for  vec in Z]), axis=0)
        covar = np.add(covXX, np.add(covYY, covZZ))
        self.covarmat = covar

    def calc_distances(self):
        N = self.prot.xyz.shape[1]
        xmat = np.tile(self.prot.xyz[0][:,0], N).reshape((N,N))
        ymat = np.tile(self.prot.xyz[0][:,1], N).reshape((N,N))
        zmat = np.tile(self.prot.xyz[0][:,2], N).reshape((N,N))

        xdiff = xmat.T - xmat
        ydiff = ymat.T - ymat
        zdiff = zmat.T - zmat

        total_diff = np.sqrt(np.power(xdiff, 2) + np.power(ydiff, 2) + np.power(zdiff, 2))

        self.distmat = 10.0*total_diff

    def collect_bins(self, n_bins=35):
        udi = np.triu_indices(self.covarmat.shape[0], 1)
        covar_scatter = self.covarmat[udi].flatten()
        dist_scatter = self.distmat[udi].flatten()
        # Sort the distance and covariance arrays
        dist_idx = np.argsort(dist_scatter)
        dist_sorted = dist_scatter[dist_idx]
        covar_sorted = covar_scatter[dist_idx]
        # Create array of bins width "bin_width"
        # with range "range - bin_width"
        #dist_bins = np.linspace(0, np.max(dist_scatter), num=n_bins)
        dist_bins = self.dist
        # Create array of bins for covariance with
        # the same number of bins as "dist_bins"
        covar_mean_bins = []
        covar_sem_bins = []
        for i in range(len(dist_bins)-1):
            _dist_last = dist_bins[i]
            _dist = dist_bins[i+1]
            _dist_idx_1 = np.where(dist_sorted >= _dist_last)
            _dist_idx_2 = np.where(dist_sorted <= _dist)
            _dist_idx = np.intersect1d(_dist_idx_1, _dist_idx_2)
            _dist_subarray = dist_sorted[_dist_idx]
            _covar_subarray = covar_sorted[_dist_idx]
            covar_mean_bins.append(np.mean(_covar_subarray))
            covar_sem_bins.append(sem(_covar_subarray))

        covar_mean_bins = np.array(covar_mean_bins)
        covar_sem_bins = np.array(covar_sem_bins)

        return dist_bins[1:], covar_mean_bins, covar_sem_bins

    def __init__(self, dist=None,  MODEL=None, N_FRAMES=None, ROT_STD=None, TRANS_STD=None):
        if type(dist) != np.ndarray:
            self.dist = np.array([3.30216069+(0.8112175*(i-1)) for i in range(50)])
        else:
            self.dist = dist
            self.dist = np.append([self.dist[0]-(self.dist[1] - self.dist[0])], list(self.dist))
        if MODEL == None:
            self.model = "rot+trans"
        else:
            self.model = MODEL
        if N_FRAMES == None:
            self.n_frames = 100
        else:
            self.n_frames = N_FRAMES
        if ROT_STD == None:
            self.rot_std = 1.25
        else:
            self.rot_std = ROT_STD
        if TRANS_STD == None:
            self.trans_std = 0.025
        else:
            self.trans_std = TRANS_STD


        sys = md.load("4wor_complete.pdb")
        top = sys.topology
        self.prot = sys.atom_slice(top.select("name 'CA' and (resid 6 to 141)"))
        self.prot.center_coordinates()
        self.coords = self.prot.xyz
        self.traj = np.array(list(self.coords)*self.n_frames)

    def run(self):
        if self.model == "rot":
            self.rotation()
            self.calc_covarmat()
            self.calc_distances()
            dist_bins_model, covar_means_model, covar_sems_model = self.collect_bins()
            return dist_bins_model, covar_means_model, covar_sems_model
        if self.model == "trans":
            self.translation()
            self.calc_covarmat()
            self.calc_distances()
            dist_bins_model, covar_means_model, covar_sems_model = self.collect_bins()
            return dist_bins_model, covar_means_model, covar_sems_model
        if self.model == "rot+trans":
            self.rotation()
            self.translation()
            self.calc_covarmat()
            self.calc_distances()
            dist_bins_model, covar_means_model, covar_sems_model = self.collect_bins()
            return dist_bins_model, covar_means_model, covar_sems_model
