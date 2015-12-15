/* LSHIFTSFLT.C - Shift a structure factor lattice by an arbitraty vector.
   
   Author: Mike Wall
   Date: 7/31/15
   Version: 1.
   
   */

#include<mwmask.h>
#include<complex.h>

int lshiftsflt(LAT3D *lat1,LAT3D *lat2)
{
  
  int return_value=0;

  int
    i,j,k;

  int
    hh,kk,ll;

  int u0,v0,w0;

  int N1,N2,N3;

  float u1,v1,w1;

  float complex *data;
  
  float complex p000,p100,p010,p001,p110,p101,p011,p111;

  float complex g000,g100,g010,g001,g110,g101,g011,g111;

  float complex F0;

  float complex ph,pkl,pl;

  //  printf("in lshiftsflt\n");

  data = (float complex *)malloc(lat1->lattice_length*sizeof(float complex));

  N1 = (int)lat1->xvoxels;
  N2 = (int)lat1->yvoxels;
  N3 = (int)lat1->zvoxels;

  u0 = floor(lat1->shift.x);
  v0 = floor(lat1->shift.y);
  w0 = floor(lat1->shift.z);

  //  printf("u0=%d,v0=%d,w0=%d\n",u0,v0,w0);

  u1 = lat1->shift.x - u0;
  v1 = lat1->shift.y - v0;
  w1 = lat1->shift.z - w0;

  //  printf("u1=%f,v1=%f,w1=%f\n",u1,v1,w1);

  g000 = (1.-u1)*(1.-v1)*(1.-w1) + 0.*I;
  g100 = u1*(1.-v1)*(1.-w1) + 0.*I;
  g010 = (1.-u1)*v1*(1.-w1) + 0.*I;
  g001 = (1.-u1)*(1.-v1)*w1 + 0.*I;
  g110 = u1*v1*(1.-w1) + 0.*I;
  g101 = u1*(1.-v1)*w1 + 0.*I;
  g011 = (1.-u1)*v1*w1 + 0.*I;
  g111 = u1*v1*w1 + 0.*I;

  size_t lat_index=0;

  //  printf("Starting loop...\n");

  for(k = 0; k < lat1->zvoxels; k++) {
    ll = k-lat1->origin.k;
    pl = cexp(-TWOPI*I*ll*w0/(float)N3);
    p001 = cexp(-TWOPI*I*ll/(float)N3);
    for(j = 0; j < lat1->yvoxels; j++) {
      kk = j - lat1->origin.j;
      pkl = pl*cexp(-TWOPI*I*kk*v0/(float)N2);
      p010 = cexp(-TWOPI*I*kk/(float)N2);
      p011 = p001*p010;
      for (i = 0; i < lat1->xvoxels; i++) {
	hh = i - lat1->origin.i;
	ph = cexp(-TWOPI*I*hh*u0/(float)N1);
	p100 = cexp(-TWOPI*I*hh/(float)N1);
	p101 = p100*p001;
	p110 = p100*p010;
	p111 = p110*p001;
	F0 = (lat1->lattice[lat_index]+lat2->lattice[lat_index]*I)*ph*pkl;
	data[lat_index]=F0*(g000+p100*g100+p010*g010+p001*g001+p110*g110+p101*g101+p011*g011+p111*g111);
	lat_index++;
      }
    }
  }

  for (lat_index=0;lat_index<lat1->lattice_length;lat_index++) {
    lat1->lattice[lat_index]=creal(data[lat_index]);
    lat2->lattice[lat_index]=cimag(data[lat_index]);
  }

  CloseShop:
  return(return_value);
}


