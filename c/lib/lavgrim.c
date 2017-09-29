/* LAVGRIM.C - Calculate the average intensity vs. radius for an
   input image.

Author: Mike Wall   
Date: 4/3/93
Version: 1.

Edited by Veronica Pillar
Date: 4/21/14
Version: 2.

 */

#include<mwmask.h>

int lavgrim(DIFFIMAGE *imdiff)
{
    size_t
        *n,
        radius,
        index = 0;

    RCCOORDS_DATA
        r,
        c;

    struct xycoords rvec;

    n = (size_t *)calloc(MAX_RFILE_LENGTH, sizeof(size_t));
    imdiff->rfile_length = 0;
    for(r = 0; r < imdiff->vpixels; r++) {
        rvec.y = (XYZCOORDS_DATA)(r - imdiff->origin.r);
        for(c = 0; c < imdiff->hpixels; c++) {
            rvec.x = (XYZCOORDS_DATA)(c - imdiff->origin.c);
            radius = (size_t)sqrtf(rvec.y*rvec.y + rvec.x*rvec.x);
            if (radius > imdiff->rfile_length) 
                imdiff->rfile_length = radius;
            if (radius > MAX_RFILE_LENGTH)
                printf("Warning: MAX_RFILE_LENGTH is too small to store all the data from this image. Please edit MAX_RFILE_LENGTH in the header file and recompile.\n");

            if ((imdiff->image[index] != imdiff->overload_tag) &&
                    (imdiff->image[index] != imdiff->ignore_tag) &&
                    (imdiff->image[index] != imdiff->punch_tag)) {
                imdiff->rfile[radius] = (RFILE_DATA_TYPE)
                    ((float)(imdiff->image[index]-imdiff->value_offset)+ 
                     (float)n[radius]*(float)imdiff->rfile[radius]) /
                    (RFILE_DATA_TYPE)(n[radius] + 1);
                n[radius]++;
            }
            index++;
        }
    }
    free((size_t *)n);
}
