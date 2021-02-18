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

  lclearim(imdiff1_in);

  for (pidx = 0; pidx < imdiff2_in->num_panels; pidx++) {
    imdiff1 = &imdiff1_in[pidx];
    imdiff2 = &imdiff2_in[pidx];

    if (imdiff1->num_panels != imdiff2->num_panels) {
      perror("LCLONEIM: Images have different number of panels. Aborting.\n");
      exit(1);
    }

    *imdiff1 = *imdiff2;
    imdiff1->image = (IMAGE_DATA_TYPE *)malloc(imdiff1->image_length*sizeof(IMAGE_DATA_TYPE));
    memcpy((void *)imdiff1->image,(void *)imdiff2->image,imdiff1->image_length*sizeof(IMAGE_DATA_TYPE));
    imdiff1->header = (char *)malloc(imdiff1->header_length*sizeof(char));
    memcpy((void *)imdiff1->header,(void *)imdiff2->header,imdiff1->header_length*sizeof(char));
    imdiff1->footer = (char *)malloc(imdiff1->footer_length*sizeof(char));
    memcpy((void *)imdiff1->footer,(void *)imdiff2->footer,imdiff1->footer_length*sizeof(char));
    imdiff1->rfile = (RFILE_DATA_TYPE *)malloc(imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
    memcpy((void *)imdiff1->rfile,(void *)imdiff2->rfile,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));    
    imdiff1->imscaler = (RFILE_DATA_TYPE *)malloc(imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
    memcpy((void *)imdiff1->imscaler,(void *)imdiff2->imscaler,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));    
    imdiff1->imoffsetr = (RFILE_DATA_TYPE *)malloc(imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
    memcpy((void *)imdiff1->imoffsetr,(void *)imdiff2->imoffsetr,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));    
    imdiff1->correction = (float *)malloc(imdiff1->image_length*sizeof(float));
    memcpy((void *)imdiff1->correction,(void *)imdiff2->correction,imdiff1->image_length*sizeof(float));
    //    imdiff1->imscaler = NULL;
    //    imdiff1->imoffsetr = NULL;
    imdiff1->mask = NULL;
    imdiff1->overload = NULL;
    imdiff1->peak = NULL;
    imdiff1->weights = NULL;
  }
  return(return_value);

}
