/* CCRIM_FULL.C - Compute the rmsd between two sets of images in resolution 
 * shells.
 * 
Author: Veronica Pillar (modified from Mike Wall's codes)
Date: 5/26/15
Version: 1.

"ccrim <input images X> <input images Y> <chunk size>" 

Input two sets of diffraction images X and Y in CCD .img format, plus 
the side length (in pixels) of a box in which to average values into 
single chunks. It is assumed that the two sets of images are the same size.

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
	num_images,
	box_size;

    double
	*n,
	*sum_x,
	*sum_y,
	*sum_xx,
	*sum_yy,
	*sum_xy;

    double
	pcc,
	pcc1,
	pcc2,
	pcc3,
	theta,
	reslim,
	pixel_size,
	distance,
	wavelength;

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

    if (argc < 4) {
	printf("\n Usage: ccrim_full <input images X> <input images Y> <chunk size>\n\n");
	exit(0);
    }


    box_size = atoi(argv[argc-1]);
    if (box_size < 1) box_size = 1;

    num_images = (argc - 2)/2; // Images per set
    /*
       if (strcmp(argv[2], "-") == 0) {
       imageY = stdin;
       }
       else {
       if ( (imageY = fopen(argv[2],"rb")) == NULL ) {
       printf("Can't open %s.",argv[2]);
       exit(0);
       }
       }
       if (strcmp(argv[1], "-") == 0) {
       imageX = stdin;
       }
       else {
       if ( (imageX = fopen(argv[1],"rb")) == NULL ) {
       printf("Can't open %s.",argv[1]);
       exit(0);
       }
       }
       */

    /*
     * Allocate memory for sum arrays:
     */

    n = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_x = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_y = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_xx = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_yy = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix
    sum_xy = (double*)calloc(25, sizeof(double)); //25 bins of radial width 50 pix

    double *sums_all[6] = {n, sum_x, sum_y, sum_xx, sum_yy, sum_xy};

    for (i = 0; i < num_images; i++) {

	if ( (imageX = fopen(argv[i+1],"rb")) == NULL ) {
	    printf("Can't open %s.",argv[i+1]);
	    exit(0);
	}
	if ( (imageY = fopen(argv[i+1+num_images],"rb")) == NULL ) {
	    printf("Can't open %s.",argv[i+1+num_images]);
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

	if (lccrim_helper(imdiffX,imdiffY,sums_all) != 0) {
	    perror(imdiffY->error_msg);
	    goto CloseShop;
	}

	if (i==0) {
	    pixel_size = imdiffX->pixel_size_mm;
	    distance = imdiffX->distance_mm;
	    wavelength = imdiffX->wavelength;
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

    /*
     * Calculate and print correlation coefficients:
     */

    printf("Res. limit(A)\tCorr. coeff.\n");

    for (i = 0; i < 25; i++) {
	pcc1 = n[i]*sum_xy[i] - sum_x[i]*sum_y[i];
	pcc2 = n[i]*sum_xx[i] - sum_x[i]*sum_x[i];
	pcc3 = n[i]*sum_yy[i] - sum_y[i]*sum_y[i];
	pcc = pcc1/(sqrt(pcc2)*sqrt(pcc3));
	theta = 0.5*atan((i+1)*50*pixel_size/distance);
	reslim = wavelength/(2*sin(theta));
	printf("%0.2f\t\t%0.2f\n", reslim, pcc);
    }

    free(n);
    free(sum_x);
    free(sum_y);
    free(sum_xx);
    free(sum_xy);
    free(sum_yy);

}

