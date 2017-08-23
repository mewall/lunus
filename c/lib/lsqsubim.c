/* LSQSUBIM.C - Calculate the square of the difference between two images, storing the result in the third image.

Author: Veronica Pillar (based on Mike Wall's codes)
Version:1.0
Date:4/28/16

Input argument is three diffraction images, the last of which is not read.  
Output is stored in the third image. If the squared difference is too high, it's stored as the max image value minus 3.

Third image must have an origin that coincides with the origin of at least one
of the other two images.
*/

#include<mwmask.h>

int lsqsubim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2, DIFFIMAGE *imdiff3)
{
    RCCOORDS_DATA
        r1,
        r2,
        r3,
        c1,
        c2,
        c3;
    size_t
        radius,
        index1 = 0,
        index2 = 0,
        index3 = 0;
    IMAGE_DATA_TYPE
        diff;

    struct rccoords 
        rvec;

    int 
        return_value = 0;

    for(r1 = 0; r1 < imdiff1->vpixels; r1++) {
        rvec.r = r1 - imdiff1->origin.r;
        r2 = (rvec.r + imdiff2->origin.r);
        r3 = (rvec.r + imdiff3->origin.r);
        for(c1 = 0; c1 < imdiff1->hpixels; c1++) {
            rvec.c = c1 - imdiff1->origin.c;
            c3 = (rvec.c + imdiff3->origin.c);
            index3 = r3*imdiff3->hpixels + c3;
            if ((imdiff1->image[index1] != imdiff1->overload_tag) &&
                    (imdiff1->image[index1] != imdiff1->ignore_tag) &&
                    (imdiff1->image[index1] != imdiff1->punch_tag)) {
                c2 = (rvec.c + imdiff2->origin.c);
                if ((r2 >= 0) && (r2 < imdiff2->vpixels) && (c2 >= 0) && 
                        (c2 < imdiff2->hpixels)) {
                    index2 = r2*imdiff2->hpixels + c2;
                    if ((imdiff2->image[index2] != imdiff2->overload_tag) &&
                            (imdiff2->image[index2] != imdiff2->ignore_tag ) &&
                            (imdiff2->image[index2] != imdiff2->punch_tag )) {

                        diff = imdiff1->image[index1] - imdiff2->image[index2];
                        if (diff*diff < (MAX_IMAGE_DATA_VALUE -3))
                            imdiff3->image[index3] = diff*diff;
                        else
                            imdiff3->image[index3] = imdiff3->punch_tag;
                        // above lines won't work if image 3 center is at neither image2's 
                        // nor image1's, but there's no reason that should be.		    
                    }
                    else {
                        imdiff3->image[index3] = imdiff3->ignore_tag;
                    }
                }
                else if ((r3 >= 0) && (r3 < imdiff3->vpixels) && (c3 >= 0) &&
                        (c3 < imdiff3->hpixels)) {
                    index3 = r3*imdiff3->hpixels + c3;
                    imdiff3->image[index3] = imdiff3->ignore_tag;
                }
            }
            else if ((r3 >= 0) && (r3 < imdiff3->vpixels) && (c3 >= 0) &&
                        (c3 < imdiff3->hpixels)) {
                imdiff3->image[index3] = imdiff3->ignore_tag;
            }
            index1++;
        }
    }
    return(return_value);

}

