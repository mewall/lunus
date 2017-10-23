/* LFILLIM.C - Replace values in an unfiltered diffraction image with their mode-filtered values if the squared difference between the two is above a certain threshold.

Author: Veronica Pillar (based on Mike Wall's codes)
Version:1.0
Date:5/22/16

Input argument is two diffraction images. Output is stored in the first diffraction image. Origins of both images are assumed to coincide.
*/

#include<mwmask.h>

int lfillim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
    RCCOORDS_DATA
        r,
        c;

    size_t
        index = 0;

    int 
        return_value = 0;

    index = 0;
    for(r = 0; r < imdiff1->vpixels; r++) {
        for(c = 0; c < imdiff1->hpixels; c++) {
            if ((imdiff1->image[index] == imdiff1->overload_tag) ||
                    (imdiff1->image[index] == imdiff1->punch_tag)) {
                if ((imdiff2->image[index] != imdiff2->ignore_tag) &&
                        (imdiff2->image[index] != imdiff2->overload_tag) &&
                        (imdiff2->image[index] != imdiff2->punch_tag )) {
                    imdiff1->image[index] = imdiff2->image[index];
                }
                else {
                    imdiff1->image[index] = imdiff1->ignore_tag;
                    printf("why are we here?\n");
                }
            }
            index++;
        }
    }

    return(return_value);

}
