/* LSETPARAMSIM.C - Set image parameters using a parameter definitions string.

   Author: Mike Wall
   Date: 12/13/2018
   Version: 1.

*/

#include<mwmask.h>
#include <string.h>

int lsetparamsim(DIFFIMAGE *imdiff)
{

  char *deck = imdiff->params;

	if (lgettag(deck,"\npunchim_xmax") != NULL) {
	  imdiff->punchim_upper.c = atoi(lgettag(deck,"\npunchim_xmax"));
	}

	if (lgettag(deck,"\npunchim_xmin") != NULL) {
	  imdiff->punchim_lower.c = atoi(lgettag(deck,"\npunchim_xmin"));
	}

	if (lgettag(deck,"\npunchim_ymax") != NULL) {
	  imdiff->punchim_upper.r = atoi(lgettag(deck,"\npunchim_ymax"));
	}

	if (lgettag(deck,"\npunchim_ymin") != NULL) {
	  imdiff->punchim_lower.r = atoi(lgettag(deck,"\npunchim_ymin"));
	}

	if (lgettag(deck,"\nwindim_xmax") != NULL) {
	  imdiff->window_upper.c = atoi(lgettag(deck,"\nwindim_xmax"));
	}

	if (lgettag(deck,"\nwindim_xmin") != NULL) {
	  imdiff->window_lower.c = atoi(lgettag(deck,"\nwindim_xmin"));
	}

	if (lgettag(deck,"\nwindim_ymax") != NULL) {
	  imdiff->window_upper.r = atoi(lgettag(deck,"\nwindim_ymax"));
	}

	if (lgettag(deck,"\nwindim_ymin") != NULL) {
	  imdiff->window_lower.r = atoi(lgettag(deck,"\nwindim_ymin"));
	}

	if (lgettag(deck,"\nscale_inner_radius") != NULL) {
	  imdiff->scale_inner_radius = atoi(lgettag(deck,"\nscale_inner_radius"));
	}

	if (lgettag(deck,"\nscale_outer_radius") != NULL) {
	  imdiff->scale_outer_radius = atoi(lgettag(deck,"\nscale_outer_radius"));
	}

	if(lgettag(deck,"\nthrshim_max") != NULL) {
	  imdiff->upper_threshold = atoi(lgettag(deck,"\nthrshim_max"));
	}

	if(lgettag(deck,"\nthrshim_min") != NULL) {
	  imdiff->lower_threshold = atoi(lgettag(deck,"\nthrshim_min"));
	}

	if(lgettag(deck,"\nmodeim_bin_size") != NULL) {
	  imdiff->mode_binsize = atoi(lgettag(deck,"\nmodeim_bin_size"));
	}

	if(lgettag(deck,"\nmodeim_kernel_width") != NULL) {
	  imdiff->mode_height = atoi(lgettag(deck,"\nmodeim_kernel_width")) - 1;
	  imdiff->mode_width = imdiff->mode_height;
	}

	if(lgettag(deck,"\npolarim_offset") != NULL) {
	  imdiff->polarization_offset = atof(lgettag(deck,"\npolarim_offset"));
	}

	if(lgettag(deck,"\npolarim_polarization") != NULL) {
	  imdiff->polarization = atof(lgettag(deck,"\npolarim_polarization"));
	}

	if (lgettag(deck,"\ndistance_mm") != NULL) {
	  imdiff->distance_mm = atof(lgettag(deck,"\ndistance_mm"));
	}

  return(0);
}

