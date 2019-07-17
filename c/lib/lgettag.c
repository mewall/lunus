/* LGETTAG.C - Read a tag from a diffraction image header.

   Author: Mike Wall
   Date: 4/12/2013
   Version: 1.
   Date: 4/4/2019
   Version: 1.1

*/

#include<mwmask.h>
#include<string.h>

int lfindtag(const char *target,const char *tag,char **pos_begin_ptr,char **pos_end_ptr) 
{
  char *pos_begin = NULL, *pos_end = NULL;

  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(6);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,'=')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    return(6);
  } else pos_begin = strchr(pos_begin,'=');

  if (strchr(pos_begin,';')==NULL) {
    if (strchr(pos_begin,'\n')==NULL) {
      perror("\nImage header tag syntax not recognized\n\n");
      return(6);
    } else {
      pos_end = strchr(pos_begin,'\n');
    }
  } else pos_end = strchr(pos_begin,';');

  *pos_begin_ptr = pos_begin;
  *pos_end_ptr = pos_end;

  return(0);
}

int lfindcbftag(const char *target,const char *tag,char **pos_begin_ptr,char **pos_end_ptr) 
{
  char *pos_begin = NULL, *pos_end = NULL;

  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(6);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,' ')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    return(6);
  } else pos_begin = strchr(pos_begin,' ');

  if (strstr(pos_begin,"\r\n")==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    return(6);
  } else pos_end = strstr(pos_begin,"\r\n");

  *pos_begin_ptr = pos_begin;
  *pos_end_ptr = pos_end;

  return(0);
}

char * lgettag(const char *target,const char *tag)
{
  char *pos_begin,*pos_end,*pos_tmp;
  char *val;
  int err;

  if ((err = lfindtag(target,tag,&pos_begin,&pos_end)) != 0) {
    //    perror("Couldn't find tag in lfindtag().\n");
    //    printf("tag = %s\n",tag);
    return(NULL);
  }

  int len = pos_end-pos_begin-1;
  val = (char *)calloc(sizeof(char),len+1);
  memcpy(val,pos_begin+1,len);
  val[len]=0;
  return(val);
}

int lgettagi(const char *target,const char *tag)
{
  char *pos_begin,*pos_end,*pos_tmp;
  char *valstr;
  
  int val;
  int err;

  if ((err = lfindtag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindtag().\n");
    exit(err);
  }

  int len = pos_end-pos_begin-1;
  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atoi(valstr);
  free(valstr);
  return(val);
}

float lgettagf(const char *target,const char *tag)
{
  char *pos_begin,*pos_end,*pos_tmp;
  char *valstr;
  
  float val;
  int err;

  if ((err = lfindtag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindtag().\n");
    exit(err);
  }

  int len = pos_end-pos_begin-1;
  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atof(valstr);
  free(valstr);
  return(val);
}

struct xyzcoords lgettagxyz(const char *target,const char *tag)
{
  char *pos_begin,*pos_end,*pos_tmp;
  char *valstr;
  
  struct xyzcoords val;
  int err;

  if ((err = lfindtag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindtag().\n");
    exit(err);
  }

  int len = pos_end-pos_begin-1;
  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  sscanf(valstr,"%f,%f,%f",&val.x,&val.y,&val.z);
  free(valstr);
  return(val);
}

char * lgetcbftag(const char *target,const char *tag)
{
  char *pos_begin = NULL,*pos_end = NULL,*pos_tmp = NULL;
  char *val;
  int err;

  if ((err = lfindcbftag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindcbftag().\n");
    exit(err);
  }

  int len = pos_end-pos_begin-1;

  val = (char *)calloc(sizeof(char),len+1);
  memcpy(val,pos_begin+1,len);
  val[len]=0;
  return(val);
}

int lgetcbftagi(const char *target,const char *tag)
{
  char *pos_begin = NULL,*pos_end = NULL,*pos_tmp = NULL;
  char *valstr;
  int val;
  int err;

  if ((err = lfindcbftag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindcbftag().\n");
    exit(err);
  }
  
  int len = pos_end-pos_begin-1;

  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atoi(valstr);
  free(valstr);
  return(val);
}

long lgetcbftagl(const char *target,const char *tag)
{
  char *pos_begin = NULL,*pos_end = NULL,*pos_tmp = NULL;
  char *valstr;
  long val;
  int err;

  if ((err = lfindcbftag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindcbftag().\n");
    exit(err);
  }
  
  int len = pos_end-pos_begin-1;

  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atol(valstr);
  free(valstr);
  return(val);
}

float lgetcbftagf(const char *target,const char *tag)
{
  char *pos_begin = NULL,*pos_end = NULL,*pos_tmp = NULL;
  char *valstr;
  float val;
  int err;

  if ((err = lfindcbftag(target,tag,&pos_begin,&pos_end)) != 0) {
    perror("Couldn't find tag in lfindcbftag().\n");
    exit(err);
  }
  
  int len = pos_end-pos_begin-1;

  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atof(valstr);
  free(valstr);
  return(val);
}
