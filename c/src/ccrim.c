/* CCRIM.C - Compute the rmsd between two images in resolution shells.
 * 
    Author: Veronica Pillar (modified from Mike Wall's codes)
    Date: 4/13/15
    Version: 1.

    "ccrim <input image X> <input image Y> <chunk size>" 

    Input two diffraction images X and Y in CCD .img format, plus the side
    length (in pixels) of a box in which to average values into single chunks.

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
	box_size;

    DIFFIMAGE 
	*imdiffX,
	*imdiffY;

    /*
     * Set input line defaults:
     */

    imageX = stdin;
    imageY = stdin;
    box_size = 1;

    /*
     * Read information from input line:
     */

    switch(argc) {
	case 4:
	    box_size = atoi(argv[3]);
	    if (box_size < 1) box_size = 1;
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
	    printf("\n Usage: ccrim <input image X> <input image Y> <chunk size>\n\n");
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

    if (lccrim(imdiffX,imdiffY) != 0) {
	perror(imdiffY->error_msg);
	goto CloseShop;
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

