/* LSETPED - Change the pedestal value of an image.

Author: Veronica Pillar
Date: 10/1/15
Version: 1.

*/

#include<mwmask.h>

int lsetped(DIFFIMAGE *imdiff)
{
    size_t 
	index;

    int 
	return_value = 0;

    IMAGE_DATA_TYPE 
	new_ped,
	old_ped;

    new_ped = imdiff->mode_binsize; //actually the new pedestal value
    old_ped = imdiff->value_offset;
    for(index=0; index < imdiff->image_length; index++) {
	if((imdiff->image[index] != imdiff->overload_tag) &&
		(imdiff->image[index] != imdiff->ignore_tag) &&
		(imdiff->image[index] != imdiff->ignore_tag)) {
	    imdiff->image[index] = imdiff->image[index] - old_ped + new_ped;
	}
    }
    return(return_value);
}
