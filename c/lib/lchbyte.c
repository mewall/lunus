/* LCHBYTE.C - Reverse the byte order of a number.
   
   Author: Mike Wall
   Date: 6/22/94
   Version: 1.
   
   */

#include<mwmask.h>

int lchbyte(void *ptr, size_t packet_size, size_t list_length)
{
	size_t 
		i,
		j;
	char 
		*char_list,
		*packet,
		*temp_packet;

	temp_packet = (char *)malloc(packet_size+1);
	char_list = (char *)ptr;

	for(i=0;i<list_length;i++) {
		packet = (char *)&char_list[i*packet_size];
		for(j=0;j<packet_size;j++) {
			temp_packet[j] = packet[packet_size-j-1];
		}
		for(j=0;j<packet_size;j++) {
			packet[j] = temp_packet[j];
		}
	}
	free((char *)temp_packet);
}

