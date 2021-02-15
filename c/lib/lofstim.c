/* LSCALEIM.C - Scale one image to another pixel-by-pixel within a radial region.

   Author: Mike Wall
   Version:1.0
   Date:10/25/2016
				 
*/

#include<mwmask.h>

int lofstim(DIFFIMAGE *imdiff1)
{
	float
	  f,ff,fxf,
	  avg_f = 0.0,
	  avg_ff = 0.0,
	  avg_fxf = 0.0;

	int 
		return_value = 0;

 	size_t
	  i,r,c,index,ct;

	short
	  radius;

	RFILE_DATA_TYPE *rf1, *rf2;

	struct xycoords r_mm;

	lavgrim(imdiff1);
	rf1 = (RFILE_DATA_TYPE *)calloc(imdiff1->rfile_length,sizeof(RFILE_DATA_TYPE));
	ct = 0;
	for (i=0; i<imdiff1->rfile_length;i++)
	  rf1[i] = imdiff1->rfile[i];
#ifdef DEBUG
	printf("LSCALEIM: RF1: \n");
	for (i=0;i<imdiff1->rfile_length;i++) printf("%d %f\n",i,rf1[i]);
#endif
	//	}
	//	printf("\n");
	//	memcpy((void *)rf1,(void *)imdiff1->rfile,imdiff1->rfile_length*sizeof(RFILE_DATA_TYPE));
	lavgrcf(imdiff1);
	rf2 = (RFILE_DATA_TYPE *)calloc(imdiff1->rfile_length,sizeof(RFILE_DATA_TYPE));
	for (i=0; i<imdiff1->rfile_length;i++) 
	  rf2[i] = imdiff1->rfile[i];
#ifdef DEBUG
	printf("LSCALEIM: RF2: \n");
	for (i=0;i<imdiff1->rfile_length;i++) printf("%d %f\n",i,rf2[i]);
#endif
	//	}
	//	memcpy((void *)rf2,(void *)imdiff2->rfile,imdiff2->rfile_length*sizeof(RFILE_DATA_TYPE));
	size_t rfile_length = imdiff1->rfile_length;

	if (imdiff1->correct_offset == 1) {
	  short inner_radius, outer_radius;
	  if (imdiff1->correct_offset_inner_radius == 0 && imdiff1->correct_offset_outer_radius == 0) {
	    inner_radius = rfile_length - 100;
	    outer_radius = rfile_length - 1;
	  } else {
	    inner_radius = imdiff1->correct_offset_inner_radius;
	    outer_radius = imdiff1->correct_offset_outer_radius;
	  }

	  if (outer_radius >= rfile_length) {
	    printf("LOFSTIM: imdiff1->correct_offset_outer_radius=%d is greater than rfile length=%d, aborting.\n",outer_radius,rfile_length);
	    exit(1);
	  }

	  for (i=inner_radius;i<outer_radius;i++) {
	    if (rf1[i] != imdiff1->overload_tag && rf1[i] != imdiff1->ignore_tag && rf1[i] != 0.0 &&
		rf2[i] != imdiff1->overload_tag && rf2[i] != imdiff1->ignore_tag && rf2[i] != 0.0) {
	      float f;
	      f = (float)rf2[i];
	      avg_f += f;
	      ct++;
	    }
	  }
	  
	  avg_f /= (float)ct;
	  
	  ct = 0;
	  for (i=inner_radius;i<outer_radius;i++) {
	    if (rf1[i] != imdiff1->overload_tag && rf1[i] != imdiff1->ignore_tag && rf1[i] != 0.0 &&
		rf2[i] != imdiff1->overload_tag && rf2[i] != imdiff1->ignore_tag && rf2[i] != 0.0) {
	      float x,f;
	      x = (float)rf1[i];
	      f = (float)rf2[i];
	      ff = f*(f-avg_f);
	      fxf = f*x*(f-avg_f);
	      avg_ff += ff;
	      avg_fxf += fxf;
	      ct++;
	    }
	  }
	  
	  avg_ff /= (float)ct;
	  avg_fxf /= (float)ct;

#ifdef DEBUG
	  printf("LOFSTIM: avg_f = %f, avg_ff = %f, avg_fxf = %f\n",avg_f,avg_ff,avg_fxf);
#endif

	  imdiff1->correction_offset = (RFILE_DATA_TYPE)avg_fxf/avg_ff;

#ifdef DEBUG
	  printf("LOFSTIM: ct = %d,offset = %f\n",ct,imdiff1->correction_offset);
#endif
	}
   return(return_value);
}
