/* RF2CHAR2.C - Convert an ascii rfile from tv6 to a list of human-readable values. 
    Author: Mike Wall, then Veronica Pillar
    Date: 4/14/14
    Version: 2.0
    Description:

	"rf2char < infile > outfile"

    Input is an ascii rfile.  
    Output is a readable list of numbers.
*/

#include<mwmask.h>

int main(int argc, char *argv[])
{
  RFILE_DATA_TYPE
    value;

  while (fread(&value, sizeof(RFILE_DATA_TYPE), 1, stdin) == 1) {
    fprintf(stdout, "%f\n", value);
  }
}
