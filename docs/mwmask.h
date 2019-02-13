/*
  MWMASK.H - Header for diffraction image manipulation routines.


  Modifications:

	5/23/94 	Change the ignore tag and overload tag to 32767 from 
			65535
*/


/*
 * Includes:
 */

#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<malloc.h>
#include<string.h>
#include<errno.h>

/*
 * Defines:
 */

/*
 * Data set selection:
 */

/*#define SNCPS                           /* sncps data set */
#define SNAB                            /* snab data set */
/*#define SNC                             /* snc data set */
/*#define LYS                             /* lys data set */

/*
 * I/O specifications:
 */

#define DOS_BYTE_ORDER 1		/* DOS byte order? 1=yes */
#define LINESIZE 120			/* # chars in input line */

/*
 * Diffraction image specifications:
 */

#define DEFAULT_PIXEL_SIZE_MM 0.08	/* Pixel size in mm */
#define DEFAULT_VSIZE 1024		/* # Vertical pixels */
#define DEFAULT_HSIZE 1024		/* # Horizontal pixels */
#define DEFAULT_HEADER_LENGTH 4096	/* TV6 TIFF image header length */
#define DEFAULT_IMAGELENGTH 1048576	/* TV6 TIFF image #pixels */
#define DEFAULT_OVERLOAD_TAG 0x7ffe	/* 32766 */
#define DEFAULT_IGNORE_TAG 0x7fff	/* 32767 */
/* #define DEFAULT_OVERLOAD_TAG 0xffff     /* 65535 */
/* #define DEFAULT_IGNORE_TAG 0xffff       /* 65535 */
#define MAX_OVERLOADS 10000		/* Max # of overloads in an image */
#define MAX_PEAKS 20000			/* Max # of peaks in an image */
#define MAX_IMAGE_DATA_VALUE 32767	/* Maximum value of pixel in image */
#define DEFAULT_IMAGE_ORIGIN 512	/* Default x and y for image origin */

/*
 * Lattice specifications:
 */

#define DEFAULT_LATTICE_IGNORE_TAG -32768	/* -32768 */
#define DEFAULT_XVOXELS 64		/* Lattice size in x */
#define DEFAULT_YVOXELS 64		/* Lattice size in y */
#define DEFAULT_ZVOXELS 64		/* Lattice size in z */
#define DEFAULT_LATTICE_ORIGIN 31       /* Default i,j,k for lattice */
					/* origin */
#define DEFAULT_BOUND_MIN -30		/* Lower bound in all dims */
#define DEFAULT_BOUND_MAX 30		/* Upper bound in all dims */
#define DEFAULT_LAT_INNER_RADIUS 0      /* Inner radius of lattice */
#define DEFAULT_LAT_OUTER_RADIUS 30     /* Outer radius of lattice */
#define DEFAULT_LATTICE_MASK_TAG -32768	/* Mask tag for lattice object */
#define DEFAULT_SAMPLE_PITCH 5		/* Sample image every Nth */
					/* pixel in x and y for voxel 
					/* generation */
#define DEFAULT_MINRANGE 0.15            /* Minimum valid dist. to */
					/*   Bragg peak */
#define DEFAULT_INNER_RADIUS_LT 0.	/* Inner radius hreshold for mapping */
					/* voxels to a lattice */
#define DEFAULT_OUTER_RADIUS_LT 1.	/* Outer radius threshold for mapping*/
					/* voxels to a lattice */

/* 
 * Shell image specifications:
 */

#define DEFAULT_SHIM_SIZE 256           /* Default size for shell */
#define DEFAULT_SHIM_IGNORE_TAG 0       /* Ignore tag for shell image */

/*
 * One-dimensional "rfile" specifications:
 */

#define DEFAULT_RFILE_MASK_TAG 0	/* Mask tag for rfiles */
#define DEFAULT_RFILE_LENGTH 724	/* Default rfile length for writing */
#define MAX_RFILE_LENGTH 2000		/* Maximum length of rfile */

/*
 * Old Bragg peak and overflow "masking" specifications:
 */

#define PUNCH_TAG 0x7ffe		/* 32766 */
#define MAX_MASK_PIXELS 10000		/* Max # of pixels in a mask */
#define DEFAULT_INNER_RADIUS 0		/* Inner radius of annular mask */
#define DEFAULT_OUTER_RADIUS 2		/* Outer radius of annular mask */
#define DEFAULT_OVERLOAD_RADIUS 5	/* Radius for overload punch-out */

/*
 * Mode filter specifications:
 */

#define DEFAULT_MODE_MASK_SIZE 11       /* Pixel size of mode mask */
#define DEFAULT_MODE_BIN_SIZE 1         /* Pixel value bin size for */
					/* mode filter */
#define DEFAULT_MODE_DIMENSION 10       /* Dimension of mode filter */

/*
 * Window specifications:
 */

#define DEFAULT_WINDOW_LOWER 0		/* Default lower r,c of window */
#define DEFAULT_WINDOW_UPPER 1024	/* Default upper r,c of window */

/*
 * Obsolete diffuse features specifications:
 */

#define MAX_DIFFUSE_FEATURES 100        /* Maximum number of diffuse feats. */

/*
 * Butterfly specifications:
 */

#define DEFAULT_BUTTERFLY_OFFSET 0
#define DEFAULT_OPENING_ANGLE 90

/*
 * Smoothing specifications:
 */

#define DEFAULT_WEIGHTS_DIMENSION 10	/* Dimension of weights matrix */
#define MAX_WEIGHTS_DIMENSION 50	/* Maximum smoothing weights matrix */
					/* dim */

/*
 * Simulated peaks image specifications:
 */

#define DEFAULT_AMPLITUDE 500
#define DEFAULT_PITCH 60

/*
 * Experimental defaults:
 */
#define DEFAULT_SPINDLE_DEG 0		/* Spindle position */
#define DEFAULT_POLARIZATION 0.85	/* Beam polarization */
#define DEFAULT_POLARIZATION_OFFSET 0   /* Polarization offset angle */

/*
 * Miscellaneous:
 */

#define PI 3.14159			/* PI */
#define VALUE_ALLOCATED 1		/* Flag for imdiff->value allocated */
#define RDATA_MALLOC_FACTOR 7		/* Bigger than 2*PI */
#define POLARIZATION_CORRECTION_THRESHOLD .01 /* Threshold for */
					      /* correcting for */
					      /* polarization */

/* 
 * Data-set dependent defines
 */

#ifdef SNCPS
#define DEFAULT_WAVELENGTH 0.92       /* Wavelength for sncps */
#define DEFAULT_DISTANCE_MM 58.0	/* Sample-detector distance in */
					/* mm for sncps*/
#define DEFAULT_X_BEAM 42.550		/* Beam position in x (denzo) */
					/* for sncps*/
#define DEFAULT_Y_BEAM 46.2		/* Beam position in y (denzo) */
					/* for sncps */
#define DEFAULT_CASSETTE_ROTX 0.18      /* Cassette rotx for sncps */
#define DEFAULT_CASSETTE_ROTY -.67      /* Cassette roty for sncps */
#define DEFAULT_CASSETTE_ROTZ -.5       /* Cassette rotz for sncps */
#define DEFAULT_CELL_A 48.154           /* A for sncps */
#define DEFAULT_CELL_B 48.154           /* B for sncps */
#define DEFAULT_CELL_C 63.880           /* C for sncps */
#define DEFAULT_CELL_ALPHA 90.0         /* B-C angle for sncps */
#define DEFAULT_CELL_BETA 90.0          /* C-A angle for sncps */
#define DEFAULT_CELL_GAMMA 90.0         /* A-B angle for sncps */
#else
#ifdef SNAB
#define DEFAULT_WAVELENGTH 0.908      /* Wavelength for snab, snc, */
                                        /*lys*/
#define DEFAULT_DISTANCE_MM 67.44	/* Sample-detector distance in */
					/* mm for snb*/
#define DEFAULT_X_BEAM 39.728		/* Beam position in x (denzo) */
					/* for snb*/
#define DEFAULT_Y_BEAM 42.540		/* Beam position in y (denzo) */
					/* for snb */
#define DEFAULT_CASSETTE_ROTX -.24    /* Cassette rotx for snb(MEW10P100)*/
#define DEFAULT_CASSETTE_ROTY -.27    /* Cassette roty for snb(MEW10P100)*/
#define DEFAULT_CASSETTE_ROTZ .95     /* Cassette rotz for snb(MEW10P100)*/
#define DEFAULT_CELL_A 48.333           /* A for snb */
#define DEFAULT_CELL_B 48.333           /* B for snb */
#define DEFAULT_CELL_C 63.353           /* C for snb */
#define DEFAULT_CELL_ALPHA 90.0         /* B-C angle for snb */
#define DEFAULT_CELL_BETA 90.0          /* C-A angle for snb */
#define DEFAULT_CELL_GAMMA 90.0         /* A-B angle for snb */
#else
#ifdef SNC
#define DEFAULT_WAVELENGTH 0.908      /* Wavelength for snab, snc, */
                                        /*lys*/
#define DEFAULT_CASSETTE_ROTX -2.8    /* Cassette rotx for snc */
#define DEFAULT_CASSETTE_ROTY -.49    /* Cassette roty for snc */
#define DEFAULT_CASSETTE_ROTZ .8      /* Cassette rotz for snc */
#define DEFAULT_DISTANCE_MM 57.40	/* Sample-detector distance in */
					/* mm for snc*/
#define DEFAULT_X_BEAM 40.04		/* Beam position in x (denzo) */
					/* for snc*/
#define DEFAULT_Y_BEAM 42.60		/* Beam position in y (denzo) */
					/* for snc */
#define DEFAULT_CELL_A 48.458           /* A for snc */
#define DEFAULT_CELL_B 48.458           /* B for snc */
#define DEFAULT_CELL_C 63.398           /* C for snc */
#define DEFAULT_CELL_ALPHA 90.0         /* B-C angle for snc */
#define DEFAULT_CELL_BETA 90.0          /* C-A angle for snc */
#define DEFAULT_CELL_GAMMA 90.0         /* A-B angle for snc */
#else
#ifdef LYS
#define DEFAULT_WAVELENGTH 0.908      /* Wavelength for snab, snc, */
                                        /*lys*/
#define DEFAULT_DISTANCE_MM 57.26	/* Sample-detector distance in */
                                        /* mm  for lys*/
#define DEFAULT_X_BEAM 39.637		/* Beam position in x (denzo) */
					/* for lys*/
#define DEFAULT_Y_BEAM 42.516		/* Beam position in y (denzo) */
					/* for lys */
#define DEFAULT_CELL_A 79.120           /* A for lys */
#define DEFAULT_CELL_B 79.120           /* B for lys */
#define DEFAULT_CELL_C 38.058           /* C for lys */
#define DEFAULT_CELL_ALPHA 90.0         /* B-C angle for lys */
#define DEFAULT_CELL_BETA 90.0          /* C-A angle for lys */
#define DEFAULT_CELL_GAMMA 90.0         /* A-B angle for lys */
/* NOTE NO CASSETTE ANGLES */
#endif
#endif
#endif
#endif

/*
 * Structures and typedefs:
 */

typedef long IJKCOORDS_DATA;
typedef float XYZCOORDS_DATA;
typedef short RCCOORDS_DATA;
typedef float RFILE_DATA_TYPE;
typedef float LATTICE_DATA_TYPE;
typedef short SHIM_DATA_TYPE;
typedef short IMAGE_DATA_TYPE;
typedef float WEIGHTS_DATA_TYPE;
typedef struct {
  IMAGE_DATA_TYPE *value;	/* Pixel value */
  char allocate_flag;           /* Has the array been allocated? */
  size_t count;                 /* Number of pixels in value array */
} RDATA_DATA_TYPE;
struct rccoords		/* 2D coordinates in type short */
{
  RCCOORDS_DATA r;	        /* Row coordinate */
  RCCOORDS_DATA c;	        /* Column coordinate */
};
struct xycoords		/* 2D coordinates in type float */
{
  XYZCOORDS_DATA x;	        /* X coordinate */
  XYZCOORDS_DATA y;	        /* Y coordinate */ 
};
struct ijkcoords
{
  IJKCOORDS_DATA i;
  IJKCOORDS_DATA j;
  IJKCOORDS_DATA k;
};
struct xyzcoords	/* 3D coordinates in type float */
{
  XYZCOORDS_DATA x;	        /* X coordinate */
  XYZCOORDS_DATA y;	        /* Y coordinate */
  XYZCOORDS_DATA z;	        /* Z coordinate */
};
struct xyzmatrix
{
  XYZCOORDS_DATA xx;
  XYZCOORDS_DATA xy;
  XYZCOORDS_DATA xz;
  XYZCOORDS_DATA yx;
  XYZCOORDS_DATA yy;
  XYZCOORDS_DATA yz;
  XYZCOORDS_DATA zx;
  XYZCOORDS_DATA zy;
  XYZCOORDS_DATA zz;
};
struct voxel		/* 3D coordinate plus value */
{	
  struct xyzcoords pos;	        /* Position (float) */
  float value;		        /* Value at (x,y,z) */
};
struct bounds		/* Upper and lower bounds structure */
{
  LATTICE_DATA_TYPE min;	/* Minimum value */
  LATTICE_DATA_TYPE max;	/* Maximum value */
};
struct unit_cell	/* Unit cell geometry structure */
{
  float a;	                /* a-axis length in angstroms */
  float b;	                /* b-axis length in angstroms */
  float c;	                /* c-axis length in angstroms */
  float alpha;	                /* b-c angle */
  float beta;	                /* a-c angle */
  float gamma;	                /* a-b angle */
};

/*
 * Diffuse feature data type:
 */

typedef struct 
{
  struct rccoords pixel_pos;
  float radius;
  long peak_value;
  IMAGE_DATA_TYPE average_value;
} DIFFUSE_FEATURE;

/*
 * Diffraction image data type:
 */

typedef struct 
{
  char *filename;
  FILE *infile;
  FILE *outfile;
  char *header;		        /* Image header (typically TIFF) */
  size_t header_length;	        /* Length of image header (4096) */
  IMAGE_DATA_TYPE *image;	/* Pointer to image */
  size_t image_length;	        /* Total number of pixels in image */
  short vpixels;		/* Number of vertical pixels */
  short hpixels;		/* Number of horizontal pixels */
  IMAGE_DATA_TYPE ignore_tag;   /* Ignore this pixel value */
  struct rccoords *overload;    /* Pointer to overload coords */
  IMAGE_DATA_TYPE overload_tag; /* Pixel value indicating ovld */
  LATTICE_DATA_TYPE lattice_ignore_tag; /* Lattice ign. tag */
  long overload_count;	        /* Number of overload pixels in img */
  struct xycoords *peak;	/* X-Y coords of Bragg peak posns */
  long peak_count;	        /* Number of peaks in image */
  struct rccoords *mask;	/* Pixel-by-pixel peak mask shape */
  long mask_count;	        /* Number of pixels in mask shape */
  short mask_inner_radius;      /* Inner radius of annular mask */
  short mask_outer_radius;      /* Outer radius of annular mask */
  IMAGE_DATA_TYPE mask_tag;     /* Value which mask puts in image */
  IMAGE_DATA_TYPE punch_tag;
  struct rccoords pos;	        /* Coordinates of current pixel */
  float spindle_deg;	        /* Spindle angle for this image */
  struct voxel *map3D;	        /* List of voxels */
  float pixel_size_mm;	        /* Size of square detector pixel (mm) */
  float distance_mm;	        /* Sample-detector distance (mm) */
  struct xycoords beam_mm;      /* Beam position (mm) */
  float polarization;	        /* Beam polarization */
  float polarization_offset;    /* Offset angle for polarization */
				/* correction */
  RFILE_DATA_TYPE *imscaler;    /* Scale as fn of radius */
  RFILE_DATA_TYPE *imoffsetr;   /* Offset as fn of radius */
  RFILE_DATA_TYPE *rfile;	/* Pointer to rfile */
  size_t rfile_length;	        /* Length of rfile */
  RFILE_DATA_TYPE rfile_mask_tag;/* Tag for masked rfile value */
  RFILE_DATA_TYPE avg_pixel_value;/* Single average value */
  struct rccoords origin;       /* Origin of image */
  char error_msg[LINESIZE];     /*Error message string */
  WEIGHTS_DATA_TYPE *weights;   /* Smoothing weights matrix */
  size_t weights_height;        /* Height of weights matrix */
  size_t weights_width;         /* Width of weights matrix */
  size_t mode_height;           /* Height of mode matrix */
  size_t mode_width;            /* Width of mode matrix */
  IMAGE_DATA_TYPE mode_binsize; /* Pixel value bin size for mode */
				/* filter */
  RDATA_DATA_TYPE *rdata;	/* Radial indexing image */
  size_t rdata_radius;	        /* Maximum radius in rdata */
  struct rccoords window_lower; /* Upper left corner of window */
  struct rccoords window_upper; /* Lower right corner of window */
  IMAGE_DATA_TYPE lower_threshold;
  IMAGE_DATA_TYPE upper_threshold;
  DIFFUSE_FEATURE *feature;     /* List of diffuse features */
  size_t feature_count;
  struct unit_cell cell;        /* Unit cell data type */
  float wavelength;
  struct xyzmatrix u;
  struct xyzcoords cassette;    /* Cassette rotation angles */
  float amplitude;              /* Amplitude of noise or fluctuation */
  float pitch;                  /* Pitch of fluctuation */
} DIFFIMAGE;

/*
 * Lattice data type:
 */

typedef struct {
  char *filename;
  FILE *infile;
  FILE *outfile;
  char error_msg[LINESIZE];     /*Error message string */
  struct voxel *map3D;	        /* Pointer to list of voxels */
  LATTICE_DATA_TYPE *lattice;   /* Pointer to lattice */
  size_t xvoxels;		/* Number of x-voxels */
  size_t yvoxels;		/* Number of y-voxels */
  size_t zvoxels;		/* Number of z-voxels */
  size_t xyvoxels;              /* Number of voxels in an xy section */
  double xscale;                /* Scale factor for x */
  double yscale;                /* Scale factor for y */
  double zscale;                /* Scale factor for z */
  size_t lattice_length;	/* Number of voxels */
  struct bounds xbound;	        /* Max and min of x-coord */
  struct bounds ybound;	        /* Max and min of y-coord */
  struct bounds zbound;	        /* Max and min of z-coord */
  struct bounds valuebound;     /* Max and min of voxel value */
  struct unit_cell cell;        /* Unit cell descriptor */
  LATTICE_DATA_TYPE mask_tag;   /* Masked voxel value tag */
  struct ijkcoords origin;      /* Origin voxel position */
  RFILE_DATA_TYPE *rfile;       /* Radial distribution function */
  size_t rfile_length;	        /* Number of rfile values */
  struct xyzcoords minrange;    /* Minimum valid distances to Bragg */
				/* peak */
  size_t inner_radius;          /* Inner radius threshold for lattice */
				/* calcs */
  size_t outer_radius;          /* Outer radius threshold for lattice */
				/* calcs */
  float peak;                   /* Gaussian peak value */
  float width;                  /* Gaussian width */
  SHIM_DATA_TYPE *shim;        /* Shell image */
  size_t shim_length;           /* Total number of pixels in shell image */
  size_t shim_hsize;            /* Number of horizontal pixels in */
				/* shell image */
  size_t shim_vsize;            /* Number of vertical pixels in shell */
				/* image */
  float wavelength;             /* X-ray wavelength */
  struct ijkcoords *symvec;     /* Symmetry related vectors */
  size_t symop_count;           /* Number of symmetry operations */
				/* performed */
  size_t symop_index;           /* Index of selected symmetry */
				/* operation */
} LAT3D;

/*
 * Subroutines:
 */

int lavgim(DIFFIMAGE *imdiff);
int lavgr(LAT3D *lat);
int lavgrf(DIFFIMAGE *imdiff1);
int lavgrim(DIFFIMAGE *imdiff);
int lavgrlt(LAT3D *lat);
int lavgsqim(DIFFIMAGE *imdiff);
int lavsqrim(DIFFIMAGE *imdiff);
int lavsqrlt(LAT3D *lat);
int lbuttim(DIFFIMAGE *imdiff);
int lchbyte(void *ptr, size_t packet_size, size_t list_length);
int lconstim(DIFFIMAGE *imdiff);
int lconstlt(LAT3D *lat);
int lconstrf(DIFFIMAGE *imdiff);
int lculllt(LAT3D *lat);
int lcutim(DIFFIMAGE *imdiff);
int ldf2im(DIFFIMAGE *imdiff);
int ldfrflt(LAT3D *lat1, LAT3D *lat2);
int ldfsqrlt(LAT3D *lat1, LAT3D *lat2);
int ldivlt(LAT3D *lat1, LAT3D *lat2);
int ldivrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lexplt(LAT3D *lat);
void lfft(float *data,int *nn,int ndim,int isign);
int lfreeim(DIFFIMAGE *imdiff);
int lfreelt(LAT3D *lat);
int lgausslt(LAT3D *lat);
int lgensv(DIFFIMAGE *imdiff);
int lgetanls(DIFFIMAGE *imdiff);
int lgetmat(DIFFIMAGE *imdiff);
int lgetovld(DIFFIMAGE *imdiff);
int lgetpks(DIFFIMAGE *imdiff);
DIFFIMAGE *linitim(void);
LAT3D *linitlt(void);
int lintdfim(DIFFIMAGE *imdiff);
int lmedim(DIFFIMAGE *imdiff);
size_t lmin(size_t arg1, size_t arg2);
int lminr(LAT3D *lat);
int lminrim(DIFFIMAGE *imdiff);
int lmodeim(DIFFIMAGE *imdiff);
int lmulim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lmullt(LAT3D *lat1, LAT3D *lat2);
int lmulrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lmulrfim(DIFFIMAGE *imdiff);
int lmulsclt(LAT3D *lat);
int lnign(DIFFIMAGE *imdiff);
int lnoiseim(DIFFIMAGE *imdiff);
int lnormim(DIFFIMAGE *imdiff);
int lpeakim(DIFFIMAGE *imdiff);
int lpolarim(DIFFIMAGE *imdiff);
int lpunch(DIFFIMAGE *imdiff);
int lpunchim(DIFFIMAGE *imdiff);
int lratioim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lreaddf(DIFFIMAGE *imdiff);
int lreadim(DIFFIMAGE *imdiff);
int lreadlt(LAT3D *lat);
int lreadrf(DIFFIMAGE *imdiff);
int lrf2lt(LAT3D *lat);
int lrmpkim(DIFFIMAGE *imdiff);
int lscaleim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lscalelt(LAT3D *lat1, LAT3D *lat2);
int lshim4lt(LAT3D *lat);
int lshimlt(LAT3D *lat);
int lsmthim(DIFFIMAGE *imdiff);
int lsolidlt(LAT3D *lat);
int lsubim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsublt(LAT3D *lat1, LAT3D *lat2);
int lsubrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsubrfim(DIFFIMAGE *imdiff);
int lsubrflt(LAT3D *lat);
int lsumim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsumlt(LAT3D *lat1, LAT3D *lat2);
int lsumrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsymlt(LAT3D *lat);
int ltagim(DIFFIMAGE *imdiff);
int ltaglt(LAT3D *lat);
int lthrshim(DIFFIMAGE *imdiff);
int ltordata(DIFFIMAGE *imdiff);
int lupdbd(LAT3D *lat);
int lwaveim(DIFFIMAGE *imdiff);
int lwindim(DIFFIMAGE *imdiff);
int lwritedf(DIFFIMAGE *imdiff);
int lwriteim(DIFFIMAGE *imdiff);
int lwritelt(LAT3D *lat);
int lwriterf(DIFFIMAGE *imdiff);
int lwritesh(LAT3D *lat);
int lxavgr(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lxavgrim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lxf1lt(LAT3D *lat);
int lxfmask(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
