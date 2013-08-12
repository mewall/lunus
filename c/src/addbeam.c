/* ADDBEAM.C - Add beam info to .img header.
   
   Author: Mike Wall
   Date: 4/12/2013
   Version: 1.
   
   "addbeam <image in> <image out> <BEAM_CENTER_X> <BEAM_CENTER_Y>" 

   Input is image and constant.  Output is input image with all pixel values 
   of value <constant> converted to an ingore_tag.

   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
	*imagein,
	*imageout;
  
  size_t
    i;

  DIFFIMAGE 
	*imdiff;

  IMAGE_DATA_TYPE
	image_value;

  float
    beam_center_x=0,beam_center_y=0;

  char
    *buf;

/*
 * Set input line defaults:
 */
	
	imagein = stdin;
	imageout = stdout;

/*
 * Read information from input line:
 */
	switch(argc) {
                case 5:
		  beam_center_y = atof(argv[4]);
                case 4:
		  beam_center_x = atof(argv[3]);
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
			printf("\n Usage: addbeam <image in> "
			       "<image out> <x beam> <y beam> \n\n");
			exit(0);
	}
 
/*
 * Initialize diffraction image:
 */
    
  if ((imdiff = linitim()) == NULL) {
    perror("\nCouldn't initialize diffraction image.\n\n");
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
 * Change the header:
 */

   if (beam_center_x==0) {
     beam_center_x=imdiff->beam_mm.x;
   }
   if (beam_center_y==0) {
     beam_center_y=imdiff->beam_mm.y;
   }

   buf = imdiff->header;
   if (strstr(buf,"BEAM_CENTER_X")==NULL&&strstr(buf,"BEAM_CENTER_Y")==NULL) {
     char *beamstr;
     beamstr = (char *)malloc(1000);
     sprintf(beamstr,"BEAM_CENTER_X=%f;%cBEAM_CENTER_Y=%f;%c}  ",beam_center_x,0x0a,beam_center_y,0x0a);
     beamstr[strlen(beamstr)-2]=0x0c;
     beamstr[strlen(beamstr)-1]=0x0a;
     if (strchr(buf,0x7d)==NULL) {
       perror("Couldn't find terminator sequence in header\n");
       exit(0);
     }
     strcpy(strchr(buf,0x7d),beamstr);
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
  
/*
 * Free allocated memory:
 */

  lfreeim(imdiff);

/*
 * Close files:
 */
  
  fclose(imagein);
  fclose(imageout);
  
}

