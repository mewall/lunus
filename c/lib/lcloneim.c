/* LCLONEIM.C - Copy an image, creating a deep copy of the image data

   Author: Mike Wall
   Version:1.0
   Date:2/15/2018
			
	 Input argument is two diffraction images.  Output clone of imdiff2 is in imdiff1.

*/

#include<mwmask.h>

int lcloneim(DIFFIMAGE *imdiff1_in, DIFFIMAGE *imdiff2_in)
{
  int 
    return_value = 0;

  int pidx;

  DIFFIMAGE *imdiff1, *imdiff2;

  // Need to free arrays that are malloc'd here before copying the struct

  for (pidx = 0; pidx < imdiff2_in->num_panels; pidx++) {
    imdiff1 = &imdiff1_in[pidx];
    imdiff2 = &imdiff2_in[pidx];

    if (imdiff1->num_panels != imdiff2->num_panels) {
      perror("LCLONEIM: Images have different number of panels. Aborting.\n");
      exit(1);
    }

    if (imdiff1->image != NULL) free(imdiff1->image);
    if (imdiff1->rfile != NULL) free(imdiff1->rfile);
    *imdiff1 = *imdiff2;
    //	imdiff1->image = NULL;
    imdiff1->image = (IMAGE_DATA_TYPE *)malloc(imdiff1->image_length*sizeof(IMAGE_DATA_TYPE));
    memcpy((void *)imdiff1->image,(void *)imdiff2->image,imdiff1->image_length*sizeof(IMAGE_DATA_TYPE));
    /*
      imdiff1->header = NULL;
      imdiff1->header = (IMAGE_DATA_TYPE *)malloc(imdiff1->header_length*sizeof(char));
      memcpy((void *)imdiff1->header,(void *)imdiff2->header,imdiff1->header_length*sizeof(char));
      imdiff1->rfile = NULL;
    */
    imdiff1->rfile = (RFILE_DATA_TYPE *)malloc(imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
    memcpy((void *)imdiff1->rfile,(void *)imdiff2->rfile,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
    /*
      imdiff1->imscaler = NULL;
      imdiff1->imoffsetr = NULL;
      imdiff1->mask = NULL;
      imdiff1->overload = NULL;
      imdiff1->peak = NULL;
      imdiff1->weights = NULL;
      imdiff1->correction = NULL;
      imdiff1->correction = (float *)malloc(imdiff1->image_length*sizeof(float));
      memcpy((void *)imdiff1->correction,(void *)imdiff2->correction,imdiff1->image_length*sizeof(float));
    */
  }
  return(return_value);

}
