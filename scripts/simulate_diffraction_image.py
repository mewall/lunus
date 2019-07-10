from time import clock, time
import numpy as np
import re
#from xfel.cxi.display_spots import run_one_index_core
from cctbx.array_family import flex
#from labelit.command_line.imagefiles import QuickImage
from multiprocessing import Pool
import glob
import dxtbx
from dxtbx.format.FormatCBFMini import FormatCBFMini

def get_experiment_xvectors(experiments):

  beam = experiments[0].beam
  detector = experiments[0].detector

  x = []

  for panel in detector: 
    lab_coordinates = flex.vec3_double()
    pixels = flex.vec2_double(panel.get_image_size())
    mms = panel.pixel_to_millimeter(pixels)
    lab_coordinates.extend(panel.get_lab_coord(mms))

    # generate s1 vectors
    s1 = lab_coordinates.each_normalize() * (1/beam.get_wavelength())
    # Generate x vectors
    x.append((s1 - beam.get_s0()))

  return(x)

def procimg_single(Isize1,Isize2,scale,lattice_mask_tag,A_matrix,rvec,D):
  # returns a 3D lattice with integrated data from a chunk of data points
  from scitbx.matrix import col
  # define the lattice indices at which h,k,l = 0,0,0
  global image_mask_tag,pphkl
  imp=np.zeros((Isize1,Isize2))
#  for r in xrange(100): # slow dimension
  for r in xrange(Isize1): # slow dimension
    if (r%10 == 0):
        print "row=",r
    for c in xrange(Isize2): # fast dimension
      # calculate index into the data points
      z = r*Isize2 + c
      tmid = clock()
      # Skip this pixel if the resolution is higher than the cutoff
      this_rvec_sqr = rvec[z][0]*rvec[z][0]+rvec[z][1]*rvec[z][1]+rvec[z][2]*rvec[z][2]
      rsn = 1./np.sqrt(this_rvec_sqr)
      if (rsn < rsn_min):
          imp[r,c] = image_mask_tag
          continue
      # calculate h,k,l for this data point
      H = A_matrix * col(rvec[z]) * pphkl
      # Calculate fractional index into diffuse intensity
      isz = len(D)
      jsz = len(D[0])
      ksz = len(D[0][0])
      fi = H[0]+int(isz/2)
      fj = H[1]+int(jsz/2)
      fk = H[2]+int(ksz/2)
      # calculate reference point integer index into diffuse intensity
      i = int(fi)
      j = int(fj)
      k = int(fk)
      # calculate the displacement of this data point from the nearest Miller index
      di = fi - i
      dj = fj - j
      dk = fk - k
# labelit values
#      val = int(DATA[(x,y)])
# 
      if (i>=0 and i<(isz-1) and j>=0 and j<(jsz-1) and k>=0 and k<(ksz-1)):
          wtot = 0.0
          for ii in range(2):
              wi = (1-di)*(1-ii)+di*ii
              for jj in range(2):
                  wj = (1-dj)*(1-jj)+dj*jj
                  for kk in range(2):
                      wk = (1-dk)*(1-kk)+dk*kk
                      if (D[i+ii][j+jj][k+kk] != lattice_mask_tag):
                          this_w = wi*wj*wk
                          imp[r,c]+=D[i+ii][j+jj][k+kk]*this_w
                          wtot += this_w
          if (wtot>0.0):
              imp[r,c] /= wtot
          else:
              imp[r,c] = image_mask_tag
      else:
          imp[r,c] = image_mask_tag
  return imp

# WARNING: The following parallel code doesn't work and isn't consistent with above

def procimg(Isize1,Isize2,scale,mask_tag,A_matrix,rvec,D,procid):
  # returns a 3D lattice with integrated data from a chunk of data points
  from scitbx.matrix import col
  # define the lattice indices at which h,k,l = 0,0,0

  # calculate the range of data points to be integrated in this process
  chunksize = int(Isize2/nproc)
  if (Isize2 % nproc !=  0):
    chunksize += 1
  y1 = procid*chunksize
  y2 = y1 + chunksize
  if (y2>Isize2):
    y2=Isize2
  # walk through the data points and accumulate the integrated data in the lattice

  ip=np.zeros(y2-y1+1,Isize1)
  for x in xrange(Isize1): # slow dimension
    for y in xrange(y1,y2): # fast dimension
      # calculate index into the data points
      z = x*Isize2 + y
      tmid = clock()
      # calculate h,k,l for this data point
      H = A_matrix * col(rvec[z])
      # Calculate fractional index into diffuse intensity
      isz = len(D)
      jsz = len(D[0])
      ksz = len(D[0][0])
      fi = H[0]+len(D)/2-1
      fj = H[1]+len(D[0])/2-1
      fk = H[2]+len(D[0][0])/2-1
      # calculate reference point integer index into diffuse intensity
      i = int(fi)
      j = int(fj)
      k = int(fk)
      # calculate the displacement of this data point from the nearest Miller index
      di = fi - i
      dj = fj - j
      dk = fk - k
# labelit values
#      val = int(DATA[(x,y)])
# 
      if (i>=0 and i<(isz-1) and j>=0 and j<(jsz-1) and k>=0 and k<(ksz-1)):
          ndat = 0
          for ii in range(2):
              wi = (1-di)*(1-ii)+di*ii
              for jj in range(2):
                  wj = (1-dj)*(1-jj)+dj*jj
                  for kk in range(2):
                      wk = (1-dk)*(1-kk)+dk*kk
                      if (D[i+ii][j+jj][k+kk] != ignore_tag):
                          ip[y][x]+=D[i+ii][j+jj][k+kk]*wi*wj*wk
                          ndat = ndat+1
          ip[y][x] *= 8/ndat
  return ip

def procimgstar(args):
  # wrapper for procimg to be used for parallel processing using pool
  return procimg(*args)

def process_one_glob():

    from dxtbx.imageset import ImageSetFactory
    from dxtbx.model.experiment_list import Experiment, ExperimentList
    from dxtbx.serialize import dump

    imnum=1

    metrolist = glob.glob(metro_glob)
    metrolist.sort()

    filelist=glob.glob(image_glob)
    filelist.sort()

    if (rotation_series):
      experiments = ExperimentListFactory.from_json_file(metrolist[0], check_format=False)
      x = get_experiment_xvectors(experiments)

    for i in range(len(filelist)):
      print "{0}...".format(i),
      sys.stdout.flush()

      if (not rotation_series):
        experiments = ExperimentListFactory.from_json_file(metrolist[i], check_format=False)
        x = get_experiment_xvectors(experiments)

      imgname=filelist[i]
      img = dxtbx.load(imgname)
#      data = img.get_raw_data()

#      print "min of data = ",flex.min(data)
#      print "max of data = ",flex.max(data)

      beam = img.get_beam()
      detector = img.get_detector()

      crystal = copy.deepcopy(experiments.crystals()[0])

      scan = img.get_scan()
      start_angle, delta_angle = scan.get_oscillation()      

      gonio = img.get_goniometer()
      axis = gonio.get_rotation_axis()
        
      Isize1,Isize2 = detector[0].get_image_size()

      pixel_values = flex.int(range(Isize1*Isize2))
      pixel_values.reshape(flex.grid(Isize2,Isize1))

      if (rotation_series):
        crystal.rotate_around_origin(axis, start_angle + (delta_angle/2), deg=True)

      from scitbx import matrix

      A_matrix = matrix.sqr(crystal.get_A()).inverse()

      scale = 1.0

      diffim = procimg_single(Isize1,Isize2,scale,lattice_mask_tag,A_matrix,x[0],D)

# Scale pixel values

      dmin=np.amin(diffim)

      print "dmin = ",dmin

#  s = 256./(dmax-dmin)


#      for i in range(len(diffim)):
#          for j in range(len(diffim[i])):
#              if (diffim[i][j] != image_mask_tag):
#                  diffim[i][j] = (diffim[i][j]-dmin)*scale

      for j in range(Isize2):
          for i in range(Isize1):
              pixel_values[j,i] = np.int(diffim[i,j])

      outname = prefout+"_{0}.cbf".format(imnum)

      FormatCBFMini.as_file(detector,beam,gonio,scan,pixel_values,outname)

      imnum = imnum +1

    print

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
 # minimum resolution
  try:
    idx = [a.find("dmin")==0 for a in args].index(True)
  except ValueError:
    rsn_min = 1.5
  else:
    rsn_min = float(args.pop(idx).split("=")[1])
 # rotation series mode
  try:
    idx = [a.find("rotation_series")==0 for a in args].index(True)
  except ValueError:
    rotation_series=True
  else:
    rotation_series_str = args.pop(idx).split("=")[1]
    if (rotation_series_str == "False"):
      rotation_series=False

 # Input json
  keep_going = True
  metro_glob_list = []
  while keep_going:
    try:
      metroidx = [(a.find("metrology")==0 or (a.find("experiments")==0)) for a in args].index(True)
    except ValueError:
      keep_going = False
    else:
      metro_glob_list.append(args.pop(metroidx).split("=")[1])
  if (len(metro_glob_list) == 0):
    raise ValueError,"Experiments .json file must be specified using experiments="
  
 # Image input glob
  keep_going = True
  image_glob_list = []
  while (keep_going):
    try:
      imageglobidx = [(a.find("image_glob")==0 or (a.find("images")==0)) for a in args].index(True)
    except ValueError:
      keep_going = False
    else:
      image_glob_list.append(args.pop(imageglobidx).split("=")[1])
# Points per hkl in the input diffuse .hkl file
  try:
    pphklidx = [a.find("pphkl")==0 for a in args].index(True)
  except ValueError:
    pphkl = 1.
  else:
    pphkl = float(args.pop(pphklidx).split("=")[1])
# Diffuse .hkl file
  try:
    hklidx = [a.find("input.hkl")==0 for a in args].index(True)
  except ValueError:
    diffusein = "diffuse.hkl"
  else:
    diffusein = (args.pop(hklidx).split("=")[1])
# Output file name
  try:
    idx = [a.find("output_prefix")==0 for a in args].index(True)
  except ValueError:
    prefout = "diffuse"
  else:
    prefout = (args.pop(idx).split("=")[1])

  lattice_mask_tag = -32768.
  image_mask_tag = 32767.

  import copy, os

  import dxtbx
  from dxtbx.model.experiment_list import ExperimentListFactory
  from dials.array_family import flex

  # Read the diffuse lattice in .hkl format
  DhklI = np.loadtxt(diffusein)
  mx = np.zeros(3,np.int)
  for i in range(3):
      mx[i]=int(max(abs(DhklI[:,i])))
  D = np.zeros((mx[0]*2+1,mx[1]*2+1,mx[2]*2+1))
  D[:,:,:] = lattice_mask_tag
  m = 0.0
  for i in range(len(DhklI)):
      hh,kk,ll=DhklI[i][:3]
      D[int(hh)+mx[0]][int(kk)+mx[1]][int(ll)+mx[2]]=float(DhklI[i][3])
      if (m > float(DhklI[i][3])):
          m = float(DhklI[i][3])
  print "min from read = ",m
#  fileidx = 0

  #Create parallel processing pool

  pool = Pool(processes=nproc)

  for i in range(len(metro_glob_list)):

    print "Image set ",i+1,":",

    metro_glob = metro_glob_list[i]
    image_glob = image_glob_list[i]

    process_one_glob()
