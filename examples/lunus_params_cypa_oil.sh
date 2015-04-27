num_images=360
#path and image name info
work_dir=~/app/lunus_dials/cypa_auto
raw_image_dir=~/cypa_oil/bragg_and_diffuse
image_prefix=set_1_1
lattice_dir=$work_dir/lattices
#lunus image processing - these may vary depending on data collection
#punchim - punch out a window around the beam stop arm
punchim_xmin=1156
punchim_xmax=2459
punchim_ymin=1224
punchim_ymax=1363
#windim - define a mask at the border of the image, to ensure no edge effects
windim_xmin=10
windim_xmax=2363
windim_ymin=100
windim_ymax=2427
#thrshim - threshold the intensity to eliminate artifacts
thrshim_min=1
thrshim_max=10000
#polarim - polarization correction, including Lorentz correction
polarim_dist=182.87
polarim_polarization=0.8
polarim_offset=0.0
#normim - solid angle normalization correction
normim_tilt_x=0.0
normim_tilt_y=0.0
#modeim - mode filter to eliminate bragg peaks
modeim_kernel_width=20
modeim_bin_size=1
#scaling
# make sure to use the lunus processed images for scaling instead of the originals
reference_image_number=1
scale_inner_radius=100
scale_outer_radius=800
scales_output_file=genlat.input
# integration
# make sure to use the lunus processed images for scaling instead of the originals
cctbx_dir=~/packages/cctbx-xfel
integration_script=~/packages/lunus/scripts/integrate_diffuse_dials.py
indexing_data_file_one=/netapp/home/mewall/cypa_oil/bragg_and_diffuse/set_1_1_00001.img
indexing_data_file_two=/netapp/home/mewall/cypa_oil/bragg_and_diffuse/set_1_1_00045.img
indexing_data_file_three=/netapp/home/mewall/cypa_oil/bragg_and_diffuse/set_1_1_00090.img
cella=43.16
cellb=52.13
cellc=91.67
alpha=90.0
beta=90.0
gamma=90.0
resolution=1.4
diffuse_lattice_prefix=cypa
maxcell=110
knownsetting=5
num_images=360
reference_image_number=1
scales_input_file=~/app/lunus_dials/cypa_auto/genlat.input
#reduce params
#num_images=180
