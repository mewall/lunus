/* HISTLT.C - Histogram voxel values in a lattice in a specified */
/* spherical shell

    Author: Mike Wall
    Date: 3/22/95
    Version: 1.

    Usage:
        "histlt <input lattice> <output histogram> <inner
            radius> <outer radius> <binsize>"

    Input is a 3D lattice.  Output is a histogram. Radii are in units of pixels. 
    Printed bin values are the lower bounds for each bin.
 */

#include<mwmask.h>

int main(int argc, char *argv[])
{
    FILE
        *histout,
        *latticein;

    char
        error_msg[LINESIZE];

    LAT3D 
        *lat;

    int
        binsize = 1,
        val;

    size_t
        index = 0,
        i,
        j,
        k,
        *histogram;

    float
        r,
        rscale,
        inner_radius,
        outer_radius;

    struct ijkcoords rvec;

    struct xyzcoords rfloat;

    /*
     * Set input line defaults:
     */

    latticein = stdin;

    /*
     * Read information from input line:
     */
    switch(argc) {
        case 6:
            binsize = atoi(argv[5]);
        case 5:
            outer_radius = atof(argv[4]);
        case 4:
            inner_radius = atof(argv[3]);
        case 3:
            if (strcmp(argv[2],"-") == 0) {
                histout = stdout;
            }
            else {
                if ( (histout = fopen(argv[2],"w")) == NULL ) {
                    printf("\nCan't open %s.\n\n",argv[2]);
                    exit(0);
                }
            }
        case 2:
            if (strcmp(argv[1],"-") == 0) {
                latticein = stdin;
            }
            else {
                if ( (latticein = fopen(argv[1],"rb")) == NULL ) {
                    printf("\nCan't open %s.\n\n",argv[1]);
                    exit(0);
                }
            }
            break;
        default:
            printf("\n Usage: histlt <input lattice> <output histogram> "
                    "<inner radius> <outer radius> <binsize>\n\n");
            exit(0);
    }

    /*
     * Initialize lattice:
     */

    if ((lat = linitlt()) == NULL) {
        perror("Couldn't initialize lattice.\n\n");
        exit(0);
    }

    /*
     * Read in lattices:
     */

    lat->infile = latticein;
    if (lreadlt(lat) != 0) {
        perror("Couldn't read lattice.\n\n");
        exit(0);
    }

    /*
     * Allocate memory for histogram:
     */

    histogram = (size_t *)calloc(65536,sizeof(size_t));
    if (!histogram) {
        perror("Couldn't allocate histogram.\n\n");
        exit(0);
    }

    /*
     * Generate the histogram:
     */

    printf("Generating the histogram\n");

    //lat->inner_radius = inner_radius;
    //lat->outer_radius = outer_radius;

    rscale = (lat->xscale*lat->xscale + lat->yscale*lat->yscale +
            lat->zscale*lat->zscale); // inverse Angstroms per voxel diagonal, squared
    for(k=0;k<lat->zvoxels;k++) {
        for(j=0;j<lat->yvoxels;j++) {
            for(i=0;i<lat->xvoxels;i++) {
                rvec.i = i - lat->origin.i; //origin is voxel coord. location of hkl = 0,0,0 (I think)
                rvec.j = j - lat->origin.j;
                rvec.k = k - lat->origin.k;
                rfloat.x = lat->xscale * rvec.i;
                rfloat.y = lat->yscale * rvec.j;
                rfloat.z = lat->zscale * rvec.k;
                r = sqrtf((rfloat.x*rfloat.x + rfloat.y*rfloat.y + 
                            rfloat.z*rfloat.z) / rscale);
                if ((r >= inner_radius) &&
                        (r <= outer_radius)) {
                    val = (int)lat->lattice[index];
                    if (val>-32768 && val<32768) {
                        histogram[val - (val % binsize)+32768]++;
                    }
                }
                index++;
            }
        }
    }
    /*
     * Write the output file:
     */

    for(i=0;i<=65535;i++) {
        if (histogram[i] != (size_t)0) {
            fprintf(histout,"%d %ld\n",(int)i-32768,histogram[i]);
        }
    }

CloseShop:

    /*
     * Free allocated memory:
     */

    lfreelt(lat);

    /*
     * Close files:
     */

    fclose(latticein);
    fclose(histout);
}

