/* LMINRIM.C - Calculate the minimum intensity as a function of radius for an
   input image.

Author: Mike Wall   
Date: 4/4/93
Version: 1.

 */

#include<mwmask.h>

int lminrim(DIFFIMAGE *imdiff)
{
    size_t
        radius,
        index = 0;

    RCCOORDS_DATA
        r,
        c;

    struct xycoords rvec;

    size_t *n;

    n = (size_t *)calloc(10000,sizeof(size_t));

    imdiff->rfile_length = 0;
    for(r = 0; r < imdiff->vpixels; r++) {
        rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
        for(c = 0; c < imdiff->hpixels; c++) {
            rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
            radius = (size_t)(sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm+.5);
            if (radius > imdiff->rfile_length) imdiff->rfile_length = radius;
            if (imdiff->image[index] != imdiff->overload_tag) {
                if ((imdiff->image[index] != (IMAGE_DATA_TYPE)imdiff->rfile_mask_tag) && 
                        ((imdiff->rfile[radius] == imdiff->rfile_mask_tag) ||
                         (imdiff->rfile[radius] > (RFILE_DATA_TYPE)imdiff->image[index]))) {
                    imdiff->rfile[radius] = (RFILE_DATA_TYPE)imdiff->image[index];
                    n[radius]++;
                }
            }
            index++;
        }
    }

}
