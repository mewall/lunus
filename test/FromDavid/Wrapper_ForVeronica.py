import Modules_ForVeronica as DRM # These are the modules that go along with the program
# I renamed them as Modules_ForVeronica
import fabio
import os

# (Trying) set your variables
INTEGRATION_DIRECTORY = '~/data/CHESS_July2015/lyso6/img_data'
CORRECT_NAME = '~/data/CHESS_July2015/lyso6/xds_run1/CORRECT.LP'

#INTEGRATION_DIRECTORY is the directory where everything gets stored
if os.path.isdir(INTEGRATION_DIRECTORY) == False:	# prevents overwriting
	os.mkdir(INTEGRATION_DIRECTORY)	 

# This pulls some basic information from the CORRECT.LP file
# CORRECT_NAME is the file name for the CORRECT.LP file from xds
[n_frames, wavelength, nx, ny, beam_x, beam_y, distance, pixel_size] = DRM.GetInformation_IceRings(CORRECT_NAME)

# This stores all of the diffraction images. to_mask[1] is the number of frames I believe.
# If you are dealing with many frames, it might take a bunch of RAM, or you could do something
# That would load each frame individually.

data = np.zeros([ny,nx,to_mask[1]],dtype='int32')	
for i in range(to_mask[1]):
	if i >=9:
		image = IMAGETEMPLATE.replace('???','0'+str(i+1))
	else:
		image = IMAGETEMPLATE.replace('???','00'+str(i+1))
	data[:,:,i]=fabio.open(image).data

# This just gets the background as the average of image. It works much better with a quantum image.
background = DRM.GetBackground(FIRSTIMAGE_NAME)
threshold = 3*background

# Moves through the lines in SPOT.XDS - equivalently the Bragg peaks

CORRECT_NAME = RESULTS_BASE+'/'+PREFIX+'/Run_'+str(run)+'_XDS/CORRECT.LP'
SPOT_NAME = RESULTS_BASE+'/'+PREFIX+'/Run_'+str(run)+'_XDS/XDS_ASCII.HKL'
spots = open(SPOT_NAME,'r')	#SPOT_NAME is the XDS_ASCII.HKL file
[n_frames, wavelength, nx, ny, beam_x, beam_y, distance, pixel_size] = DRM.GetInformation_IceRings(CORRECT_NAME)

with spots as f:
	# Make the mask for each frame
	DRM.MakeMask(f, run, to_mask[0], data, threshold, INTEGRATION_DIRECTORY, PREFIX)

	#Mask images 
	DRM.MaskImage(nx, ny, to_mask[0], run, IMAGETEMPLATE, INTEGRATION_DIRECTORY, PREFIX)

	# Integrate each frame
	DRM.IntegrateFrames(nx, ny, beam_x, beam_y, to_mask[0], run, distance, pixel_size, wavelength,
		RESOLUTION, INTEGRATION_DIRECTORY, PREFIX)

