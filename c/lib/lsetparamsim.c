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

	if (strstr(deck,"\npunchim_xmax") != NULL) {
	  imdiff->punchim_upper.c = lgettagi(deck,"\npunchim_xmax");
	}

	if (strstr(deck,"\npunchim_xmin") != NULL) {
	  imdiff->punchim_lower.c = lgettagi(deck,"\npunchim_xmin");
	}

	if (strstr(deck,"\npunchim_ymax") != NULL) {
	  imdiff->punchim_upper.r = lgettagi(deck,"\npunchim_ymax");
	}

	if (strstr(deck,"\npunchim_ymin") != NULL) {
	  imdiff->punchim_lower.r = lgettagi(deck,"\npunchim_ymin");
	}

	if (strstr(deck,"\nwindim_xmax") != NULL) {
	  imdiff->window_upper.c = lgettagi(deck,"\nwindim_xmax");
	}

	if (strstr(deck,"\nwindim_xmin") != NULL) {
	  imdiff->window_lower.c = lgettagi(deck,"\nwindim_xmin");
	}

	if (strstr(deck,"\nwindim_ymax") != NULL) {
	  imdiff->window_upper.r = lgettagi(deck,"\nwindim_ymax");
	}

	if (strstr(deck,"\nwindim_ymin") != NULL) {
	  imdiff->window_lower.r = lgettagi(deck,"\nwindim_ymin");
	}

	if (strstr(deck,"\nscale_inner_radius") != NULL) {
	  imdiff->scale_inner_radius = lgettagi(deck,"\nscale_inner_radius");
	}

	if (strstr(deck,"\nscale_outer_radius") != NULL) {
	  imdiff->scale_outer_radius = lgettagi(deck,"\nscale_outer_radius");
	}

	if (strstr(deck,"\nthrshim_max") != NULL) {
	  imdiff->upper_threshold = lgettagi(deck,"\nthrshim_max");
	}

	if (strstr(deck,"\nthrshim_min") != NULL) {
	  imdiff->lower_threshold = lgettagi(deck,"\nthrshim_min");
	}

	if (strstr(deck,"\nmodeim_bin_size") != NULL) {
	  imdiff->mode_binsize = lgettagi(deck,"\nmodeim_bin_size");
	}

	if (strstr(deck,"\nmodeim_kernel_width") != NULL) {
	  imdiff->mode_height = lgettagi(deck,"\nmodeim_kernel_width") - 1;
	  imdiff->mode_width = imdiff->mode_height;
	}

	if (strstr(deck,"\npolarim_offset") != NULL) {
	  imdiff->polarization_offset = lgettagf(deck,"\npolarim_offset");
	}

	if (strstr(deck,"\npolarim_polarization") != NULL) {
	  imdiff->polarization = lgettagf(deck,"\npolarim_polarization");
	}

	if (strstr(deck,"\ndistance_mm") != NULL) {
	  imdiff->distance_mm = lgettagf(deck,"\ndistance_mm");
	}

  return(0);
}

