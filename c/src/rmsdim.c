/* RMSDIM.C - Compute the rmsd between two images in resolution shells.
 * 
    Author: Veronica Pillar (modified from Mike Wall's codes)
    Date: 3/3/15
    Version: 1.

    Added tag for fractional option: 3/19/15
    Version: 2.

    Added option for chunk size: 3/24/15
    Version: 3.

    "rmsdim <input image X> <input image Y> <tag> <chunk size>" 

    Input two diffraction images X and Y in CCD .img format, plus a tag value
    to indicate whether and how to compute fractional values, plus the side
    length (in pixels) of a box in which to average values into single chunks.

    Tag values:
	0: absolute rmsd 
	1: fractional, as compared to rms of first image
	2: fractional, as compared to rms of second image

    Results print to stdout. 
*/

//Later maybe add an option for user to define resolution shells.


#include<mwmask.h>

int main(int argc, char *argv[])
{
    FILE
	*imageX,
	*imageY;

    size_t
	i,
	tag,
	box_size;

    DIFFIMAGE 
	*imdiffX,
	*imdiffY;

    /*
     * Set input line defaults:
     */

    imageX = stdin;
    imageY = stdin;
    tag = 0;
    box_size = 1;

    /*
     * Read information from input line:
     */

    switch(argc) {
	case 5:
	    box_size = atoi(argv[4]);
	    if (box_size < 1) box_size = 1;
	case 4:
	    tag = atoi(argv[3]);
	    if ((tag < 0) || (tag > 2)) {
		printf("Invalid tag: %s\n", argv[3]);
		exit(0);
	    }
	case 3:
	    if (strcmp(argv[2], "-") == 0) {
		imageY = stdin;
	    }
	    else {
		if ( (imageY = fopen(argv[2],"rb")) == NULL ) {
		    printf("Can't open %s.",argv[2]);
		    exit(0);
		}
	    }
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
	    printf("\n Usage: rmsdim <input image X> <input image Y> <tag> <chunk size>\n\n");
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

    imdiffY->infile = imageY;
    if (lreadim(imdiffY) != 0) {
	perror(imdiffY->error_msg);
	goto CloseShop;
    }

    /*
     * Initialize parameter:
     */

    imdiffX->mode_width = box_size; //not what mode_width is usually for
    imdiffY->mode_width = box_size; //but that's ok

    /*
     * Compute and print the rmsd values:
     */

    if (tag == 0) {
	if (lrmsdim(imdiffX,imdiffY) != 0) {
	    perror(imdiffY->error_msg);
	    goto CloseShop;
	}
    }
    else if (tag == 1) {
	if (lfrmsdim(imdiffX,imdiffY) != 0) {
	    perror(imdiffY->error_msg);
	    goto CloseShop;
	}
    }
    else if (tag == 2) {
	if (lfrmsdim(imdiffY,imdiffX) != 0) {
	    perror(imdiffY->error_msg);
	    goto CloseShop;
	}
    }
CloseShop:

    /*
     * Free allocated memory:
     */

    lfreeim(imdiffX);
    lfreeim(imdiffY);

    /*
     * Close files:
     */

    fclose(imageX);
    fclose(imageY);
}

