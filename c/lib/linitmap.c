/* LINITMAP.C - Initialize a CCP4 map.

   Author: Mike Wall
   Date: 2/13/2014
   Version: 1.

*/

#include<mwmask.h>

CCP4MAP *linitmap(void) 
{

  CCP4MAP *map;

  size_t
    index;
  
  map = (CCP4MAP *)malloc(sizeof(CCP4MAP));
  if (!map) {
    printf("\nLINITLT:  Unable to allocate all memory (map).\n");
    map = NULL;
    goto CloseShop;
  }

  CloseShop:

  return(map);

}












