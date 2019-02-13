#include<stdio.h>

int main(void)
{
	short i;

	for (i=0;i<=196;i++) {
		printf("dd if=/dev/nrtapensv of=lysh%d.tif bs=2101248\n",i);
	}
} 

