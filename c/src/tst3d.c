#include<stdio.h>
#include<math.h>
#include<stdlib.h>

#define DIM 101
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
		j,
		k;

	long
		res[3] = {DIM, DIM, DIM};

	float
		ifl,
		jfl,
		kfl,
		bBox[6] = {LOWI*2, HIGHI*2,
				LOWJ, HIGHJ,
				LOWK, HIGHK},
		val,
		isc,
		jsc,
		ksc;

	isc = (HIGHI - LOWI) / (float)DIM;
	jsc = (HIGHJ - LOWJ) / (float)DIM;
	ksc = (HIGHK - LOWK) / (float)DIM;
	fwrite(res, sizeof(long), 3, stdout);
	fwrite(bBox, sizeof(float), 6, stdout);
	for (k=1;k<=DIM;k++) {
		for(j=1;j<=DIM;j++) {
			for (i=1;i<=DIM;i++) {
				ifl = LOWI + isc*(float)i;
				jfl = LOWJ + jsc*(float)j;
				kfl = LOWK + ksc*(float)k;
				val = sqrtf(ifl*ifl+jfl*jfl+kfl*kfl);
				fwrite(&val, sizeof(float), 1, stdout);
			}
		}
	}
}
