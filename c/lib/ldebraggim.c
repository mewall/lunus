/* LDEBRAGGIM.C - Replace values in an unfiltered diffraction image with their mode-filtered values if the squared difference between the two is above a certain threshold.

Author: Veronica Pillar (based on Mike Wall's codes)
Version:1.0
Date:4/29/16

Input argument is three diffraction images. Output is stored in the first diffraction image. Origins of all images are assumed to coincide.
*/

#include<mwmask.h>

int ldebraggim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2, DIFFIMAGE *imdiff3)
{
    RCCOORDS_DATA
        r,
        c,
        rm,
        cm;
    size_t
        radius,
        index = 0,
        index2;

    IMAGE_DATA_TYPE
        thresh,
        moat_tag;

    int
        moat;

    struct rccoords 
        rvec;

    int 
        return_value = 0;

    thresh = imdiff3->upper_threshold;
    moat = imdiff3->mode_width;

    //set the moat tag
    moat_tag = MAX_IMAGE_DATA_VALUE;
    while ((moat_tag == imdiff3->overload_tag) || (moat_tag == imdiff3->ignore_tag) ||
            (moat_tag == imdiff3->punch_tag)) {
        moat_tag--;
    }

    // first pass: mark imdiff3 pixels above thresh as to be punched
    index = 0;
    for(r = 0; r < imdiff3->vpixels; r++) {
        for(c = 0; c < imdiff3->hpixels; c++) {
            if ((imdiff3->image[index] != imdiff3->overload_tag) &&
                    (imdiff3->image[index] != imdiff3->ignore_tag ) && 
                    (imdiff3->image[index] != imdiff3->punch_tag )) {
                if (imdiff3->image[index] > thresh) {
                    imdiff3->image[index] = imdiff3->punch_tag;
                }
            }
            index++;
        }
    }

    // second pass: mark moat pixels in imdiff3
    index = 0;
    for(r = 0; r < imdiff3->vpixels; r++) {
        for(c = 0; c < imdiff3->hpixels; c++) {
            if (imdiff3->image[index] == imdiff3->punch_tag) {
                //found a punch tag, so look through surrounding pixels
                for (rm = r - moat; rm <= r+moat; rm++) {
                    if ((rm >= 0) && (rm < imdiff3->vpixels)) {
                        for (cm = c - moat; cm <= c+moat; cm++) {
                            if ((cm >= 0) && (cm < imdiff3->hpixels)) {
                                index2 = rm*imdiff3->hpixels+cm;
                                if ((imdiff3->image[index2] != imdiff3->overload_tag) &&
                                        (imdiff3->image[index2] != imdiff3->ignore_tag) &&
                                        (imdiff3->image[index2] != imdiff3->punch_tag)) {
                                    imdiff3->image[index2] = moat_tag;
                                }    
                            }
                        }
                    }
                }
            }
            index++;
        }
    }

    // third pass: if imdiff3 has either a punch tag or moat tag, replace imdiff1 value
    // with imdiff2 value
    /*
    index = 0;
    for(r = 0; r < imdiff1->vpixels; r++) {
        for(c = 0; c < imdiff1->hpixels; c++) {
            if ((imdiff1->image[index] != imdiff1->overload_tag) &&
                    (imdiff1->image[index] != imdiff1->ignore_tag)) {
                if ((imdiff2->image[index] != imdiff2->ignore_tag) &&
                        (imdiff2->image[index] != imdiff2->overload_tag) &&
                        (imdiff3->image[index] != imdiff3->overload_tag) &&
                        (imdiff3->image[index] != imdiff3->ignore_tag )) {
                    if ((imdiff3->image[index] == imdiff3->punch_tag) ||
                            (imdiff3->image[index] == moat_tag)) {
                        imdiff1->image[index] = imdiff2->image[index];
                        //imdiff1->image[index] = imdiff1->punch_tag;
                    }
                }
                else {
                    imdiff1->image[index] = imdiff1->ignore_tag;
                    printf("why are we here?\n");
                }
            }
            index++;
        }
    }
*/
    // third pass: if imdiff3 has either a punch tag or moat tag, replace imdiff1 value with a punch tag.
    index = 0;
    for(r = 0; r < imdiff1->vpixels; r++) {
        for(c = 0; c < imdiff1->hpixels; c++) {
            if ((imdiff1->image[index] != imdiff1->overload_tag) &&
                    (imdiff1->image[index] != imdiff1->ignore_tag) &&
                    (imdiff1->image[index] != imdiff1->punch_tag)) {
                if ((imdiff2->image[index] != imdiff2->ignore_tag) &&
                        (imdiff2->image[index] != imdiff2->overload_tag) &&
                        (imdiff3->image[index] != imdiff3->overload_tag) &&
                        (imdiff3->image[index] != imdiff3->ignore_tag )) {
                    if ((imdiff3->image[index] == imdiff3->punch_tag) ||
                            (imdiff3->image[index] == moat_tag)) {
                        imdiff1->image[index] = imdiff1->punch_tag;
                    }
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
