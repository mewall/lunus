/* LREADBUF.C - Read a buffer from a file. The buffer is allocated to the proper length. Returns the length.

   Author: Mike Wall
   Date: 2/14/2018
   Version: 1.

*/

#include<mwmask.h>

size_t lreadbuf(void **buf,const char *fname)
{
  size_t this_pos, end_pos, buf_length, num_read;
  FILE *f;
  
  if ( (f = fopen(fname,"rb")) == NULL ) {
    printf("LREADBUF: Can't open %s. Returning with value -1.\n",fname);
    return(-1);
  }

  this_pos = ftell(f);
  fseek(f, 0, SEEK_END); // seek to end of file
  end_pos = ftell(f); // get current file pointer
  fseek(f,this_pos,SEEK_SET);
  buf_length = (size_t)(end_pos-this_pos);
  //  if (*buf != NULL) free(*buf);
  //  printf("buf_length = %ld\n",buf_length);
  *buf = (void *)calloc(buf_length+1,sizeof(char));
  if (*buf == NULL) {
    printf("LREADBUF: Could not allocate buffer\n");
    exit(1);
  }
  num_read = fread(*buf, sizeof(void), buf_length,f);
  if (num_read != buf_length) {
    printf("LREADBUF: number of bytes read differs from length\n");
    exit(1);
  }
  fclose(f);
  return(num_read);
}
