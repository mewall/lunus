/* LCLONEIM.C - Copy an image, creating a deep copy of the image data

   Author: Mike Wall
   Version:1.0
   Date:2/15/2018
			
	 Input argument is two diffraction images.  Output clone of imdiff2 is in imdiff1.

*/

#include<mwmask.h>

int lcloneim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	int 
		return_value = 0;

	*imdiff1 = *imdiff2;
	imdiff1->image = (IMAGE_DATA_TYPE *)malloc(imdiff1->image_length*sizeof(IMAGE_DATA_TYPE));
	memcpy((void *)imdiff1->image,(void *)imdiff2->image,imdiff1->image_length*sizeof(IMAGE_DATA_TYPE));
	return(return_value);

}
