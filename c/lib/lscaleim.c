/* LSCALEIM.C - Scale one image to another pixel-by-pixel within a radial region.

   Author: Mike Wall
   Version:1.0
   Date:10/25/2016
				 
*/

#include<mwmask.h>

int lscaleim(DIFFIMAGE *imdiff1, DIFFIMAGE *imdiff2)
{
	float
	  xx,xy,yy,
	  avg_xx,
	  avg_xy,
	  avg_yy;

	int 
		return_value = 0;

 	size_t
	  i,r,c,index,ct;

	short
	  radius;

	struct xycoords r_mm;

/*
 * Calculate averages:
 */
	ct = 0;
	avg_xx = 0.0;
	avg_xy = 0.0;
	avg_yy = 0.0;
	index = 0;
	for (r=0; r < imdiff1->vpixels; r++) {
	  r_mm.y = r*imdiff1->pixel_size_mm - imdiff1->beam_mm.y;
	  for (c=0; c < imdiff1->hpixels; c++) {
	    r_mm.x = c*imdiff1->pixel_size_mm - imdiff1->beam_mm.x;
	    radius = (short)(sqrtf(r_mm.x*r_mm.x+r_mm.y*r_mm.y)/imdiff1->pixel_size_mm+0.5);
	    if ((imdiff1->image[index] != imdiff1->overload_tag) &&
		  (imdiff1->image[index] != imdiff1->ignore_tag) &&
		(imdiff2->image[index] != imdiff2->overload_tag) &&
		(imdiff2->image[index] != imdiff2->ignore_tag)) {
	      if (radius > imdiff1->mask_inner_radius &&
		  radius < imdiff1->mask_outer_radius) {
		xx = imdiff1->image[index]*imdiff1->image[index];
		xy = imdiff2->image[index]*imdiff1->image[index];
		yy = imdiff2->image[index]*imdiff2->image[index];
		avg_xx += xx;
		avg_xy += xy;
		avg_yy += yy;
		ct++;
	      }
	    }
	    index++;
	  }
	}
	//	avg_xx /= (float)ct;
	//	avg_xy /= (float)ct;
	//				printf("avg_xx=%g, avg_xy=%g, ct=%ld\n",avg_xx,avg_xy,ct);

	imdiff1->rfile[0] = (RFILE_DATA_TYPE)avg_xx/avg_xy;

//	tmp = imdiff1->rfile[0];
	
//	printf("scale=%f\n",imdiff1->rfile[0]);

	avg_xx /= (float)ct;
	avg_xy /= (float)ct;
	avg_yy /= (float)ct;

	imdiff1->rfile[0] = (RFILE_DATA_TYPE)avg_xx/avg_xy;
	imdiff1->rfile[1] = (RFILE_DATA_TYPE)sqrtf(avg_xx+avg_yy*imdiff1->rfile[0]*imdiff1->rfile[0]-2.*imdiff1->rfile[0]*avg_xy)/sqrtf(avg_xx);
	//sqrt($xx+$yy*$this_scale*$this_scale-2.*$this_scale*$xy)/sqrt($xx)	


EndPaper:
   
   return(return_value);
}
