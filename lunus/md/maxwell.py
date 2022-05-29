# lunusmd Prototype MD code
# Author: Michael E. Wall, Los Alamos National Laboratory
import lunus.md.units as u
import numpy as np
import scipy
import scipy.interpolate

def get_random_velocities(m,T):

    speeds = np.zeros_like(m,dtype=np.float64)

    natoms = len(m)

    randlist = np.random.random(natoms)

    inv_cdf = []
    for this_m in np.unique(m):
        vlist = np.arange(0,100./np.sqrt(this_m),0.002/np.sqrt(this_m))
        cdf = MB_CDF(vlist,this_m,T)
        inv_cdf = scipy.interpolate.interp1d(cdf,vlist)
        speeds[m == this_m] = inv_cdf(randlist[m == this_m])

    theta = np.arccos(np.random.uniform(-1.,1.,natoms))
    phi = np.random.uniform(0,2.*np.pi,natoms)
    

    dx = np.sin(theta) * np.cos(phi) 
    dy = np.sin(theta) * np.sin(phi)
    dz = np.cos(theta)

    vx = speeds * dx
    vy = speeds * dy
    vz = speeds * dz

    v = np.array([vx,vy,vz])

    vnet = np.einsum("i,...i",m,v)/np.sum(m)

    for i in range(3):
        v[i,:] -= vnet[i]

    K = 0.5*np.sum(np.einsum("i...,i...",v,v)*m)
    T_from_K = 2./3.*K / u.boltzmann/len(m)
    scale_factor = np.sqrt(T/T_from_K)
    print("Temperature from random velocities = ",T_from_K, "rescaling using factor of ",scale_factor)

    v *= scale_factor

    return v

# copied from https://notebook.community/tommyogden/quantum-python-lectures/11_Monte-Carlo-Maxwell-Boltzmann-Distributions
def MB_CDF(v,m,T):
    """ Cumulative Distribution function of the Maxwell-Boltzmann speed distribution """
    a = np.sqrt(u.boltzmann*T/m)
    return scipy.special.erf(v/(np.sqrt(2)*a)) - np.sqrt(2/np.pi)* v* np.exp(-v**2/(2*a**2))/a

