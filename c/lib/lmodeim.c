/* LMODEIM.C - Remove the peaks from an image by mode filtering using
   a specified mask size.

Author: Mike Wall 
Date: 2/2/95
Version: 1.

"modeim <image in> <image out> <mask size> <bin size>"

Input is ascii coordinates file.  Output is 16-bit 
image of specified size (1024 x 1024 default).

 */


#include<mwmask.h>
#ifdef USE_OPENMP
#include<omp.h>
#endif

int lmodeim(DIFFIMAGE *imdiff) 
{

    RCCOORDS_DATA 
        half_height,
        half_width,
        n, 
        m, 
        r, 
        c; 

    size_t
        avg_max_count_count = 0,
        index = 0; 

    float
        avg_max_count = 0;

    IMAGE_DATA_TYPE
        *image;

    int
        return_value = 0,
        nt;

    /* 
     * Allocate working image: 
     */ 

    image = (IMAGE_DATA_TYPE *)calloc(imdiff->image_length, 
            sizeof(IMAGE_DATA_TYPE)); 
    if (!image) {
        sprintf(imdiff->error_msg,"\nLMODEIM:  Couldn't allocate arrays.\n\n");
        return_value = 1;
        goto CloseShop;
    }

    half_height = imdiff->mode_height / 2;
    half_width = imdiff->mode_width / 2;
    index = 0;


    size_t j;

#ifdef USE_OPENMP
    nt = omp_get_max_threads();
//    printf("Using OpenMP with %d threads\n",nt);
    {
#pragma omp parallel for shared(imdiff,image,half_height,half_width) private(j)
#endif
        for (j=0; j<imdiff->vpixels; j++) {
            size_t i;
            size_t *count;
            count = (size_t *)calloc(MAX_IMAGE_DATA_VALUE+32769,sizeof(size_t)); 
            unsigned int *count_pointer;
            count_pointer = (unsigned int *)calloc((imdiff->mode_height+1) *
                    (imdiff->mode_width+1), 
                    sizeof(unsigned int));
            for (i=0; i<imdiff->hpixels; i++) {
                float mode_value=0.0;
                size_t max_count=0;
                size_t mode_ct=1;
                size_t l=0;
                RCCOORDS_DATA n,m,r,c;
                size_t k;
                size_t index;

                index = j*imdiff->hpixels+i;
                if (imdiff->image[index] != imdiff->ignore_tag) {
                    for(n=-half_height; n<=half_height; n++) {
                        r = j + n;
                        for(m=-half_width; m<=half_width; m++) {
                            c = i + m;
                            if (!((r < 0) || (r >= imdiff->vpixels) || (c < 0) ||       
                                        (c >= imdiff->hpixels))) {
                                size_t imd_index;
                                imd_index = index + n*imdiff->hpixels + m;
                                if ((imdiff->image[imd_index] != imdiff->overload_tag) &&
                                        (imdiff->image[imd_index] != imdiff->ignore_tag)) {
                                    count_pointer[l]=(imdiff->image[imd_index] - 
                                            (imdiff->image[imd_index] % 
                                             imdiff->mode_binsize));
                                    count[count_pointer[l++]]++;
                                }
                            }
                        }
                    }
                    if (l==0) {
                        image[index]=imdiff->ignore_tag;
                    }
                    else {
                        for(k=0;k<l;k++) {
                            if (count[count_pointer[k]] == max_count) {
                                mode_value = (float)((float)(mode_ct*mode_value +
                                            (float)count_pointer[k])/ 
                                        (float)(mode_ct+1));
                                mode_ct++;
                            }
                            else if (count[count_pointer[k]] > max_count) {
                                mode_value = (float)count_pointer[k];
                                max_count = count[count_pointer[k]];
                                mode_ct = 1;
                            }
                        }
                        for(k=0;k<l;k++) count[count_pointer[k]] = 0;
                        image[index] = (IMAGE_DATA_TYPE)mode_value;
                        avg_max_count = (avg_max_count*avg_max_count_count +
                                max_count) / (float)(avg_max_count_count + 1);
                        avg_max_count_count++;

                    }
                }
                else {
                    image[index] = imdiff->image[index];
                }
            }
            free(count);
            free(count_pointer);
        }
#ifdef USE_OPENMP
    }
#endif
    for(index=0;index<imdiff->image_length; index++) {
        imdiff->image[index] = image[index];
    }
    printf("avg_max_count = %f\n\n",avg_max_count);/***/
    free((IMAGE_DATA_TYPE *)image);/***/
CloseShop:
    return(return_value);
}







