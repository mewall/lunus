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

	RFILE_DATA_TYPE *rf1, *rf2;

	struct xycoords r_mm;

	rf1 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
	rf2 = (RFILE_DATA_TYPE *)calloc(MAX_RFILE_LENGTH,sizeof(RFILE_DATA_TYPE));
	lavgrim(imdiff1);
	for (i=0; i<imdiff1->rfile_length;i++) rf1[i] = imdiff1->rfile[i];
	//	memcpy((void *)rf1,(void *)imdiff1->rfile,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
	lavgrim(imdiff2);
	for (i=0; i<imdiff2->rfile_length;i++) rf2[i] = imdiff2->rfile[i];
	//	memcpy((void *)rf2,(void *)imdiff2->rfile,imdiff2->rfile_length*sizeof(RFILE_DATA_TYPE));

	for (i=0;i<imdiff1->rfile_length;i++) {
	  if (rf1[i] != imdiff1->overload_tag && rf1[i] != imdiff1->mask_tag &&
	      rf2[i] != imdiff1->overload_tag && rf2[i] != imdiff1->mask_tag) {
	  if ((imdiff1->mask_inner_radius == 0 && imdiff1->mask_outer_radius == 0) || (i > imdiff1->mask_inner_radius && i < imdiff1->mask_outer_radius)) {
	    float x,y;
	    x = (float)rf1[i];
	    y = (float)rf2[i];
	    xx = x*x;
	    xy = x*y;
	    yy = y*y;
	    avg_xx += xx;
	    avg_xy += xy;
	    avg_yy += yy;
	    ct++;
	  }
	  }
	}
	  
	avg_xx /= (float)ct;
	avg_xy /= (float)ct;
	avg_yy /= (float)ct;

	imdiff1->rfile[0] = (RFILE_DATA_TYPE)avg_xx/avg_xy;
	imdiff1->rfile[1] = (RFILE_DATA_TYPE)sqrtf(avg_xx+avg_yy*imdiff1->rfile[0]*imdiff1->rfile[0]-2.*imdiff1->rfile[0]*avg_xy)/sqrtf(avg_xx);

/*
 * Calculate averages:
 */
/*
	if (imdiff1->rfile == NULL) {
	  perror("LSCALEIM: imdiff1->rfile == NULL\n");
	  exit(1);
	}
	//	printf("%d, %d\n",imdiff1->mask_inner_radius,imdiff1->mask_outer_radius);
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
	      if ((imdiff1->mask_inner_radius == 0 && imdiff1->mask_outer_radius == 0) || (radius > imdiff1->mask_inner_radius &&
											   radius < imdiff1->mask_outer_radius)) {
		float x,y;
		x = (float)imdiff1->image[index];// - imdiff1->value_offset;
		y = (float)imdiff2->image[index];// - imdiff2->value_offset;
		xx = x*x;
		xy = x*y;
		yy = y*y;
		avg_xx += xx;
		avg_xy += xy;
		avg_yy += yy;
		ct++;
	      }
	    }
	    index++;
	  }
	}
	//	printf("ct = %ld\n",ct);
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
	*/

//EndPaper:
   
	free(rf1);
	free(rf2);
   return(return_value);
}
