/* MATHIM.C - Perform aX + bY where a and b are scale factors, and X and Y are images (Y optional).

Author: Veronica Pillar (modified from Mike Wall's codes)
Date: 1/4/16
Version: 2.

"mathim <input image X> <scale factor a> <input image Y> <scale factor b> <output image> <punch-and tag>" 

Input two diffraction images X and Y in CCD .img format, with scale factors a and b. Output is aX + bY (an image). 

The optional punch-addition tag tells whether to punch a pixel in the output if that pixel is punched in both images, as opposed to either image (default).
0 = false = default = OR (punch if punched in either input image)
any nonzero value = true = AND (punch only if punched in both input images)

NOTE: AND punch addition (tag = true) only works when aX and bY are on the same
scale. If you are not sure this is the case, do not use this option.

Pedestal value of output image will be the larger of the two pedestal values of the input
images.

*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
    FILE
	*imageX,
	*imageY,
	*imageout;

    size_t
	i,
	tag;

    DIFFIMAGE 
	*imdiffX,
	*imdiffY;

    double
	a,
	b,
	isY;

    //typedef float IMAGE_DATA_TYPE;

    /*
     * Set input line defaults:
     */

    imageX = stdin;
    imageY = stdin;
    imageout = stdout;
    a = 1.;
    b = 1.;
    isY = 0;
    tag = 0;

    /*
     * Read information from input line:
     */
    switch(argc) {
	case 7:
	    tag = atoi(argv[6]);
	    if (tag) {
		printf("Using strict (AND) punch addition\n");
	    }

	case 6:
	    if (strcmp(argv[5], "-") == 0) {
		imageout = stdout;
	    }
	    else {
		if ( (imageout = fopen(argv[5],"wb")) == NULL ) {
		    printf("Can't open %s.",argv[5]);
		    exit(0);
		}
	    }
	case 5:
	    b = atof(argv[4]);
	    isY = 1;
	case 4:	
	    if (isY == 1) {
		if (strcmp(argv[3], "-") == 0) {
		    imageY = stdin;
		}
		else {
		    if ( (imageY = fopen(argv[3],"rb")) == NULL ) {
			printf("Can't open %s.",argv[3]);
			exit(0);
		    }
		}
	    }

	    else {
		if (strcmp(argv[3], "-") == 0) {
		    imageout = stdout;
		}
		else {
		    if ( (imageout = fopen(argv[3],"wb")) == NULL ) {
			printf("Can't open %s.",argv[3]);
			exit(0);
		    }
		}

	    }
	case 3:
	    a = atof(argv[2]);
	case 2:
	    if (strcmp(argv[1], "-") == 0) {
		imageX = stdin;
	    }
	    else {
		if ( (imageX = fopen(argv[1],"rb")) == NULL ) {
		    printf("Can't open %s.",argv[1]);
		    exit(0);
		}
	    }
	    break;
	default:
	    printf("\n Usage: mathim <input image X> <scale factor a> <input image Y> <scale factor b> <output image>\n\n");
	    exit(0);
    }
    /*
     * Initialize diffraction images:
     */

    if (((imdiffX = linitim()) == NULL) || ((imdiffY = linitim()) == NULL)) {
	perror("Couldn't initialize diffraction images.\n\n");
	exit(0);
    }

    /*
     * Read diffraction image:
     */

    imdiffX->infile = imageX;
    if (lreadim(imdiffX) != 0) {
	perror(imdiffX->error_msg);
	goto CloseShop;
    }

    if (isY == 1) {
	imdiffY->infile = imageY;
	if (lreadim(imdiffY) != 0) {
	    perror(imdiffY->error_msg);
	    goto CloseShop;
	}
    }
    else
	imdiffY = NULL;

    /*
     * Set punch addition tag 
     */

    imdiffX->tag = tag;

    if (lmathim(imdiffX,a,imdiffY,b) != 0) {
	perror(imdiffY->error_msg);
	goto CloseShop;
    }


    /*
     * Write the output image:
     */

    imdiffX->outfile = imageout;
    if(lwriteim(imdiffX) != 0) {
	perror(imdiffX->error_msg);
	goto CloseShop;
    }

CloseShop:

    /*
     * Free allocated memory:
     */

    lfreeim(imdiffX);
    if (isY==1) lfreeim(imdiffY);

    /*
     * Close files:
     */

    fclose(imageX);
    fclose(imageY);
    fclose(imageout);
}

