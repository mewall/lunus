/* LGETPIX.C - Extract the pixel values from a genlat_parallel_labelit_punch output file.

Author: Veronica Pillar (modified from Mike Wall's lgetpks.c)
Date: 4/22/15
Version: 1.

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mwmask.h>


int lgetpix(DIFFIMAGE *imdiff)
{
    double
	index;
    int
	count;

    count = 0;
    
    while(fscanf(imdiff->infile, "%lf", &index) != EOF) {
        if (count > MAX_PEAKS) {
            printf("read %d pixels to punch, which is the maximum allowed; skipping remainder of file\n");
            break;
        }
        imdiff->peak[count].x = ((int)index) % imdiff->hpixels;
        imdiff->peak[count].y = ((int)index) / imdiff->hpixels;
        count++;
    }

    imdiff->peak_count = count;

    return(0);
}
