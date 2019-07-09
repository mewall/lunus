#include<lunus.h>
#include<cJSON.h>

int readExptJSON(struct xyzmatrix *a,char **image_name,char **pedestal_name,const char *json_name) {

  char *json_text;

  cJSON 
    *j = NULL, 
    *imageset = NULL, 
    *crystal = NULL, 
    *real_space_a = NULL,
    *real_space_b = NULL,
    *real_space_c = NULL;

  if (lreadbuf((void **)&json_text,json_name) == -1) {
    perror("Can't read json file\n");
    goto readExptJSONFail;
  }

  j = cJSON_Parse(json_text);

  imageset = cJSON_GetArrayItem(cJSON_GetObjectItem(j,"imageset"),0);
  
  *image_name = cJSON_GetArrayItem(cJSON_GetObjectItem(imageset,"images"),0)->valuestring;

  *pedestal_name = cJSON_GetObjectItem(imageset,"pedestal")->valuestring;

  crystal = cJSON_GetArrayItem(cJSON_GetObjectItem(j,"crystal"),0);

  real_space_a = cJSON_GetObjectItem(crystal,"real_space_a");
  real_space_b = cJSON_GetObjectItem(crystal,"real_space_b");
  real_space_c = cJSON_GetObjectItem(crystal,"real_space_c");

  a->xx = cJSON_GetArrayItem(real_space_a,0)->valuedouble;
  a->xy = cJSON_GetArrayItem(real_space_a,1)->valuedouble;
  a->xz = cJSON_GetArrayItem(real_space_a,2)->valuedouble;
  a->yx = cJSON_GetArrayItem(real_space_b,0)->valuedouble;
  a->yy = cJSON_GetArrayItem(real_space_b,1)->valuedouble;
  a->yz = cJSON_GetArrayItem(real_space_b,2)->valuedouble;
  a->zx = cJSON_GetArrayItem(real_space_c,0)->valuedouble;
  a->zy = cJSON_GetArrayItem(real_space_c,1)->valuedouble;
  a->zz = cJSON_GetArrayItem(real_space_c,2)->valuedouble;

#ifdef DEBUG
  //    printf("Amatrix for image %s: ",image_name);
  //    printf("(%f, %f, %f) ",a->xx,a->xy,a->xz);
  //    printf("(%f, %f, %f) ",a->yx,a->yy,a->yz);
  //    printf("(%f, %f, %f) ",a->zx,a->zy,a->zz);
  //    printf("\n");
#endif		

  return(0);

 readExptJSONFail:
  return(1);

}

int readPanelJSON(struct xyzcoords *fast_vec,struct xyzcoords *slow_vec,struct xyzcoords *origin_vec,const char *json_name) {

  char *json_text;

  cJSON 
    *j = NULL, 
    *detector = NULL,
    *panel = NULL,
    *fast = NULL,
    *slow = NULL,
    *origin = NULL;

  if (lreadbuf((void **)&json_text,json_name) == -1) {
    perror("Can't read json file\n");
    goto readPanelJSONFail;
  }

  j = cJSON_Parse(json_text);

  detector = cJSON_GetArrayItem(cJSON_GetObjectItem(j,"detector"),0);
  
  panel = cJSON_GetArrayItem(cJSON_GetObjectItem(detector,"panels"),0);

  fast = cJSON_GetObjectItem(panel,"fast_axis");

  slow = cJSON_GetObjectItem(panel,"slow_axis");

  origin = cJSON_GetObjectItem(panel,"origin");

  fast_vec->x = cJSON_GetArrayItem(fast,0)->valuedouble;
  fast_vec->y = cJSON_GetArrayItem(fast,1)->valuedouble;
  fast_vec->z = cJSON_GetArrayItem(fast,2)->valuedouble;

  slow_vec->x = cJSON_GetArrayItem(slow,0)->valuedouble;
  slow_vec->y = cJSON_GetArrayItem(slow,1)->valuedouble;
  slow_vec->z = cJSON_GetArrayItem(slow,2)->valuedouble;

  origin_vec->x = cJSON_GetArrayItem(origin,0)->valuedouble;
  origin_vec->y = cJSON_GetArrayItem(origin,1)->valuedouble;
  origin_vec->z = cJSON_GetArrayItem(origin,2)->valuedouble;

#ifdef DEBUG
  struct xyzcoords normal_vec;

  normal_vec = lcrossvec(*fast_vec,*slow_vec);

  printf("fast, slow, normal, and origin:\n");
  printf("(%f, %f, %f) ",fast_vec->x,fast_vec->y,fast_vec->z);
  printf("(%f, %f, %f) ",slow_vec->x,slow_vec->y,slow_vec->z);
  printf("(%f, %f, %f) ",normal_vec.x,normal_vec.y,normal_vec.z);
  printf("(%f, %f, %f) ",origin_vec->x,origin_vec->y,origin_vec->z);
  printf("\n");
#endif		

  return(0);

 readPanelJSONFail:
  return(1);

}

int readBeamJSON(struct xyzcoords *beam_vec,struct xyzcoords *polarization_vec,float *wavelength, const char *json_name) {

  char *json_text;

  cJSON 
    *j = NULL, 
    *beam = NULL,
    *direction = NULL,
    *polarization_normal = NULL;

  if (lreadbuf((void **)&json_text,json_name) == -1) {
    perror("Can't read json file\n");
    goto readBeamJSONFail;
  }

  j = cJSON_Parse(json_text);

  beam = cJSON_GetArrayItem(cJSON_GetObjectItem(j,"beam"),0);
  
  direction = cJSON_GetObjectItem(beam,"direction");

  polarization_normal = cJSON_GetObjectItem(beam,"polarization_normal");

  *wavelength = (float)cJSON_GetObjectItem(beam,"wavelength")->valuedouble;

  // Note: the beam direction is apparently opposite to what is in the .json file? Check with Nick and Aaron about this.

  beam_vec->x = - cJSON_GetArrayItem(direction,0)->valuedouble;
  beam_vec->y = - cJSON_GetArrayItem(direction,1)->valuedouble;
  beam_vec->z = - cJSON_GetArrayItem(direction,2)->valuedouble;

  polarization_vec->x = cJSON_GetArrayItem(polarization_normal,0)->valuedouble;
  polarization_vec->y = cJSON_GetArrayItem(polarization_normal,1)->valuedouble;
  polarization_vec->z = cJSON_GetArrayItem(polarization_normal,2)->valuedouble;

  return(0);

 readBeamJSONFail:
  return(1);

}

int readAmatrix(struct xyzmatrix *a,const char *amatrix_format,const size_t i) {

  int num_read,str_length;

  char *amatrix_path;

  struct xyzmatrix *amatrix;

  str_length = snprintf(NULL,0,amatrix_format,i);
	    
  amatrix_path = (char *)malloc(str_length+1);
  
  sprintf(amatrix_path,amatrix_format,i);
  
  num_read = lreadbuf((void **)&amatrix,amatrix_path);

  if (num_read != -1) {
  

  // Calculate the transpose of a, to use matrix multiplication method
  
    *a = lmatt(*amatrix);

#ifdef DEBUG
    //    printf("Amatrix for image %d: ",i);
    //    printf("(%f, %f, %f) ",a->xx,a->xy,a->xz);
    //    printf("(%f, %f, %f) ",a->yx,a->yy,a->yz);
    //    printf("(%f, %f, %f) ",a->zx,a->zy,a->zz);
    //    printf("\n");
#endif		

    return(0);
  
  } else {
    return(-1);
  }
}
