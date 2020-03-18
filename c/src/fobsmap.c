/* FOBSMAP.C - Calculate an experimental electron density using model phases.
   
   Author: Mike Wall
   Date: 5/4/2015
   Version: 1.
   
   Usage:
   		"fobsmap <input reflections .hkl> <input ccp4 map> <output ccp4 map>"

		Input is a .hkl reflections file and a map in CCP4 format. 
			Output is a map calcaulted using reflections for amplitudes and the map for phases.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *mapin,
    *hklin,
    *mapout;
  
  char
    error_msg[LINESIZE],filename[256];
  
  size_t
    fft_index,
    map_index,
    friedel_index,
    num_read,
    num_wrote;

  CCP4MAP
    *map;

  LAT3D 
    *lat;

  int
    h,
    k,
    l,
    nn[4],
    c,r,s,cc,rr,ss;
  
  float
    I,sigI,
    F,sigF,
    freal,fimag,fmodel;
/*
 * Set input line defaults:
 */
	hklin = stdin;
	mapout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
	  case 4:
	    if (strcmp(argv[3],"-") == 0) {
	      mapout = stdout;
	    }
	  else {
	    if ((mapout = fopen(argv[3],"wb")) == NULL) {
	      printf("\nCan't open %s.\n\n",argv[3]);
	      exit(0);
	    }
	  }
	  case 3:
	    if ( (mapin = fopen(argv[2],"rb")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  case 2:
	  if (strcmp(argv[1],"-") == 0) {
	    hklin = stdin;
	  }
	  else {
	    if ( (hklin = fopen(argv[1],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[1]);
	      exit(0);
	    }
	  }
	  break;
	  default:
	  printf("\n Usage: fobsmap <input reflections .hkl> <input ccp4 map> <output ccp4 map>\n\n");
	  exit(0);
	}
  
  /*
   * Initialize map:
   */

  if ((map = linitmap()) == NULL) {
    perror("Couldn't initialize map.\n\n");
    exit(0);
  }
  
  /*
   * Read in map:
   */

  map->filename = filename;
  map->infile = mapin;
  if (lreadmap(map) != 0) {
    perror("Couldn't read map.\n\n");
    exit(0);
  }

  /*
   * Prepare the map data for fft:
   */

  float *fft_data;
  fft_data = (float *)calloc(map->map_length*2+1,sizeof(float));
  fft_index = 1;
  map_index = 0;
  for (s=0;s<map->ns;s++) {
    for (r=0;r<map->nr;r++) {
      for (c=0;c<map->nc;c++) {
	fft_data[fft_index]=(float)map->data[map_index];
	fft_index += 2;
	map_index++;
      }
    }
  }

  nn[1] = map->ns;
  nn[2] = map->nr;
  nn[3] = map->nc;

  lfft(fft_data,nn,3,1);

  /*
   * Read the .hkl reflections file one line at a time, and replace fft amplitudes with reflections. 
   * Assume amplitudes instead of intensities in hkl file. Also assume only positive values of hkl.
   */

  float *scaled_fft_data;
  scaled_fft_data = (float *)calloc(map->map_length*2+1,sizeof(float));
  /*
  for (h=1;h<=map->map_length*2+1;h++) {
    scaled_fft_data[h] = fft_data[h];
  }
  */
  printf("Scaling the map\n");
  float xx=0.0,xy=0.0;
  while (fscanf(hklin,"%d %d %d %f %f",&h,&k,&l,&I,&sigI)!=EOF) {
    //    printf("%d %d %d\n",k+lat->origin.k,j+lat->origin.j,i+lat->origin.i);
    if (I>0 && sigI>0) {
      F=sqrtf(I);
      sigF=sigI/2./F;
    if (map->mapc == 3 && map->mapr == 1 && map->maps == 2) {
      r = h; s = k; c = l;
    } else if (map->mapc == 3 && map->mapr == 2 && map->maps == 1) {
            r = k; s = h; c = l;
      //      r = k; s = l; c = h;
      //      r = l; s = k; c = h;
      //      r = l; s = h; c = k;
      //      r = h; s = l; c = k;
      //       r = h; s = k; c = l;
    } else {
      printf("\nUnrecognized CCP4 map x,y,z definitions (MAPC,MAPR,MAPS) = (%ld,%ld,%ld)\n\n",map->mapc,map->mapr,map->maps);
      exit(4);
    }
    if (s>-map->ns/2 && s <= map->ns/2 && r>-map->nr/2 && r <= map->nr/2 && c>-map->nc/2 && c <= map->nc/2) {
      // negative
      if (s < 0) {
	ss = map->ns + s;
      } else ss=s;
      if (r < 0) {
	rr = map->nr + r;
      } else rr = r;
      if (c < 0) {
	cc = map->nc + c;
      } else cc = c;
      map_index = ss*map->section_length+rr*map->nc+cc;
      fft_index = map_index*2+1;
      freal=fft_data[fft_index]; fimag=fft_data[fft_index+1];
      fmodel=sqrtf(freal*freal+fimag*fimag);
      scaled_fft_data[fft_index] = F/fmodel*fft_data[fft_index];
      scaled_fft_data[fft_index+1] = F/fmodel*fft_data[fft_index+1];
      // accumulate statistics for calculation of overall scale factor
      xx += fmodel*fmodel; xy += F*fmodel;
      //      friedel_index = (s*map->section_length+r*map->nc+c)*2+1;
      //      scaled_fft_data[friedel_index] = F/fmodel*fft_data[friedel_index];
      //      scaled_fft_data[friedel_index+1] = F/fmodel*fft_data[friedel_index+1];
      /*
      if (fft_data[friedel_index] != fft_data[fft_index]) {
	printf("Friedel violation\n");
	exit(0);
      }
      scaled_fft_data[friedel_index] = scaled_fft_data[fft_index];
      scaled_fft_data[friedel_index+1] = -scaled_fft_data[fft_index+1];
      */
    }
    }}

  // Expand using Friedel symmetry

  float w = xx/xy; // overall scale factor

  printf("Scale factor = %f\n",w);

  fft_index = 1;
  for (s=0;s<map->ns;s++) {
    for (r=0;r<map->nr;r++) {
      for (c=0;c<map->nc;c++) {
	if (scaled_fft_data[fft_index]==0&&scaled_fft_data[fft_index+1]==0) {
	  if (s != 0.0) ss = map->ns - s; else ss=0.0;
	  if (r != 0.0) rr = map->nr - r; else rr=0.0;
	  if (c != 0.0) cc = map->nc - c; else cc=0.0;
	  friedel_index = (ss*map->section_length+rr*map->nc+cc)*2+1;
	  scaled_fft_data[fft_index]=scaled_fft_data[friedel_index];
	  scaled_fft_data[fft_index+1]=-scaled_fft_data[friedel_index+1];
	} 
	fft_index += 2;
      }
    }
  }

  // Apply overall scale factor and replace zeros with model reflections
  size_t zero_count=0;
  for (h=1;h<=map->map_length*2+1;h=h+2) {
    if (scaled_fft_data[h] == 0.0&&scaled_fft_data[h+1] == 0.0) {
      scaled_fft_data[h]=fft_data[h];
      scaled_fft_data[h+1]=fft_data[h+1];
      zero_count++;
    } else {
      scaled_fft_data[h] *= w;
      scaled_fft_data[h+1] *= w;
    }
  }
  printf("%ld zeros out of %ld total\n",zero_count,map->map_length);

  /*
   * Inverse FFT:
   */

  lfft(scaled_fft_data,nn,3,-1);

  /*
   * Load the fft data back into the map:
   */

  fft_index = 1;
  map_index = 0;
  for (s=0;s<map->ns;s++) {
    for (r=0;r<map->nr;r++) {
      for (c=0;c<map->nc;c++) {
	map->data[map_index]=(MAP_DATA_TYPE)scaled_fft_data[fft_index]/(MAP_DATA_TYPE)map->map_length;
	fft_index += 2;
	map_index++;
      }
    }
  }

/*
 * Write map to output file:
 */

  map->outfile = mapout;
  if (lwritemap(map) != 0) {
    perror("Couldn't write map.\n\n");
    exit(0);
  }

CloseShop:
  
  /*
   * Free allocated memory:
   */

  free(fft_data);

  //  lfreelt(lat);

  /*
   * Close files:
   */
  
  fclose(mapin);
  fclose(mapout);
}

