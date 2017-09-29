/* LSUBRFIM.C - Subtract I(r) from a diffraction image.

    Author: Mike Wall
    Date: 4/4/94
    Version: 1.

 */

#include<mwmask.h>

int lsubrfim(DIFFIMAGE *imdiff)
{
    size_t
        r,
        c,
        radius,
        index = 0;

    struct xycoords rvec;

    int
        i,
        return_value;

    // First, compute the spline from the rfile

    float *tau, *cv;
    int n,ibcbeg,ibcend;
    int l,jd,kk;
    kk = 4;
    jd = 0;

    tau = (float *)malloc(sizeof(float)*imdiff->rfile_length);
    cv = (float *)malloc(sizeof(float)*4*imdiff->rfile_length);
    //  n = (int)lat->rfile_length;
    ibcbeg = 0;
    ibcend = 0;

    // populate variables for spline

    n=0;

    for (i=0;i<imdiff->rfile_length;i++) {
        if (imdiff->rfile[i] != imdiff->overload_tag && imdiff->rfile[i] != imdiff->ignore_tag && imdiff->rfile[i] != imdiff->mask_tag) {
            tau[n]=(float)i;
            cv[4*n] = imdiff->rfile[i];
            n++;
        }
    }

    lspline(tau,cv,&n,&ibcbeg,&ibcend);

    float radiusf;

    for(r = 0; r < imdiff->vpixels; r++) {
        rvec.y = r*imdiff->pixel_size_mm - imdiff->beam_mm.y;
        for(c = 0; c < imdiff->hpixels; c++) {
            rvec.x = c*imdiff->pixel_size_mm - imdiff->beam_mm.x;
            radiusf = (sqrtf(rvec.x*rvec.x + rvec.y*rvec.y)/imdiff->pixel_size_mm);
            radius = (size_t)radiusf;
            l = (int)radius + 1;

            index = r*imdiff->hpixels+c;

            if ((imdiff->image[index] != imdiff->overload_tag) &&
                    (imdiff->image[index] != imdiff->ignore_tag) &&
                    (imdiff->image[index] != imdiff->punch_tag)) {
                if (radiusf>= tau[0] && radiusf <= tau[n-2]) {
                    imdiff->image[index] -= (IMAGE_DATA_TYPE)lspleval(tau,cv,&l,&kk,&radiusf,&jd);
                } else {
                    imdiff->image[index] = imdiff->ignore_tag;
                }
            } else {
                imdiff->image[index] = imdiff->ignore_tag;
            }		  

            index++;
        }
    }

    free(tau);
    free(cv);
    return(return_value);
}


