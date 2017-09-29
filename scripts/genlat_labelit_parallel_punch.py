from time import clock, time
import numpy as np
import re
from xfel.cxi.display_spots import run_one_index_core
from cctbx.array_family import flex
from labelit.command_line.imagefiles import QuickImage
from multiprocessing import Pool
try:
    import cPickle as pickle
except:
    import pickle
    
# FUNCTION: pixmap
# goes thru all pixels, uses indexing info to map those to 3d positions in real
# space in the lab. it's that 3d rep. that calcs hkl for any pixel
def pixmap(Isize1,Isize2,this_frame_phi_deg,pixel_size,size1,size2,spot_convention,procid):
  # returns a list of data points in a chunk of each image with info to calculate the h,k,l
  from spotfinder.applications.xfel import cxi_phil
  from iotbx.detectors.context.spot_xy_convention import spot_xy_convention
  SXYC = spot_xy_convention(pixel_size*size1,pixel_size*size2)
  from spotfinder.math_support import pixels_to_mmPos
  # for parallel running; calculate the range that will be worked on by this process
  chunksize = int(Isize1/nproc)
  if (Isize1 % nproc !=  0):
    chunksize += 1
  x1 = procid*chunksize
  x2 = x1 + chunksize
  if (x2>Isize1):
    x2=Isize1
  # now walk through the pixels and create the list of data points
  raw_spot_input = flex.vec3_double()
  for x in xrange(x1,x2): # slow dimension
    for y in xrange(Isize2): # fast dimension
      mmPos = pixels_to_mmPos(x,y,pixel_size)
      rawspot = (mmPos[0],mmPos[1],this_frame_phi_deg)
      transpot = SXYC.select(rawspot,spot_convention) #spot_convention comes from indexing results. transpot is an x,y,z coordinate, in some form to be used w/ orientation matrix to calculate hkl.
      raw_spot_input.append(transpot) #contains xyz coords of each pixel in real space wrt some reference frame
  return raw_spot_input



# FUNCTION: pixmapstar
def pixmapstar(args):
  # wrapper that can be called for parallel processing using pool
  return pixmap(*args)


# FUNCTION: procimg
# will be called nproc times per image; each time it's called it will run on 1/nproc of an image (e.g. if nproc = 4, it will run though 1/4 of one image).
def procimg(Isize1,Isize2,scale,mask_tag,A_matrix,rvec,DATA,procid): # calcs hkl; if close to a Bragg point, adds Lunus-style index to the list of punchee pixels.
  
  from scitbx.matrix import col
  #lat = np.zeros(latsize*2, dtype=np.float32).reshape((2,latsize)) # initializing

  # calculate the range of data points to be integrated in this process
  chunksize = int(Isize2/nproc) 
  if (Isize2 % nproc !=  0):
    chunksize += 1
  y1 = procid*chunksize
  y2 = y1 + chunksize
  if (y2>Isize2):
    y2=Isize2

  punch_list = flex.sym_mat3_double() # array of 6-tuple doubles

  # walk through the data points and accumulate the integrated data in the lattice
  for x in xrange(Isize1): # slow dimension
    for y in xrange(y1,y2): # fast dimension
      # calculate index into the data points
      z = x*Isize2 + y # index of this pt in array of data pts
      tmid = clock()
      # calculate h,k,l for this data point
      H = A_matrix * col(rvec[z]) # computing hkl; rvec is raw_spot_input transformed
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
      val = int(DATA[(x,y)]) #the actual value of this pixel, taken from original image
      # integrate the data only if it's not in the immediate neighborhood of a Bragg peak

      if ((val != mask_tag) and ((dh < .25) and (dk < .25) and (dl < .25))):
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
      # print "len(raw_spot_input_all) = ",len(raw_spot_input_all),"; I.size1*I.size2 = ",I.size1*I.size2
    raise ValueError,"Lattice constant cell.c must be specified"
  else:
    cellc = float(args.pop(cellcidx).split("=")[1])
  # input file with list of diffuse images and scale factors
  try:
    ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
  except ValueError:
    ifname = "genlat.input"
  else:
    ifname = args.pop(ifnameidx).split("=")[1]

  import os

  # read input file with list of diffraction images and scale factors (genlat.input)
  f = open(ifname,"r")
  lines = []
  for line in f:
    if ((line.strip()!="") and (line[0] != '.')):
      lines.append(line)
  f.close()

  from spotfinder.applications.xfel import cxi_phil
  horizons_phil = cxi_phil.cxi_versioned_extract(args)

  print "indexing..."
  t0 = clock()
  # indexing can be slow. would be good to save indexing info and read it back in
  results = run_one_index_core(horizons_phil) # takes command-line info and does indexing: this is what makes big output stream in terminal
  tel = clock()-t0
  print "done indexing (",tel," sec)"

  AI = results.indexing_ai
  mask_tag = 32767 #check this value against lunus values

  #Create parallel processing pool
  pool = Pool(processes=nproc)

  for line in lines: #in genlat.input; i.e. this part is looping over images

    # parse the input file line into a diffuse image file name and scale factor
    words = line.split()

    imgname = words[0] # these indices are if genlat.input has 2 columns only
    scale = float(words[1])

    print "processing file %s with scale factor %f"%(imgname,scale)
    I = QuickImage(imgname)
    I.read()
    DATA = I.linearintdata
  
    print "transform pixel numbers to mm positions and rotational degrees"
    from iotbx.detectors.context.spot_xy_convention import spot_xy_convention
    SF = results.spotfinder_results
    SXYC = spot_xy_convention(SF.pixel_size*SF.size1,SF.pixel_size*SF.size2)
    from spotfinder.math_support import pixels_to_mmPos
    this_frame_phi_deg = I.deltaphi/2.0+I.osc_start # info from image header

    print "Creating pixel map in parallel..."
    t0 = clock()
    raw_spot_input_all = flex.vec3_double()
    Isize1 = I.size1
    Isize2 = I.size2
    pixel_size = SF.pixel_size
    # prepare the list of arguments to run pixmap in parallel
    pixmap_tasks = [(Isize1,Isize2,this_frame_phi_deg,SF.pixel_size,SF.size1,SF.size2,results.horizons_phil.spot_convention,procid) for procid in range(nproc)]
    # run pixmap in parallel and collect results
    raw_spot_input_it = pool.map(pixmapstar,pixmap_tasks)
    # gather pixmap results into a single collection of data points
    for raw_spot_input_this in raw_spot_input_it:
      raw_spot_input_all.extend(raw_spot_input_this)
    tel = clock()-t0
    print "done creating pixel map (",tel," sec)"
    

    print "transform to laboratory axis reciprocal space coordinates"
    AI.setData(raw_spot_input_all) #AI is sort of a portion of the indexing results, A_matrix is also coming from AI
    f = AI.film_to_camera()
    rvec = AI.camera_to_xyz()
    # so now, I think, rvec is a list of data points that transform a pixel's coordinates in the image to xyz coordinates in the 3D reciprocal space lattice that is to be populated now.
  
    print "transform to fractional miller indices and populate diffuse lattice"
    from scitbx.matrix import sqr,col
    A_matrix = sqr(AI.getOrientation().direct_matrix())

    print "Building punch list in parallel..."
    telmatmul=0
    t0 = clock()
    Isize1 = I.size1
    Isize2 = I.size2
    punch_list_all = flex.sym_mat3_double()
    # prepare list of arguments to run procimg in parallel
    tasks = [(Isize2,Isize2,scale,mask_tag,A_matrix,rvec,DATA,procid) for procid in range(nproc)]
    # run procimg in parallel and collect results
    punch_list_it = pool.map(procimgstar,tasks)
    tel = clock()-t0
    print "done building punch list (",tel," sec wall clock time)"
    t0 = clock()
    # gather procimg results into a single list of indices
    for punch_list_this in punch_list_it:
        punch_list_all.extend(punch_list_this)
    tel = clock()-t0
    print "Took ",tel," secs to build this image's punch list"

    # print out the punch_list_all
    # create filename: imgname + .pks
    outname = imgname+".pks"
    outfile = open(outname,"w")
    print "Writing punch list to ",outname
    # print to that file
    for punch in punch_list_all:
        #print >>outfile,'{0:2.0f}{1:2.0f}{2:2.0f} {3:4.0f} {4:4.0f}'.format(punch[0], punch[1], punch[2], punch[3], punch[4])
        print >>outfile,"%03.0f%03.0f%03.0f %4.0f %4.0f" % (punch[0]+100, punch[1]+100, punch[2]+100, punch[3], punch[4]) #majorly hacky, only for use with fillpixim!!! clean up later 
        #print >>outfile,""   # seemed like this made extra lines before
   # now just to see if this works

