/* DEBRAGGIM.C - Replace bright local areas in an image by comparing with the mode-filtered version of the image.

Author: Veronica Pillar (modified from Mike Wall's codes)
Date: 4/28/16
Version: 1.

"debraggim <original image> <mode-filtered image> <output image> (optional <variance factor> <moat factor>)" 

Input the non-mode-filtered and mode-filtered versions of an image in CCD .img format. Output is the non-mode-filtered image with bright pixels and their surrounding areas replaced by the corresponding values from the mode-filtered image.

Pixels whose squared difference from the mode-filtered value are above the overall variance x variance factor will be replaced; default variance factor is 1.

The moat factor determines the radius (in pixels) around each pixel tagged by the above variance configuration that will also be replaced by the mode-filtered values. Default moat factor is 3.

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
        moat,
        i;

    DIFFIMAGE 
        *imdiffraw,
        *imdiffmode,
        *imdiffdiff;

    double
        var;

    //typedef float IMAGE_DATA_TYPE;

    /*
     * Set input line defaults:
     */

    imageraw = stdin;
    imagemode = stdin;
    imageout = stdout;
    var = 1.;
    moat = 3;

    /*
     * Read information from input line:
     */
    switch(argc) {

        case 6:
            moat = atoi(argv[5]);
        case 5:
            var = atof(argv[4]);
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
            printf("\n Usage: debraggim <original image> <mode-filtered image> <output image> <variance factor> <moat factor>\n\n");
            exit(0);
    }
    /*
     * Initialize diffraction images:
     */

    if (((imdiffraw = linitim()) == NULL) || ((imdiffmode = linitim()) == NULL) || 
            ((imdiffdiff = linitim()) == NULL)) {
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
     * Set up imdiffdiff;
     */   

    fseek(imageraw, 0, SEEK_SET); //return pointer to beginning of file
    imdiffdiff->infile = imageraw;
    if (lreadim(imdiffdiff) != 0) {
        perror(imdiffdiff->error_msg);
        goto CloseShop;
    }
    imdiffdiff->image[0] = 0;
    lconstim(imdiffdiff);
    // so now imdiffdiff is an image of zeros. 

    /*
     * Create image of squared differences:
     */

    lsqsubim(imdiffraw, imdiffmode, imdiffdiff); 

    /*
     * Find variance:
     */

    lavgim(imdiffdiff);
    printf("Average squared difference is %f\n",imdiffdiff->avg_pixel_value);
    imdiffdiff->upper_threshold = var*imdiffdiff->avg_pixel_value;
    imdiffdiff->mode_width = moat;

    /* 
     * Go through all three images, replacing pixels whose squared difference is too large with their mode-filtered equivalent:
     */

    ldebraggim(imdiffraw, imdiffmode, imdiffdiff);

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
    lfreeim(imdiffdiff);

    /*
     * Close files:
     */

    fclose(imageraw);
    fclose(imagemode);
    fclose(imageout);
}

