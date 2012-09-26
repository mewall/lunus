#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define DIM 20
#define LOWI -10.5
#define HIGHI 10.5
#define LOWJ -10.5
#define HIGHJ 10.5
#define LOWK -10.5
#define HIGHK 10.5
int main(void)
{
	int
		i,
		nrem,
		k;
	long
		count,
		matrix_size;

	long
		res[3];

	float
		max = 0,
		bBox[6],
		val[6];
	
	char 
		inl[80];

	gets(inl);
	gets(inl);
	sscanf(inl,"%d",&nrem);
	for(k=1;k<=nrem;k++) {
		gets(inl);
	}
	gets(inl);
	sscanf(inl,"%12ld%12g%12g%12ld%12g%12g%12ld%12g%12g",
					&res[0],&bBox[0],&bBox[1], 
					&res[1],&bBox[2],&bBox[3],
					&res[2],&bBox[4],&bBox[5]);
	res[0] = (long)(bBox[1] - bBox[0])+1;
	res[1] = (long)(bBox[3] - bBox[2])+1;
	res[2] = (long)(bBox[5] - bBox[4])+1;
	fwrite(res, sizeof(long), 3, stdout);
	fwrite(bBox, sizeof(float), 6, stdout);
	matrix_size = (res[0] * res[1]);
	gets(inl);
	for (k=1;k<=res[2];k++) {
		gets(inl);		/* skip k-index line */
		count = 0;
		while(count < matrix_size) {
		  gets(inl);
		  sscanf(inl,"%12g%12g%12g%12g%12g%12g",
						&val[0],&val[1],&val[2],
						&val[3],&val[4],&val[5]);
		  count += 6;
		  if (count > matrix_size) {
		    fwrite(val, sizeof(float), count % matrix_size, stdout);
		  }
		  else {
		    fwrite(val, sizeof(float), 6, stdout);
		  }
		}
	}
	printf("\n\n%ld %ld %ld\n\n",res[0],res[1],res[2]);
}
