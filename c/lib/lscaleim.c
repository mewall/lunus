/* LSCALEIM.C - Calculate the scale and offset between two diffraction
		images as a function of radius.

   Author: Mike Wall
   Version:1.0
   Date:4/7/94
				 
	 Input argument is two diffraction images.  Output scale and offset
	 are stored in imdiff2->imscaler and imdiff2->imoffsetr.
*/

#include<mwmask.h>

int lscaleim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	RFILE_DATA_TYPE
		*avg1,
		*avg2,
		*avg_sq1,
		*avg_sq2,
		*xavg;

	int 
		return_value = 0;

 	size_t
		i;
/*
 * Allocate memory for temporary rfiles:
 */

   avg1 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
   avg2 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
   avg_sq1 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,
				sizeof(RFILE_DATA_TYPE));
   avg_sq2 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,
				sizeof(RFILE_DATA_TYPE));
   xavg = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));

   if (!avg1 || !avg2 || !avg_sq1 || !avg_sq2 || !xavg) {
	sprintf(imdiff2->error_msg,"\nLSCALEIM: Couldn't allocate all "
				"pointers.\n\n");
	return_value = 1;
	goto EndPaper;
   }

/*
 * Make sure both images are identically masked about the origin:
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
 * Calculate average square as a function of radius:
 */

   lavsqrim(imdiff1);
   for (i = 0; i < imdiff1->rfile_length; i++) { 
     avg_sq1[i] = imdiff1->rfile[i];
     imdiff1->rfile[i] = 0;
   }
   lavsqrim(imdiff2);
   for (i = 0; i < imdiff2->rfile_length; i++) { 
     avg_sq2[i] = imdiff2->rfile[i];
     imdiff2->rfile[i] = 0;
   }

/*
 * Caluculate cross multiple average as fn of radius:
 */

   lxavgrim(imdiff1,imdiff2);
   for (i = 0; i < imdiff1->rfile_length; i++) { 
     xavg[i] = imdiff1->rfile[i];
     imdiff1->rfile[i] = 0;
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
     imdiff2->imscaler[i] = ( xavg[i] - avg1[i]*avg2[i] ) / 
		   ( avg_sq2[i] - avg2[i]*avg2[i] );  
     imdiff2->imoffsetr[i] = avg1[i] - imdiff2->imscaler[i] * avg2[i];
   }
/*   for(i = 0; i < imdiff1->rfile_length; i++) {
     imdiff2->imscaler[i] = avg1[i];
     imdiff2->imoffsetr[i] = xavg[i] - avg_sq1[i];
   }
*/

/*
 * Free memory:
 */

   free((RFILE_DATA_TYPE *)avg1);
   free((RFILE_DATA_TYPE *)avg2);
   free((RFILE_DATA_TYPE *)avg_sq1);
   free((RFILE_DATA_TYPE *)avg_sq2);
   free((RFILE_DATA_TYPE *)xavg);

EndPaper:
   
   return(return_value);
}
