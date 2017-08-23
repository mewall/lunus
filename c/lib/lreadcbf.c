/* LREADCBF.C - Read a diffraction image from a .cbf file.
 
  Author: Veronica Pillar
  Date: 7/18/13
  Version: 1

*/

#include <mwmask.h>
#include <string.h>

const char* getTag(const char *target, const char *tag);

int lreadcbf(DIFFIMAGE *imdiff)
{

  size_t
    num_read;

  int
    return_value = 0;

  char
    *buf;

  buf = (char*)malloc(DEFAULT_HEADER_LENGTH); //need to check that this is long enough

  /*
   * Read diffraction image header 
   */




}
