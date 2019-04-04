/* LGETTAG.C - Read a tag from a diffraction image header.

   Author: Mike Wall
   Date: 4/12/2013
   Version: 1.

*/

#include<mwmask.h>
#include<string.h>

int lfindtag(const char *target,const char *tag,const char sep,char **pos_begin_ptr,char **pos_end_ptr) 
{
  char *pos_begin = NULL, *pos_end = NULL;

  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(NULL);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,' ')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    return(6);
  } else pos_begin = strchr(pos_begin,sep);

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
  
  int pos_begin_found;

  pos_begin_found=0;

  //  printf("In lgettag()\n");

  if ((pos_tmp = strstr(target,tag)) != NULL) {
    pos_begin = pos_tmp;
    pos_begin_found=1;
  }

  //  printf("Found the tag\n");

  if (pos_begin_found==0) {
    //    printf("\nWarning: Couldn't find tag %s in image header\n\n",tag);
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
    if ((pos_end = strchr(pos_begin,'\n'))==NULL) {
      perror("\nImage header tag syntax not recognized\n\n");
      exit(6);
    }
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

  int pos_begin_found;

  pos_begin_found=0;

  //  printf("In lgettag()\n");

  if ((pos_tmp = strstr(target,tag)) != NULL) {
    pos_begin = pos_tmp;
    pos_begin_found=1;
  }

  //  printf("Found the tag\n");

  if (pos_begin_found==0) {
    //    printf("\nWarning: Couldn't find tag %s in image header\n\n",tag);
    return(0);
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
    if ((pos_end = strchr(pos_begin,'\n'))==NULL) {
      perror("\nImage header tag syntax not recognized\n\n");
      exit(6);
    }
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

  int pos_begin_found;

  pos_begin_found=0;

  //  printf("In lgettag()\n");

  if ((pos_tmp = strstr(target,tag)) != NULL) {
    pos_begin = pos_tmp;
    pos_begin_found=1;
  }

  //  printf("Found the tag\n");

  if (pos_begin_found==0) {
    //    printf("\nWarning: Couldn't find tag %s in image header\n\n",tag);
    return(0);
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
    if ((pos_end = strchr(pos_begin,'\n'))==NULL) {
      perror("\nImage header tag syntax not recognized\n\n");
      exit(6);
    }
  }
  int len = pos_end-pos_begin-1;
  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atof(valstr);
  free(valstr);
  return(val);
}

char * lgetcbftag(const char *target,const char *tag)
{
  char *pos_begin = NULL,*pos_end = NULL,*pos_tmp = NULL;
  char *val;

  if (lfindtag(target,tag,' ',&pos_begin,&pos_end) != 0) {
    perror("Couldn't find tag in lfindtag().\n");
    exit(1);
  }
  /*
  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(NULL);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,' ')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_begin = strchr(pos_begin,' ');

  if (strstr(pos_begin,"\r\n")==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_end = strstr(pos_begin,"\r\n");
  */
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
  
  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(-1);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,' ')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_begin = strchr(pos_begin,' ');

  if (strstr(pos_begin,"\r\n")==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_end = strstr(pos_begin,"\r\n");

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
  
  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(-1);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,' ')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_begin = strchr(pos_begin,' ');

  if (strstr(pos_begin,"\r\n")==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_end = strstr(pos_begin,"\r\n");

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
  
  if (strstr(target,tag) != NULL) {
    pos_begin = strstr(target,tag);
  }

  if (pos_begin==NULL) {
    return(-1.0);
  }

  while (strstr(pos_begin+1,tag) != NULL) {
    pos_begin = strstr(pos_begin+1,tag);
  }

  if (strchr(pos_begin,' ')==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_begin = strchr(pos_begin,' ');

  if (strstr(pos_begin,"\r\n")==NULL) {
    perror("\nImage header tag syntax not recognized\n\n");
    exit(6);
  } else pos_end = strstr(pos_begin,"\r\n");

  int len = pos_end-pos_begin-1;

  valstr = (char *)calloc(sizeof(char),len+1);
  memcpy(valstr,pos_begin+1,len);
  valstr[len]=0;
  val = atof(valstr);
  free(valstr);
  return(val);
}
