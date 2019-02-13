/* LGETOVLD.C - Generate a list of overload positions from an input TV6 tiff 
		file.

   Author: Mike Wall
   Date: 3/14/94
   Version: 1.0
   Description:
		Generate the list of overload pixels in the input diffraction
		image *imdiff, and write the list in a table in the input
		structure, imdiff->(*overload).
*/

#include <mwmask.h>

int lgetovld(DIFFIMAGE *imdiff)
{
	long 
		i=0,
		j=0;

	while (i < imdiff->image_length) {
		if ((imdiff->image[i] == imdiff->overload_tag) ||
		    (imdiff->image[i] == imdiff->ignore_tag)) {
			imdiff->overload[j].r = (short)(i/imdiff->hpixels);
			imdiff->overload[j].c = (short)(i%imdiff->hpixels);
			j++;
		}
		i++;
	}
	imdiff->overload_count = j;
	return(0);
}
