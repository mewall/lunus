/* DETECTORMASK.C - Mark specified ranges of pixels with an ignore_tag. Intended to mask a certain detector's chip boundaries and the pixels immediately neghboring them.

Author: Veronica Pillar (modified from Mike Wall's punchim.c)
Date: 5/16/16
Version: 1.

"detectormask <image in> <image out> <detector nickname>

Input is diffraction image.  Output is diffraction image with pixels inside bounds
marked to be ignored.

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
        tag;

    DIFFIMAGE 
        *imdiff;

    struct rccoords
        upper_bound,
        lower_bound;

    /*
     * Set input line defaults:
     */
    imagein = stdin;
    imageout = stdout;
    tag = 0;

    /*
     * Read information from input line:
     */
    switch(argc) {
        case 4:
            if ((strcmp(argv[3], "p6m") == 0) || (strcmp(argv[3], "Pilatus6M") == 0)
                    || (strcmp(argv[3], "pilatus6m") == 0)) {
                tag = 0;
            }       
            else {
                printf("Can't read detector name; assuming a Pilatus 6M\n");
                tag = 0;
            }
        case 3:
            if (strcmp(argv[2], "-") == 0) {
                imageout = stdout;
            }
            else {
                if ( (imageout = fopen(argv[2],"wb")) == NULL ) {
                    printf("Can't open %s.\n",argv[2]);
                    exit(0);
                }
            }
        case 2:
            if (strcmp(argv[1], "-") == 0) {
                imagein = stdin;
            }
            else {
                if ( (imagein = fopen(argv[1],"rb")) == NULL ) {
                    printf("Can't open %s.\n",argv[1]);
                    exit(0);
                }
            }
            break;
        default:
            printf("\n Usage: detectormask <image in> <image out> <detector nickname>\n\n");
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
     * Mark chip boundary pixels with an ignore_tag according to prespecified mask:
     */

    if (tag == 0)
        lp6mmask(imdiff);

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

