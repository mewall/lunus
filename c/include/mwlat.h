/*
 * Includes:
 */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<errno.h>

/*
 * Defines:
 */


#define DEFAULT_HEADER_LENGTH 4096	/* TV6 TIFF image header length */
#define DEFAULT_IMAGELENGTH 1048576	/* TV6 TIFF image #pixels */
#define DEFAULT_SPINDLE_DEG 0		/* Spindle position */
#define DEFAULT_OVERLOAD_TAG 0xffff	/* 65535 */
#define DEFAULT_VSIZE 1024		/* # Vertical pixels */
#define DEFAULT_HSIZE 1024		/* # Horizontal pixels */
#define LINESIZE 120			/* # chars in input line */
#define MAX_OVERLOADS 10000		/* Max # of overloads in an image */
#define MAX_PEAKS 20000			/* Max # of peaks in an image */
#define MAX_MASK_PIXELS 10000		/* Max # of pixels in a mask */
#define MAX_RFILE_LENGTH 1500		/* Maximum length of rfile */
#define PUNCH_TAG 0xffff		/* 65535 */
#define DEFAULT_INNER_RADIUS 0		/* Inner radius of annular mask */
#define DEFAULT_OUTER_RADIUS 10		/* Outer radius of annular mask */
#define DEFAULT_SAMPLE_PITCH 5		/* Sample image every Nth pixel in
						x and y for voxel generation */
#define DEFAULT_DISTANCE_MM 56.4	/* Sample-detector distance in mm */
#define DEFAULT_X_BEAM 27.426		/* Beam position in x (denzo) */
#define DEFAULT_Y_BEAM 25.431		/* Beam position in y (denzo) */
#define DEFAULT_PIXEL_SIZE_MM 0.0508	/* Pixel size in mm */
#define PI 3.14159			/* PI */
#define DOS_BYTE_ORDER 1		/* DOS byte order? 1=yes */
#define DEFAULT_XVOXELS 101		/* Lattice size in x */
#define DEFAULT_YVOXELS 101		/* Lattice size in y */
#define DEFAULT_ZVOXELS 101		/* Lattice size in z */
#define DEFAULT_BOUND_MIN -.5		/* Lower bound in all dims */
#define DEFAULT_BOUND_MAX .5		/* Upper bound in all dims */
#define DEFAULT_LATTICE_MASK_TAG 0	/* Mask tag for lattice object */
#define DEFAULT_IMAGE_ORIGIN 512	/* Default x and y for image origin */
/*
 * Structures and typedefs:
 */

typedef long IJKCOORDS_DATA;
typedef float XYZCOORDS_DATA;
typedef short RCCOORDS_DATA;
typedef float RFILE_DATA_TYPE;
typedef float LATTICE_DATA_TYPE;

struct rccoords		/* 2D coordinates in type short */
{
	RCCOORDS_DATA r;	/* Row coordinate */
	RCCOORDS_DATA c;	/* Column coordinate */
};
struct xycoords		/* 2D coordinates in type float */
{
	XYZCOORDS_DATA x;	/* X coordinate */
	XYZCOORDS_DATA y;	/* Y coordinate */ 
};
struct ijkcoords
{
	IJKCOORDS_DATA i;
	IJKCOORDS_DATA j;
	IJKCOORDS_DATA k;
};
struct xyzcoords	/* 3D coordinates in type float */
{
	XYZCOORDS_DATA x;	/* X coordinate */
	XYZCOORDS_DATA y;	/* Y coordinate */
	XYZCOORDS_DATA z;	/* Z coordinate */
};
struct voxel		/* 3D coordinate plus value */
{	
	struct xyzcoords pos;	/* Position (float) */
	float value;		/* Value at (x,y,z) */
};
struct bounds		/* Upper and lower bounds structure */
{
	LATTICE_DATA_TYPE min;	/* Minimum value */
	LATTICE_DATA_TYPE max;	/* Maximum value */
};
struct unit_cell	/* Unit cell geometry structure */
{
	float a;	/* a-axis length in angstroms */
	float b;	/* b-axis length in angstroms */
	float c;	/* c-axis length in angstroms */
	float alpha;	/* b-c angle */
	float beta;	/* a-c angle */
	float gamma;	/* a-b angle */
};

/*
 * Diffraction image data type:
 */

typedef struct {
		char *filename;
		FILE *infile;
		FILE *outfile;
		char *header;		/* Image header (typically TIFF) */
		size_t header_length;	/* Length of image header (4096) */
		unsigned short *image;	/* Pointer to image */
		size_t image_length;	/* Total number of pixels in image */
		short vpixels;		/* Number of vertical pixels */
		short hpixels;		/* Number of horizontal pixels */
		struct rccoords *overload;   /* Pointer to overload coords */
		unsigned short overload_tag; /* Pixel value indicating ovld */
		long overload_count;	/* Number of overload pixels in img */
		struct xycoords *peak;	/* X-Y coords of Bragg peak posns */
		long peak_count;	/* Number of peaks in image */
		struct rccoords *mask;	/* Pixel-by-pixel peak mask shape */
		long mask_count;	/* Number of pixels in mask shape */
		short mask_inner_radius;/* Inner radius of annular mask */
		short mask_outer_radius;/* Outer radius of annular mask */
		unsigned short mask_tag;/* Value which mask puts in image */
		struct rccoords pos;	/* Coordinates of current pixel */
		float spindle_deg;	/* Spindle angle for this image */
		struct voxel *map3D;	/* List of voxels */
		float pixel_size_mm;	/* Size of square detector pixel (mm) */
		float distance_mm;	/* Sample-detector distance (mm) */
		struct xycoords beam_mm;/* Beam position (mm) */
		RFILE_DATA_TYPE *rfile;	/* Pointer to rfile */
		size_t rfile_length;	/* Length of rfile */
		struct rccoords origin; /* Origin of image */
} DIFFIMAGE;

/* 
 * Diffraction image type with I/O:
 */

typedef struct {
		DIFFIMAGE *imdiff;	/* Diffraction image */ 
		FILE *infile;		/* Input file */
		FILE *outfile;		/* Output file */
} IO_DIFFIMAGE;

/*
 * 3D lattice data type:
 */

typedef struct {
		char *filename;
		FILE *infile;
		FILE *outfile;
		struct voxel *map3D;	/* Pointer to list of voxels */
		LATTICE_DATA_TYPE *lattice; /* Pointer to lattice */
		size_t xvoxels;		/* Number of x-voxels */
		size_t yvoxels;		/* Number of y-voxels */
		size_t zvoxels;		/* Number of z-voxels */
		size_t lattice_length;	/* Number of voxels */
		struct bounds xbound;	/* Max and min of x-coord */
		struct bounds ybound;	/* Max and min of y-coord */
		struct bounds zbound;	/* Max and min of z-coord */
		struct bounds valuebound; /* Max and min of voxel value */
		struct unit_cell cell;  /* Unit cell descriptor */
		LATTICE_DATA_TYPE mask_tag;/* Masked voxel value tag */
		struct ijkcoords origin;/* Origin voxel position */
		RFILE_DATA_TYPE *rfile;/* Radial distribution function */
		size_t rfile_length;	/* Number of rfile values */
} LAT3D;

/*
 * Subroutines:
 */

short lgetanls(DIFFIMAGE *imdiff);	/* Get mask coordinates of annulus */
short lgetovld(DIFFIMAGE *imdiff);	/* Get overload coordinates */
short lgetpks(DIFFIMAGE *imdiff);	/* Get peak positions */
short lpunch(DIFFIMAGE *imdiff);	/* Punch out mask around peaks */
short lchbyte(void *ptr, size_t packet_size, size_t list_length);
short lgensv(DIFFIMAGE *imdiff);
short lupdbd(LAT3D *lat);
