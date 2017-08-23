/* LSMOOTHRF.C - Smooth the values in an rfile.

Author: Veronica Pillar
Date: 1/2/14
Version: 1.

*/

#include<mwmask.h>

int lsmoothrf(DIFFIMAGE *imdiff, int width)
{
  int
    i,
    j,
    n;

  RFILE_DATA_TYPE
    sum,
    *temprf;

  temprf = (RFILE_DATA_TYPE *)calloc(imdiff->rfile_length, sizeof(RFILE_DATA_TYPE));

  for (i = 0; i < imdiff->rfile_length; i++) {

    sum = 0;
    n = 0;

    for (j = i-width; j <= i+width; j++) {
      if ((j < 0) || (j >= imdiff->rfile_length))
	continue;
      sum += imdiff->rfile[j];
      n++;
    }
    temprf[i] = sum/((RFILE_DATA_TYPE)n);

  }


  for (i = 0; i < imdiff->rfile_length; i++) {
    imdiff->rfile[i] = temprf[i];
  }

// For debugging purposes: print some of the new & old rfiles
/*
  printf("length = %d\n", imdiff->rfile_length);
  for (i = 500; i < 530; i++)
    printf("%f %f\n", imdiff->rfile[i], temprf[i]);

  for (i = 0; i < imdiff->rfile_length; i++) {
    imdiff->rfile[i] = temprf[i];
  }
  */
}
