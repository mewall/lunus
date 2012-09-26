#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define SPACE 32

int main(int argc,char *argv[])
{
  char 
    inl[255],
    workstr[255],
    *ch;
  
  while (gets(inl) != NULL) {
    strcpy(workstr,inl);
    ch = strchr(workstr,SPACE);
    *ch = 0;
    if (strcmp(workstr,"pcat") != 0) {
      strcpy(workstr, ch + 1);
      ch = strchr(workstr,SPACE);
      *ch = 0;
      strcpy(workstr, ch + 1);
      ch = strchr(workstr, SPACE);
      *ch = 0;
      printf("groff -man -Tascii %s | more -s -f\n", 
	     workstr);
    }
    else {
      inl[strlen(inl)-1] = 0;
      strcat(inl,"l| ul | more -s -f");
      puts(inl);
    }
  }  
}


