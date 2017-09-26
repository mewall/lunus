from time import clock, time
import numpy as np
import re
from multiprocessing import Pool
#from scipy.sparse import csr_matrix
try:
    import cPickle as pickle
except:
    import pickle
    

def procimg(Isize1,Isize2,scale,mask_tag,A_matrix,rvec,DATA,latxdim,latydim,latzdim,procid):
  # returns a 3D lattice with integrated data from a chunk of data points
  # define the lattice indices at which h,k,l = 0,0,0
  i0=int(latxdim/2-1)
  j0=int(latydim/2-1)
  k0=int(latzdim/2-1)
  # total number of voxels in the lattice
  latsize = latxdim*latydim*latzdim
  lat = np.zeros(latsize*2, dtype=np.float32).reshape((2,latsize))
#  lat = csr_matrix((2,latsize),dtype=np.float32)

  # calculate the range of data points to be integrated in this process
  chunksize = int(Isize2/nproc)
  if (Isize2 % nproc !=  0):
    chunksize += 1
  y1 = procid*chunksize
  y2 = y1 + chunksize
  if (y2>Isize2):
    y2=Isize2
  # walk through the data points and accumulate the integrated data in the lattice
  At = A_matrix
#  print A_matrix
#  print At
#  print np.dot(np.asarray(rvec),At)[0]
#  print A_matrix*col(rvec[0])
  rvec_size = len(rvec)
  for y in xrange(y1,y2): # fast dimension
    Hlist = np.dot(np.asarray(rvec[y:rvec_size:Isize2]),At)
    Hintlist = np.rint(Hlist)
    dHlist = np.abs(Hlist - Hintlist)
    vallist = np.asarray(DATA[y*Isize1:(y+1)*Isize1])
    Hintpphkllist = np.rint(Hlist*float(pphkl))
    ilist = Hintpphkllist[:,0] + i0
    jlist = Hintpphkllist[:,1] + j0
    klist = Hintpphkllist[:,2] + k0
    maskrightvals = np.logical_and((vallist>0),(vallist != mask_tag))
    masknotnearhkl = np.logical_or(np.logical_or((dHlist[:,0] >= 0.25),(dHlist[:,1] >= 0.25)),(dHlist[:,2] >= 0.25))
#    print 'not near hkl = {0}, total = {1}'.format(np.sum(masknotnearhkl),len(dHlist[:,0]))
    maskinbounds = np.logical_and(np.logical_and(np.logical_and(np.logical_and(np.logical_and(ilist>=0,jlist>=0),klist>=0),ilist<latxdim),jlist<latydim),klist<latzdim)
    if (filterhkl):
        maskall = np.logical_and(np.logical_and(maskrightvals,masknotnearhkl),maskinbounds)
    else:
        maskall = np.logical_and(maskrightvals,maskinbounds)
#    indexlist = klist*latxdim*latydim + jlist*latxdim + ilist
    indexlist = np.int32(klist[maskall]*latxdim*latydim + jlist[maskall]*latxdim + ilist[maskall])
#    print len(indexlist2)
#    indexlist[maskall] = 0
    valscalelist = vallist[maskall]*scale
    for x in range(len(indexlist)): # slow dimension
#        if maskall[x]:
            lat[0][indexlist[x]] += valscalelist[x]
            lat[1][indexlist[x]] += 1
      # calculate index into the data points
#      z = x*Isize2 + y
#      z = y*Isize1 + x
#      tmid = time()
      # calculate h,k,l for this data point
#      H = A_matrix * col(rvec[z])
#      H = np.dot(np.asarray(rvec_list[x]),At)
#      print H
#      print A_matrix * col(rvec[z])
#      hh = Hintlist[x][0]
#      kk = Hintlist[x][1]
#      ll = Hintlist[x][2]
      # calculate the displacement of this data point from the nearest Miller index
#      dh = dHlist[x][0]
#      dk = dHlist[x][1]
#      dl = dHlist[x][2]
# labelit values
#      val = int(DATA[(x,y)])
#dials values
#      val = int(DATA[(y,x)])
#      val = int(DATA[y*Isize1+x])
#      val = vallist[x]
      # integrate the data only if it's not in the immediate neighborhood of a Bragg peak
#      if ((val != mask_tag) and (val != 0) and not((dh < .25) and (dk < .25) and (dl < .25))):
        # i, j, and k are indices into the lattice
#      if (not maskwrongvals[x] and not masknearhkl[x]):
#        i = ilist[x]
#        j = jlist[x]
#        k = klist[x]
#        if ((i>=0) and (j>=0) and (k>=0) and (i<latxdim) and (j<latydim) and (k<latzdim)):
#        if maskbounds[x]:
#        index = indexlist[x]
#          index = k*latxdim*latydim + j*latxdim + i
#          if ((val>0)  and (val < 32767)):
            # accumulate the data for this lattice point. keep track of count for averaging
#        lat[0][index] += val*scale
#        lat[1][index] += 1
#  lat[0][0] = 0
#  lat[1][0] = 0
  return lat

def procimgstar(args):
  # wrapper for procimg to be used for parallel processing using pool
  return procimg(*args)

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
    raise ValueError("Lattice constant cell.a must be specified")
  else:
    cella = float(args.pop(cellaidx).split("=")[1])
  # unit cell b
  try:
    cellbidx = [a.find("cell.b")==0 for a in args].index(True)
  except ValueError:
    raise ValueError("Lattice constant cell.b must be specified")
  else:
    cellb = float(args.pop(cellbidx).split("=")[1])
  # unit cell c
  try:
    cellcidx = [a.find("cell.c")==0 for a in args].index(True)
  except ValueError:
    raise ValueError("Lattice constant cell.c must be specified")
  else:
    cellc = float(args.pop(cellcidx).split("=")[1])
  # target cell for indexing
  try:
      targetcellidx = [a.find("target_cell")==0 for a in args].index(True)
  except ValueError:
      raise ValueError("Target cell target_cell must be specified")
  else:
      target_cell = args.pop(targetcellidx).split("=")[1]
  # spacegroup for indexing
  try:
      targetsgidx = [a.find("target_sg")==0 for a in args].index(True)
  except ValueError:
      raise ValueError("Target space group target_sg must be specified")
  else:
      target_sg = args.pop(targetsgidx).split("=")[1]
  # maximum resolution of diffuse lattice
  try:
    residx = [a.find("diffuse.lattice.resolution")==0 for a in args].index(True)
  except ValueError:
    print ("diffuse.lattice.resolution not specified, looking for explicit lattice dimensions")
    residx = -1
  else:
    res = float(args.pop(residx).split("=")[1])
  # input file with list of diffuse images and scale factors
  try:
    ifnameidx = [a.find("inputlist.fname")==0 for a in args].index(True)
  except ValueError:
    ifname = ""
  else:
    ifname = args.pop(ifnameidx).split("=")[1]
  # scale factor for diffuse image
  try:
    scalenameidx = [a.find("scale.fname")==0 for a in args].index(True)
  except ValueError:
    scalename = ""
  else:
    scalename = args.pop(scalenameidx).split("=")[1]    
  # frame number
  try:
    framenumidx = [a.find("framenum")==0 for a in args].index(True)
  except ValueError:
    framenum=-1
  else:
    framenum=int(args.pop(framenumidx).split("=")[1])
    print "integrating frame ",framenum
  # output diffuse lattice file
  try:
    ofnameidx = [a.find("diffuse.lattice.fname")==0 for a in args].index(True)
  except ValueError:
    ofname = "diffuse.vtk"
  else:
    ofname = args.pop(ofnameidx).split("=")[1]
  # output diffuse lattice file
  try:
    cfnameidx = [a.find("counts.lattice.fname")==0 for a in args].index(True)
  except ValueError:
    cfname = "counts.vtk"
  else:
    cfname = args.pop(cfnameidx).split("=")[1]
  # type of lattice output. "sum" = sum, "mean" = mean
  try:
    lattypeidx = [a.find("diffuse.lattice.type")==0 for a in args].index(True)
  except ValueError:
    lattype = "mean"
  else:
    lattype = args.pop(lattypeidx).split("=")[1]
    if not((lattype == "sum") or (lattype == "mean") or (lattype == "npz")):
      raise Exception("Lattice type must be ""sum"" or ""mean"" or ""npz""")
  # diffuse lattice points per hkl value
  try:
    pphklidx = [a.find("pphkl")==0 for a in args].index(True)
  except ValueError:
    pphkl = 1
  else:
    pphkl = int(args.pop(pphklidx).split("=")[1])
  # diffuse lattice points per hkl value
  try:
    filterhklidx = [a.find("filterhkl")==0 for a in args].index(True)
  except ValueError:
    filterhkl = True
  else:
    if (args.pop(filterhklidx).split("=")[1] == "False"):
        filterhkl = False
    else:
        filterhkl = True
  # apply correction factor
  try:
    apply_correctionidx = [a.find("apply_correction")==0 for a in args].index(True)
  except ValueError:
    apply_correction = True
  else:
    if (args.pop(apply_correctionidx).split("=")[1] == "False"):
        apply_correction = False
    else:
        apply_correction = True
  # size of diffuse lattice in x direction
  try:
    latxdimidx = [a.find("latxdim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latxdim = -1
  else:
    latxdim = int(args.pop(latxdimidx).split("=")[1])
  # size of diffuse lattice in y direction
  try:
    latydimidx = [a.find("latydim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latydim = -1
  else:
    latydim = int(args.pop(latydimidx).split("=")[1])
  # size of diffuse lattice in z direction
  try:
    latzdimidx = [a.find("latzdim")==0 for a in args].index(True)
  except ValueError:
    if (residx == -1):
        latzdim = -1
  else:
    latzdim = int(args.pop(latzdimidx).split("=")[1])
  # image mask tag
  try:
    image_mask_tagidx = [a.find("image.mask.tag")==0 for a in args].index(True)
  except ValueError:
    mask_tag = 32767
  else:
    mask_tag = int(args.pop(image_mask_tagidx).split("=")[1])
  # specify path to proc directory
  try:
    procpathidx = [a.find("path.to.proc")==0 for a in args].index(True)
  except ValueError:
    procpath=""
  else:
    procpath = args.pop(procpathidx).split("=")[1]
  # read indexing info from a file instead of calculating (not implemented)
  try:
    readindexidx = [a.find("readindex")==0 for a in args].index(True)
  except ValueError:
    readindex = False
  else:
    readindex = bool(args.pop(readindexidx).split("=")[1])

  if (residx == -1) and ((latxdim == -1) or (latydim == -1) or (latzdim == -1)):
    raise Exception("Must provide either diffuse.lattice.resolution or latxdim, latydim, and latzdim.")

  import os

  # otherwise, create a line from provided diffraction image and scale factor
  if (scalename != ""):
      scale = np.loadtxt(scalename,dtype=np.float)[0]
  else:
      raise Exception("Must either provide a list of files and scale factors using inputlist.fname or filename and scale factor using diffimg.fname and scale.") 
  #########################################################################
  # new dials

# lattice dimensions
  latxdim = (int(float(pphkl)*cella/res)+1)*2
  latydim = (int(float(pphkl)*cellb/res)+1)*2
  latzdim = (int(float(pphkl)*cellc/res)+1)*2

  latsize = latxdim*latydim*latzdim
  print "Lattice size = ",latsize
  lat = np.zeros(latsize, dtype=np.float32)
  ct = np.zeros(latsize, dtype=np.float32)

  #name_of_interest = results.organizer.Files.filenames.FN[0]
# old labelit
#  AI = results.indexing_ai
  i0=latxdim/2-1
  j0=latydim/2-1
  k0=latzdim/2-1
#  mask_tag = 32767.

#  fileidx = 0

  #Create parallel processing pool

  pool = Pool(processes=nproc)

  print "processing file with scale factor %f"%(scale)
  # Read x vectors
  x_vectors = np.load("../tmpdir_common/x_vectors.npy")

  print "there are ",len(x_vectors)," elements in x_vectors"
  
  # Read At matrix
  A_matrix = np.load("At.npy")
  print "Integrating diffuse scattering in parallel using ",nproc," processors..."
  telmatmul=0
  t0 = time()
  latit = None
  # Read image size
  Isize1, Isize2 = np.load("../tmpdir_common/DATAsize.npy")
  print "Isize1 = ",Isize1,", Isize2 = ",Isize2
  print "there are ",Isize1*Isize2," pixels in this diffraction image"
  # Read image data values
  DATAimg = np.load("DATA.npy")
  # Read correction file
  if (apply_correction):
      correction = np.fromfile(open("../tmpdir_common/correction.imf","rb"),dtype=np.float32)
      #  print "Mean of correction,DATAimg is {0},{1}".format(np.mean(correction),DATAimg.astype(np.float32))
      correction[DATAimg==mask_tag]=1.
      # apply correction (normim, polarim steps using floating point arithmetic)
      DATA = np.multiply(correction,DATAimg.astype(np.float32))
  else:
      DATA = DATAimg.astype(np.float32)
  tasks = [(Isize1,Isize2,scale,mask_tag,A_matrix,x_vectors,DATA,latxdim,latydim,latzdim,procid) for procid in range(nproc)]
  # run procimg in parallel and collect results
  tmp_latit = pool.map(procimgstar,tasks)
  if latit is None:
      latit = tmp_latit
  else:
      latit += tmp_latit
  tel = time()-t0
  print "done integrating diffuse scattering (",tel," sec wall clock time)"
  t0 = time()
  # accumulate integration data into a single lattice
  for l in latit:
#      lat = np.add(lat,l.toarray()[0])
#      ct = np.add(ct,l.toarray()[1])
      lat = np.add(lat,l[0])
      ct = np.add(ct,l[1])
  tel = time()-t0
  print "Took ",tel," secs to update the lattice"

  if (lattype == "mean"):
      # compute the mean intensity at each lattice point
      for index in range(0,latsize):
          if ((ct[index] > 0) and (lat[index] != mask_tag)):
              lat[index] /= ct[index]
          else:
              lat[index] = -32768

  if ((lattype == "mean" or lattype == "sum")):
      # write lattice to output file
      vtkfile = open(ofname,"w")

      a_recip = 1./cella/float(pphkl)
      b_recip = 1./cellb/float(pphkl)
      c_recip = 1./cellc/float(pphkl)

      print >>vtkfile,"# vtk DataFile Version 2.0"
      print >>vtkfile,"lattice_type=PR;unit_cell={0};space_group={1};".format(target_cell,target_sg)
      print >>vtkfile,"ASCII"
      print >>vtkfile,"DATASET STRUCTURED_POINTS"
      print >>vtkfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
      print >>vtkfile,"SPACING %f %f %f"%(a_recip,b_recip,c_recip)
      print >>vtkfile,"ORIGIN %f %f %f"%(-i0*a_recip,-j0*b_recip,-k0*c_recip)
      print >>vtkfile,"POINT_DATA %d"%(latsize)
      print >>vtkfile,"SCALARS volume_scalars float 1"
      print >>vtkfile,"LOOKUP_TABLE default\n"

      index = 0
      for k in range(0,latzdim):
          for j in range(0,latydim):
              for i in range(0,latxdim):
                  print >>vtkfile,lat[index],
                  index += 1
              print >>vtkfile,""

      vtkfile.close()

  # write counts lattice to output file, if lattice type is "sum"
  if (lattype=="sum"):
      vtkfile = open(cfname,"w")

      a_recip = 1./cella/float(pphkl)
      b_recip = 1./cellb/float(pphkl)
      c_recip = 1./cellc/float(pphkl)

      print >>vtkfile,"# vtk DataFile Version 2.0"
      print >>vtkfile,"lattice_type=PR;unit_cell={0};space_group={1};".format(target_cell,target_sg)
      print >>vtkfile,"ASCII"
      print >>vtkfile,"DATASET STRUCTURED_POINTS"
      print >>vtkfile,"DIMENSIONS %d %d %d"%(latxdim,latydim,latzdim)
      print >>vtkfile,"SPACING %f %f %f"%(a_recip,b_recip,c_recip)
      print >>vtkfile,"ORIGIN %f %f %f"%(-i0*a_recip,-j0*b_recip,-k0*c_recip)
      print >>vtkfile,"POINT_DATA %d"%(latsize)
      print >>vtkfile,"SCALARS volume_scalars float 1"
      print >>vtkfile,"LOOKUP_TABLE default\n"

      index = 0
      for k in range(0,latzdim):
          for j in range(0,latydim):
              for i in range(0,latxdim):
                  print >>vtkfile,ct[index],
                  index += 1
              print >>vtkfile,""

      vtkfile.close()

  if ((lattype == "npz")):
      np.savez_compressed(ofname,lat=lat)
      np.savez_compressed(cfname,ct=ct)
