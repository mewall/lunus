/* SETPED.C - Set the pedestal value for an image.
   
   Author: Veronica Pillar
   Date: 10/1/2015
   Version: 1.
   
   "setped <image in> <pedestal value> <image out>" 

   Input is input image, output image, and pedestal value.  Output is input image with new pedestal value (and header changed appropriately).

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

  char
      *buf,
      *tagval,
      *tag = "IMAGE_PEDESTAL";

  tagval = (char*)malloc(1000);

  /*
   * Set input line defaults:
   */
  
  imagein = stdin;
  imageout = stdout;

  /*
   * Read information from input line:
   */
  switch(argc) {
      case 4:
	  if (strcmp(argv[3], "-") == 0) {
	      imageout = stdout;
	  }
	  else {
	      if ( (imageout = fopen(argv[3],"wb")) == NULL ) {
		  printf("Can't open %s.",argv[3]);
		  exit(0);
	      }
	  }
      case 3:
	  image_value = atof(argv[2]);
	  strcpy(tagval, argv[2]);
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
	  printf("\n Usage: setped <image in> "
		  "<pedestal value> <image out> \n\n");
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
   * Change the pedestal value:
   */

  imdiff->mode_binsize = image_value; //not what this field is usually used for
  lsetped(imdiff);

  /*
   * Change the header:
   */

  buf = imdiff->header;
  if (strstr(buf,tag)==NULL) {
    char *tagstr;
    tagstr = (char *)malloc(1000);
    sprintf(tagstr,"%s=%d;%c}  ",tag,image_value,0x0a);
    tagstr[strlen(tagstr)-2]=0x0c;
    tagstr[strlen(tagstr)-1]=0x0a;
    if (strchr(buf,0x7d)==NULL) {
      perror("Couldn't find terminator sequence in header\n");
      exit(0);
    }
    strcpy(strchr(buf,0x7d),tagstr);
  }
  else {
    printf("Found tag\n");
    lsettag(imdiff->header,tag,tagval);
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

