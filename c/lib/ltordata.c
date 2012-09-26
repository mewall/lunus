/* LTORDATA.C - Generate a data structure which indexes pixels by radius
		rather than by (x,y).

   Author: Mike Wall
   Version:1.0
   Date:4/7/94
				 
	 Input argument is a diffraction image.  Output is stored in structure
		pointed to by pointer in diffraction image structure.
*/

#include<mwmask.h>

int ltordata(DIFFIMAGE *imdiff)
{
	RCCOORDS_DATA
		r,
		c;
	size_t
		bytes_allocated = 0,
		radius,
		index = 0;

	struct rccoords 
		rvec;

  imdiff->rdata_radius = 0;
  for(r = 0; r < imdiff->vpixels; r++) {
    rvec.r = r - imdiff->origin.r;
    for(c = 0; c < imdiff->hpixels; c++) {
      rvec.c = c - imdiff->origin.c;
      radius = (size_t)sqrtf((float)(rvec.r*rvec.r) + 
	  	(float)(rvec.c*rvec.c));
      if ((imdiff->image[index] != imdiff->overload_tag) &&
	    (imdiff->image[index] != imdiff->ignore_tag)) {
        if (radius > imdiff->rdata_radius) {
          imdiff->rdata_radius = radius;
        }
	if (imdiff->rdata[radius].allocate_flag != VALUE_ALLOCATED) {
          imdiff->rdata[radius].value = (IMAGE_DATA_TYPE *)
	                            calloc(RDATA_MALLOC_FACTOR * 
	                            (radius+1),sizeof(IMAGE_DATA_TYPE));
          imdiff->rdata[radius].allocate_flag = VALUE_ALLOCATED;
	  bytes_allocated += RDATA_MALLOC_FACTOR * radius * 
				sizeof(IMAGE_DATA_TYPE);
printf("\nRadius = %ld, bytes allocated = %ld\n\n",(long)radius,
	(long)bytes_allocated);
        }
        imdiff->rdata[radius].value[imdiff->rdata[radius].count] = 
			      imdiff->image[index];
        imdiff->rdata[radius].count++;
      }
      index++;
    }
  }
}
