/* LP6MMASK.C - Mark all pixels on or adjacent to a Pilatus 6M chip boundary with an ignore tag.
   
   Author: Veronica Pillar  
   Date: 10/11/17
   Version: 2.
   (Updated to be more aggressive--wider module boundary mask and added chip boundary masks)
   
   */

#include<mwmask.h>

int lp6mmask(DIFFIMAGE *imdiff)
{
    size_t 
        index = 0;

    RCCOORDS_DATA
        r,
        c;

    int 
        return_value = 0;

    for (r=0; r < imdiff->vpixels; r++) {
        for (c=0; c < imdiff->hpixels; c++) {

            // Mask module boundaries with 5 pixels padding on each side
            if (((r-5) % 212 > 184 ) || ((c-5) % 494 > 476)) {
                imdiff->image[index] = imdiff->ignore_tag;
            }

            // Mask chip boundaries (3 pixels wide total)
            if ((r+116) % 212 < 3) {
                imdiff->image[index] = imdiff->ignore_tag;
            }
            if ((c - 6*((int)(c/494)) - 1) % 61 > 57) {
                imdiff->image[index] = imdiff->ignore_tag;
            }

            // Block out that one chip that's always too light on CHESS's P6M
            if ((r < 96) && (c > 1787) && (c < 1846)) {
                imdiff->image[index] = imdiff->ignore_tag;
            }

            index++;
        }
    }


    return(return_value);
}
