# Sign conventions and orientations are for the Pilatus 6M at CHESS F1
# For other detectors and beamlines, may need to be changed
# (edited by Veronica Pillar on 8-10-15)

import time
import numpy as np
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
def pixmap(Isize1,Isize2,pixel_size,beamx,beamy,beam_axis,distance,wavelength,procid):
  # returns a list of data points in a chunk of each image with info to calculate the h,k,l
  from spotfinder.math_support import pixels_to_mmPos
  # for parallel running; calculate the range that will be worked on by this process
  chunksize = int(Isize2/nproc)
  if (Isize2 % nproc !=  0):
    chunksize += 1
  y1 = procid*chunksize
  y2 = y1 + chunksize
  if (y2>Isize2):
    y2=Isize2

  # now walk through the pixels and create the list of data points
  qvec = flex.vec3_double()
  for y in xrange(y1,y2): # slow dimension
    for x in xrange(Isize1): # fast dimension
      mmPos = pixels_to_mmPos(x-beamx,y-beamy,pixel_size) 

      k_initial = (beam_axis[0]/wavelength, beam_axis[1]/wavelength, beam_axis[2]/wavelength) # beam_axis is normalized at this point
      denom = wavelength*math.sqrt((mmPos[0] ** 2) + (mmPos[1] ** 2) + (distance ** 2))
      k_final = (mmPos[0]/denom, mmPos[1]/denom, distance/denom)
      q_alt = tuple(np.subtract(k_final,k_initial))
      qvec.append(q_alt) #contains xyz components of q in reciprocal space

  return qvec


# FUNCTION: pixmapstar
def pixmapstar(args):
  # wrapper that can be called for parallel processing using pool
  return pixmap(*args)


# FUNCTION: procimg
# will be called nproc times per image; each time it's called it will run on 1/nproc of an image (e.g. if nproc = 4, it will run though 1/4 of one image).
def procimg(Isize1,Isize2,mask_tag,A_matrix_rot,qvec,DATA,prad,procid): # calcs hkl; if close to a Bragg point, adds Lunus-style index to the list of punchee pixels.
  
  from scitbx.matrix import col

  # calculate the range of data points to be integrated in this process
  chunksize = int(Isize1/nproc) 
  if (Isize1 % nproc !=  0):
    chunksize += 1
  x1 = procid*chunksize
  x2 = x1 + chunksize
  if (x2>Isize1):
    x2=Isize1

  punch_list = flex.sym_mat3_double() # array of 6-tuple doubles

  # walk through the data points and accumulate the integrated data in the lattice
  for y in xrange(Isize2): # slow dimension
    for x in xrange(x1,x2): # fast dimension
      # calculate index into the data points
      z = y*Isize1 + x # index of this pt in array of data pts
      # calculate h,k,l for this data point
      H = A_matrix_rot.dot(col(qvec[z])) # computing hkl; rvec is raw_spot_input transformed
      # H is a triplet of #s for the particular pixel called
      if (H[0]<0):
        hh = int(H[0]-.5)
      else:
        hh = int(H[0]+.5)
      if (H[1]<0):
        kk = int(H[1]-.5)
      else:
        kk = int(H[1]+.5)
      if (H[2]<0):
        ll = int(H[2]-.5)
      else:
        ll = int(H[2]+.5)
      # calculate the displacement of this data point from the nearest Miller index
      dh = abs(H[0]-hh)
      dk = abs(H[1]-kk)
      dl = abs(H[2]-ll)
      val = int(DATA[y*Isize1+x]) #the actual value of this pixel, taken from original image
      # integrate the data only if it's not in the immediate neighborhood of a Bragg peak
    # we want to include overloads in the punch_list, but NOT ignore_tags.
      if ((val != mask_tag) and (dh*dh + dk*dk + dl*dl < prad*prad)):
        # add (y*Isize1 + x) into a list
        # punch_list.append(y*Isize1 + x) 
        # actually we want to print h, k, l, x, y
        punch_list.append((H[0],H[1],H[2],x,y,0)) # pad 6th one with a zero

  return punch_list


# FUNCTION: procimgstar
def procimgstar(args):
  # wrapper for procimg to be used for parallel processing using pool
  return procimg(*args)


# MAIN METHOD
if __name__=="__main__":
  import sys


  args = sys.argv[1:] # normally the user puts these things on command line, not in quotes, no commas

 # Read command line arguments

 # number of processors available for pool
  try:
    nprocidx = [a.find("np")==0 for a in args].index(True)
  except ValueError:
    nproc = 1
  else:
    nproc = int(args.pop(nprocidx).split("=")[1])
  # input file with list of diffuse images and xds output files
  try:
    ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
  except ValueError:
    ifname = "genlat.input"
  else:
    ifname = args.pop(ifnameidx).split("=")[1]
  # ignore_tag in images, which we'll set mask_tag to 
  try:
    ignore_tagidx = [a.find("ignore_tag")==0 for a in args].index(True)
  except ValueError:
    ignore_tag = 65535
  else:
    ignore_tag = int(args.pop(ignore_tagidx).split("=")[1])
  
  # punch radius
  try:
    pradidx = [a.find("punch.radius")==0 for a in args].index(True)
  except ValueError:
    prad = 0.40
  else:
    prad = float(args.pop(pradidx).split("=")[1])
 

  import os

  # read input file with list of diffraction images and xds output files (genlat.input)
  f = open(ifname,"r")
  lines = []
  for line in f:
    #if ((line.strip()!="") and (line[0] != '.')):
    if (line.strip()!=""):
      lines.append(line)
  f.close()

  mask_tag = ignore_tag 

  #Create parallel processing pool
  pool = Pool(processes=nproc)

  prevxdsname = 'init' # will store xdsname here in case it's the same file to file

  for line in lines: #in genlat.input; i.e. this part is looping over images

    # parse the input file line into a diffuse image file name and xds file name
    words = line.split()

    imgname = words[0] # these indices are if genlat.input has 3 columns
    xdsname = words[1]

    print "processing file %s"%(imgname)
    I = QuickImage(imgname)
    I.read()
    DATA = I.linearintdata

    print "transform pixel numbers to mm positions and rotational degrees"
    from spotfinder.math_support import pixels_to_mmPos
    this_frame_phi_deg = I.deltaphi/2.0+I.osc_start # info from image header
    #this_frame_phi_deg = this_frame_phi_deg - 198.6 #temporary

    if xdsname != prevxdsname: # if we need to read a new xds file
      print "Creating pixel map in parallel..."
      t0 = time.time()
      qvec_all = flex.vec3_double()

      # reading info from image
      # we are assuming this is the same if XDS file is the same
      Isize1 = I.size2
      Isize2 = I.size1
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


      # testing
     # beam_axis[1] = -1*beam_axis[1]

      # get rotation matrix to put beam on z-axis
      beam_axis = beam_axis/math.sqrt(np.dot(beam_axis, beam_axis)) #normalizing
      axis = np.cross(beam_axis, [0,0,1])
      theta = math.acos(np.dot(beam_axis, [0,0,1]))
      #beam_matrix = rotation_matrix(axis, theta)
      
      #temporary, for testing
      #beam_matrix = np.array([[1,0,0],[0,1,0],[0,0,1]])
      #A_matrix = np.array([[79,0,0],[0,79,0],[0,0,38]])


      # prepare the list of arguments to run pixmap in parallel
      pixmap_tasks = [(Isize1,Isize2,Ipixelsize,Ibeamx,Ibeamy,beam_axis,Idistance,Iwavelength,procid) for procid in range(nproc)]
      # run pixmap in parallel and collect results
      qvec_it = pool.map(pixmapstar,pixmap_tasks)
      #qvec_it = pixmap(Isize1,Isize2,Ipixelsize,Ibeamx,Ibeamy,beam_matrix,beam_axis,Idistance,Iwavelength,1)
      # gather pixmap results into a single collection of data points
      for qvec_this in qvec_it:
        qvec_all.extend(qvec_this)
      tel = time.time()-t0
      print "done creating pixel map (",tel," sec)"
    

    print "transform to fractional miller indices and populate diffuse lattice"
    from scitbx.matrix import sqr,col

    print "Building punch list in parallel..."
    t0 = time.time()
    Isize1 = I.size1
    Isize2 = I.size2
 
    # rotate A_matrix according to phi and beam
    # A_matrix_rot = A_matrix rotated by phi around rot axis, then by beam_matrix
    phi = math.radians(this_frame_phi_deg)
    R_matrix = rotation_matrix(rot_axis, phi)
    A_matrix_rot = np.transpose(R_matrix.dot(np.transpose(A_matrix)))

    punch_list_all = flex.sym_mat3_double()
    # prepare list of arguments to run procimg in parallel
    tasks = [(Isize1,Isize2,mask_tag,A_matrix_rot,qvec_all,DATA,prad,procid) for procid in range(nproc)]
    # run procimg in parallel and collect results
    punch_list_it = pool.map(procimgstar,tasks)
    #punch_list_it = procimg(Isize1,Isize2,mask_tag,A_matrix_rot,qvec_all,DATA,1)

    tel = time.time()-t0
    print "done building punch list (",tel," sec wall clock time)"
    t0 = time.time()
    # gather procimg results into a single list of indices
    for punch_list_this in punch_list_it:
        punch_list_all.extend(punch_list_this)
    
    tel = time.time()-t0
    print "Took ",tel," secs to build this image's punch list"

    # print out the punch_list_all
    # create filename: imgname + .pks
    outname = imgname+".pks"
    outfile = open(outname,"w")
    print "Writing punch list to ",outname
    # print to that file
    for punch in punch_list_all:
        #print >>outfile,"%03.0f%03.0f%03.0f %4.0f %4.0f" % (punch[0]+100, punch[1]+100, punch[2]+100, punch[3], punch[4]) #majorly hacky, only for use with fillpixim!!! clean up later 
        print >>outfile,"%4.0f" % (punch[4]*Isize1 + punch[3])

