/* LPARALLEL.C - MPI wrappers

   Author: Mike Wall
   Date: 6/13/2017
   Version: 1.

*/

#include<mwmask.h>
#ifdef USE_MPI
#include<mpi.h>
#endif

void linitMPI(MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Init(&mpiv->argc, &mpiv->argv);
  mpiv->ierr = MPI_Comm_rank(MPI_COMM_WORLD, &mpiv->my_id);
  mpiv->ierr = MPI_Comm_size(MPI_COMM_WORLD, &mpiv->num_procs);
#else
  mpiv->my_id = 0;
  mpiv->num_procs = 1;
  mpiv->ierr = 0;
#endif
}

void lfinalMPI(MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Finalize();
#endif
}

void lbarrierMPI(MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Barrier(MPI_COMM_WORLD);
#endif

}

void lbcastImageMPI(IMAGE_DATA_TYPE *data,size_t datalen, int root, MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Bcast((void *)data, (int)datalen, MPI_SHORT, root, MPI_COMM_WORLD);
#endif

}

void lbcastBufMPI(void *data,size_t datalen, int root, MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Bcast((void *)data, (int)datalen, MPI_CHAR, root, MPI_COMM_WORLD);
#endif

}

void lreduceSumLatticeMPI(LATTICE_DATA_TYPE *data,LATTICE_DATA_TYPE *target,size_t datalen, int root, MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Reduce((void *)data, (void *)target,(int)datalen, MPI_FLOAT, MPI_SUM, root, MPI_COMM_WORLD);
#else
  memcpy((void *)target,(void *)data,(size_t)datalen*sizeof(float));
#endif

}

void lreduceSumLatctMPI(size_t *data,size_t *target,size_t datalen, int root, MPIVARS *mpiv) {

#ifdef USE_MPI
  mpiv->ierr = MPI_Reduce((void *)data, (void *)target,(int)datalen, MPI_UNSIGNED_LONG, MPI_SUM, root, MPI_COMM_WORLD);
#else
  memcpy((void *)target,(void *)data,(size_t)datalen*sizeof(size_t));
#endif

}
