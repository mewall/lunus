/* LWRITEIM.C - Write a diffraction image to a file.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<mwmask.h>

int lwriteim(DIFFIMAGE *imdiff)
{
	size_t
	  i,
		num_wrote;

	int
		return_value = 0;  


/*
 * Reverse byte order if neccessary:
 */

  if (DOS_BYTE_ORDER!=0 || imdiff->big_endian!=0) {
        lchbyte(imdiff->image, sizeof(IMAGE_DATA_TYPE), imdiff->image_length);
  }

/*
 * Write image header
 */

  if (strstr(imdiff->header,"HEADER_BYTES=")!=NULL &&
      strstr(imdiff->header,"PIXEL_SIZE=")!=NULL) {

    num_wrote = fwrite(imdiff->header, sizeof(char), imdiff->header_length,
                        imdiff->outfile);
    if (num_wrote != imdiff->header_length) {
      return_value = 1;
      sprintf(imdiff->error_msg,"\nCouldn't write all of header.\n\n");
    }
    if (ferror(imdiff->outfile) != 0) {
  	return_value = 2;
	sprintf(imdiff->error_msg,"\nError while writing header.\n\n");
    }
    
    /*
     * Write SMV image:
     */
    //    int ct=0;
    SMV_DATA_TYPE *imbuf;
    imbuf = (SMV_DATA_TYPE *)malloc(imdiff->image_length*sizeof(SMV_DATA_TYPE));
    for (i = 0;i < imdiff->image_length; i++) {
      if (imdiff->image[i] < SMV_MIN || imdiff->image[i] > SMV_MAX) {
	imbuf[i] = SMV_IGNORE_TAG;
	//	ct++;
      } else {
	imbuf[i] = (SMV_DATA_TYPE)imdiff->image[i];
      }
    }
    //    printf("ct = %d",ct);
    num_wrote = fwrite(imbuf, sizeof(SMV_DATA_TYPE), 
		       imdiff->image_length, imdiff->outfile);

    if (num_wrote != imdiff->image_length) {
      return_value = 3;
      sprintf(imdiff->error_msg,"\nCouldn't write all of image.\n\n");
    }
    if (ferror(imdiff->outfile) != 0) {
      return_value = 4;
      sprintf(imdiff->error_msg,"\nError while writing image\n\n");
    }
    
  } else {
    if (strcmp(imdiff->format,"CBF") == 0) {
      // Convert image to int type
      //      printf("Writing .cbf image\n");
      CBF_DATA_TYPE *image_cbf;
      image_cbf = (CBF_DATA_TYPE *)malloc(sizeof(CBF_DATA_TYPE)*imdiff->image_length);
      for (i = 0;i<imdiff->image_length;i++) {
	if (imdiff->image[i] == imdiff->ignore_tag) {
	  image_cbf[i] = -1;
	} else {
	  image_cbf[i] = (CBF_DATA_TYPE)imdiff->image[i];
	}
      }
      char *packed;
      packed = (char *)malloc(imdiff->image_length*sizeof(int));
      size_t n;
      n = lbufcompress(image_cbf,imdiff->image_length,packed);
      realloc(packed,n);
      //      printf("Compressed image\n");
      char tag[256];
      sprintf(tag,"%ld",n);
      //      printf("Setting the Binary-Size tag = %s. Header length = %ld\n",tag,imdiff->header_length);
      lsetcbftag(&imdiff->header,&imdiff->header_length,"X-Binary-Size:",tag);
      //      printf("Set the Binary-Size tag. New header length = %ld\n",imdiff->header_length);
      num_wrote = fwrite(imdiff->header, sizeof(char), imdiff->header_length,
			 imdiff->outfile);
      if (num_wrote != imdiff->header_length) {
	return_value = 1;
	sprintf(imdiff->error_msg,"\nCouldn't write all of CBF header.\n\n");
      }
      //      printf("Writing CBF image data...\n");
      num_wrote = fwrite(packed,sizeof(char),n,imdiff->outfile);
      if (num_wrote != n) {
	return_value = 1;
	sprintf(imdiff->error_msg,"\nCouldn't write all of CBF image data.\n\n");
      }
      //      printf("Writing CBF footer...\n");
      num_wrote = fwrite(imdiff->footer,sizeof(char),imdiff->footer_length,imdiff->outfile);
      if (num_wrote != imdiff->footer_length) {
	return_value = 1;
	sprintf(imdiff->error_msg,"\nCouldn't write all of CBF footer.\n\n");
      }
      free(image_cbf);
      free(packed);
    }
  }
  return(return_value);
}
  
