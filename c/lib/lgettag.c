/* LGETTAG.C - Read a tag from a diffraction image header.

   Author: Mike Wall
   Date: 4/12/2013
   Version: 1.

*/

#include<string.h>

const char * lgettag(const char *target,const char *tag)
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
