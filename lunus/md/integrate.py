# lunusmd Prototype MD code
# Author: Michael E. Wall, Los Alamos National Laboratory
import lunus.md.units as u
from lunus.md.energy import *
import time

def vverlet(xyz,cell,v,m,dt=None,bond=None,angle=None,vdw=None,q=None,qq=None,C2=None,nsteps=0,minimize=False,outfreq=None,elec_method=None,alpha=None,disable_bond=False,disable_angle=False,disable_vdw=False,disable_elec=False):

    natoms = xyz.shape[1]

    t_nonb = 0.0
    t_geom = 0.0
    tct = 0

    Evdw,Fvdw,Eelec,Felec = get_nonbond_terms(vdw,q,qq,C2,xyz,cell,elec_method=elec_method,alpha=alpha,
                                              disable_vdw=disable_vdw,disable_elec=disable_elec)
    Ebond,Fbond = get_bond_terms(bond,xyz,cell,disable=disable_bond)
    Eangle,Fangle = get_angle_terms(angle,xyz,cell,disable=disable_angle)

    U = Eelec + Evdw + Ebond + Eangle
    F = Felec + Fvdw + Fbond + Fangle

    if outfreq is not None:
        print("Starting U = ",U.item(),"Eelec = ",Eelec.item(),"Evdw = ",Evdw.item(),"Ebond = ",Ebond.item(),"Eangle = ",Eangle.item())
 
    if (minimize):
        v.fill_(0)
        if dt is None:
            dt = 0.001
    else:
        if dt is None:
            dt = 0.001

        print("Dynamics for ",nsteps,"steps with time step ",dt,"ps")

    for i in range(nsteps):

        v = v + 0.5 * F/m * dt
        xyz = xyz + v * dt

        for k in range(3):
            xyz[k] = xyz[k] % cell[k]

        tic = time.perf_counter()
        Evdw,Fvdw,Eelec,Felec = get_nonbond_terms(vdw,q,qq,C2,xyz,cell,elec_method=elec_method,alpha=alpha,
                                                  disable_vdw=disable_vdw,disable_elec=disable_elec)
        t_nonb += time.perf_counter()-tic

        tic = time.perf_counter()
        Ebond,Fbond = get_bond_terms(bond,xyz,cell,disable=disable_bond)
        Eangle,Fangle = get_angle_terms(angle,xyz,cell,disable=disable_angle)
        t_geom += time.perf_counter()-tic

        tct += 1
            
        F = Felec + Fvdw + Fbond + Fangle

        if minimize:
            v.fill_(0)
        else:
            v = v + 0.5 * F/m * dt

        if outfreq is None:
            outfreq = 1

        if i % outfreq == 0:
            U = Eelec + Evdw + Ebond + Eangle
            K = 0.5*tc.sum(tc.einsum("i...,i...",v,v)*m)/u.ekcal
            T_this = 2. / 3. * K * u.ekcal / u.boltzmann / len(m)
            E = U + K
            p = tc.einsum("i,...i",m,v)
            pmag = tc.sqrt(tc.dot(p,p))
            t_nonb /= tct
            t_geom /= tct

            print("Step = %d" % i,"t_nonb = %0.3f" % t_nonb,"t_geom = %0.3f" % t_geom,
                  "U = %0.2f" % U.item(),"Eelec = %0.2f" % Eelec.item(),
                  "Evdw = %0.2f" % Evdw.item(),"Ebond = %0.2f" % Ebond.item(),
                  "Eangle = %0.2f" % Eangle.item(),"|p| = %0.2e" % pmag.item(),
                  "K = %0.2f" % K.item(),"T = %0.2f" % T_this.item(),"E = %0.6f" % E.item(),flush=True)

            t_nonb = 0.0
            t_geom = 0.0
            tct = 0

            yield xyz,v

