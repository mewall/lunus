/* DEBEAMIM.C - Add a circular mask to an image at a specified point (intended for masking the beamstop area).

Author: Veronica Pillar
Date: 10/23/17
Version: 1.

"debeamim <image in> <image out> <radius> <x> <y>"

Input is a diffraction image in standard .img format.
Output is the same image with a circular area masked with ignore_tags.
The circular area is specified on the command line--radius (in pixels) is required, while x and y (in pixels) are optional and default to the beam center if not provided.

 */

#include<mwmask.h>

int main(int argc, char *argv[])
{
    FILE
        *imagein,
        *imageout;

    char
        error_msg[LINESIZE];

    size_t
        num_wrote,
        num_read;

    float
        radius;

    short
        x,
        y;

    DIFFIMAGE 
        *imdiff;

    /*
     * Set input line defaults:
     */

    imagein = stdin;
    imageout = stdout;
    radius = 0;

    /*
     * Read information from input line:
     */
    switch(argc) {
        case 6:
            y = atoi(argv[5]);
            x = atoi(argv[4]);
        case 4:
            radius = atof(argv[3]);            
        case 3:
            if (strcmp(argv[2], "-") == 0) {
                imageout = stdout;
            }
            else {
                if ( (imageout = fopen(argv[2],"wb")) == NULL ) {
                    printf("Can't open %s.",argv[2]);
                    exit(0);
                }
            }
        case 2:
            if (strcmp(argv[1], "-") == 0) {
                imagein = stdin;
            }
            else {
                if ( (imagein = fopen(argv[1],"rb")) == NULL ) {
                    printf("Can't open %s.",argv[1]);
                    exit(0);
                }
            }
            break;
        default:
            printf("\n Usage: debeamim <image in> <image out> " 
                "<radius (pixels)> <x (pixels)> <y (pixels)> \n\n");
            exit(0);
    }

    /*
     * Initialize diffraction image:
     */

    if ((imdiff = linitim()) == NULL) {
        perror("Couldn't initialize diffraction image.\n\n");
        exit(0);
    }

    /*
     * Read diffraction image:
     */

    imdiff->infile = imagein;
    if (lreadim(imdiff) != 0) {
        perror(imdiff->error_msg);
        goto CloseShop;
    }

    /*
     * Set x & y defaults:
     */

    x = imdiff->origin.c;
    y = imdiff->origin.r;

    /*
     * Tag the circle:
     */

    imdiff->mask_inner_radius = 0;
    imdiff->mask_outer_radius = radius;
    imdiff->pos.c = x;
    imdiff->pos.r = y;
    ldebeamim(imdiff);

    /*
     * Write the output image:
     */

    imdiff->outfile = imageout;
    if(lwriteim(imdiff) != 0) {
        perror(imdiff->error_msg);
        goto CloseShop;
    }

CloseShop:

    lfreeim(imdiff);

    /*
     * Close files:
     */

    fclose(imagein);
    fclose(imageout);

}

