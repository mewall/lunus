/* LSMOOTHIM.C - Smooth an image with rolling-box averaging.

Author: Veronica Pillar   
Date: 4/22/14
Version: 1.

*/

#include<mwmask.h>

int lsmoothim(DIFFIMAGE *imdiff, int width)
{
    size_t
        index = 0,
              ind;

    int
        n,
        i,
        j;

    RCCOORDS_DATA
        r,
        c;

    float
        sum;

    struct xycoords rvec;

    IMAGE_DATA_TYPE
        *tempimage;

    tempimage = (IMAGE_DATA_TYPE*)calloc(imdiff->image_length, sizeof(IMAGE_DATA_TYPE));

    for(r = 0; r < imdiff->vpixels; r++) {
        for(c = 0; c < imdiff->hpixels; c++) {
            if ((imdiff->image[index] != imdiff->overload_tag) && 
                    (imdiff->image[index] != imdiff->ignore_tag)) {

                /*
                 * Rolling-box averaging:
                 */

                sum = 0;
                n = 0;
                for (i = r-width; i <= r+width; i++) {
                    if ((i < 0) || (i >= imdiff->vpixels))
                        continue;
                    for (j = c-width; j <= c+width; j++) {
                        if ((j < 0) || (j >= imdiff->hpixels))
                            continue;
                        ind = i*imdiff->hpixels + j;

                        if ((imdiff->image[ind] != imdiff->overload_tag ) && 
                                (imdiff->image[ind] != imdiff->ignore_tag)) {
                            sum += imdiff->image[ind];
                            n++;
                        }
                    }
                }
                tempimage[index] = sum/(float)n;

            }
            else {
                tempimage[index] = imdiff->image[index];
            }
            index++;
        }
    }

    for (i = 0; i < imdiff->image_length; i++) {
        imdiff->image[i] = tempimage[i];
    }

}
