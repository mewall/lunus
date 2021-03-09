from __future__ import print_function
import future,six
import lunus
import dxtbx
from dxtbx.format.cbf_writer import FullCBFWriter
from scitbx.array_family import flex
from lunus import LunusDIFFIMAGE
import os
import numpy as np
import sys
import argparse
import time

def get_image_params(beam,detector):

  from scitbx.matrix import col

  image_params = []

  beam_direction = col(beam.get_sample_to_source_direction())
  wavelength = beam.get_wavelength()

  beam_params = "\nbeam_vec={0},{1},{2}\nwavelength={3}".format(-beam_direction[0],-beam_direction[1],-beam_direction[2],wavelength)

  panel_ct = 0

  for panel in detector: 
    pixel_size_mm = panel.get_pixel_size()[0]
    beam_mm_x = panel.get_beam_centre(beam.get_s0())[0]
    beam_mm_y = panel.get_beam_centre(beam.get_s0())[1]
    distance_mm = panel.get_distance()
    fast_vec = col(panel.get_fast_axis())
    slow_vec = col(panel.get_slow_axis())
    origin_vec = col(panel.get_origin())
    normal_vec = col(panel.get_normal())
    
    fast_vec_params = "\nfast_vec={0},{1},{2}".format(fast_vec[0],fast_vec[1],fast_vec[2])
    slow_vec_params = "\nslow_vec={0},{1},{2}".format(slow_vec[0],slow_vec[1],slow_vec[2])
    origin_vec_params = "\norigin_vec={0},{1},{2}".format(origin_vec[0],origin_vec[1],origin_vec[2])
    normal_vec_params = "\nnormal_vec={0},{1},{2}".format(normal_vec[0],normal_vec[1],normal_vec[2])

    more_params = "\npixel_size_mm={0}\nbeam_mm_x={1}\nbeam_mm_y={2}\ndistance_mm={3}".format(pixel_size_mm,beam_mm_x,beam_mm_y,distance_mm)

    image_params.append(beam_params+fast_vec_params+slow_vec_params+origin_vec_params+normal_vec_params+more_params)

#    print "panel_ct = ",panel_ct,origin_vec_params

    panel_ct += 1

  return(image_params)

if __name__=="__main__":

# Parse command line arguments

    parser = argparse.ArgumentParser(description='Mode filter a diffraction image')

    parser.add_argument("--infile",help="Input file name",required=True)

    parser.add_argument("--outfile",help="Output file name",default="mode.cbf")

    args=parser.parse_args()

    ts = time.time()
    writer = FullCBFWriter(filename=args.infile)
    cbf = writer.get_cbf_handle()
    data = writer.imageset[0]

    print("Took %g seconds to get CBF handle and read data" % (time.time() - ts))

    ts = time.time()
    img = dxtbx.load(args.infile)

    detector = img.get_detector()
    beam = img.get_beam()
    gonio = img.get_goniometer()
    scan = img.get_scan()

#    data = img.get_raw_data()

    print("Took %g seconds to get image metadata" % (time.time() - ts))

    if not isinstance(data, tuple):
      data = (data,)
    data = list(data)

    A = LunusDIFFIMAGE(len(data))

    print("Set up images")
    sys.stdout.flush()

    for pidx in range(len(data)):
      A.set_image(pidx,data[pidx])

    deck = '''
#lunus input deck
#punchim_xmin=1203
#punchim_ymin=1250
#punchim_xmax=2459
#punchim_ymax=1314
#windim_xmin=100
#windim_ymin=100
#windim_xmax=2362
#windim_ymax=2426
#thrshim_min=0
#thrshim_max=50
modeim_bin_size=1
modeim_kernel_width=15
'''
    print("Get image params")
    sys.stdout.flush()
    image_params = get_image_params(beam,detector)

    for pidx in range(len(image_params)):
        deck_and_extras = deck+image_params[pidx]
        A.LunusSetparamsim(pidx,deck_and_extras)
#    data_np = data.as_numpy_array()
#    hist=np.histogram(data,range=(0,50),bins=50,density=False)
#    print(hist[0:50])
#    print(data[0])
#    A.LunusSetparamsim(deck)
#A.LunusPunchim()
#A.LunusWindim()
#A.LunusThrshim()
    print("About to perform mode filter")
    sys.stdout.flush()
    A.LunusModeim()
    print("Mode filter finished.")
#    data2 = A.get_image();
#if not ref_name is None:
#    ref = dxtbx.load(ref_name)
#    ref_data_np = ref.get_raw_data().as_numpy_array().flatten()
#    data2_np = data2.as_numpy_array().flatten()
##    data2_np_rand = np.random.random_sample((len(data2_np),))
#    cc = np.corrcoef(ref_data_np,data2_np)[0,1]
#dxtbx.format.FormatCBFMini.FormatCBFMini.as_file(detector,beam,gonio,scan,data,path,header_convention="GENERIC_MINI",det_type="GENERIC")
    for pidx in range(len(data)):
      data[pidx] = A.get_image_double(pidx)
    writer.add_data_to_cbf(cbf, data=tuple(data))
    writer.write_cbf(args.outfile, cbf=cbf)
#FormatCBFMini.as_file(detector,beam,gonio,scan,data2,args.outfile)
#if np.abs(cc - 1.0) < 0.00001:
#    sys.exit(0)
#else: 
#    sys.exit(1)
