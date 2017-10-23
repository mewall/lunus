/* LREADRF.C - Read an rfile from a file.

Author: Mike Wall
Date: 9/27/95
Version: 1.

 */

#include<mwmask.h>

int lreadrf(DIFFIMAGE *imdiff)
{
    size_t
        i = 0,
        num_read;

    RFILE_DATA_TYPE
        basket;

    int
        return_value = 0;  

    while ((num_read = fread(&imdiff->rfile[i],sizeof(RFILE_DATA_TYPE), 1,
                    imdiff->infile)) == 1) {
        i++;
        if (i == imdiff->rfile_length) {
            if ((num_read = fread(&basket,sizeof(RFILE_DATA_TYPE),1, 
            imdiff->infile)) == 1) {
               // printf("Warning: rfile is too long for this image. Only %zu "
                 //   "elements were read.\n",i); 
            }
            break;
        }

    }
    imdiff->rfile_length = i;
    if (imdiff->rfile_length == 0) {
        sprintf(imdiff->error_msg, "\nLREADRF: Didn't read anything from "
                "rfile.\n\n");
        return_value = 1;
    }
    return(return_value);
}
