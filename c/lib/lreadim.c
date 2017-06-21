/* LREADIM.C - Read a diffraction image from a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>
#include <string.h>

int lreadim(DIFFIMAGE *imdiff)
{
	size_t
	  i,
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
      strcpy(imdiff->format,"SMV");
      imdiff->header_length = (size_t)atoi(lgettag(buf, "HEADER_BYTES"));
      imdiff->header = (char *)realloc(imdiff->header,sizeof(char)*imdiff->header_length);
      fseek(imdiff->infile,0,SEEK_SET);
      num_read = fread(imdiff->header, sizeof(char), imdiff->header_length,
                imdiff->infile);
      if (num_read != imdiff->header_length) {
	sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
	return(1);
      }
      imdiff->hpixels = (size_t)atoi(lgettag(imdiff->header,"SIZE1"));
      imdiff->vpixels = (size_t)atoi(lgettag(imdiff->header,"SIZE2"));
      imdiff->window_lower.r = imdiff->window_lower.c = 0;
      imdiff->window_upper.r = imdiff->vpixels;
      imdiff->window_upper.c = imdiff->hpixels;
      if (lgettag(imdiff->header,"IMAGE_PEDESTAL")!=NULL) {
	imdiff->value_offset = (IMAGE_DATA_TYPE)atoi(lgettag(imdiff->header,"IMAGE_PEDESTAL"));
      } else {
	imdiff->value_offset = DEFAULT_VALUE_OFFSET;
      }
      imdiff->rfile_length = (size_t)(imdiff->hpixels < imdiff->vpixels ? imdiff->hpixels : imdiff->vpixels);
      imdiff->rfile = (RFILE_DATA_TYPE *)realloc(imdiff->rfile,
		      imdiff->rfile_length*sizeof(RFILE_DATA_TYPE));
      imdiff->image_length = imdiff->hpixels*imdiff->vpixels;
      imdiff->image = (IMAGE_DATA_TYPE *)realloc(imdiff->image,imdiff->image_length*sizeof(IMAGE_DATA_TYPE));
      imdiff->pixel_size_mm = atof(lgettag(imdiff->header,"PIXEL_SIZE"));
      if (lgettag(imdiff->header,"BEAM_CENTER_X")!=NULL) {
	imdiff->beam_mm.y = atof(lgettag(imdiff->header,"BEAM_CENTER_X"));
      } else {
	imdiff->beam_mm.x = imdiff->pixel_size_mm*(float)imdiff->hpixels/2.0;
      }
      if (lgettag(imdiff->header,"BEAM_CENTER_Y")!=NULL) {
	imdiff->beam_mm.x = atof(lgettag(imdiff->header,"BEAM_CENTER_Y"));
      } else {
	imdiff->beam_mm.y = imdiff->pixel_size_mm*(float)imdiff->vpixels/2.0;
      }
      imdiff->origin.c = imdiff->beam_mm.x/imdiff->pixel_size_mm+.5;
      imdiff->origin.r = imdiff->beam_mm.y/imdiff->pixel_size_mm+.5;
      imdiff->distance_mm = atof(lgettag(imdiff->header,"DISTANCE"));
      imdiff->wavelength = atof(lgettag(imdiff->header,"WAVELENGTH"));
      imdiff->osc_start = atof(lgettag(imdiff->header,"OSC_START"));
      imdiff->osc_range = atof(lgettag(imdiff->header,"OSC_RANGE"));
      if (!strcmp(lgettag(imdiff->header,"BYTE_ORDER"),"little_endian"))
	imdiff->big_endian=0;
      else if (!strcmp(lgettag(imdiff->header,"BYTE_ORDER"),"big_endian"))
	imdiff->big_endian=1;
      else {
	sprintf(imdiff->error_msg,"\nByte order %s not recognized.\n\n",lgettag(imdiff->header,"BYTE_ORDER"));
	return(7);
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
    } else {
      if (strstr(buf,"###CBF")!=NULL) {
	  //	  printf("CBF format\n");
	// CBF format
	strcpy(imdiff->format,"CBF");
	  size_t header_length=0;
	  char *cif_binary_pos;
	  char *image_pos;
	  size_t cif_binary_offset;
	  while ((cif_binary_pos = strstr(buf,"--CIF-BINARY-FORMAT-SECTION--")) == NULL) {
	    header_length += DEFAULT_HEADER_LENGTH;
	    realloc(buf,(size_t)(header_length+DEFAULT_HEADER_LENGTH));
	    num_read = fread(&buf[header_length], sizeof(char), DEFAULT_HEADER_LENGTH,
			 imdiff->infile);
	    if (num_read != DEFAULT_HEADER_LENGTH) {
	      sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
	      return(1);
	    }
	  }
	  cif_binary_offset = (size_t)cif_binary_pos - (size_t)buf;
	  //	  printf("Read .cbf file with --CIF-BINARY-FORMAT-SECTION-- position = %ld\n",cif_binary_offset);
	  while ((image_pos = strstr(cif_binary_pos,"\x0c\x1a\x04\xd5")) == NULL) {
	    header_length += DEFAULT_HEADER_LENGTH;
	    realloc(buf,(size_t)(header_length+DEFAULT_HEADER_LENGTH));
	    num_read = fread(&buf[header_length], sizeof(char), DEFAULT_HEADER_LENGTH,
			 imdiff->infile);
	    if (num_read != DEFAULT_HEADER_LENGTH) {
	      sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
	      return(1);
	    }
	  }
	  image_pos += 4;
	  // Read in the header
	  imdiff->header_length = (size_t)image_pos-(size_t)buf;
	  imdiff->header = (char *)realloc(imdiff->header,sizeof(char)*(imdiff->header_length+1));
	  fseek(imdiff->infile,0,SEEK_SET);
	  num_read = fread(imdiff->header, sizeof(char), imdiff->header_length,
			   imdiff->infile);
	  if (num_read != imdiff->header_length) {
	    sprintf(imdiff->error_msg,"\nCouldn't read all of header.\n\n");
	    return(1);
	  }
	  //	  printf("imdiff->header_length = %ld\n",imdiff->header_length);
	  //	  imdiff->header[imdiff->header_length]=0;
	  // Write header
	  //	  printf("Header, length %ld:\n\n%s\n",strlen(imdiff->header),imdiff->header);
	  // Parse the header for tag info
	  //	  printf("Pixel_size = %s\n",lgetcbftag(imdiff->header,"Pixel_size"));
	  //	  printf("Wavelength = %s\n",lgetcbftag(imdiff->header,"Wavelength"));
	  /*	  printf("Detector_distance = %s\n",lgetcbftag(imdiff->header,"Detector_distance"));
	  printf("Start_angle = %s\n",lgetcbftag(imdiff->header,"Start_angle"));
	  printf("Beam_xy = %s\n",lgetcbftag(imdiff->header,"Beam_xy"));
	  printf("X-Binary-Element-Byte-Order = %s\n",lgetcbftag(imdiff->header,"X-Binary-Element-Byte-Order:"));
	  printf("X-Binary-Number-of-Elements = %s\n",lgetcbftag(imdiff->header,"X-Binary-Number-of-Elements:"));
	  printf("X-Binary-Size-Fastest-Dimension = %s\n",lgetcbftag(imdiff->header,"X-Binary-Size-Fastest-Dimension:"));
	  printf("X-Binary-Size-Second-Dimension = %s\n",lgetcbftag(imdiff->header,"X-Binary-Size-Second-Dimension:"));
	  printf("X-Binary-Size = %s\n",lgetcbftag(imdiff->header,"X-Binary-Size:"));
	  printf("X-Binary-Size-Padding = %s\n",lgetcbftag(imdiff->header,"X-Binary-Size-Padding:"));
	  */
	  char units[256];
	  imdiff->hpixels = atoi(lgetcbftag(imdiff->header,"X-Binary-Size-Fastest-Dimension:"));
	  imdiff->vpixels = atoi(lgetcbftag(imdiff->header,"X-Binary-Size-Second-Dimension:"));
	  imdiff->image_length = imdiff->hpixels * imdiff->vpixels;
	  sscanf(lgetcbftag(imdiff->header,"Start_angle"),"%f %s",&imdiff->osc_start,units);
	  sscanf(lgetcbftag(imdiff->header,"Angle_increment"),"%f %s",&imdiff->osc_range,units);
	  sscanf(lgetcbftag(imdiff->header,"Detector_distance"),"%f %s",&imdiff->distance_mm,units);
	  if (strcmp(units,"m")==0) {
	    imdiff->distance_mm *= 1000.;
	  }
	  sscanf(lgetcbftag(imdiff->header,"Pixel_size"),"%g %s",&imdiff->pixel_size_mm,units);
	  if (strcmp(units,"m")==0) {
	    imdiff->pixel_size_mm *= 1000.;
	  }
	  sscanf(lgetcbftag(imdiff->header,"Beam_xy"),"%*c%f, %f%*c %s",&imdiff->beam_mm.x,&imdiff->beam_mm.y,units);
	  if (strcmp(units,"pixels")==0) {
	    imdiff->beam_mm.x *= imdiff->pixel_size_mm;
	    imdiff->beam_mm.y *= imdiff->pixel_size_mm;
	  }
	  sscanf(lgetcbftag(imdiff->header,"Wavelength"),"%g %s",&imdiff->wavelength,units);
	  if (strcmp(units,"nm")==0) {
	    imdiff->wavelength /= 10.;
	  }
	  printf("osc_start,osc_range,distance_mm,pixel_size_mm,beam_mm.x,beam_mm.y,wavelength=%f,%f,%f,%f,%f,%f,%f\n",imdiff->osc_start,imdiff->osc_range,imdiff->distance_mm,imdiff->pixel_size_mm,imdiff->beam_mm.x,imdiff->beam_mm.y,imdiff->wavelength);
	  size_t buf_length, padding;
	  buf_length = atol(lgetcbftag(imdiff->header,"X-Binary-Size:"));
	  padding = atol(lgetcbftag(imdiff->header,"X-Binary-Size-Padding:"));
	  free(buf);
	  buf = (char *)malloc(buf_length+padding);
	  num_read = fread(buf, sizeof(char), buf_length,
			   imdiff->infile);
	  // Read the footer
	  int this_pos,end_pos;
	  this_pos = ftell(imdiff->infile);
	  fseek(imdiff->infile, 0, SEEK_END); // seek to end of file
	  end_pos = ftell(imdiff->infile); // get current file pointer
	  fseek(imdiff->infile,this_pos,SEEK_SET);
	  imdiff->footer_length = (size_t)(end_pos-this_pos);
	  imdiff->footer = (char *)malloc(buf_length);
	  num_read = fread(imdiff->footer, sizeof(char), imdiff->footer_length,
			   imdiff->infile);
	  if (num_read != imdiff->footer_length) {
	    sprintf(imdiff->error_msg,"\nCouldn't read all of CBF footer.\n\n");
	    return(1);
	  }	  
	  /*
	  // Look for end of binary CIF region
	  buf[buf_length+1]=0;
	  size_t taglen;
	  taglen = strlen("\r\n--CIF-BINARY-FORMAT-SECTION--");
	  i = taglen;
	  while ((i < taglen+50) && memcmp(&buf[buf_length-i],"\r\n--CIF-BINARY-FORMAT-SECTION--",taglen) != 0) 
	    {
	    //	    printf("%s\n",buf[buf_length-i]
	      i++;
	    }
	  if (i>=taglen+50) {
	    printf("Couldn't find end of CIF binary section\n");
	    exit(1);
	  }
	  printf("Size of packed image = %ld\n",buf_length-i);
	  //	  printf("End of image:\n\n%s",buf+buf_length-i);
	  */
	  /*	  if ((cif_binary_pos = strstr(buf,"--CIF-BINARY-FORMAT-SECTION--")) == NULL) {
	    printf("Couldn't find end of CIF binary region\n");
	    exit(1);
	  }
	  */
	  //	  printf("buf[0], buf[1], buf[2], buf[3] = %d,%d,%d,%d\n",(char)buf[0],(char)buf[1],(char)buf[2],(char)buf[3]);
	  int *image_cbf;
	  image_cbf = (int *)malloc(imdiff->image_length*sizeof(int));
	  //	  printf("Uncompress, buf_length = %ld\n",buf_length);
	  lbufuncompress(buf,buf_length,image_cbf,imdiff->image_length);
	  // Convert image to imdiff format
	  free(imdiff->image);
	  imdiff->image = (IMAGE_DATA_TYPE *)malloc(sizeof(IMAGE_DATA_TYPE)*imdiff->image_length);
	  for (i=0;i<imdiff->image_length;i++) {
	    if (image_cbf[i] < 0 || image_cbf[i] > 32767) {
	      imdiff->image[i]=imdiff->ignore_tag;
	    } else {
	      imdiff->image[i] = (IMAGE_DATA_TYPE)image_cbf[i];
	    }
	  }
	  //	  printf("Read .cbf image\n");
	}
    }
  }
   

  return(0);
}

