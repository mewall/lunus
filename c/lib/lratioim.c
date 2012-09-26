/* LRATIOIM.C - Calculate the scale and offset between two diffraction
		images as a function of radius.

   Author: Mike Wall
   Version:1.0
   Date:4/14/94
				 
	 Input argument is two diffraction images.  Output ratio is stored 
	 in imdiff2->imscaler.
*/

#include<mwmask.h>

int lratioim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	RFILE_DATA_TYPE
		*avg1,
		*avg2;

	int 
		return_value = 0;

 	size_t
		i;
/*
 * Allocate memory for temporary rfiles:
 */

   avg1 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
   avg2 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
   if (!avg1 || !avg2) {
	sprintf(imdiff2->error_msg,"\nLSCALEIM: Couldn't allocate all "
				"pointers.\n\n");
	return_value = 1;
	goto EndPaper;
   }

/*
 * Make sure that images are masked equivalently about the origin:
 */

   lxfmask(imdiff1,imdiff2);
   lxfmask(imdiff2,imdiff1);

/*
 * Calculate averages as a function of radius:
 */

   lavgrim(imdiff1);
   for (i = 0; i < imdiff1->rfile_length; i++) { 
     avg1[i] = imdiff1->rfile[i];
     imdiff1->rfile[i] = 0;
   }
   lavgrim(imdiff2);
   for (i = 0; i < imdiff2->rfile_length; i++) { 
     avg2[i] = imdiff2->rfile[i];
     imdiff2->rfile[i] = 0;
   }

/*
 * Generate imscaler and imoffsetr:
 */

   i = imdiff2->origin.r*imdiff2->hpixels+imdiff2->origin.c;
   if (imdiff2->image[i] != 0) {
     imdiff2->imscaler[0] = (RFILE_DATA_TYPE)imdiff1->image[imdiff1->origin.r*
		imdiff1->hpixels+imdiff1->origin.c] / 
		imdiff2->image[i];
   }
   else imdiff2->imscaler[0] = 0;
   imdiff2->imoffsetr[0] = 0;
   for(i = 1; i < imdiff2->rfile_length; i++) {
     if (avg2[i] != 0) {
       imdiff2->imscaler[i] = (avg1[i] / avg2[i]); 
     }
     else imdiff2->imscaler[i] = 0;
     imdiff2->imoffsetr[i] = 0;
   }

/*
 * Free memory:
 */

   free((RFILE_DATA_TYPE *)avg1);
   free((RFILE_DATA_TYPE *)avg2);

EndPaper:
   
   return(return_value);
}
