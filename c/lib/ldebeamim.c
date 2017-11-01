/* LDEBEAMIM.C - Mark a specified circular region in an image with ignore tags.

    Author: Veronica Pillar
    Date: 10/26/17
    Version: 1.

 */

#include<mwmask.h>

int ldebeamim(DIFFIMAGE *imdiff)
{
    long 
        l,
        m,
        r,
        c,
        radius;

    int
        return_value = 0;

    radius = imdiff->mask_outer_radius;

    for (l = -radius; l <= radius; l++){
        for (m = -radius; m <= radius; m++){
            if (l*l + m*m <= radius*radius){
                r = imdiff->pos.r + l;
                c = imdiff->pos.c + m;
                imdiff->image[r*imdiff->hpixels + c] = imdiff->ignore_tag;
            }
        }
    }
    return(return_value);
}

