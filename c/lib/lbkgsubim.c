/* LBKGSUBIM.C - Calculate A*X - B for two images A, B, storing the result
		in the first image.

   Author: Mike Wall
   Version:1.0
   Date:12/17/19
			
	 Input argument is two diffraction images.  Output is
  stored in imdiff1->image.

*/

#include<mwmask.h>

int lbkgsubim(DIFFIMAGE *imdiff1_in, DIFFIMAGE *imdiff2_in)
{
  RCCOORDS_DATA
    r1,
    r2,
    c1,
    c2;
  size_t
    radius,
    i,
    index = 0,
    index1 = 0,
    index2 = 0;

  struct rccoords 
    rvec;

  int 
    return_value = 0;

  IMAGE_DATA_TYPE minval = 0;

  int pidx;

  DIFFIMAGE *imdiff1, *imdiff2;

  if (imdiff1_in->num_panels != imdiff2_in->num_panels) {
    perror("LBKGSUBIM: Images do not have the same number of panels. Aborting\n");
    exit(1);
  }

  for (pidx = 0; pidx < imdiff1_in->num_panels; pidx++) {
    imdiff1 = &imdiff1_in[pidx];
    imdiff2 = &imdiff2_in[pidx];

    if ((imdiff1->hpixels != imdiff2->hpixels) || (imdiff1->vpixels != imdiff2->vpixels)) {
      perror("LBKGSUBIM: Image panels do not have the same shape. Aborting.\n");
      exit(1);
    }

    for (index=0;index<imdiff1->image_length;index++) {
      if ((imdiff2->image[index] != imdiff2->overload_tag) &&
	  (imdiff2->image[index] != imdiff2->ignore_tag )) {
	imdiff1->image[index] = roundf((imdiff1->image[index]-imdiff1->value_offset) - (imdiff2->image[index]-imdiff2->value_offset)*imdiff1->background_subtraction_factor);
	if (imdiff1->image[index] < minval) {
	  minval = imdiff1->image[index];
	}
      }
      else {
	imdiff1->image[index] = imdiff1->ignore_tag;
      }
    }

    if (minval < 0) {
      for (i = 0;i<imdiff1->image_length;i++) {	    
	if ((imdiff1->image[i] != imdiff1->overload_tag) &&
	    (imdiff1->image[i] != imdiff1->ignore_tag)) {
	  imdiff1->image[i] -= minval;
	}
      }
      imdiff1->value_offset = -minval;
      //      printf("LBKGSUBIM: value_offset changed to %d\n",imdiff1->value_offset);
    } else imdiff1->value_offset = 0;
  }

  return(return_value);

}

int lbkgsubim_align(DIFFIMAGE *imdiff1_in, DIFFIMAGE *imdiff2_in)
{
  RCCOORDS_DATA
    r1,
    r2,
    c1,
    c2;
  size_t
    radius,
    i,
    index1 = 0,
    index2 = 0;

  struct rccoords 
    rvec;

  int 
    return_value = 0;

  IMAGE_DATA_TYPE minval = 0;

  int pidx;

  DIFFIMAGE *imdiff1, *imdiff2;

  if (imdiff1_in->num_panels != imdiff2_in->num_panels) {
    perror("LBKGSUBIM: Images do not have the same number of panels. Aborting\n");
    exit(1);
  }

  for (pidx = 0; pidx < imdiff1_in->num_panels; pidx++) {
    imdiff1 = &imdiff1_in[pidx];
    imdiff2 = &imdiff2_in[pidx];

    if ((imdiff1->hpixels != imdiff2->hpixels) || (imdiff1->vpixels != imdiff2->vpixels)) {
      perror("LBKGSUBIM: Image panels do not have the same shape. Aborting.\n");
      exit(1);
    }

    index1 = 0;
    for(r1 = 0; r1 < imdiff1->vpixels; r1++) {
      rvec.r = r1 - imdiff1->origin.r;
      r2 = (rvec.r + imdiff2->origin.r);
      for(c1 = 0; c1 < imdiff1->hpixels; c1++) {
	rvec.c = c1 - imdiff1->origin.c;
	if ((imdiff1->image[index1] != imdiff1->overload_tag) &&
	    (imdiff1->image[index1] != imdiff1->ignore_tag)) {
	  c2 = (rvec.c + imdiff2->origin.c);
	  if ((r2 > 0) && (r2 < imdiff2->vpixels) && (c2 > 0) && 
	      (c2 < imdiff2->hpixels)) {
	    index2 = r2*imdiff2->hpixels + c2;
	    if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
		(imdiff2->image[index2] != imdiff2->ignore_tag )) {
	      imdiff1->image[index1] = roundf((imdiff1->image[index1]-imdiff1->value_offset) - (imdiff2->image[index2]-imdiff2->value_offset)*imdiff1->background_subtraction_factor);
	      if (imdiff1->image[index1] < minval) {
		minval = imdiff1->image[index1];
	      }
	    }
	    else {
	      imdiff1->image[index1] = imdiff1->ignore_tag;
	    }
	  }
	  else {
	    imdiff1->image[index1] = imdiff1->ignore_tag;
	  }
	}
	index1++;
      }
    }

    if (minval < 0) {
      for (i = 0;i<imdiff1->image_length;i++) {	    
	if ((imdiff1->image[i] != imdiff1->overload_tag) &&
	    (imdiff1->image[i] != imdiff1->ignore_tag)) {
	  imdiff1->image[i] -= minval;
	}
      }
      imdiff1->value_offset = -minval;
      printf("LBKGSUBIM: value_offset changed to %d\n",imdiff1->value_offset);
    } else imdiff1->value_offset = 0;
  }

  return(return_value);

}
