/* BINASC.C - Perform various binary/ascii conversions.
   Author: Mike Wall
   Date: 3/29/94
   Version: 1.0
   Description:

		"binasc [options] < input > output"

		Input is a binary/ascii stream.  
		Output is a binary/ascii stream.
*/

#include<mwmask.h>
#define SCALE 200.

int main(int argc, char *argv[])
{
  float
    x,
    y,
    z,
    upper,
    lower,
    scale_factor,
    value;
  size_t 
    i;
  
  int
    option = 0;
  
  char
    input_string[LINESIZE];
  
  short
    shortvalue;
  
  /*
   * Read information from input line:
   */
  
  switch(argc) {
    case 4:
    upper = atof(argv[3]);
    case 3:
    scale_factor = atof(argv[2]);
    case 2:
    option = atoi(argv[1]);
    break;
    default:
    printf("\nUsage:  binasc [options] [scale_factor] < [input file] > "
	   "[output file]\n\n"
	   "    Options:\n"
	   "       0: float input -> ascii output\n"
	   "       1: ascii input -> float output\n"
	   "       2: float input -> ascii output w/ leading index\n"
	   "       3: ascii input w/ leading index -> float output\n"
	   "       4: ascii input * scale_factor -> float output\n" 
	   "       5: float input -> scaled image\n"
	   "       6: float input -> short output\n"
	   "       7: short input -> float output\n"
	   "       8: (x,y,z,I) ascii input -> (x,y,z,I) float output\n\n");
    exit(0);
  }
  
  switch(option) {
    case 0:
    while (fread (&value, sizeof(float), 1, stdin) == 1) {
      printf("%f\n",value);
    }
    break;
    case 1:
    while (fgets (input_string, LINESIZE, stdin) != NULL) {
      sscanf(input_string, "%g", &value);
      fwrite(&value, sizeof(float), 1, stdout);
    }
    break;
    case 2:
    i=0;
    while (fread (&value, sizeof(float), 1, stdin) == 1) {
      printf("%ld %f\n",i,value);
      i++;
    }
    break;
    case 3: 
    while (fgets (input_string, LINESIZE, stdin) != NULL) {
      sscanf(input_string, "%ld %g", &i, &value);
      fwrite(&value, sizeof(float), 1, stdout);
    }
    break;
    case 4:
    while (fgets (input_string, LINESIZE, stdin) != NULL) {
      sscanf(input_string, "%g", &value);
      value *= scale_factor;
      fwrite(&value, sizeof(float), 1, stdout);
    }
    break;
    case 5:
    lower = scale_factor;
    scale_factor = upper - lower;
    while (fread (&value, sizeof(float), 1, stdin) == 1) {
      if (value <= lower) {
	value = 0;
      }
      if (value >= upper) {
	value = 255.;
      }
      if ((value > lower) && (value < upper)) {
	value = (value - lower) / (scale_factor) * 255.;
      }
      fwrite(&value, sizeof(float), 1, stdout);
    }
    break;
    case 6:
    while (fread (&value, sizeof(float),1,stdin) == 1) {
      shortvalue = (short)value;
      fwrite(&shortvalue, sizeof(short), 1, stdout);
    }
    break;
    case 7:
    while (fread (&shortvalue, sizeof(short),1,stdin) == 1) {
      value = (float)shortvalue;
      fwrite(&value, sizeof(float), 1, stdout);
    }
    break;
    case 8:
    while (fgets (input_string, LINESIZE, stdin) != NULL) {
      sscanf(input_string, "%g %g %g %g", &x, &y, &z, &value);
      fwrite(&x, sizeof(float), 1, stdout);
      fwrite(&y, sizeof(float), 1, stdout);
      fwrite(&z, sizeof(float), 1, stdout);
      fwrite(&value, sizeof(float), 1, stdout);
    }
    break;
    default:
    printf("\nUsage:  binasc [options] < [input file] > [output file]\n\n"
	   "    Options:\n"
	   "       0: float input -> ascii output\n"
	   "       1: ascii input -> float output\n"
	   "       2: float input -> ascii output w/ leading index\n"
	   "       3: ascii input w/ leading index -> float output\n"
	   "       4: ascii input * scale_factor -> float output\n"
	   "       5: float input -> scaled image\n"
	   "       6: float input -> short output\n"
	   "       7: short input -> float output\n"
	   "       8: (x,y,z,I) ascii input -> (x,y,z,I) float output\n\n");
    exit(0);
  }
}





