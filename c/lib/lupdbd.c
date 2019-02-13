/* LUPDBD.C - Update the current bounds of a lattice.

   Author: Mike Wall
   Date: 9/27/95
   Version: 1.

*/

#include<stdio.h>
#include<stdlib.h>
#include<mwmask.h>

int lupdbd(LAT3D *lat)
{
	if (lat->map3D->pos.x > lat->xbound.max) lat->xbound.max = lat->map3D->pos.x;
	if (lat->map3D->pos.x < lat->xbound.min) lat->xbound.min = lat->map3D->pos.x;
	if (lat->map3D->pos.y > lat->ybound.max) lat->ybound.max = lat->map3D->pos.y;
	if (lat->map3D->pos.y < lat->ybound.min) lat->ybound.min = lat->map3D->pos.y;
	if (lat->map3D->pos.z > lat->zbound.max) lat->zbound.max = lat->map3D->pos.z;
	if (lat->map3D->pos.z < lat->zbound.min) lat->zbound.min = lat->map3D->pos.z;
	if (lat->map3D->value != lat->mask_tag ) {
	  if (lat->map3D->value > lat->valuebound.max) {
		lat->valuebound.max = lat->map3D->value;
	  }
	  if (lat->map3D->value < lat->valuebound.min) {
		lat->valuebound.min = lat->map3D->value;
	  }
	}
}
