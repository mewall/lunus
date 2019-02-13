/* PATTIM.C - Create an image with a pattern.
   
   Author: Mike Wall
   Date: 10/3/2012
   Version: 1.
   
   "pattim <image in> <image out>"
   
   Input is image. Output is image with pattern.

*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  char
    error_msg[LINESIZE];

  DIFFIMAGE 
	*imdiff;

  int sz,cell_szx,cell_szy,mode_sel=0,nrand=2,cellflg,usedots = 0;

  size_t
    i,j;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;

/*
 * Read information from input line:
 */
  switch(argc) {
  case 8:
    nrand = atoi(argv[7]);
  case 7:
    mode_sel = atoi(argv[6]);
  case 6:
    cell_szy = atoi(argv[5]);
  case 5:
    cell_szx = atoi(argv[4]);
  case 4:
    sz = atoi(argv[3]);
  case 3:
    if (strcmp(argv[2], "-") == 0) {
      imageout = stdout;
    }
    else {
      if ( (imageout = fopen(argv[2],"wb")) == NULL ) {
	printf("Can't open %s.",argv[2]);
	exit(0);
      }
    }
    case 2:
    if (strcmp(argv[1], "-") == 0) {
      imagein = stdin;
    }
    else {
      if ( (imagein = fopen(argv[1],"rb")) == NULL ) {
	printf("Can't open %s.",argv[1]);
	exit(0);
      }
    }
    break;
    default:
    printf("\n Usage: pattim "
	   "<image in> <image out> \n\n");
    exit(0);
  }

  int hsz = sz/2;
  int qsz = sz/4;

  /*
   * Initialize diffraction image:
   */
  
  if ((imdiff = linitim()) == NULL) {
    perror("Couldn't initialize diffraction image.\n\n");
    exit(0);
  }
 
/*
 * Read diffraction image:
 */

  imdiff->infile = imagein;
  if (lreadim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }


  /*
   * Replace image with pattern for diffraction:
   */

  size_t index = 0;
  int sgni,sgnhi,sgnj,sgnhj,sgnqi,sgnqj;
  int sgni_last = 0;

  int ic,jc,cellidx;
  int mask;

  int *cells;
  int xcells = imdiff->hpixels/cell_szx;
  int ycells = imdiff->vpixels/cell_szy;
  cells = (int *)malloc(sizeof(int)* xcells * ycells);

  for (i=0;i<xcells*ycells;i++) {
    switch(mode_sel) {
    case 0:
      cells[i] = 1.0;
      break;
    case 1:
      cells[i] = rand() % nrand;
      usedots = 0;
      break;
    case 2:
      cells[i] = rand() % nrand;
      usedots = 1;
      break;
    }
  }

  for (j=0;j<imdiff->vpixels;j++) {
    jc = j/cell_szy;
    mask = 1;
    if (j%sz<sz/2) sgnj = 0; else sgnj = 1;
    if (j%hsz<hsz/2) sgnhj = 0; else sgnhj = 1;
    if (j%qsz<qsz/2) sgnqj = 0; else sgnqj = 1;
    for (i=0;i<imdiff->hpixels;i++) {
      ic = i/cell_szx;
      cellidx = jc*xcells+ic;
      if (i%sz<sz/2) sgni = 0; else sgni = 1;
      if (i%hsz < hsz/2) sgnhi = 0; else sgnhi = 1;
      if (i%qsz < qsz/2) sgnqi = 0; else sgnqi = 1;
      //      if ((j%sz<sz/8)||(j%sz>=7*sz/8)) mask = 0; else mask = 1;
      //      if ((i%sz<sz/8) || (i%sz >= 7*sz/8)) mask = 0;
      //      if (mask==0) {
      //	imdiff->image[index] = 10000;
      //      } else {
      switch(mode_sel) {
      case 0:
	imdiff->image[index] = 0;
	break;
      case 1:
	if (cells[cellidx]==0)
	  cellflg=1;
	else 
	  cellflg = 0;
	break;
      case 2: 
	if (cells[cellidx]==0)
	  cellflg=1;
	else 
	  cellflg = 0;
	break;
      }
      int check = ((1-(1-sgni)*(1-sgnj))*(1-sgni*sgnj));
      int dot = (1-(1-(1-sgnhi)*sgnqi)*(1-sgnhi*(1-sgnqi)))*(1-(1-(1-sgnhj)*sgnqj)*(1-sgnhj*(1-sgnqj)));
      int patt;
      if (usedots == 1) 
	patt = ((1-(1-check)*(1-dot))*(1-check*dot));
      else 
	patt = 1 - check;
      imdiff->image[index] = 30000*(patt*cellflg+check*(1-cellflg));;
      /*
      imdiff->image[index] = 10000*mask*((1-sgni)*cells[cellidx] + sgni*(1-cells[cellidx]));
      else
	imdiff->image[index] = 10000*mask*(sgni*cells[cellidx]+(1-sgni)*(1-cells[cellidx]));
      */
      //    }
      index++;
    }
  }
    
/*
 * Write the output image:
 */

  imdiff->outfile = imageout;
  if(lwriteim(imdiff) != 0) {
    perror(imdiff->error_msg);
    goto CloseShop;
  }

CloseShop:
  
  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(imagein);
  fclose(imageout);
  
}

