from time import clock, time
import numpy as np
import re
from xfel.cxi.display_spots import run_one_index_core
from cctbx.array_family import flex
from labelit.command_line.imagefiles import QuickImage

# The get_image function does not appear to be used
def get_image(imageno, container):
  import os
  # helper function adapts the internal state from LABELIT into an image object for study
  # not intended to be altered by user
  name_of_interest = results.organizer.Files.filenames.FN[0]
  #  temp_of_interest = os.path.join(name_of_interest.cwd,name_of_interest.template)
  temp_of_interest = os.path.join(name_of_interest.cwd,"tnm.set_1_1_#####.img")
  cnt = temp_of_interest.count('#')
  format = "%%0%dd"%cnt
  path_of_interest = temp_of_interest.replace('#'*cnt, format%imageno)
  from labelit.command_line.imagefiles import QuickImage
  return QuickImage(path_of_interest)







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

  imageidx = [a.find("analyze.image")==0 for a in args].index(True)
  image = int(args.pop(imageidx).split("=")[1])
  latdim = 64

  import os

  f = open("genlat.input","r")
  lines = []
  for line in f:
    if ((line.strip()!="") & (line[0] != '.')):
      lines.append(line)
  f.close()

  from spotfinder.applications.xfel import cxi_phil
  horizons_phil = cxi_phil.cxi_versioned_extract(args)

  print "indexing..."
  t0 = time()
  results = run_one_index_core(horizons_phil)
  tel = time()-t0
  print "done (",tel," sec)"

  latsize = latdim*latdim*latdim
  lat = np.zeros(latsize, dtype=np.float32)
  ct = np.zeros(latsize, dtype=np.int32)

  name_of_interest = results.organizer.Files.filenames.FN[0]
  AI = results.indexing_ai
  i0=31
  j0=31
  k0=31
  mask_tag = 32766

  fileidx = 0

  for line in lines: # these are the lines in genlat.input

    words = line.split()

    #AVB: changed imgname and scale list indices from 1,2 to 0,1 (getting IndexError: list out of range message)

    imgname = words[0]
    scale = float(words[1])

    # path_of_interest = os.path.join(name_of_interest.cwd,imgname)

    # print path_of_interest

    print "processing file %s with scale factor %f"%(imgname,scale)
    I = QuickImage(imgname)
    #    fileidx += 1
    # I = get_image(fileidx,results)
    I.read()
    DATA = I.linearintdata
  
    print "transform pixel numbers to mm positions and rotational degrees"
    from iotbx.detectors.context.spot_xy_convention import spot_xy_convention
    SF = results.spotfinder_results
    SXYC = spot_xy_convention(SF.pixel_size*SF.size1,SF.pixel_size*SF.size2)
    from spotfinder.math_support import pixels_to_mmPos
    this_frame_phi_deg = I.deltaphi/2.0+I.osc_start

    raw_spot_input = flex.vec3_double()
    for x in xrange(I.size1): # slow dimension
      for y in xrange(I.size2): # fast dimension
        mmPos = pixels_to_mmPos(x,y,SF.pixel_size)
        rawspot = (mmPos[0],mmPos[1],this_frame_phi_deg)
        transpot = SXYC.select(rawspot,results.horizons_phil.spot_convention)
        raw_spot_input.append(transpot)

    print "transform to laboratory axis reciprocal space coordinates"
    AI.setData(raw_spot_input)
    f = AI.film_to_camera()
    rvec = AI.camera_to_xyz()
  
    print "transform to fractional miller indices and populate diffuse lattice"
    from scitbx.matrix import sqr,col
    A_matrix = sqr(AI.getOrientation().direct_matrix())

    z = 0
    for x in xrange(I.size1): # slow dimension
      for y in xrange(I.size2): # fast dimension
        H = A_matrix * col(rvec[z])
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
        dh = abs(H[0]-hh)
        dk = abs(H[1]-ll)
        dl = abs(H[2]-kk)
        val = int(DATA[(x,y)]) # this is the intensity value contributed by this pixel; it was read straight from the QuickImage
        # this is excluding 7/8 of data rather than 1/8, I think...
        if ((val != mask_tag) & (val != 0) & (dh > .25)&(dk > .25)&(dl > .25)):
          i = int(H[0]+i0+.5)
          j = int(H[1]+j0+.5)
          k = int(H[2]+k0+.5)
          if ((i>0)&(j>0)&(k>0)&(i<64)&(j<64)&(k<64)):
            index = k*latdim*latdim + j*latdim + i #this is the lattice index that this pixel maps into
            if ((val>0) & (val < 32766)):
              lat[index] += (val)*scale
              ct[index] += 1
            else:
              print "image %s, slow %d, fast %d, value %d"%(imgname,x,y,val)
        #      print "slow %4d fast %4d signal %8d HKL= %6.2f %6.2f %6.2f"%(
        #        y, x, DATA[(x,y)], H[0], H[1], H[2])
        z+=1

# Below here is the same as in the parallel file - just masking and printing out

  for index in range(0,latsize):
      if ((ct[index] > 0) & (lat[index] != mask_tag)):
        lat[index] /= ct[index]
      else:
        lat[index] = -32768

  vtkfile = open("out.vtk","w")

  a_recip = 1./79.4
  b_recip = 1./79.4
  c_recip = 1./37.9

  print >>vtkfile,"# vtk DataFile Version 2.0"
  print >>vtkfile,"Generated using labelit tools"
  print >>vtkfile,"ASCII"
  print >>vtkfile,"DATASET STRUCTURED_POINTS"
  print >>vtkfile,"DIMENSIONS %d %d %d"%(latdim,latdim,latdim)
  print >>vtkfile,"SPACING %f %f %f"%(a_recip,b_recip,c_recip)
  print >>vtkfile,"ORIGIN %f %f %f"%(-i0*a_recip,-j0*b_recip,-k0*c_recip)
  print >>vtkfile,"POINT_DATA %d"%(latsize)
  print >>vtkfile,"SCALARS volume_scalars float 1"
  print >>vtkfile,"LOOKUP_TABLE default\n"

  index = 0
  for k in range(0,latdim):
    for j in range(0,latdim):
      for i in range(0,latdim):
        print >>vtkfile,lat[index],
        index += 1
      print >>vtkfile,""
