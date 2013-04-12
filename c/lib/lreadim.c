/* LREADIM.C - Read a diffraction image from a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>
#include <string.h>

const char * getTag(const char *target,const char *tag);

int lreadim(DIFFIMAGE *imdiff)
{
	size_t
		num_read;

	int
		return_value = 0;  

	char
	  *buf;

	buf = (char *)malloc(DEFAULT_HEADER_LENGTH); // TV6 TIFF header size is generous enough

  /*
   * Read diffraction image header
   */


  num_read = fread(buf, sizeof(char), DEFAULT_HEADER_LENGTH,
                imdiff->infile);
  if (num_read != DEFAULT_HEADER_LENGTH) {
	sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
	return(1);
  }

  if (strstr(buf,"TV6 TIFF")==NULL) {
    // Not TV6 image format
    if (strstr(buf,"HEADER_BYTES=")!=NULL &&
	strstr(buf,"PIXEL_SIZE=")!=NULL) {
      // assume ADSC .img format
      //      printf("ADSC .img image\n");
      imdiff->header_length = (size_t)atoi(getTag(buf, "HEADER_BYTES"));
      imdiff->header = (char *)realloc(imdiff->header,sizeof(char)*imdiff->header_length);
      fseek(imdiff->infile,0,SEEK_SET);
      num_read = fread(imdiff->header, sizeof(char), imdiff->header_length,
                imdiff->infile);
      if (num_read != imdiff->header_length) {
	sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
	return(1);
      }
      imdiff->hpixels = (size_t)atoi(getTag(imdiff->header,"SIZE1"));
      imdiff->vpixels = (size_t)atoi(getTag(imdiff->header,"SIZE2"));
      imdiff->window_lower.r = imdiff->window_lower.c = 0;
      imdiff->window_upper.r = imdiff->vpixels;
      imdiff->window_upper.c = imdiff->hpixels;
      if (getTag(imdiff->header,"IMAGE_PEDESTAL")!=NULL) {
	imdiff->value_offset = (IMAGE_DATA_TYPE)atoi(getTag(imdiff->header,"IMAGE_PEDESTAL"));
      } else {
	imdiff->value_offset = DEFAULT_VALUE_OFFSET;
      }
      imdiff->rfile_length = (size_t)(imdiff->hpixels < imdiff->vpixels ? imdiff->hpixels : imdiff->vpixels);
      imdiff->image_length = imdiff->hpixels*imdiff->vpixels;
      imdiff->image = (IMAGE_DATA_TYPE *)realloc(imdiff->image,imdiff->image_length*sizeof(IMAGE_DATA_TYPE));
      imdiff->pixel_size_mm = atof(getTag(imdiff->header,"PIXEL_SIZE"));
      imdiff->beam_mm.y = atof(getTag(imdiff->header,"BEAM_CENTER_X"));
      imdiff->beam_mm.x = atof(getTag(imdiff->header,"BEAM_CENTER_Y"));
      imdiff->origin.c = imdiff->beam_mm.x/imdiff->pixel_size_mm+.5;
      imdiff->origin.r = imdiff->beam_mm.y/imdiff->pixel_size_mm+.5;
      imdiff->distance_mm = atof(getTag(imdiff->header,"DISTANCE"));
      imdiff->wavelength = atof(getTag(imdiff->header,"WAVELENGTH"));
      if (!strcmp(getTag(imdiff->header,"BYTE_ORDER"),"big_endian"))
	imdiff->big_endian=1;
      else if (!strcmp(getTag(imdiff->header,"BYTE_ORDER"),"little_endian"))
	imdiff->big_endian=0;
      else {
	sprintf(imdiff->error_msg,"\nByte order %s not recognized.\n\n",getTag(imdiff->header,"BYTE_ORDER"));
	return(7);
      }
    }
  }

  if (ferror(imdiff->infile) != 0) {
  	sprintf(imdiff->error_msg,"\nError while reading header.\n\n");
	return(2);
  }

  /*
   * Read image:
   */

  num_read = fread(imdiff->image, sizeof(IMAGE_DATA_TYPE),
                        imdiff->image_length, imdiff->infile);
  if (num_read != imdiff->image_length) {
	sprintf(imdiff->error_msg,"\nCouldn't read all of image.\n\n");
	return(3);
  }
  if (ferror(imdiff->infile) != 0) {
    sprintf(imdiff->error_msg,"\nError while reading image\n\n");
    return(4);
  }

/*
 * Reverse byte order if neccessary:
 */

  if (imdiff->big_endian!=0) {
        lchbyte(imdiff->image, sizeof(IMAGE_DATA_TYPE), imdiff->image_length);
  }

  return(0);
}

const char * getTag(const char *target,const char *tag)
{
  char *pos_begin,*pos_end;
  char *val;
  
  if ((pos_begin = strstr(target,tag)) == NULL) {
    //printf("\nWarning: Couldn't find tag %s in image header\n\n",tag);
    return(NULL);
  }
  if ((pos_begin = strchr(pos_begin,'='))==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  }
  if ((pos_end = strchr(pos_begin,';'))==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  }
  int len = pos_end-pos_begin-1;
  val = (char *)calloc(sizeof(char),len+1);
  memcpy(val,pos_begin+1,len);
  return(val);
}
