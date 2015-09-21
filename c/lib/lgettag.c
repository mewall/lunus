/* LGETTAG.C - Read a tag from a diffraction image header.

   Author: Mike Wall
   Date: 4/12/2013
   Version: 1.

*/

#include<mwmask.h>
#include<string.h>

const char * lgettag(const char *target,const char *tag)
{
  char *pos_begin,*pos_end,*pos_tmp;
  char *val;
  
  int pos_begin_found;

  pos_begin_found=0;

  //  printf("In lgettag()\n");

  if ((pos_tmp = strstr(target,tag)) != NULL) {
    pos_begin = pos_tmp;
    pos_begin_found=1;
  }

  //  printf("Found the tag\n");

  if (pos_begin_found==0) {
    printf("\nWarning: Couldn't find tag %s in image header\n\n",tag);
    return(NULL);
  }
  while (pos_tmp != NULL) {
    if ((pos_tmp = strstr(pos_tmp+1,tag)) != NULL) {
      pos_begin = pos_tmp;
    }
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
