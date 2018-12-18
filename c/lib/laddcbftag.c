/* LADDCBFTAG.C - Add a tag to a diffraction image header.

   Author: Mike Wall
   Date: 12/18/2019
   Version: 1.

*/

#include<mwmask.h>

int laddcbftag(DIFFIMAGE *imdiff,const char *tag,const char *tagval)
{
  char *pos_begin,*pos_end;
  char *tail;
  char *startpos;
  char *newtarget;
  char insert_string[1000];
  size_t frontlen,backlen;
  int i;

  if ((startpos = strstr(imdiff->header,"_array_data.header_contents"))==NULL) {
    perror("Couldn't find .cbf header start\n");
    exit(1);
  } else {
    startpos = strstr(startpos,"#");
    startpos = strstr(startpos,";")-1;
    frontlen = (size_t)startpos - (size_t)imdiff->header + 1;
    backlen = imdiff->header_length - frontlen;
    sprintf(insert_string,"# %s %s\r\n",tag,tagval);
    newtarget = (char *)malloc(strlen(insert_string)+imdiff->header_length);
    
    memcpy(newtarget,imdiff->header,frontlen);
    memcpy(newtarget+frontlen,insert_string,strlen(insert_string));
    memcpy(newtarget+frontlen+strlen(insert_string),startpos+1,backlen);
    free(imdiff->header);
    imdiff->header = newtarget;
    imdiff->header_length += strlen(insert_string);
  }  
  return(0);
}

