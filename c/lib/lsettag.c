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
  tail = (char *)malloc(len_tail+1);
  strcpy(tail,pos_end);
  memcpy(pos_begin+1,val,len_val);
  memcpy(pos_begin+len_val+1,tail,len_tail);
  for (i = len_val;i<len;i++) {
    target[len_targ-len+i]=0;
  }
  return(0);
}

int lsetcbftag(char **target,size_t *target_length, const char *tag,const char *val)
{
  char *pos_begin,*pos_end;
  char *head,*tail;
  int i;


  if ((pos_begin = strstr(*target,tag)) == NULL) {
    printf("\nCouldn't find tag %s in image header\n\n",tag);
    exit(6);
  }
  if ((pos_begin = strchr(pos_begin,' '))==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  }
  if ((pos_end = strstr(pos_begin,"\r\n"))==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  }
  size_t len = pos_end-pos_begin-1;
  size_t len_val = strlen(val);
  //  printf("len=%ld, len_val=%ld\n",len,len_val);
  //  printf("First char at pos_begin = %d\n",pos_begin[0]);
  //  printf("First, second char at pos_end = %d, %d\n",pos_end[0],pos_end[1]);
  size_t head_length;
  head_length = (size_t)pos_begin+1-(size_t)*target;
  head = (char *)malloc(sizeof(char)*head_length);
  memcpy(head,*target,head_length);
  size_t tail_length;
  tail_length = (size_t)*target_length-((size_t)pos_end-(size_t)*target);
  tail = (char *)malloc(sizeof(char)*tail_length);
  memcpy(tail,pos_end,tail_length);
  free(*target);
  *target_length += len_val - len;
  *target = (char *)malloc(*target_length);
  memcpy(*target,head,head_length);
  //  printf("val = %s\n",val);
  memcpy(*target+head_length,val,len_val);
  memcpy(*target+head_length+len_val,tail,tail_length);
  return(0);
}
