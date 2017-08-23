import fabio	# fabio is a python based diffraction image reader 
import numpy as np
import csv

###########################################
##### Things that remove the Bragg peaks #####
###########################################

# This pulls basic information from the CORRECT.LP file
def GetInformation_IceRings(CORRECT_NAME):
	correct = open(CORRECT_NAME,'r')
	for line in correct.readlines():
		if line.startswith(' DATA_RANGE='):
			first = False
			for contents in line.split(' ')[2:]:
				if contents != '' and first == False:
					first_frame=float(contents)
					first = True
				elif contents != '' and first == True:
					last_frame = float(contents.split('\\')[0])		
			n_frames = int(last_frame-first_frame+1)
		if line.startswith(' X-RAY_WAVELENGTH= '):
			for contents in line.split(' ')[2:]:
				if contents != '':
					wavelength = float(contents.split('\\')[0])
		if line.startswith(' NX= '):
			i = 0
			for contents in line.split(' '):
				if contents == 'NX=':
					stop = False
					j=1
					while stop == False:
						if line.split(' ')[i+j] != '':
							nx = int(float(line.split(' ')[i+j]))
							stop = True
						else:
							j+=1
				if contents == 'NY=':
					stop = False
					j=1
					while stop == False:
						if line.split(' ')[i+j] != '':
							ny = int(float(line.split(' ')[i+j]))
							stop = True
						else:
							j+=1
				if contents == 'QX=':
					stop = False
					j=1
					while stop == False:
						if line.split(' ')[i+j] != '':
							pixel_size = float(line.split(' ')[i+j])
							stop = True
						else:
							j+=1
				i += 1
		if line.startswith(' DETECTOR COORDINATES (PIXELS) OF DIRECT BEAM'):
			first = False
			for contents in line.split(' ')[7:]:
				if contents != '' and first == False:
					beam_x = float(contents)
					first = True
				elif contents != '' and first == True:
					beam_y = float(contents.split('\\')[0])
		if line.startswith(' CRYSTAL TO DETECTOR DISTANCE (mm)'):			
			for contents in line.split(' ')[6:]:
				if contents != '':
					distance = float(contents.split('\\')[0])
	return [n_frames, wavelength, nx, ny, beam_x, beam_y, distance, pixel_size]

# This program makes the mask, it is a .csv file with the locations of pixels that are in diffraction peaks.
# Here f is the XDS_ASCII.HKL file
# It saves the mask to:
#	INTEGRATION_DIRECTORY/PREFIX_RUN_FRAME.csv

# Data is a nx X ny X # frames array. This is very fast if there are only a few frames, since it doesn't need 
# to load them individually.
# I'm dealing with 10-20. A full data would take enough RAM to crash the computer
def MakeMask(f, run, n_frames, data, threshold, INTEGRATION_DIRECTORY, PREFIX):
	mask = [[] for x in xrange(n_frames)]
        print 'testing'
	for line in f:

		# Parse the 'XDS_ASCII.HKL' file for spot locations
		if line.startswith('!') == False:		
			# Break up the line with space delimits	
			columns = line.split(' ')
			# location of the spot
			column_index = 0
			for element in columns:
				if element != '':
					if column_index == 5:
						x = int(round(float(element)))
					if column_index == 6:
						y = int(round(float(element)))
					if column_index == 7:
                                                print element
					# This gives the frame closest to the spots centroid
						frame0 = int(round(float(element/10)))
                                                print frame0
					column_index += 1

			if x and y < 2400: # This skips pixels close to the edge
				# move backwards - spots are on multiple frames
				currentframe = frame0
				spot=True
				while spot == True:
					spot = IsSpot(data[:,:,currentframe-1],x,y,threshold)	# is there a diffraction peak there?
					radius = GetRadius(data[:,:,currentframe-1],x,y,threshold)	# Find radius of the peak
					mask = EditMask(currentframe,x,y,radius,mask)	# add peak to the mask
					currentframe -= 1 	# move to the next frame
					if currentframe <= 0: 	# 1 is the first frame...
						spot = False

				# move forwards
				currentframe = frame0+1
				spot=True
				while spot == True:
					if currentframe >= n_frames:
						spot = False
					else:
						spot = IsSpot(data[:,:,currentframe-1],x,y,threshold)
						radius = GetRadius(data[:,:,currentframe-1],x,y,threshold)
						mask = EditMask(currentframe,x,y,radius,mask)
						currentframe += 1

	print ' '
	print 'Masks have been constructed from XDS_ASCII.HKL'
	print 'Now saving masks'
	# Save the masks, 

	for currentframe in range(n_frames):
		print ' Saving mask: ' + str(currentframe+1)
		if currentframe >= 9:
			mask_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_0'+str(currentframe+1)+'_mask.csv'
		else:
			mask_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_00'+str(currentframe+1)+'_mask.csv'
		with open(mask_filename, 'w') as csvfile:
			writer = csv.writer(csvfile)
			writer.writerows(mask[currentframe])
	print ' '

	return mask

# Check to see if the pixel has an intensity consistent with a Bragg peak
def IsSpot(data,x,y,threshold):
	if data[x,y] > threshold:
		spot = True
	else:
		spot = False
	return spot

# This estimates the radius of a Bragg peak by moving from the center in all directions
# It takes the largest step.
# this could be made better by defining a new center to the spot and drawing a circle around that. 
def GetRadius(data,x,y,threshold):
	radii = [1,1,1,1]
	stop = False
	# Positive x
	while stop == False:
		if data[x+radii[0],y] < threshold:
			stop = True
		radii[0] += 1
	stop = False
	# Negative x
	while stop == False:
		if data[x-radii[1],y] < threshold:
			stop = True
		radii[1] += 1
	stop = False
	# positive y
	while stop == False:
		if data[x,y+radii[2]] < threshold:
			stop = True
		radii[2] += 1
	stop = False
	# negative y
	while stop == False:
		if data[x,y-radii[3]] < threshold:
			stop = True
		radii[3] += 1
	return max(radii)

# This makes a circular mask around the center of the bragg peak
def EditMask(currentframe,x,y,radius,mask):
	mask[currentframe-1].append([x,y])	# Get the center
	for i in range(-radius,radius+1):	# Get the axes
		mask[currentframe-1].append([x,y+i])
		mask[currentframe-1].append([x+i,y])
	for i in range(1,radius+1):	# fill in
		for j in range(i,radius+1):
			if (i**2+j**2) <= radius**2:
				mask[currentframe-1].append([x+i,y+j])
				mask[currentframe-1].append([x+j,y+i])

				mask[currentframe-1].append([x+i,y-j])
				mask[currentframe-1].append([x+j,y-i])

				mask[currentframe-1].append([x-i,y+j])
				mask[currentframe-1].append([x-j,y+i])

				mask[currentframe-1].append([x-i,y-j])
				mask[currentframe-1].append([x-j,y-i])
	return mask

# This applies the mask to the images
# The IMAGETEMPLATE is the IMAGETEMPLATE parameter from XDS.INP, basically.
def MaskImage(nx,ny,frames,run,IMAGETEMPLATE,INTEGRATION_DIRECTORY,PREFIX):
	masked_frames = np.zeros([ny,nx,frames],dtype='int32') # These will be the data frames with the Bragg peaks removed
	for currentframe in range(frames):
		print 'Applying mask to frame: ' + str(currentframe+1)

		# Load the image and mask
		if currentframe >= 9:
			image = IMAGETEMPLATE.replace('???','0'+str(currentframe+1))
			mask_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_0'+str(currentframe+1)+'_mask.csv'
		else:
			image = IMAGETEMPLATE.replace('???','00'+str(currentframe+1))
			mask_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_00'+str(currentframe+1)+'_mask.csv'

		masked_frames[:,:,currentframe] = fabio.open(image).data
		with open(mask_filename, 'r') as csvfile:
			reader = csv.reader(csvfile)
			mask = list(reader) # makes csv file a list of coordinates

		# Go through each pixel and change the peaks to nan - I'm sure there is a faster way
		for peak_pix in mask:
			#masked_frames[peak_pix[1],peak_pix[0],currentframe] = float('NaN')
			masked_frames[peak_pix[1],peak_pix[0],currentframe] = np.ma.masked

	# Save the masked frames, then delete
	print ' '
	for currentframe in range(frames):
		print 'Saving masked frames: ' + str(currentframe+1)
		if currentframe >= 9:
			frame_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_0'+str(currentframe+1)+'_masked.txt'
		else:
			frame_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_00'+str(currentframe+1)+'_masked.txt'
		np.savetxt(frame_filename,masked_frames[:,:,currentframe],fmt='%5u')

	print ' '
	print 'Masks are applied - now setting up radial integration'
	return []

# This could be better probably
def GetBackground(image):
	data = fabio.open(image)
	background = data.data.mean()
	return background

# This does a radial integration
def IntegrateFrames(nx,ny,beam_x,beam_y,frames,run,distance,pixel_size,wavelength,RESOLUTION,INTEGRATION_DIRECTORY,PREFIX):

	y_int,x_int = np.ogrid[:ny,:nx]
	x_int -= int(round(beam_x))
	y_int -= int(round(beam_y))
	r = np.sqrt(x_int*x_int+y_int*y_int)
	resolution_vector = np.arange(RESOLUTION[0], RESOLUTION[1], -0.01)
	BINS = np.size(resolution_vector)
	pixel_bins =(distance/pixel_size)*np.tan(2*np.arcsin(wavelength/(2*resolution_vector)))

	I = np.zeros([BINS-1,frames])
	for currentframe in range(frames):
		print 'Integrating frame: ' + str(currentframe+1)
		if currentframe >= 9:
			frame_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_0'+str(currentframe+1)+'_masked.txt'
		else:
			frame_filename = INTEGRATION_DIRECTORY+'/'+PREFIX+'_'+str(run)+'_00'+str(currentframe+1)+'_masked.txt'
		image = np.loadtxt(frame_filename)
		R,I[:,currentframe] = Integrate_Hist(image,r,pixel_bins)
	resolution = wavelength/(2*np.sin(0.5*np.arctan(R*pixel_size/distance)))

	# Save data 
	np.savetxt(INTEGRATION_DIRECTORY+'/'+PREFIX+'_integration.txt',np.column_stack((resolution,I)),fmt='%6.3f')

	return []

# This routine was ripped from a presentation:
# Introduction to PyFAI
# The European Synchrotron (ESRF)
# Jerome Kieffer - 05/06/2014
# It is very fast and accurate.
def Integrate_Hist(img, radius, bins):
	u_hist, edges = np.histogram(radius, bins)
	w_hist, edges = np.histogram(radius, bins, weights=img)
	positions = (edges[:-1]+edges[1:])/2.0
	integrated = w_hist/(1.0*u_hist)
	integrated[u_hist==0] = 0

	return positions, integrated



# Generates an XDS.INP that works for data collected at CHESS using the Pilatus 6M detector 
# located at the F1 station. It is based on the template from the MacCHESS website but with 
# several modifications made based on the specific crystal - apoferritin with a F432 (209)
# space group. Also, changes were made based on tutorials for how to set-up an XDS.INP 
# specifically for a Pilatus detector. 
# 	The SENSOR_THICKNESS was changed to 0.32 
# 	CHESS uses a non-standard ROTATION_AXIS= -1 0 0 
# 		Changes made to reflect the smaller point-spread-function (spot size) on the pilatus detectors:
# 	SEPMIN = 4
# 	CLUSTER_RADIUS = 2
#		I'm not entirely sure what these are, but it is recommmended to change them for the Pilatus. 	
# 		It is making a grid finer by increasing the amount of points. They are parameters used
#		in the integration step
#	NUMBER_OF_PROFILE_GRID_POINTS_ALONG_ALPHA/BETA=13 
# 	NUMBER_OF_PROFILE_GRID_POINTS_ALONG_GAMMA= 13


def GenerateXDS(directory, template, run, first_frame, last_frame, unit_cell, wavelength, distance, \
				beam_x, beam_y, angle_increment, job, resolution, \
				unit_cell_aaxis='none', \
				unit_cell_baxis='none', \
				unit_cell_caxis='none'):
	# Unit cell A-axis
	if unit_cell_aaxis == 'none':
		unit_cell_aaxis_line = '! UNIT_CELL_A-AXIS= no input'
	else:
		unit_cell_aaxis_line = ' UNIT_CELL_A-AXIS= '+unit_cell_aaxis

	# Unit cell B-axis
	if unit_cell_baxis == 'none':
		unit_cell_baxis_line = '! UNIT_CELL_B-AXIS= no input'
	else:
		unit_cell_baxis_line = ' UNIT_CELL_B-AXIS= '+unit_cell_baxis

	# Unit cell C-axis
	if unit_cell_caxis == 'none':
		unit_cell_caxis_line = '! UNIT_CELL_C-AXIS= no input'
	else:
		unit_cell_caxis_line = ' UNIT_CELL_C-AXIS= '+unit_cell_caxis

	xds_file = open(directory+'/XDS.INP','w')
	xds_file.write('DETECTOR=PILATUS  MINIMUM_VALID_PIXEL_VALUE=0  OVERLOAD=2000000000\n\
 DIRECTION_OF_DETECTOR_X-AXIS= 1.0 0.0 0.0\n\
 DIRECTION_OF_DETECTOR_Y-AXIS= 0.0 1.0 0.0\n\
 TRUSTED_REGION=0.0 1.15 !Relative radii limiting trusted detector region\n\
 SENSOR_THICKNESS=0.32\n\
 UNTRUSTED_RECTANGLE= 487  495     1 2527  ! these do not have to be specified\n\
 UNTRUSTED_RECTANGLE= 981  989     1 2527  ! but it is better\n\
 UNTRUSTED_RECTANGLE=1475 1483     1 2527  ! ...\n\
 UNTRUSTED_RECTANGLE=1969 1977     1 2527  ! painstakingly checked KD 2012-01-14\n\
 UNTRUSTED_RECTANGLE=   1 2463   195  213  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463   407  425  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463   619  637  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463   831  849  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  1043 1061  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  1255 1273  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  1467 1485  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  1679 1697  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  1891 1909  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  2103 2121  ! ...\n\
 UNTRUSTED_RECTANGLE=   1 2463  2315 2333  ! ...\n\
NX=2463 NY=2527 QX=0.172 QY=0.172   !Pilatus 6M\n\
! MAXIMUM_NUMBER_OF_PROCESSORS= 4 \n\
! MAXIMUM_NUMBER_OF_JOBS=16 \n\
!====================== JOB CONTROL PARAMETERS =============================== \n\
JOB= ' + job + '\n\
!====================== GEOMETRICAL PARAMETERS =============================== \n\
 ORGX=   '+beam_x+'  ORGY=   '+beam_y+'\n\
 DETECTOR_DISTANCE= '+distance+' \n\
ROTATION_AXIS= -1.0 0.0 0.0          ! A1, F1, F2, F3\n\
OSCILLATION_RANGE= '+angle_increment+'           ! degrees (>0)  *** Edit this *** \n\
X-RAY_WAVELENGTH= '+wavelength+'        ! Angstroem, F1 as of September 2015 \n\
 INCIDENT_BEAM_DIRECTION=0.0 0.0 1.0\n\
 FRACTION_OF_POLARIZATION=0.90 !default=0.5 for unpolarized beam\n\
 POLARIZATION_PLANE_NORMAL= 0.0 1.0 0.0\n\
!======================= CRYSTAL PARAMETERS =================================\n\
SPACE_GROUP_NUMBER=209  !0 for unknown crystals; cell constants are ignored.\n\
UNIT_CELL_CONSTANTS= '+unit_cell+'    90 90 90\n'\
+unit_cell_aaxis_line+'\n'\
+unit_cell_baxis_line+'\n'\
+unit_cell_caxis_line+'\n\
!==================== SELECTION OF DATA IMAGES ==============================\n\
 NAME_TEMPLATE_OF_DATA_FRAMES= '+template+' \n\
 DATA_RANGE='+first_frame+'  '+last_frame+'      !Numbers of first and last data image collected\n\
 BACKGROUND_RANGE='+first_frame+' '+last_frame+'  !Numbers of first and last data image for background\n\
 SPOT_RANGE='+first_frame+'   '+last_frame+'      !First and last data image number for finding spots\n\
!====================== INDEXING PARAMETERS =================================\n\
SEPMIN=4.0 CLUSTER_RADIUS=2\n\
!============== DECISION CONSTANTS FOR FINDING CRYSTAL SYMMETRY =============\n\
!Decision constants for detection of lattice symmetry (IDXREF, CORRECT)\n\
 MAX_CELL_AXIS_ERROR=0.03 ! Maximum relative error in cell axes tolerated\n\
 MAX_CELL_ANGLE_ERROR=2.0 ! Maximum cell angle error tolerated\n\
!Decision constants for detection of space group symmetry (CORRECT).\n\
!Resolution range for accepting reflections for space group determination in\n\
!the CORRECT step. It should cover a sufficient number of strong reflections.\n\
 TEST_RESOLUTION_RANGE=8.0 4.5\n\
 MIN_RFL_Rmeas= 50 ! Minimum #reflections needed for calculation of Rmeas\n\
 MAX_FAC_Rmeas=2.0 ! Sets an upper limit for acceptable Rmeas\n\
 !================= PARAMETERS CONTROLLING REFINEMENTS =======================\n\
!REFINE(IDXREF)=BEAM AXIS ORIENTATION CELL !POSITION\n\
!REFINE(INTEGRATE)=!POSITION BEAM ORIENTATION CELL !AXIS\n\
REFINE(CORRECT)=BEAM ORIENTATION CELL AXIS\n\
!================== CRITERIA FOR ACCEPTING REFLECTIONS ======================\n\
 VALUE_RANGE_FOR_TRUSTED_DETECTOR_PIXELS= 6000 30000 !Used by DEFPIX\n\
				   !for excluding shaded parts of the detector.\n\
 INCLUDE_RESOLUTION_RANGE='+str(resolution[1])+' '+str(resolution[0])+' !Angstroem; used by DEFPIX,INTEGRATE,CORRECT\n\
!============== INTEGRATION AND PEAK PROFILE PARAMETERS =====================\n\
NUMBER_OF_PROFILE_GRID_POINTS_ALONG_ALPHA/BETA=13 !used by: INTEGRATE\n\
NUMBER_OF_PROFILE_GRID_POINTS_ALONG_GAMMA= 13     !used by: INTEGRATE')
	return directory

# Runs xds
def RunXDS(directory):
	directory_old = os.getcwd()
	os.chdir(directory)
	os.system('xds_par')	# This runs XDS - the _par indicates using the parallelized code
	# GXPARM.XDS stores information that is optimized by the INTEGRATE and CORRECT steps. XPARM.XDS is the input parameters.
	# It is generally recommended to rerun the INTEGRATE and CORRECT steps with
	# These parameters include unit cell parameters 
	os.system('mv XPARM.XDS XPARM_1.XDS')
	os.system('mv GXPARM.XDS XPARM.XDS')
	os.system("sed -i 's/JOB= XYCORR INIT COLSPOT IDXREF DEFPIX XPLAN INTEGRATE CORRECT/JOB= INTEGRATE CORRECT/' XDS.INP")
	os.system('xds_par')
	os.chdir(directory_old)
	return directory
