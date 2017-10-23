/*
  LUNUS.H - Header for diffraction image manipulation routines.


  Modifications:

	5/23/94 	Change the ignore tag and overload tag to 32767 from 
			65535
	7/3/14		Change all tags to 1048577
	sometime before 2/24/15: Change all tags to 65535
    5/2/16: Changing ignore, overload, and punch tags to 65535, 65534, 65533 respectively.
*/


/*
 * Includes:
 */

#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>
#include<string.h>
#include<errno.h>

/*
 * Defines:
 */

/*
 * Data set selection:
 */

/*#define EIGER				  /* for all Feb 2014 CHESS F1 data */
/*#define P200K				  /* Pilatus 200k; CHESS June 2014 */
#define P6M				  /* Pilatus 6M, lysozyme, CHESS July 2015 */

/*
 * I/O specifications:
 */

#ifndef DOS_BYTE_ORDER
#define DOS_BYTE_ORDER 0		/* DOS byte order? 1=yes */
#endif

#define LINESIZE 120			/* # chars in input line */

/*
 * Diffraction image specifications:
 */

#define DEFAULT_PIXEL_SIZE_MM 0.08	/* Pixel size in mm */
#define DEFAULT_VSIZE 1024		/* # Vertical pixels */
#define DEFAULT_HSIZE 1024		/* # Horizontal pixels */
#define DEFAULT_HEADER_LENGTH 4096	/* TV6 TIFF image header length */
#define DEFAULT_IMAGELENGTH 1048576	/* TV6 TIFF image #pixels */
#define DEFAULT_VALUE_OFFSET 0          /* Default offset. 0 for TV6, PILATUS, 40 for ADXV .img */
#define MAX_OVERLOADS 10000		/* Max # of overloads in an image */
#define MAX_PEAKS 2000000			/* Max # of peaks in an image */
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
					/* pixel in x and y for voxel*/ 
					/* generation */
#define DEFAULT_MINRANGE 0.15            /* Minimum valid dist. to */
					/*   Bragg peak */
#define DEFAULT_INNER_RADIUS_LT 0.	/* Inner radius hreshold for mapping */
					/* voxels to a lattice */
#define DEFAULT_OUTER_RADIUS_LT 1.	/* Outer radius threshold for mapping*/
					/* voxels to a lattice */
#define DEFAULT_SPACE_GROUP_STR_LEN 256       /* Default space group string length */
#define DEFAULT_CELL_STR_LEN 1024       /* Default unit cell string length */
#define DEFAULT_LATTICE_TYPE_STR_LEN 8  /* Default lattice type string length */


// Crystal structure specifications:

#define DEFAULT_NATOMS 100000           /* Number of atoms */


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
#define MAX_RFILE_LENGTH 25000		/* Maximum length of rfile */

/*
 * Old Bragg peak and overflow "masking" specifications:
 */

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

#define PI 3.14159265359	      	/* PI */
#define TWOPI 6.28318530718             /* 2.*PI */
#define BOHR 0.52917721092              /* Bohr radius in Angstroms */
#define DEFAULT_MACHST 16708            /* Default CCP4 machine stamp */
#define VALUE_ALLOCATED 1		/* Flag for imdiff->value allocated */
#define RDATA_MALLOC_FACTOR 7		/* Bigger than 2*PI */
#define POLARIZATION_CORRECTION_THRESHOLD .01 /* Threshold for */
					      /* correcting for */
					      /* polarization */

/* 
 * Data-set dependent defines
 */
#ifdef P6M
#define DEFAULT_WAVELENGTH 0.9774       /* Wavelength for sncps */
#define DEFAULT_DISTANCE_MM 250.0	/* Sample-detector distance in mm*/
#define DEFAULT_X_BEAM 41.72		/* Beam position in x (denzo) */
#define DEFAULT_Y_BEAM 41.74		/* Beam position in y (denzo) */
#define DEFAULT_CELL_A 79.2           /* A */
#define DEFAULT_CELL_B 79.2           /* B */
#define DEFAULT_CELL_C 38.1           /* C */
#define DEFAULT_CELL_ALPHA 90.0         /* B-C angle */
#define DEFAULT_CELL_BETA 90.0          /* C-A angle */
#define DEFAULT_CELL_GAMMA 90.0         /* A-B angle */
#define DEFAULT_CASSETTE_ROTX -0.22         /* temp */
#define DEFAULT_CASSETTE_ROTY -1.38         /* temp */
#define DEFAULT_CASSETTE_ROTZ 0         /* temp */
#define DEFAULT_OVERLOAD_TAG 0xffff
#define DEFAULT_IGNORE_TAG 0xfffe	
#define PUNCH_TAG 0xfffd		
#define MAX_IMAGE_DATA_VALUE 65535	
#endif

/*
 * Structures and typedefs:
 */

typedef long IJKCOORDS_DATA;
typedef float XYZCOORDS_DATA;
typedef short RCCOORDS_DATA;
typedef float RFILE_DATA_TYPE;
typedef float LATTICE_DATA_TYPE;
typedef float MAP_DATA_TYPE;
typedef short SHIM_DATA_TYPE;
typedef short IMAGE_DATA_TYPE;
typedef float WEIGHTS_DATA_TYPE;

// MPI

typedef struct {
  int argc;
  char **argv;
  int my_id;
  int num_procs;
  int ierr;
} MPIVARS;


typedef struct {
  IMAGE_DATA_TYPE *value;	/* Pixel value */
  char allocate_flag;           /* Has the array been allocated? */
  size_t count;                 /* Number of pixels in value array */
} RDATA_DATA_TYPE;
struct fom
{
  float R;
  float wR2_ccp4;
  float wR2_shelx;
  float goof;
};
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
struct adps
{
  XYZCOORDS_DATA U11;
  XYZCOORDS_DATA U22;
  XYZCOORDS_DATA U33;
  XYZCOORDS_DATA U12;
  XYZCOORDS_DATA U13;
  XYZCOORDS_DATA U23;
};
struct hklI
{
  IJKCOORDS_DATA h;
  IJKCOORDS_DATA k;
  IJKCOORDS_DATA l;
  LATTICE_DATA_TYPE I;
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
struct irange /* range of integers */
{
  int l;
  int u;
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
  char format[10];              /* Image format */
  char *header;		        /* Image header (typically TIFF) */
  size_t header_length;	        /* Length of image header (4096) */
  char *footer;		        /* Image footer (e.g. CBF) */
  size_t footer_length;	        /* Length of image footer */ 
  IMAGE_DATA_TYPE *image;	/* Pointer to image */
  char big_endian;              /* byte order, 1 = big_endian, 0 = other */
  size_t image_length;	        /* Total number of pixels in image */
  short vpixels;		/* Number of vertical pixels */
  short hpixels;		/* Number of horizontal pixels */
  IMAGE_DATA_TYPE ignore_tag;   /* Ignore this pixel value */
  struct rccoords *overload;    /* Pointer to overload coords */
  IMAGE_DATA_TYPE overload_tag; /* Pixel value indicating ovld */
  IMAGE_DATA_TYPE value_offset; /* Constant offset applied to all pixels */
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
  float osc_start;              /* Starting phi angle */
  float osc_range;              /* Phi angle oscillation range */
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
  IMAGE_DATA_TYPE min_pixel_value;/* Minimum pixel value */
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
  struct rccoords punchim_lower; /* Upper left corner of punchim */
  struct rccoords punchim_upper; /* Lower right corner of punchim */
  IMAGE_DATA_TYPE lower_threshold;
  IMAGE_DATA_TYPE upper_threshold;
  DIFFUSE_FEATURE *feature;     /* List of diffuse features */
  size_t feature_count;
  char *cell_str;               /* Unit cell string a,b,c,alpha,beta,gamma */
  struct unit_cell cell;        /* Unit cell data type */
  float wavelength;
  struct xyzmatrix u;
  struct xyzcoords cassette;    /* Cassette rotation angles */
  float amplitude;              /* Amplitude of noise or fluctuation */
  float pitch;                  /* Pitch of fluctuation */
  struct xyzcoords q;
  struct irange rfirange;       /* Range of rfile index values */
  float *correction;             /* Correction factor, pixel by pixel */
  //size_t tag;			/* Option tag for mathim, etc. */
} DIFFIMAGE;

/*
 * CCP4 Map data type:
 */

/* 
 * Format description from http://www.ccp4.ac.uk/html/maplib.html

 1      NC              # of Columns    (fastest changing in map)
 2      NR              # of Rows
 3      NS              # of Sections   (slowest changing in map)
 4      MODE            Data type
                          0 = envelope stored as signed bytes (from
                              -128 lowest to 127 highest)
                          1 = Image     stored as Integer*2
                          2 = Image     stored as Reals
                          3 = Transform stored as Complex Integer*2
                          4 = Transform stored as Complex Reals
                          5 == 0	
 
                          Note: Mode 2 is the normal mode used in
                                the CCP4 programs. Other modes than 2 and 0
                                may NOT WORK
 
 5      NCSTART         Number of first COLUMN  in map
 6      NRSTART         Number of first ROW     in map
 7      NSSTART         Number of first SECTION in map
 8      NX              Number of intervals along X
 9      NY              Number of intervals along Y
10      NZ              Number of intervals along Z
11      X length        Cell Dimensions (Angstroms)
12      Y length                     "
13      Z length                     "
14      Alpha           Cell Angles     (Degrees)
15      Beta                         "
16      Gamma                        "
17      MAPC            Which axis corresponds to Cols.  (1,2,3 for X,Y,Z)
18      MAPR            Which axis corresponds to Rows   (1,2,3 for X,Y,Z)
19      MAPS            Which axis corresponds to Sects. (1,2,3 for X,Y,Z)
20      AMIN            Minimum density value
21      AMAX            Maximum density value
22      AMEAN           Mean    density value    (Average)
23      ISPG            Space group number
24      NSYMBT          Number of bytes used for storing symmetry operators
25      LSKFLG          Flag for skew transformation, =0 none, =1 if foll
26-34   SKWMAT          Skew matrix S (in order S11, S12, S13, S21 etc) if
                        LSKFLG .ne. 0.
35-37   SKWTRN          Skew translation t if LSKFLG .ne. 0.
                        Skew transformation is from standard orthogonal
                        coordinate frame (as used for atoms) to orthogonal
                        map frame, as
 
                                Xo(map) = S * (Xo(atoms) - t)
 
38      future use       (some of these are used by the MSUBSX routines
 .          "              in MAPBRICK, MAPCONT and FRODO)
 .          "   (all set to zero by default)
 .          "
52          "

53	MAP	        Character string 'MAP ' to identify file type
54	MACHST		Machine stamp indicating the machine type
			which wrote file
55      ARMS            Rms deviation of map from mean density
56      NLABL           Number of labels being used
57-256  LABEL(20,10)    10  80 character text labels (ie. A4 format)

Symmetry records follow - if any - stored as text as in International Tables, operators separated by * and grouped into 'lines' of 80 characters (i.e. symmetry operators do not cross the ends of the 80-character 'lines' and the 'lines' do not terminate in a *).
Map data array follows.

*/



typedef struct {
  char *filename;
  FILE *infile;
  FILE *outfile;
  void *header; // 256 word (4-byte) header.
  long nc; // 0, fastest changing
  long nr; // 1
  long ns; // 2, slowest changing
  long mode; // 3
  long ncstart; // 4
  long nrstart; // 5
  long nsstart; // 6
  long nx; // 7
  long ny; // 8
  long nz; // 9
  float xlen; // 10
  float ylen; // 11
  float zlen; // 12
  float alpha; // 13
  float beta; // 14
  float gamma; // 15
  long mapc; // 16
  long mapr; // 17
  long maps; // 18
  float amin; // 19
  float amax; // 20
  float amean; // 21
  long ispg; // 22
  long nsymbt; // 23
  long lskflg; // 24
  char map[4]; // 52
  long machst; // 53
  float arms; // 54
  long nlabl; // 55
  char *label; // 56-255
  size_t section_length;
  size_t map_length;
  void *symrec_buf;
  char *symrec;
  void *data_buf;
  int decimation_factor;
  struct xyzcoords shift;
  MAP_DATA_TYPE *data;
  MAP_DATA_TYPE scale_factor;
  struct xyzcoords *xyzlist; // list of atom coords
  int natoms; // number of atoms in previous list
  XYZCOORDS_DATA arad;
  float cc_fore;
  float cc_back;
  float rsr_fore;
  float rsr_back;
} CCP4MAP;

/*
 * Lattice data type:
 */

typedef struct {
  char *filename;
  FILE *infile;
  FILE *outfile;
  char error_msg[LINESIZE];      /*Error message string */
  char *lattice_type_str;       /* Lattice type string = (P1,AU) */
  struct voxel *map3D;	        /* Pointer to list of voxels */
  LATTICE_DATA_TYPE *lattice;   /* Pointer to lattice */
  uint32_t xvoxels;		/* Number of x-voxels */
  uint32_t yvoxels;		/* Number of y-voxels */
  uint32_t zvoxels;		/* Number of z-voxels */
  uint32_t xyvoxels;            /* Number of voxels in an xy section */
  float xscale;                 /* Scale factor for x */
  float yscale;                 /* Scale factor for y */
  float zscale;                 /* Scale factor for z */
  size_t lattice_length;	/* Number of voxels */
  struct bounds xbound;	        /* Max and min of x-coord */
  struct bounds ybound;	        /* Max and min of y-coord */
  struct bounds zbound;	        /* Max and min of z-coord */
  struct bounds valuebound;     /* Max and min of voxel value */
  char *space_group_str;        /* Space group in string format */
  char *cell_str;               /* Unit cell string a,b,c,alpha,beta,gamma */
  struct unit_cell cell;        /* Unit cell descriptor */
  struct xyzcoords a;           /* Lattice vector a */
  struct xyzcoords b;           /* Lattice vector b */
  struct xyzcoords c;           /* Lattice vector c */
  struct xyzcoords celldiag;    /* Cell diagonal vector */
  struct xyzcoords astar;       /* Reciprocal lattice vector astar */
  struct xyzcoords bstar;       /* Reciprocal lattice vector bstar */
  struct xyzcoords cstar;       /* Reciprocal lattice vector cstar */
  struct xyzcoords cellstardiag;/* Reciprocal lattice cell diagonal vector */
  LATTICE_DATA_TYPE mask_tag;   /* Masked voxel value tag */
  LATTICE_DATA_TYPE threshold;  /* Threshold for use in correlation calculation, and perhaps elsewhere */
  struct ijkcoords origin;      /* Origin voxel position */
  struct ijkcoords index;       /* Currently selected voxel position index */
  RFILE_DATA_TYPE *rfile;       /* Radial distribution function */
  size_t rfile_length;	        /* Number of rfile values */
  struct xyzcoords minrange;    /* Minimum valid distances to Bragg */
				/* peak */
  size_t inner_radius;          /* Inner radius threshold for lattice */
				/* calcs */
  size_t outer_radius;          /* Outer radius threshold for lattice */
				/* calcs */
  struct bounds resolution;     /* resolution range */
  float peak;                   /* Gaussian peak value */
  float width;                  /* Gaussian width */
  float chi;                    /* LLM chi */
  float sigma;                  /* LLM sigma */
  float gamma;                  /* LLM gamma */
  SHIM_DATA_TYPE *shim;         /* Shell image */
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
  int axis;                     /* Axis of rotation */
  float angle;                  /* Angle of rotation */
  struct xyzmatrix anisoU;      /* anisotropic U matrix */
  struct xyzcoords shift;       /* translation vector */
  int decimation_factor;        /* decimation factor */
} LAT3D;

// Crystal structure data type
typedef struct {
  char *filename;
  FILE *infile;
  FILE *outfile;
  size_t natoms;
  struct xyzcoords *pos;
  struct adps *u;
} XTALSTRUCT;


/*
 * Subroutines:
 */

int labsim(DIFFIMAGE *imdiff);
int labslt(LAT3D *lat);
int lanisoult(LAT3D *lat);
void lanisolt(LAT3D *lat);
int lavgim(DIFFIMAGE *imdiff);
int lavgr(LAT3D *lat);
int lavgrf(DIFFIMAGE *imdiff1);
int lavgrim(DIFFIMAGE *imdiff);
int lavgrlt(LAT3D *lat);
int lavgpolim(DIFFIMAGE *imdiff);
int lavgsqim(DIFFIMAGE *imdiff);
int lavsqrim(DIFFIMAGE *imdiff);
int lavsqrlt(LAT3D *lat);
int lbeamim(DIFFIMAGE *imdiff);
size_t lbufcompress(const int* values, const size_t sz, char *packed);
void lbufuncompress(const char* packed, const size_t packed_sz, int* values, size_t values_sz);
int lbuttim(DIFFIMAGE *imdiff);
struct fom lcalcrsf(char *hklfname, LAT3D *lat1,LAT3D *lat2);
int lccrlt(LAT3D *lat1, LAT3D *lat2);
int lchbyte(void *ptr, size_t packet_size, size_t list_length);
int lconstim(DIFFIMAGE *imdiff);
int lconstlt(LAT3D *lat);
int lconstrf(DIFFIMAGE *imdiff);
int lcfim(DIFFIMAGE *imdiff);
float lcorrlt(LAT3D *lat1, LAT3D *lat2);
int lcpltmap(LAT3D *lat,CCP4MAP *map);
int lcpmaplt(CCP4MAP *map, LAT3D *lat);
struct xyzcoords lcrossvec(struct xyzcoords a,struct xyzcoords b);
void lcullconelt(LAT3D *lat);
int cullim(DIFFIMAGE *imdiff);
int lculllt(LAT3D *lat);
int lcullreslt(LAT3D *lat);
int lcutim(DIFFIMAGE *imdiff);
int ldecimap(CCP4MAP *map);
int ldfrflt(LAT3D *lat1, LAT3D *lat2);
int ldfsqrlt(LAT3D *lat1, LAT3D *lat2);
size_t ldiffim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int ldivlt(LAT3D *lat1, LAT3D *lat2);
int ldivrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lexplt(LAT3D *lat);
void lfft(float *data,int *nn,int ndim,int isign);
int lfilltaglt(LAT3D *lat);
int lfmtlt(LAT3D *lat);
int lfreeim(DIFFIMAGE *imdiff);
int lfreelt(LAT3D *lat);
int lgausslt(LAT3D *lat);
int lgensv(DIFFIMAGE *imdiff);
int lgetanls(DIFFIMAGE *imdiff);
int lgetmat(DIFFIMAGE *imdiff);
int lgetovld(DIFFIMAGE *imdiff);
int lgetpks(DIFFIMAGE *imdiff);
const char * lgettag(const char *target,const char *tag);
DIFFIMAGE *linitim(void);
LAT3D *linitlt(void);
CCP4MAP *linitmap(void);
void linitMPI(MPIVARS *mpiv);
XTALSTRUCT *linitxs(void);
int lintxdslt(DIFFIMAGE *imdiff, LAT3D *lat);
int lliquidcorrlt(LAT3D *lat);
int lliquidfaclt(LAT3D *lat);
int lllmhyblt(LAT3D *lat1, LAT3D *lat2);
int lllmdclt(LAT3D *lat);
int lllmlt(LAT3D *lat);
struct xyzmatrix lmatinv(struct xyzmatrix a);
struct xyzmatrix lmatmul(struct xyzmatrix a, struct xyzmatrix b);
int lmedim(DIFFIMAGE *imdiff);
size_t lmin(size_t arg1, size_t arg2);
int lminim(DIFFIMAGE *imdiff);
int lminr(LAT3D *lat);
int lminrim(DIFFIMAGE *imdiff);
int lmirrorlt(LAT3D *lat,int axis);
int lmodeim(DIFFIMAGE *imdiff);
int lmulcfim(DIFFIMAGE *imdiff);
int lmuldwflt(LAT3D *lat);
int lmulim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lmullt(LAT3D *lat1, LAT3D *lat2);
int lmulrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lmulrfim(DIFFIMAGE *imdiff);
int lmulscim(DIFFIMAGE *imdiff);
int lmulsclt(LAT3D *lat);
int lmulscmap(CCP4MAP *map);
int lnign(DIFFIMAGE *imdiff);
int lnoiseim(DIFFIMAGE *imdiff);
int lnormim(DIFFIMAGE *imdiff);
int lnormlt(LAT3D *lat);
int lp6mmask(DIFFIMAGE *imdiff);
int lpadlt(LAT3D *lat);
int lparsecelllt(LAT3D *lat);
int lpeakim(DIFFIMAGE *imdiff);
int lpolarim(DIFFIMAGE *imdiff);
int lpunch(DIFFIMAGE *imdiff);
int lpunchim(DIFFIMAGE *imdiff);
int lratioim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lrbtlt(LAT3D *lat);
int lreadcube(CCP4MAP *map);
int lreadhkl(LAT3D *lat,LAT3D *tmpl);
int lreadim(DIFFIMAGE *imdiff);
int lreadlt(LAT3D *lat);
int lreadmap(CCP4MAP *map);
int lreadrf(DIFFIMAGE *imdiff);
int lreadvtk(LAT3D *lat);
int lreadxs(XTALSTRUCT *xs);
int lresizelt(LAT3D *lat1, LAT3D *lat2);
int lrevyim(DIFFIMAGE *imdiff);
int lrf2lt(LAT3D *lat);
float lrfaclt(LAT3D *lat1, LAT3D *lat2);
int lrmpkim(DIFFIMAGE *imdiff);
int lrotlt(LAT3D *lat);
struct xyzmatrix lrotmat(float rotx, float roty, float rotz);
struct xyzcoords lrotvecz(struct xyzcoords a, float cos_theta,float sin_theta);
int lrsccmap(CCP4MAP *map1, CCP4MAP *map2);
float lrsrlt(LAT3D *lat1, LAT3D *lat2);
int lsamplt(LAT3D *lat);
int lscaleim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lscalerf(DIFFIMAGE *imdiff, float scale);
int lscalerfim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lscalelt(LAT3D *lat1, LAT3D *lat2);
struct xyzcoords lsFromIndex(LAT3D *lat);
int lsetcbftag(char **t, size_t *target_length, const char *tag, const char *val);
int lsettag(char *target,const char *tag,const char *val);
int lshiftlt(LAT3D *lat,struct ijkcoords t);
int lshiftsflt(LAT3D *lat1,LAT3D *lat2);
int lshim4lt(LAT3D *lat);
int lshimlt(LAT3D *lat);
int lsmthim(DIFFIMAGE *imdiff);
int lsolidlt(LAT3D *lat);
float lspleval(float *break__, float *coef, int *l, int *k, float *x, int *jderiv);
int lspline(float *tau, float *c__, int *n, int *ibcbeg, int *ibcend);
float lssqrFromIndex(LAT3D *lat);
int lsubenvlt(LAT3D *lat1, LAT3D *lat2);
int lsubim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsublt(LAT3D *lat1, LAT3D *lat2);
void lsubminlt(LAT3D *lat);
int lsubrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsubrfim(DIFFIMAGE *imdiff);
int lsubrflt(LAT3D *lat);
int lsumscim(DIFFIMAGE *imdiff);
int lsumim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsumlt(LAT3D *lat1, LAT3D *lat2);
int lsummap(CCP4MAP *map1, CCP4MAP *map2);
int lsumrf(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lsymlt(LAT3D *lat);
int lsymminlt(LAT3D *lat);
int ltagim(DIFFIMAGE *imdiff);
int ltaglt(LAT3D *lat);
int lthrshim(DIFFIMAGE *imdiff);
int lthrshlt(LAT3D *lat);
int ltordata(DIFFIMAGE *imdiff);
int ltranslt(LAT3D *lat1, struct ijkcoords t);
int ltransmap(CCP4MAP *map);
int lupdbd(LAT3D *lat);
int lwaveim(DIFFIMAGE *imdiff);
int lwindim(DIFFIMAGE *imdiff);
int lwritecube(LAT3D *lat);
int lwriteim(DIFFIMAGE *imdiff);
int lwritehkl(LAT3D *lat);
int lwritelt(LAT3D *lat);
int lwritemap(CCP4MAP *map);
int lwriterf(DIFFIMAGE *imdiff);
int lwritesh(LAT3D *lat);
int lwritevtk(LAT3D *lat);
int lxavgr(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lxavgrim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
int lxf1lt(LAT3D *lat);
int lxfmask(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2);
struct xyzcoords lmatvecmul(struct xyzmatrix b,struct xyzcoords a);
XYZCOORDS_DATA ldotvec(struct xyzcoords b,struct xyzcoords a);
struct xyzcoords linvvec(struct xyzcoords a);
struct xyzcoords laddvec(struct xyzcoords b,struct xyzcoords a);
struct xyzcoords lmulscvec(XYZCOORDS_DATA b,struct xyzcoords a);
struct xyzcoords lsubvec(struct xyzcoords b,struct xyzcoords a);

// Symmetry library functions:

struct ijkcoords lijksub(struct ijkcoords vec1, struct ijkcoords vec2);
  struct ijkcoords lijksum(struct ijkcoords vec1, struct ijkcoords vec2);
struct ijkcoords lijkrotk(struct ijkcoords vec, float cos_theta, 
			  float sin_theta);
struct ijkcoords lijkinv(struct ijkcoords vec);
struct ijkcoords lijkmij(struct ijkcoords vec);
struct ijkcoords lijkmjk(struct ijkcoords vec);
struct ijkcoords lijkmki(struct ijkcoords vec);
int lLaue1(LAT3D *lat);
int lLaue2(LAT3D *lat);
int lLaue3(LAT3D *lat);
int lLaue4(LAT3D *lat);
int lLaue5(LAT3D *lat);
int lLaue6(LAT3D *lat);
int lLaue7(LAT3D *lat);
int lLaue8(LAT3D *lat);
int lLaue9(LAT3D *lat);
int lLaue10(LAT3D *lat);
int lLaue11(LAT3D *lat);
