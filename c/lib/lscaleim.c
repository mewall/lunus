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
#ifdef DEBUG
	printf("imdiff1->scale_inner_radius=%d,imdiff1->scale_outer_radius=%d\n",imdiff1->scale_inner_radius,imdiff1->scale_outer_radius);
	printf("imdiff2->scale_inner_radius=%d,imdiff2->scale_outer_radius=%d\n",imdiff2->scale_inner_radius,imdiff2->scale_outer_radius);
#endif
	lavgrim(imdiff1);
	ct = 0;
	for (i=0; i<imdiff1->rfile_length;i++) {
	  rf1[i] = imdiff1->rfile[i];
#ifdef DEBUG
	  //	  if (i>100 && i<=110)  printf("rf1[%d]=%f,",i,rf1[i]);
#endif
	}
	//	printf("\n");
	//	memcpy((void *)rf1,(void *)imdiff1->rfile,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
	lavgrim(imdiff2);
	for (i=0; i<imdiff2->rfile_length;i++) {
	  rf2[i] = imdiff2->rfile[i];
#ifdef DEBUG
	  //	  if (i>100 && i<=110) printf("rf2[%d]=%f,",i,rf2[i]);
#endif
	}
	//	memcpy((void *)rf2,(void *)imdiff2->rfile,imdiff2->rfile_length*sizeof(RFILE_DATA_TYPE));

	for (i=0;i<imdiff1->rfile_length;i++) {
	  if (rf1[i] != imdiff1->overload_tag && rf1[i] != imdiff1->mask_tag &&
	      rf2[i] != imdiff1->overload_tag && rf2[i] != imdiff1->mask_tag) {
	    if ((imdiff1->scale_inner_radius == 0 && imdiff1->scale_outer_radius == 0) || (i > imdiff1->scale_inner_radius && i < imdiff1->scale_outer_radius)) {
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

   
	free(rf1);
	free(rf2);
   return(return_value);
}
