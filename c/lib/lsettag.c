/* LSETTAG.C - Set a tag from a diffraction image header to a given value.

   Author: Mike Wall
   Date: 8/15/2013
   Version: 1.

*/

#include<mwmask.h>

int lsettag(char *target,const char *tag,const char *val)
{
  char *pos_begin,*pos_end;
  char *tail;
  int i;

  if ((pos_begin = strstr(target,tag)) == NULL) {
    printf("\nCouldn't find tag %s in image header\n\n",tag);
    exit(6);
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
  int len_val = strlen(val);
  int len_tail = strlen(pos_end);
  int len_targ = strlen(target);
  tail = (char *)malloc(len_tail);
  strcpy(tail,pos_end);
  memcpy(pos_begin+1,val,len_val);
  memcpy(pos_begin+len_val+1,tail,len_tail);
  for (i = len_val;i<len;i++) {
    target[len_targ-i+1]=0;
  }
  return(0);
}
