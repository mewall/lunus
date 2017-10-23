# Creates 3D lattice out of a series of diffraction images & XDS indexing info.
# Sign conventions and orientations are for the Pilatus 6M at CHESS F1
# For other detectors and beamlines, may need to be changed

# Copied from genlat_xds_parallel.py on Feburary 16, 2016
# Then edited (starting Feb 17) to create a small, high-res chunk of lattice
# THEN edited (starting Feb 28) to create a full MEDIUM-res lattice (4 per side)

import time
import numpy as np
#import matplotlib.pyplot as plt
import re
import math
from cctbx.array_family import flex
from labelit.command_line.imagefiles import QuickImage
from multiprocessing import Pool
try:
    import cPickle as pickle
except:
    import pickle

# FUNCTION: rotation_matrix
# returns rotation matrix about axis by theta (theta in radians)
# mostly copied from user unutbu on Stack Overflow question 6802577
def rotation_matrix(axis, theta):
    axis = np.asarray(axis)
    axis = axis/math.sqrt(np.dot(axis,axis))
    a = math.cos(theta/2)
    b,c,d = axis*math.sin(theta/2)
    aa,ab,ac,ad = a*a, a*b, a*c, a*d
    bb,bc,bd = b*b, b*c, b*d
    cc,cd,dd = c*c, c*d, d*d
    return np.array([[aa+bb-cc-dd, 2*(bc-ad), 2*(bd+ac)],
        [2*(bc+ad), aa+cc-bb-dd, 2*(cd-ab)],
        [2*(bd-ac), 2*(cd+ab), aa+dd-bb-cc]])

  # FUNCTION: pixmap
# goes thru all pixels, uses indexing info to map those to 3d positions in real
# space in the lab. it's that 3d rep. that calcs hkl for any pixel
def pixmap(Isize1,Isize2,pixel_size,beamx,beamy,beam_axis,distance,wavelength,maxpix,procid):
    # returns a list of data points in a chunk of each image with info to calculate the h,k,l
    from spotfinder.math_support import pixels_to_mmPos

    # we only want to bother with the points that are going to be in the resolution range
    ymin = max(0,int(beamy-maxpix))
    ymax = min(Isize2-1,int(beamy+maxpix))
    xmin = max(0,int(beamx-maxpix))
    xmax = min(Isize1-1,int(beamx+maxpix))

    print "mapping pixels in [ymin, ymax] =",ymin,ymax,"and [xmin, xmax] =",xmin,xmax

    y1 = ymin 
    y2 = ymax+1

    # now walk through the pixels and create the list of data points
    qvec = flex.vec3_double()
    for y in xrange(y1,y2): # slow dimension
        for x in xrange(xmin,xmax+1): # fast dimension
            mmPos = pixels_to_mmPos(x-beamx,y-beamy,pixel_size) 

            k_initial = (beam_axis[0]/wavelength, beam_axis[1]/wavelength, beam_axis[2]/wavelength) # beam_axis is normalized at this point
            denom = wavelength*math.sqrt((mmPos[0] ** 2) + (mmPos[1] ** 2) + (distance ** 2))
            k_final = (mmPos[0]/denom, mmPos[1]/denom, distance/denom)
            q_alt = tuple(np.subtract(k_final,k_initial))
            qvec.append(q_alt) # contains xyz components of q in reciprocal space

    return qvec

# FUNCTION: procimg
def procimg(Isize1,Isize2,scale,mask_tag,A_matrix_rot,qvec,DATA,latxdim,latydim,latzdim,origin,pedestal,beamx,beamy,maxpix,prad,voxper,procid,logfile): # this task list is getting out of control

    from scitbx.matrix import col
    
    # we only want to bother with the points that are going to be in the resolution range
    ymin = max(0,int(beamy-maxpix))
    ymax = min(Isize2-1,int(beamy+maxpix))
    xmin = max(0,int(beamx-maxpix))
    xmax = min(Isize1-1,int(beamx+maxpix))

    x1 = xmin 
    x2 = xmax+1

    # walk through the data points and accumulate the integrated data in the lattice
    vvec = flex.vec2_double();
    for y in xrange(ymin,ymax+1): # slow dimension
        for x in xrange(x1,x2): # fast dimension
            try:
                # calculate index into the data points
                z = (y-ymin)*(xmax-xmin+1) + (x-xmin) # index of this pt in array of data pts
                # calculate h,k,l for this data point
                H = A_matrix_rot.dot(col(qvec[z])) # computing hkl
                # H is a triplet of #s for the particular pixel called

                H = H*voxper

                if (H[0]<0):
                    hh = int(H[0]-.5) # find nearest H index in the higher-res lattice
                    hp = int(H[0]/voxper-.5) # find nearest integral H value
                else:
                    hh = int(H[0]+.5)
                    hp = int(H[0]/voxper+.5)
                if (H[1]<0):
                    kk = int(H[1]-.5)
                    kp = int(H[1]/voxper-.5)
                else:
                    kk = int(H[1]+.5)
                    kp = int(H[1]/voxper+.5)
                if (H[2]<0):
                    ll = int(H[2]-.5)
                    lp = int(H[2]/voxper-.5)
                else:
                    ll = int(H[2]+.5)
                    lp = int(H[2]/voxper+.5)
                # calculate the displacement of this data point from the nearest Miller index
                dhp = abs(H[0]/voxper-hp)
                dkp = abs(H[1]/voxper-kp)
                dlp = abs(H[2]/voxper-lp)
                val = int(DATA[y*Isize1+x]) #the actual value of this pixel, taken from original image
                # integrate the data only if it's not in the immediate neighborhood of a Bragg peak
            except:
                message = "an error occurred at y="+str(y)+", x="+str(x)+"; skipping this pixel/n"+str(sys.exc_info()[0])
                print >>logfile, message
                print message
            else:
                #if ((dhp*dhp + dkp*dkp + dlp*dlp > prad*prad)):
                if (val < mask_tag): # NOT punching
                # now we have the index in the lattice where this pixel goes, but what if we stored it in a list?
                # we have 10 times as many voxels as pixels
                # the list will be of 2tuples: index & val (minus pedestal already for consistency)
                    i = hh+origin[0] # these will be in terms of the 3D data structure indices, not actual hkl values of the crystal
                    j = kk+origin[1]
                    k = ll+origin[2]
                    if ((i>=0) and (j>=0) and (k>=0) and (i<latxdim) and (j<latydim) and (k<latzdim)):
                        index = k*latxdim*latydim + j*latxdim + i
                        val = scale*(val - pedestal)
                        vvec.append([index,val]);
    return vvec

# FUNCTION: proclinesstar
def proclinesstar(args):
    # wrapper that can be called for parallel processing using pool
    return proclines(*args)

# FUNCTION: proclines
def proclines(lines,res,nproc,procid):
    
    # initialize lat (lat[0] = values; lat[1] = counts)
    lat = np.zeros(latsize*2, dtype=np.float32).reshape((2,latsize))

    prevxdsname = 'init' # will store xdsname here in case it's the same file to file

    # select which lines this process needs to deal with
    chunksize = int(nlines/nproc)
    if (nlines % nproc != 0):
        chunksize += 1
    linemin = procid*chunksize
    linemax = linemin + chunksize
    if (linemax > nlines):
        linemax = nlines
    lines_these = lines[linemin:linemax]

    for line in lines_these:
        # parse the input file line into a diffuse image file name and scale factor
        words = line.split()

        imgname = words[0] # these indices are if genlat.input has 3 columns
        xdsname = words[1]
        scale = float(words[2])

        print "proc %d: Processing file %s with scale factor %f"%(procid,imgname,scale)
        I = QuickImage(imgname)
        I.read()
        DATA = I.linearintdata

        this_frame_phi_deg = I.deltaphi/2.0+I.osc_start # info from image header
        
        Isize1 = I.size2 
        Isize2 = I.size1
        print "Isize1 = ",Isize1
        print "Isize2 = ",Isize2
         
        if xdsname != prevxdsname: # if we need to read a new xds file
            print "proc %d: Creating pixel map in parallel..."%(procid)
            t0 = time.time()

            # reading info from image
            # we are assuming this is the same if XDS file is the same
            Ipixelsize = I.pixel_size

            prevxdsname = xdsname

            # reading in xds header
            xdsfile = open(xdsname,'r')
            xdslines = []
            for line in xdsfile:
                if 'END_OF_HEADER' in line:
                    break
                else:
                    xdslines.append(line)
            xdsfile.close()

            # reading info from xds header
            A_matrix = np.zeros((3,3))
            rot_axis = np.zeros(3)
            beam_axis = np.zeros(3)
            for line in xdslines:
                left,sep,right = line.partition('ORGX=')
                if sep:
                    left2,sep2,right2 = right.partition('ORGY=')
                    Ibeamx = float(left2)
                    Ibeamy = float(right2)
                    continue
                left,sep,right = line.partition('DETECTOR_DISTANCE=')
                if sep:
                    Idistance = float(right)
                    continue
                left,sep,right = line.partition('WAVELENGTH=')
                if sep:
                    Iwavelength = float(right)
                    continue
                left,sep,right = line.partition('UNIT_CELL_A-AXIS=')
                if sep:
                    numbersA = right.split()
                    continue
                left,sep,right = line.partition('UNIT_CELL_B-AXIS=')
                if sep:
                    numbersB = right.split()
                    continue
                left,sep,right = line.partition('UNIT_CELL_C-AXIS=')
                if sep:
                    numbersC = right.split()
                    continue
                left,sep,right = line.partition('ROTATION_AXIS=')
                if sep:
                    numbersR = right.split()
                    continue
                left,sep,right = line.partition('INCIDENT_BEAM_DIRECTION=')
                if sep:
                    numbers_beam = right.split()
                    continue
            for i in range(0,3):
                A_matrix[0][i] = float(numbersA[i])
                A_matrix[1][i] = float(numbersB[i])
                A_matrix[2][i] = float(numbersC[i])
                rot_axis[i] = float(numbersR[i])
                beam_axis[i] = float(numbers_beam[i])
                
            # Figure out what pixel range in the images to use, given the resolution
            # doing this here only bc this is where we read the wavelength, currently
            qstar = Iwavelength/res #  not dealing with factors of 2pi
            max_mmPos = qstar*Idistance/math.sqrt(1-(qstar ** 2))
            maxpix = 1.15*max_mmPos/Ipixelsize # add 15% in case of errors

            # normalize beam_axis 
            beam_axis = beam_axis/math.sqrt(np.dot(beam_axis, beam_axis)) 

            # run pixmap not in parallel
            qvec_all = pixmap(Isize1,Isize2,Ipixelsize,Ibeamx,Ibeamy,beam_axis,Idistance,Iwavelength,maxpix,procid)

            print "proc ",procid,": qvec_all.size() = ",qvec_all.size()
            tel = time.time()-t0
            print "proc ",procid,": done creating pixel map (",tel," sec)"

        # back to the code that runs for every image regardless of XDS input    
     
        from scitbx.matrix import sqr,col

        print "proc ",procid,": Integrating diffuse scattering in parallel..."
        t0 = time.time()

        # rotate A_matrix according to phi and beam
        # A_matrix_rot = A_matrix rotated by phi around rot axis
        phi = math.radians(this_frame_phi_deg)
        R_matrix = rotation_matrix(rot_axis, phi)
        A_matrix_rot = np.transpose(R_matrix.dot(np.transpose(A_matrix)))

        # run procimg in singular
        vvec = procimg(Isize1,Isize2,scale,mask_tag,A_matrix_rot,qvec_all,DATA,latxdim,latydim,latzdim,origin,pedestal,Ibeamx,Ibeamy,maxpix,prad,voxper,procid,logfile)
        tel = time.time()-t0
        print "proc ",procid,": done integrating diffuse scattering (",tel," sec wall clock time)"
        t0 = time.time()
        
        # put procimg results into the lattice    
        for v in vvec:
            index = v[0]  
            val = v[1]
            lat[0][index] += val 
            lat[1][index] += 1

        tel = time.time()-t0
        print "proc ",procid,": Took ",tel," secs to update the lattice"

    # now we're out of the image loop
    return lat


# MAIN METHOD
if __name__=="__main__":
    import sys

    t_init = time.time()

    args = sys.argv[1:] # normally the user puts these things on command line, not in quotes, no commas

    # Read command line arguments

    # number of processors available for pool
    try:
        nprocidx = [a.find("np")==0 for a in args].index(True)
    except ValueError:
        nproc = 1
    else:
        nproc = int(args.pop(nprocidx).split("=")[1])
    # input file with list of diffuse images and scale factors
    try:
        ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
    except ValueError:
        ifname = "genlat.input"
    else:
        ifname = args.pop(ifnameidx).split("=")[1]
    # output diffuse lattice file
    try:
        ofnameidx = [a.find("diffuse.lattice.fname")==0 for a in args].index(True)
    except ValueError:
        ofname = "diffuse.vtk"
    else:
        ofname = args.pop(ofnameidx).split("=")[1]

    # counts file
    try:
        ctnameidx = [a.find("counts.fname")==0 for a in args].index(True)
    except ValueError:
        ctname = "counts.vtk"
    else:
        ctname = args.pop(ofnameidx).split("=")[1]

    # log file
    try:
        lognameidx = [a.find("log.fname")==0 for a in args].index(True)
    except ValueError:
        logname = "genlat.log"
    else:
        logname = args.pop(ofnameidx).split("=")[1]

    latxdim = -1
    latydim = -1
    latzdim = -1

    # unit cell a
    try:
        cellaidx = [a.find("cell.a")==0 for a in args].index(True)
    except ValueError:
        raise ValueError,"Lattice constant cell.a must be specified"
    else:
        cella = float(args.pop(cellaidx).split("=")[1])
    # unit cell b
    try:
        cellbidx = [a.find("cell.b")==0 for a in args].index(True)
    except ValueError:
        raise ValueError,"Lattice constant cell.b must be specified"
    else:
        cellb = float(args.pop(cellbidx).split("=")[1])
    # unit cell c
    try:
        cellcidx = [a.find("cell.c")==0 for a in args].index(True)
    except ValueError:
        raise ValueError,"Lattice constant cell.c must be specified"
    else:
        cellc = float(args.pop(cellcidx).split("=")[1])

    # voxel edges per unit h, k, or l (in one dimension; same factor is used
    # for all three dimensions)
    try:
        voxperidx = [a.find("linear.voxel.factor")==0 for a in args].index(True)
    except ValueError:
        voxper = 1
    else:
        voxper = int(args.pop(voxperidx).split("=")[1])

    # maximum resolution of diffuse lattice
    try:
        residx = [a.find("diffuse.lattice.resolution")==0 for a in args].index(True)
    except ValueError:
        print "diffuse.lattice.resolution not specified, looking for explicit lattice dimensions"
        residx = -1
    else:
        res = float(args.pop(residx).split("=")[1])
        latxdim = (int(cella/res)+1)*2*voxper+1 # should be odd
        latydim = (int(cellb/res)+1)*2*voxper+1
        latzdim = (int(cellc/res)+1)*2*voxper+1

    # size of diffuse lattice in x direction
    try:
        latxdimidx = [a.find("latxdim")==0 for a in args].index(True)
    except ValueError:
        pass
    else:
        latxdim = int(args.pop(latxdimidx).split("=")[1])
    # size of diffuse lattice in y direction
    try:
        latydimidx = [a.find("latydim")==0 for a in args].index(True)
    except ValueError:
        pass
    else:
        latydim = int(args.pop(latydimidx).split("=")[1])
    # size of diffuse lattice in z direction
    try:
        latzdimidx = [a.find("latzdim")==0 for a in args].index(True)
    except ValueError:
        pass
    else:
        latzdim = int(args.pop(latzdimidx).split("=")[1])
    try:
        pedestalidx = [a.find("pedestal")==0 for a in args].index(True)
    except ValueError:
        pedestal = 0
    else:
        pedestal = int(args.pop(pedestalidx).split("=")[1]) # pedestal value set on command line, must be same for all images (unless code is edited later)

    # punch radius
    try:
        pradidx = [a.find("punch.radius")==0 for a in args].index(True)
    except ValueError:
        prad = 0.33 # a default setting
    else:
        prad = float(args.pop(pradidx).split("=")[1])


    if (residx == -1) and ((latxdim == -1) or (latydim == -1) or (latzdim == -1)):
        raise Exception,"Must provide either diffuse.lattice.resolution or latxdim, latydim, and latzdim."

    # Calculate the resolution if it wasn't given
    if (residx == -1):
        resx = cella/((latxdim/(2*voxper))-1)
        resy = cellb/((latydim/(2*voxper))-1)
        resz = cellc/((latzdim/(2*voxper))-1)
        res = min(resx,resy,resz) # min bc smaller numbers --> further away from beam, obvs

    # For simplicity, we'll still interpret input as number of hkls.

    import os

    # read input file with list of diffraction images and scale factors (genlat.input)
    print "opening ",ifname," as input file"
    f = open(ifname,"r")
    lines = []
    nlines = 0
    for line in f:
        if ((line.strip()!="")): # note: deleted "and (line[0] != '.')" here
            lines.append(line)
            nlines += 1
    f.close()
    print nlines

    # open logfile
    logfile = open(logname,"w")

    # For simplicity, we'll still interpret input as number of hkls.
    print "Setting up lattice with dimensions",latxdim,"x",latydim,"x",latzdim

    latsize = latxdim*latydim*latzdim 
    lat = np.zeros(latsize, dtype=np.float32)
    ct = np.zeros(latsize, dtype=np.float32)

    origin = np.array([latxdim/2,latydim/2,latzdim/2], dtype=int)
    print "hkl = (0,0,0) is at",origin
    mask_tag = 65533 #this is the MINIMUM mask tag--any values at or above will be thrown right out

    # Create parallel processing pool
    pool = Pool(processes=nproc)

    # Do the main work
    proclines_tasks = [(lines,res,nproc,procid) for procid in range(nproc)]
    lat_it = pool.map(proclinesstar, proclines_tasks)

    # gather lattices into one--this runs quickly
    for l in lat_it:
        lat = np.add(lat, l[0])
        ct = np.add(ct, l[1])

    # compute the mean intensity at each lattice point
    t0 = time.time()
    for index in range(0,latsize):
        if ((ct[index] > 0) and (lat[index] < mask_tag)):
            lat[index] /= ct[index]
        else:
            lat[index] = -32768
    tel = time.time()-t0
    print "Took",tel,"secs to compute the mean lattice intensities"

    # write results to output file
    vtkfile = open(ofname,"w")
    ctfile = open(ctname,"w")

    a_space = 1./(cella*voxper)
    b_space = 1./(cellb*voxper)
    c_space = 1./(cellc*voxper)

    a_orig = -origin[0]*a_space
    b_orig = -origin[1]*b_space
    c_orig = -origin[2]*c_space

    print >>vtkfile,"# vtk DataFile Version 2.0" #standard comment
    print >>ctfile,"# vtk DataFile Version 2.0" #standard comment
    print >>vtkfile,"Generated using xds tools" #user defined
    print >>ctfile,"Generated using xds tools" #user defined
    print >>vtkfile,"ASCII" #tells it it will be ascii
    print >>ctfile,"ASCII" #tells it it will be ascii
    print >>vtkfile,"DATASET STRUCTURED_POINTS" #tells it to expect everything on regular grid
    print >>ctfile,"DATASET STRUCTURED_POINTS" #tells it to expect everything on regular grid
    print >>vtkfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
    print >>ctfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
    print >>vtkfile,"SPACING %f %f %f"%(a_space,b_space,c_space) # tells size of each grid point
    print >>ctfile,"SPACING %f %f %f"%(a_space,b_space,c_space) # tells size of each grid point
    print >>vtkfile,"ORIGIN %f %f %f"%(a_orig,b_orig,c_orig) #at grid point 0,0,0, what is the position within the actual data structure (not specifying pos on grid of origin)
    print >>ctfile,"ORIGIN %f %f %f"%(a_orig,b_orig,c_orig) #which is to say, at index 0,0,0 in this 3D structure, where are we in the data set in terms of inverse Angstroms?
    print >>vtkfile,"POINT_DATA %d"%(latsize) #how many data points are going to follow
    print >>ctfile,"POINT_DATA %d"%(latsize) #how many data points are going to follow
    print >>vtkfile,"SCALARS volume_scalars float 1" #each lat point will contain a scalar; forget what float 1 means. tells whatever is reading the format what format each data point will have (i.e. one float)
    print >>ctfile,"SCALARS volume_scalars float 1" #each lat point will contain a scalar; forget what float 1 means. tells whatever is reading the format what format each data point will have (i.e. one float)
    print >>vtkfile,"LOOKUP_TABLE default\n" #x fast, y medium, z slow
    print >>ctfile,"LOOKUP_TABLE default\n" #x fast, y medium, z slow

    index = 0
    for k in range(0,latzdim):
        for j in range(0,latydim):
            for i in range(0,latxdim):
                print >>vtkfile,lat[index], #comma means print, but don't print carriage return
                print >>ctfile,ct[index], #comma means print, but don't print carriage return
                index += 1
            print >>vtkfile,"" #just for ease of human reading I think; format doesn't care
            print >>ctfile,"" #just for ease of human reading I think; format doesn't care

      # raw spot data sgould be in correct raster order
      # if you modify proc_img, you could just have it output what pixels to ignore. 


    t_final = (time.time() - t_init)/60
    print "Took",t_final,"minutes total to generate and write",ofname

    # close your files
    logfile.close()
    vtkfile.close()
    ctfile.close()
