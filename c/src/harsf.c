/* HARSF.C - Calculate unit cell structure factor for Hirshfeld atom refinement.
   
   Author: Mike Wall
   Date: 12/2/2015
   Version: 1.
   
   Usage:
   		"harsf <directory root>  <atom shifts and ADPs .xyz> <output file root> <input .hkl>"

		Input a directory root for Hirshfeld parition structure factors, atom shifts with ADPs, and (optional) experimental reflections. 
			Output is structure factors corresponding to crystallographic HAR charge density, and (optional) figures of merit.  
   */

#include<mwmask.h>

int main(int argc, char *argv[])
{
  FILE
    *hklin,
    *partrealin,
    *partimagin,
    //    *xsrefin,
    *xsin,
    *cellrealout,
    *cellimagout;
  
  char
    error_msg[LINESIZE],fname[256],droot[256],cellroot[256],hklfname[256];
  
  size_t
    i,
    num_read,
    num_wrote;

  XTALSTRUCT 
    *xs;

  LAT3D
    **partreal,**partimag,
    *cellreal,*cellimag,
    **workreal,**workimag;

/*
 * Set input line defaults:
 */
  strcpy(cellroot,"har_sf");

/*
 * Read information from input line:
 */
	switch(argc) {

	case 5:
	  strcpy(hklfname,argv[4]);
/*
	  if ( (hklin = fopen(argv[4],"r")) == NULL ) {
	    printf("\nCan't open %s.\n\n",argv[4]);
	    exit(0);
	  }
*/
	  case 4:
	    strcpy(cellroot,argv[3]);
	  case 3:
	    if ( (xsin = fopen(argv[2],"r")) == NULL ) {
	      printf("\nCan't open %s.\n\n",argv[2]);
	      exit(0);
	    }
	  case 2:
	    strcpy(droot,argv[1]);
	  break;
	  default:
	  printf("\n Usage: harsf <directory root> <atom shifts and ADPs .xyz> <output file root> <input .hkl>\n\n");
	  exit(0);
	}
  
	// Initialize crystal structures
	/*
  if ((xsref = linitxs()) == NULL) {
    perror("Couldn't initialize crystal structure.\n\n");
    exit(0);
  }
	*/
  if ((xs = linitxs()) == NULL) {
    perror("Couldn't initialize crystal structure.\n\n");
    exit(0);
  }

  // Read in crystal structures:
  /*
  xsref->infile = xsrefin;
  if (lreadxs(xsref) != 0) {
    perror("Couldn't read crystal structure.\n\n");
    exit(0);
  }  
  */
  /*
  for (i=0;i<xsref->natoms;i++) {
    printf("%f %f %f %f %f %f %f %f %f\n",xsref->pos[i].x,xsref->pos[i].y,xsref->pos[i].z,xsref->u[i].U11,xsref->u[i].U22,xsref->u[i].U33,xsref->u[i].U12,xsref->u[i].U13,xsref->u[i].U23);
  }
  */

  xs->infile = xsin;
  if (lreadxs(xs) != 0) {
    perror("Couldn't read crystal structure.\n\n");
    exit(0);
  } 
 
  /*
  for (i=0;i<xswork->natoms;i++) {
    printf("%f %f %f %f %f %f %f %f %f\n",xsref->pos[i].x,xsref->pos[i].y,xsref->pos[i].z,xsref->u[i].U11,xsref->u[i].U22,xsref->u[i].U33,xsref->u[i].U12,xsref->u[i].U13,xsref->u[i].U23);
  }    
  */
  /*
  if (xsref->natoms != xswork->natoms) {
    perror("Working and reference structures have different number of atoms, aborting\n");
    exit(0);
  }
  */
  // Calculate unit cell structure factor from partitions, shifts, and ADPs

  // Initialize cell and work structure factor lattices

  if ((cellreal = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }
  if ((cellimag = linitlt()) == NULL) {
    perror("Couldn't initialize lattice.\n\n");
    exit(0);
  }

  // Read the real sf partition file for atom 0 into the cell and work sf

  sprintf(fname,"%s_0/part_sf_real.lat",droot);

  if ( (cellreal->infile = fopen(fname,"rb")) == NULL ) {
    printf("\nCan't open %s.\n\n",fname);
    exit(0);
  }

  if (lreadlt(cellreal) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  fclose(cellreal->infile);

  // Read the imag sf partition file for atom 0 into the cell and work sf

  sprintf(fname,"%s_0/part_sf_imag.lat",droot);

  if ( (cellimag->infile = fopen(fname,"rb")) == NULL ) {
    printf("\nCan't open %s.\n\n",fname);
    exit(0);
  }

  if (lreadlt(cellimag) != 0) {
    perror("Couldn't read lattice.\n\n");
    exit(0);
  }

  fclose(cellimag->infile);

  // Allocate and initialize all partitions

  partreal = (LAT3D **)malloc(xs->natoms*sizeof(LAT3D *));
  workreal = (LAT3D **)malloc(xs->natoms*sizeof(LAT3D *));
  for (i=0;i<xs->natoms;i++) {
    if ((partreal[i] = linitlt()) == NULL) {
      perror("Couldn't initialize lattice.\n\n");
      exit(0);
    }
    if ((workreal[i] = linitlt()) == NULL) {
      perror("Couldn't initialize lattice.\n\n");
      exit(0);
    }
  }

  partimag = (LAT3D **)malloc(xs->natoms*sizeof(LAT3D *));
  workimag = (LAT3D **)malloc(xs->natoms*sizeof(LAT3D *));
  for (i=0;i<xs->natoms;i++) {
    if ((partimag[i] = linitlt()) == NULL) {
      perror("Couldn't initialize lattice.\n\n");
      exit(0);
    }
    if ((workimag[i] = linitlt()) == NULL) {
      perror("Couldn't initialize lattice.\n\n");
      exit(0);
    }
  }

  // Read in all partitions

  for (i=0;i<xs->natoms;i++) {
    sprintf(fname,"%s_%d/part_sf_real.lat",droot,i);
    if ( (partreal[i]->infile = fopen(fname,"rb")) == NULL ) {
      printf("\nCan't open %s.\n\n",fname);
      exit(0);
    }
    if (lreadlt(partreal[i]) != 0) {
      perror("Couldn't read lattice.\n\n");
      exit(0);
    }
    fclose(partreal[i]->infile);
    if ( (workreal[i]->infile = fopen(fname,"rb")) == NULL ) {
      printf("\nCan't open %s.\n\n",fname);
      exit(0);
    }
    if (lreadlt(workreal[i]) != 0) {
      perror("Couldn't read lattice.\n\n");
      exit(0);
    }
    fclose(workreal[i]->infile);
    sprintf(fname,"%s_%d/part_sf_imag.lat",droot,i);
    if ( (partimag[i]->infile = fopen(fname,"rb")) == NULL ) {
      printf("\nCan't open %s.\n\n",fname);
      exit(0);
    }
    if (lreadlt(partimag[i]) != 0) {
      perror("Couldn't read lattice.\n\n");
      exit(0);
    }
    fclose(partimag[i]->infile);
    if ( (workimag[i]->infile = fopen(fname,"rb")) == NULL ) {
      printf("\nCan't open %s.\n\n",fname);
      exit(0);
    }
    if (lreadlt(workimag[i]) != 0) {
      perror("Couldn't read lattice.\n\n");
      exit(0);
    }
    fclose(workimag[i]->infile);
  }

  // Calculate unit cell structure factor

  // Initialize structure factor = 0
  for (i=0;i<cellreal->lattice_length;i++) {
    cellreal->lattice[i]=0;
    cellimag->lattice[i]=0;
  }

  // Sum contributions from all atom partitions
  size_t lattice_bytes = cellreal->lattice_length*sizeof(LATTICE_DATA_TYPE);
  for (i=0;i<xs->natoms;i++) {
    // Copy partition to work lattice
    memcpy(workreal[i]->lattice,partreal[i]->lattice,lattice_bytes);
    memcpy(workimag[i]->lattice,partimag[i]->lattice,lattice_bytes);
    // Define coordinate shifts
    //    workreal[i]->shift = lsubvec(xswork->pos[i],xsref->pos[i]);
    //    workimag[i]->shift = lsubvec(xswork->pos[i],xsref->pos[i]);
    workreal[i]->shift = xs->pos[i];
    workimag[i]->shift = xs->pos[i];
    // Apply the shift
    lshiftsflt(workreal[i],workimag[i]);
    // Calculate effect
    /*    float l2=0;
    int j;
    for (j=0;j<workreal[i]->lattice_length;j++) {
      l2 += (workreal[i]->lattice[j]-partreal[i]->lattice[j])*(workreal[i]->lattice[j]-partreal[i]->lattice[j]);
    }
    l2 /= workreal[i]->lattice_length;
    printf("l2 = %f\n",l2);
    */
    // Define U matrix
    workreal[i]->anisoU.xx=xs->u[i].U11;
    workreal[i]->anisoU.xy=xs->u[i].U12;
    workreal[i]->anisoU.xz=xs->u[i].U13;
    workreal[i]->anisoU.yx=xs->u[i].U12;
    workreal[i]->anisoU.yy=xs->u[i].U22;
    workreal[i]->anisoU.yz=xs->u[i].U23;
    workreal[i]->anisoU.zx=xs->u[i].U13;
    workreal[i]->anisoU.zy=xs->u[i].U23;
    workreal[i]->anisoU.zz=xs->u[i].U33;
    memcpy(&workimag[i]->anisoU,&workreal[i]->anisoU,sizeof(struct xyzmatrix));
    // Apply Debye-Waller factor
    lmuldwflt(workreal[i]);
    lmuldwflt(workimag[i]);
    // Add the result to the unit cell
    lsumlt(cellreal,workreal[i]);
    lsumlt(cellimag,workimag[i]);
  }

  // Calculate figures of merit

  struct fom f;

  f = lcalcrsf(hklfname,cellreal,cellimag);

  printf("%11.9f %11.9f %11.9f %11.9f\n",f.R,f.wR2_ccp4,f.wR2_shelx,f.goof);


  // Write the result
  
  sprintf(fname,"%s_real.lat",cellroot);
  if ( (cellreal->outfile = fopen(fname,"wb")) == NULL ) {
    printf("\nCan't open %s.\n\n",fname);
    exit(0);
  }
  if (lwritelt(cellreal) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }
  sprintf(fname,"%s_imag.lat",cellroot);
  if ( (cellimag->outfile = fopen(fname,"wb")) == NULL ) {
    printf("\nCan't open %s.\n\n",fname);
    exit(0);
  }
  if (lwritelt(cellimag) != 0) {
    perror("Couldn't write lattice.\n\n");
    exit(0);
  }
}

