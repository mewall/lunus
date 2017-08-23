/* FILLIM.C - Replace overloads and punch_tags in an image with values from the mode-filtered version of the image.

Author: Veronica Pillar (modified from Mike Wall's codes)
Date: 5/22/16
Version: 1.

"fillim <original image> <mode-filtered image> <output image>" 

Input the non-mode-filtered and mode-filtered versions of an image in CCD .img format. Output is the non-mode-filtered image with punch_ and overload_tags replaced by the corresponding values from the mode-filtered image.

Pedestal value of output image will be the larger of the two pedestal values of the input images.

*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
    FILE
        *imageraw,
        *imagemode,
        *imageout;

    size_t
        i;

    DIFFIMAGE 
        *imdiffraw,
        *imdiffmode;

    /*
     * Set input line defaults:
     */

    imageraw = stdin;
    imagemode = stdin;
    imageout = stdout;

    /*
     * Read information from input line:
     */
    switch(argc) {

        case 4:
            if (strcmp(argv[3], "-") == 0) {
                imageout = stdout;
            }
            else {
                if ( (imageout = fopen(argv[3],"wb")) == NULL ) {
                    printf("Can't open %s.",argv[3]);
                    exit(0);
                }
            }
        case 3:	
            if (strcmp(argv[2], "-") == 0) {
                imagemode = stdin;
            }
            else {
                if ( (imagemode = fopen(argv[2],"rb")) == NULL ) {
                    printf("Can't open %s.",argv[3]);
                    exit(0);
                }
            }
        case 2:
            if (strcmp(argv[1], "-") == 0) {
                imageraw = stdin;
            }
            else {
                if ( (imageraw = fopen(argv[1],"rb")) == NULL ) {
                    printf("Can't open %s.",argv[1]);
                    exit(0);
                }
            }
            break;
        default:
            printf("\n Usage: fillim <original image> <mode-filtered image> <output image>\n\n");
            exit(0);
    }
    /*
     * Initialize diffraction images:
     */

    if (((imdiffraw = linitim()) == NULL) || ((imdiffmode = linitim()) == NULL)) {
        perror("Couldn't initialize diffraction images.\n\n");
        exit(0);
    }

    /*
     * Read diffraction images:
     */

    imdiffraw->infile = imageraw;
    if (lreadim(imdiffraw) != 0) {
        perror(imdiffraw->error_msg);
        goto CloseShop;
    }

    imdiffmode->infile = imagemode;
    if (lreadim(imdiffmode) != 0) {
        perror(imdiffmode->error_msg);
        goto CloseShop;
    }

    /* 
     * Go through the image, replacing punch & overload tags with their mode-filtered equivalent:
     */

    lfillim(imdiffraw, imdiffmode);

    /*
     * Write the output image:
     */

    imdiffraw->outfile = imageout;
    if(lwriteim(imdiffraw) != 0) {
        perror(imdiffraw->error_msg);
        goto CloseShop;
    }

CloseShop:

    /*
     * Free allocated memory:
     */

    lfreeim(imdiffraw);
    lfreeim(imdiffmode);

    /*
     * Close files:
     */

    fclose(imageraw);
    fclose(imagemode);
    fclose(imageout);
}

