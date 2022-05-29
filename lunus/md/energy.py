# lunusmd Prototype MD code
# Author: Michael E. Wall, Los Alamos National Laboratory
import torch as tc
import numpy as np
import lunus.md.units as u
import time

def get_angle_terms(angle,xyz,cell,disable=False):

    natoms = xyz.shape[1]
    device = xyz.device
    if disable:
        F = tc.zeros([3,natoms],dtype=tc.float64,device=device)
        E = tc.tensor(0.0,dtype=tc.float64,device=device)
    else:
        atom1 = angle[0]
        atom2 = angle[1]
        atom3 = angle[2]
        KA = angle[3]
        theta_ref = angle[4]

        F = tc.zeros([3,xyz.shape[1]],dtype=tc.float64,device=device)
        Avec = tc.zeros([3,atom1.shape[0]],dtype=tc.float64,device=device)
        Bvec = tc.zeros([3,atom3.shape[0]],dtype=tc.float64,device=device)

        for k in range(3):
            Avec[k] = (xyz[k,atom1[:].long()] - xyz[k,atom2[:].long()] + cell[k]/2.) % cell[k] - cell[k]/2.

        for k in range(3):
            Bvec[k] = (xyz[k,atom3[:].long()] - xyz[k,atom2[:].long()] + cell[k]/2.) % cell[k] - cell[k]/2.

        A2 = tc.einsum('i...,i...',Avec,Avec)
        B2 = tc.einsum('i...,i...',Bvec,Bvec)

        A = tc.sqrt(A2)
        B = tc.sqrt(B2)

        Ahat = Avec/A
        Bhat = Bvec/B

        AxB = tc.cross(Ahat,Bhat,dim=0)
        thetahat = AxB/tc.sqrt(tc.einsum('i...,i...',AxB,AxB))
        theta = tc.arccos(tc.einsum('i...,i...',Ahat,Bhat))
        dtheta = theta - theta_ref

        E = tc.sum(KA*dtheta*dtheta * (1. - 0.014*dtheta + 0.000056*dtheta*dtheta - 0.0000007 * dtheta * dtheta * dtheta + 0.000000022*dtheta*dtheta*dtheta*dtheta))

        torque = -KA*dtheta*thetahat * (2. - 3.*0.014*dtheta + 4.*0.000056*dtheta*dtheta - 5.*0.0000007 * dtheta * dtheta * dtheta + 6.*0.000000022*dtheta*dtheta*dtheta*dtheta)

        dF0 = -tc.cross(torque,Ahat,dim=0)
        dF2 = tc.cross(torque,Bhat,dim=0)

        dF0 /= A
        dF2 /= B
        
        for k in range(3):
            F[k].index_add_(0,atom1.long(),dF0[k])
            F[k].index_add_(0,atom3.long(),dF2[k])
            F[k].index_add_(0,atom2.long(),-dF2[k]-dF0[k])
            
    return E,F*u.ekcal

def get_bond_terms(bond,xyz,cell,disable=False):

    natoms = xyz.shape[1]
    device = xyz.device
    if disable:
        F = tc.zeros([3,natoms],dtype=tc.float64,device=device)
        E = tc.tensor(0.0,dtype=tc.float64,device=device)
    else:
        atom1 = bond[0]
        atom2 = bond[1]
        KB = bond[2]
        R_ref = bond[3]

        F = tc.zeros([3,xyz.shape[1]],dtype=tc.float64,device=device)
        Rvec = tc.zeros([3,atom1.shape[0]],dtype=tc.float64,device=device)

        for k in range(3):
            Rvec[k] = (xyz[k,atom1[:].long()] - xyz[k,atom2[:].long()] + cell[k]/2.) % cell[k] - cell[k]/2.

        R2 = tc.einsum('i...,i...', Rvec, Rvec)
        R = tc.sqrt(R2)
        dR = R - R_ref

        E = tc.sum(KB*dR*dR*(1.-2.55*dR+3.793125*dR*dR))

        Fvec = tc.zeros([3,atom1.shape[0]],dtype=tc.float64,device=device)

        for k in range(3):
            Fvec[k] = - KB * dR * Rvec[k]/R * (2. - 3.*2.55*dR + 4.*3.793125*dR*dR)

        for k in range(3):
            F[k].index_add_(0,atom1.long(),Fvec[k])
            F[k].index_add_(0,atom2.long(),-Fvec[k])

    return E,F*u.ekcal

def get_nonbond_terms(vdw,q,qq,C2,xyz,cell,elec_method="dumb",alpha=None,disable_vdw=False,disable_elec=False):

    ignore_recip=False

    tic0 = time.perf_counter()
    device = xyz.device

    if alpha is None:
        alpha = 0.5

    sigma = vdw[0]
    epsilon = vdw[1]

# Compute the distance matrices

    natoms = xyz.shape[1]

    notI = tc.logical_not(tc.eye(natoms,dtype=tc.bool,device=device))

    notC2 = tc.logical_not(C2)

    Rvec = tc.zeros([3,natoms,natoms],dtype=tc.float64,device=device)

    tic = time.perf_counter()
    for k in range(3):
        Rvec[k] = xyz[k].unsqueeze(1).repeat(1,natoms)
    Rvec = tc.transpose(Rvec,1,2) - Rvec

    for k in range(3):
        Rvec[k] = tc.remainder(Rvec[k] + cell[k]/2.,cell[k]) - cell[k]/2.
    toc = time.perf_counter()

#    print("Rvec calc time = ",toc-tic)

    tic = time.perf_counter()
    R2 = tc.einsum('i...,i...', Rvec, Rvec)
    toc = time.perf_counter()
#    print("R2 calc time = ",toc-tic)

    R2.fill_diagonal_(1.)
    one_over_R2 = 1./R2

    R = tc.sqrt(R2)
    one_over_R = 1./R

    tic = time.perf_counter()
    if disable_vdw:
        Fvdw = tc.zeros([3,natoms],dtype=tc.float64,device=device)
        Evdw = tc.tensor(0.0,dtype=tc.float64,device=device)

    else:
        R2term = sigma * sigma * one_over_R2
        R4term = R2term * R2term
        R6term = R4term * R2term
        R12term = R6term * R6term

        Evdw = tc.sum(0.5*epsilon * notC2 * (R12term - 2. * R6term))

        Fvdw = - 12.*u.ekcal * tc.sum(epsilon * notC2 * (R12term - R6term) * Rvec * one_over_R2,dim=2)

        toc = time.perf_counter()
#        print("VDW calc time = ",toc-tic)

    if disable_elec:
        Felec = tc.zeros([3,natoms],dtype=tc.float64,device=device)
        Eelec = tc.tensor(0.0,dtype=tc.float64,device=device)

    else:

        qq_over_R = qq * one_over_R

        if elec_method is None:

            Eelec = 0.0
            Felec = tc.zeros([3,natoms],dtype=tc.float64,device=device)

        elif elec_method == "dumb":

            Eelec = 0.5 * u.coulomb * tc.sum(qq_over_R * notC2)

            Felec = - u.coulomb * u.ekcal * tc.sum(qq_over_R * notC2
                                                   * one_over_R2 * Rvec,dim=2)

        elif elec_method == "ewald":

# Real space Ewald sum term (include 1- and 2-bonded atoms now but subtract later): 

            tic = time.perf_counter()

            Eelec_r = 0.5 * tc.sum(qq_over_R * notI
                                   * (1. - tc.erf(alpha*R)))

            Felec_r = - tc.sum(qq_over_R * notI
                               * (1. - tc.erf(alpha*R) 
                                  + 2./np.sqrt(np.pi)*alpha*tc.exp(-alpha*alpha*R2)*R)
                               * one_over_R2 * Rvec,dim=2)

            toc = time.perf_counter()

#            print("Real space Ewald time = ",toc-tic)

            if not ignore_recip:

# Reciprocal space term setup

                four_alpha_squared = 4.*alpha*alpha
                V = cell[0] * cell[1] * cell[2]

# The k-space grid setup can be done just once for all iterations

                tic = time.perf_counter()
                if (not hasattr(get_nonbond_terms,"N")):
                    Ncpu = np.ceil(cell.cpu().numpy()/2.).astype(int)
                    kcpu = np.mgrid[0:Ncpu[0]+1,-Ncpu[1]:Ncpu[1]+1,-Ncpu[2]:Ncpu[2]+1].astype(np.float64)
                    get_nonbond_terms.N = tc.tensor(Ncpu,device=device)
                    get_nonbond_terms.k = tc.tensor(kcpu,device=device)
                    kmin = 2.*np.pi/cell
                    for i in range(3):
                        get_nonbond_terms.k[i] *= kmin[i]
                    get_nonbond_terms.ksquared = tc.einsum("i...,i...",get_nonbond_terms.k,get_nonbond_terms.k)
                    get_nonbond_terms.ksquared[0,get_nonbond_terms.N[1],get_nonbond_terms.N[2]] = 1.
                    get_nonbond_terms.kkernel = tc.exp(-get_nonbond_terms.ksquared
                                                   / four_alpha_squared) / get_nonbond_terms.ksquared

                N = get_nonbond_terms.N
                k = get_nonbond_terms.k
                ksquared = get_nonbond_terms.ksquared
                kkernel = get_nonbond_terms.kkernel

                toc = time.perf_counter()
#                print("Reciprocal space setup time = ",toc-tic)

# Structure factor computation

                tic = time.perf_counter()
                eiphi = tc.exp(tc.tensordot((1.+0.0j)*xyz,(0+1.j)*k,dims=([0],[0])))
                S = tc.tensordot((1.+0.0j)*q,eiphi,dims=([0],[0]))
                toc = time.perf_counter()
#                print("Structure factor time = ",toc-tic)

# Main term for reciprocal space contribution

                tic = time.perf_counter()
                norm = 2.*np.pi/V
                Eelec_k = tc.sum(kkernel * S * tc.conj(S))
                toc = time.perf_counter()
#                print("Main reciprocal energy time = ",toc-tic)

                Felec_k = tc.as_tensor(np.zeros([3,natoms],dtype=np.complex128),device=device)

                tic = time.perf_counter()
#                Felec_k = (0.+1.j)*2.*np.pi/V*tc.tensordot(k*kkernel*tc.conj(eiphi),S,dims=([2,3,4],[0,1,2]))*q
                for i in range(3):
                    Felec_k[i] = (q * 
                                  tc.tensordot(k[i]*kkernel*tc.conj(eiphi),S,dims=([1,2,3],[0,1,2])))

#                Felec_k += tc.conj(Felec_k)

                toc = time.perf_counter()
#                print("Main reciprocal force time = ",toc-tic)

# Multiply by 2 to include the -kx component

                Eelec_k *= 2.
                Felec_k *= 2.

# Subtract the surplus kx=0 contribution due to the multiply by 2

                Eelec_k -= tc.sum(kkernel[0] * S[0] * tc.conj(S[0]))

                for i in range(3):
                    kx0term = (q * 
                               tc.einsum("...kl,kl",
                                         k[i,0] * kkernel[0] 
                                         * tc.conj(eiphi[:,0] ),
                                         S[0]))
                    Felec_k[i] -= kx0term

# Subtract the k=0 contribution

                Eelec_k -= kkernel[0,N[1],N[2]] * S[0,N[1],N[2]] * tc.conj(S[0,N[1],N[2]])
                for i in range(3):
                    k0term = (q[:] * k[i,0,N[1],N[2]] * kkernel[0,N[1],N[2]] 
                              * tc.conj(eiphi[:,0,N[1],N[2]])
                              * S[0,N[1],N[2]])
                    Felec_k[i] -= k0term

# Multiply energy by the normalization factor

                Eelec_k *= norm

# Multiply force by normalization factor and add the complex conjugate

                Felec_k *= (0. + 1.j)*norm

                Felec_k += tc.conj(Felec_k)

# Subtract the self-interaction

                Eelec_k -= alpha / np.sqrt(np.pi) * tc.trace(qq)

# Subtract the contribution from atom pairs within 1 or 2 bonds:

                Eelec_k -= 0.5 * tc.sum(qq_over_R * C2 * notI)
        
                Felec_k -= - tc.sum(qq_over_R * C2 * notI
                                    * one_over_R2 * Rvec,dim=2)

            else:
                Felec_k = tc.as_tensor(np.zeros([3,natoms]),dtype=tc.complex128,device=device)
                Eelec_k = tc.tensor(0.0,dtype=tc.complex128,device=device)

# Convert to kcal/mol units for E, and integration units for F

            Eelec_r *= u.coulomb
            Felec_r *= u.coulomb * u.ekcal

            Eelec_k = u.coulomb * np.real(Eelec_k)
            Felec_k = u.coulomb * u.ekcal * np.real(Felec_k)

# Sum the real and reciprocal space terms

            Eelec = Eelec_r + Eelec_k
            Felec = Felec_r + Felec_k

            # Eelec = Eelec_k
            # Felec = Felec_k

#            print("Eelec_r = %0.2f, Eelec_k = %0.2f, Eelec = %0.2f" % (Eelec_r,Eelec_k,Eelec))

    toc0 = time.perf_counter()
#    print("Time within get_bonbond_terms() = ",toc0-tic0)

    return(Evdw,Fvdw,Eelec,Felec)

