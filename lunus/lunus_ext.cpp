    // Authors: Mike Wall & Nick Sauter
    // Date: 6/15/2017
    // Lat3D class, RadialAvgim, Normim, & Polarim methods wrapped by Alex Wolff (9/22/2017).



#include <cctbx/boost_python/flex_fwd.h>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <scitbx/array_family/flex_types.h>
#include <scitbx/array_family/shared.h>

extern "C" {
#include <c/include/mwmask.h>
}

#include <string>
#include <vector>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <exception>

namespace lunus {
  void foo2(){
    std::cout<<"HELLO foo2"<<std::endl;
  }

  class Process {
  protected:
    DIFFIMAGE *imdiff, *imdiff_ref, *imdiff_bkg;
    LAT3D *lat;

  public:
    inline Process(): imdiff(linitim(1)), imdiff_ref(linitim(1)), imdiff_bkg(linitim(1)), lat(linitlt()) { }

    inline Process(std::size_t n): imdiff(linitim(n)), imdiff_ref(linitim(n)), imdiff_bkg(linitim(n)), lat(linitlt()) { }

    inline ~Process() {
      lfreeim(imdiff);
      lfreelt(lat);
    }

    inline void LunusSetparamsim(std::string deck) {
      deck += '\n';
      if (imdiff->params != NULL) free(imdiff->params);
      imdiff->params = (char *)calloc(deck.length()+1,sizeof(char));
      strcpy(imdiff->params,deck.c_str());
      lsetparamsim(imdiff);
      if (imdiff_bkg->params != NULL) free(imdiff_bkg->params);
      imdiff_bkg->params = (char *)calloc(deck.length()+1,sizeof(char));
      strcpy(imdiff_bkg->params,deck.c_str());
      lsetparamsim(imdiff_bkg);
    }

    inline void LunusSetparamsim(std::size_t n,std::string deck) {
      deck += '\n';
      DIFFIMAGE *im = &imdiff[n];
      if (im->params != NULL) free(im->params);
      im->params = (char *)calloc(deck.length()+1,sizeof(char));
      strcpy(im->params,deck.c_str());
      lsetparamsim(im);
    }

    inline void LunusSetmetim() {
      lsetmetim(imdiff);
      lsetmetim(imdiff_bkg);
    }

    inline void print_image_params() {
      int pidx;
      for (pidx = 0; pidx < imdiff->num_panels; pidx++) {
	DIFFIMAGE *im = &imdiff[pidx];
	printf("Panel %d:\n",pidx);
	printf("origin.xyz = (%f, %f, %f)\n",im->origin_vec.x,im->origin_vec.y,im->origin_vec.z);
	printf("fast_vec.xyz = (%f, %f, %f)\n",im->fast_vec.x,im->fast_vec.y,im->fast_vec.z);
	printf("slow_vec.xyz = (%f, %f, %f)\n",im->slow_vec.x,im->slow_vec.y,im->slow_vec.z);
	printf("normal_vec.xyz = (%f, %f, %f)\n",im->normal_vec.x,im->normal_vec.y,im->normal_vec.z);
	printf("beam_vec.xyz = (%f, %f, %f)\n",im->beam_vec.x,im->beam_vec.y,im->beam_vec.z);
	printf("image[1001..1010] = ");
	for (int i=1001;i<=1010;i++) printf("%d ",im->image[i]);
	printf("\n");
	printf("slist[1001..1010] = ");
	for (int i=1001;i<=1010;i++) printf("(%f,%f,%f) ",im->slist[i].x,im->slist[i].y,im->slist[i].z);
	printf("\n");
      }
      printf("use_json_metrology = %d\n",imdiff->use_json_metrology);
    }

    inline void set_image_ref() {
      lcloneim(imdiff_ref,imdiff);
    }

    inline void set_image(scitbx::af::flex_int data) {
      int* begin=data.begin();
      std::size_t size=data.size();
      std::size_t slow=data.accessor().focus()[0];
      std::size_t fast=data.accessor().focus()[1];
      if (imdiff->image_length != size) {
	imdiff->image_length = size;
	imdiff->image = (IMAGE_DATA_TYPE *)realloc(imdiff->image,imdiff->image_length*sizeof(IMAGE_DATA_TYPE));
	imdiff->correction = (float *)realloc(imdiff->correction,imdiff->image_length*sizeof(float));
      }
      imdiff->hpixels = fast;
      imdiff->vpixels = slow;
      imdiff->value_offset = 0;
      IMAGE_DATA_TYPE max=-32766,min=32766;
      std::size_t ct=0;
      for (int i = 0;i<imdiff->image_length;i++) {
	if (begin[i]<0 || begin[i] >= MAX_IMAGE_DATA_VALUE) {
	  imdiff->image[i] = imdiff->ignore_tag;
	  ct++;
	} else {
	  imdiff->image[i] = (IMAGE_DATA_TYPE)begin[i];
	  if (imdiff->image[i]>max) max = imdiff->image[i];
	  if (imdiff->image[i]<min) min = imdiff->image[i];
	}
      }
      //      printf("ct = %ld,max = %d, min = %d\n",ct,max,min);
    }

    inline void set_image(std::size_t n,scitbx::af::flex_int data) {
      int* begin=data.begin();
      std::size_t size=data.size();
      std::size_t slow=data.accessor().focus()[0];
      std::size_t fast=data.accessor().focus()[1];
      DIFFIMAGE *im = &imdiff[n];
      if (im->image_length != size) {
	im->image_length = size;
	im->image = (IMAGE_DATA_TYPE *)realloc(im->image,im->image_length*sizeof(IMAGE_DATA_TYPE));
	im->correction = (float *)realloc(im->correction,im->image_length*sizeof(float));
      }
      im->hpixels = fast;
      im->vpixels = slow;
      im->value_offset = 0;
      IMAGE_DATA_TYPE max=-32766,min=32766;
      std::size_t ct=0;
      for (int i = 0;i<im->image_length;i++) {
	if (begin[i]<0 || begin[i] >= MAX_IMAGE_DATA_VALUE) {
	  im->image[i] = im->ignore_tag;
	  ct++;
	} else {
	  im->image[i] = (IMAGE_DATA_TYPE)begin[i];
	  if (im->image[i]>max) max = im->image[i];
	  if (im->image[i]<min) min = im->image[i];
	}
      }
      //      printf("ct = %ld,max = %d, min = %d\n",ct,max,min);
    }

    inline void set_background(scitbx::af::flex_int data) {
      int* begin=data.begin();
      std::size_t size=data.size();
      std::size_t slow=data.accessor().focus()[0];
      std::size_t fast=data.accessor().focus()[1];
      if (imdiff_bkg->image_length != size) {
	imdiff_bkg->image_length = size;
	imdiff_bkg->image = (IMAGE_DATA_TYPE *)realloc(imdiff_bkg->image,imdiff_bkg->image_length*sizeof(IMAGE_DATA_TYPE));
	imdiff_bkg->correction = (float *)realloc(imdiff_bkg->correction,imdiff_bkg->image_length*sizeof(float));
      }
      imdiff_bkg->hpixels = fast;
      imdiff_bkg->vpixels = slow;
      imdiff_bkg->value_offset = 0;
      std::size_t ct = 0;
      IMAGE_DATA_TYPE max=-32766,min=32766;
      for (int i = 0;i<imdiff_bkg->image_length;i++) {
	if (begin[i]<0 || begin[i] > MAX_IMAGE_DATA_VALUE) {
	  imdiff_bkg->image[i] = imdiff_bkg->ignore_tag;
	  ct++;
	} else {
	  imdiff_bkg->image[i] = (IMAGE_DATA_TYPE)begin[i];
	  if (imdiff_bkg->image[i]>max) max = imdiff_bkg->image[i];
	  if (imdiff_bkg->image[i]<min) min = imdiff_bkg->image[i];
	}
      }
      //      printf("ct = %ld,max = %d, min = %d\n",ct,max,min);
    }

    inline void set_background(std::size_t n,scitbx::af::flex_int data) {
      int* begin=data.begin();
      std::size_t size=data.size();
      std::size_t slow=data.accessor().focus()[0];
      std::size_t fast=data.accessor().focus()[1];
      DIFFIMAGE *im_bkg = &imdiff_bkg[n];
      if (im_bkg->image_length != size) {
	im_bkg->image_length = size;
	im_bkg->image = (IMAGE_DATA_TYPE *)realloc(im_bkg->image,im_bkg->image_length*sizeof(IMAGE_DATA_TYPE));
	im_bkg->correction = (float *)realloc(im_bkg->correction,im_bkg->image_length*sizeof(float));
      }
      im_bkg->hpixels = fast;
      im_bkg->vpixels = slow;
      im_bkg->value_offset = 0;
      std::size_t ct = 0;
      IMAGE_DATA_TYPE max=-32766,min=32766;
      for (int i = 0;i<im_bkg->image_length;i++) {
	if (begin[i]<0 || begin[i] > MAX_IMAGE_DATA_VALUE) {
	  im_bkg->image[i] = im_bkg->ignore_tag;
	  ct++;
	} else {
	  im_bkg->image[i] = (IMAGE_DATA_TYPE)begin[i];
	  if (im_bkg->image[i]>max) max = im_bkg->image[i];
	  if (im_bkg->image[i]<min) min = im_bkg->image[i];
	}
      }
      //      printf("ct = %ld,max = %d, min = %d\n",ct,max,min);
    }

    inline void LunusBkgsubim() {
      lbkgsubim(imdiff,imdiff_bkg);
      //      printf("imdiff->background_subtraction_factor = %f\n",imdiff->background_subtraction_factor);
      //      printf("imdiff-value_offset = %d\n",imdiff->value_offset);
    }

    inline scitbx::af::flex_int get_image() {
      std::size_t fast = imdiff->hpixels;
      std::size_t slow = imdiff->vpixels;
      scitbx::af::flex_int data(scitbx::af::flex_grid<>(slow,fast));
      int* begin=data.begin();
      std::size_t ct=0;
      for (int i = 0;i<imdiff->image_length;i++) {
	if (imdiff->image[i] == imdiff->ignore_tag) {
	  begin[i] = -1;
	  ct++;
	} else {
	  begin[i] = imdiff->image[i];
	}
      }
      return data;
    }

    inline std::size_t get_image_data_type_size() {
      return sizeof(IMAGE_DATA_TYPE);
    }

    inline void LunusSetparamslt(std::string deck) {
      deck += '\n';
      if (lat->params != NULL) free(lat->params);
      lat->params = (char *)calloc(deck.length()+1,sizeof(char));
      strcpy(lat->params,deck.c_str());
      lsetparamslt(lat);
    }

    inline void set_lattice(scitbx::af::flex_double data) {
      double* begin=data.begin();
      std::size_t size=data.size();
      std::size_t xvox=data.accessor().focus()[0];
      std::size_t yvox=data.accessor().focus()[1];
      std::size_t zvox=data.accessor().focus()[2];
      lat->lattice_length = size;
      lat->xvoxels = xvox;
      lat->yvoxels = yvox;
      lat->zvoxels = zvox;
      lat->lattice = (LATTICE_DATA_TYPE *)realloc(lat->lattice,lat->lattice_length*sizeof(LATTICE_DATA_TYPE));
      std::size_t ct=0;
      for (int i = 0;i<lat->lattice_length;i++) {
	if (begin[i]<0) {
	  lat->lattice[i] = lat->mask_tag;
	  ct++;
	} else {
	  lat->lattice[i] = (LATTICE_DATA_TYPE)begin[i];
	}
      }
    }

    inline scitbx::af::flex_double get_lattice() {
      std::size_t xvox = lat->xvoxels;
      std::size_t yvox = lat->yvoxels;
      std::size_t zvox = lat->zvoxels;
      scitbx::af::flex_double data(scitbx::af::flex_grid<>(xvox,yvox,zvox));
      double* begin=data.begin();
      std::size_t ct=0;
      for (int i = 0;i<lat->lattice_length;i++) {
	if (lat->lattice[i] == lat->mask_tag) {
	  begin[i] = -1;
	  ct++;
	} else {
	  begin[i] = lat->lattice[i];
	}
      }
      return data;
    }

    inline scitbx::af::flex_int get_counts() {
      std::size_t xvox = lat->xvoxels;
      std::size_t yvox = lat->yvoxels;
      std::size_t zvox = lat->zvoxels;
      scitbx::af::flex_int data(scitbx::af::flex_grid<>(xvox,yvox,zvox));
      int* begin=data.begin();
      std::size_t ct=0;
      for (int i = 0;i<lat->lattice_length;i++) {
	begin[i] = lat->latct[i];
	if (begin[i] == 0) {
	  ct++;
	}
      }
      return data;
    }

    inline void divide_by_counts() {
      for (std::size_t i = 0; i < lat->lattice_length; i++) {
	if (lat->latct[i] != 0) {
	  lat->lattice[i] /= (float)lat->latct[i];
	} else {
	  lat->lattice[i] = lat->mask_tag;
	}
	if (std::isnan(lat->lattice[i])) lat->lattice[i] = lat->mask_tag;
      }
    }

    inline void set_amatrix(scitbx::af::flex_double amatrix_in) {
      double* begin=amatrix_in.begin();
      for (int n = 0; n < imdiff->num_panels; n++) {
	DIFFIMAGE *im = &imdiff[n];
	im->amatrix.xx = (float)begin[0];
	im->amatrix.yx = (float)begin[1];
	im->amatrix.zx = (float)begin[2];
	im->amatrix.xy = (float)begin[3];
	im->amatrix.yy = (float)begin[4];
	im->amatrix.zy = (float)begin[5];
	im->amatrix.xz = (float)begin[6];
	im->amatrix.yz = (float)begin[7];
	im->amatrix.zz = (float)begin[8];
      /*
      imdiff->amatrix.xx = (float)begin[0];
      imdiff->amatrix.xy = (float)begin[1];
      imdiff->amatrix.xz = (float)begin[2];
      imdiff->amatrix.yx = (float)begin[3];
      imdiff->amatrix.yy = (float)begin[4];
      imdiff->amatrix.yz = (float)begin[5];
      imdiff->amatrix.zx = (float)begin[6];
      imdiff->amatrix.zy = (float)begin[7];
      imdiff->amatrix.zz = (float)begin[8];
      */    
      }
    }

    inline void set_xvectors(scitbx::af::flex_double xvectors_in) {
      double* begin=xvectors_in.begin();
      std::size_t size=xvectors_in.size();

      struct xyzcoords *xvectors_cctbx = (struct xyzcoords *)malloc(size*sizeof(struct xyzcoords));

      if (imdiff->slist == NULL) free(imdiff->slist);
      imdiff->slist = (struct xyzcoords *)malloc(size*sizeof(float));

      if (imdiff->image_length != size/3) {
	perror("set_xvectors: image length differs from size of xvectors. aborting.\n");
	exit(1);
      }


      float *xvectors_cctbx_float = (float *)xvectors_cctbx;

      for (std::size_t i = 0; i < size; i++) {
	xvectors_cctbx_float[i] = (float)begin[i];
      }

      // The xvectors for lunus should be transposed with respect to those coming from cctbx

      size_t index = 0;
      for (std::size_t j = 0; j < imdiff->vpixels; j++) {
	for (std::size_t i = 0; i < imdiff->hpixels; i++) {
	  imdiff->slist[index] = xvectors_cctbx[i*imdiff->vpixels + j];
	  index++;
	}
      }
    }

    inline void set_xvectors(std::size_t n,scitbx::af::flex_double xvectors_in) {
      double* begin=xvectors_in.begin();
      std::size_t size=xvectors_in.size();

      struct xyzcoords *xvectors_cctbx = (struct xyzcoords *)malloc(size*sizeof(struct xyzcoords));

      DIFFIMAGE *im = &imdiff[n];

      if (im->slist == NULL) free(im->slist);
      im->slist = (struct xyzcoords *)malloc(size*sizeof(float));

      if (im->image_length != size/3) {
	perror("set_xvectors: image length differs from size of xvectors. aborting.\n");
	exit(1);
      }


      float *xvectors_cctbx_float = (float *)xvectors_cctbx;

      for (std::size_t i = 0; i < size; i++) {
	xvectors_cctbx_float[i] = (float)begin[i];
      }

      // The xvectors for lunus should be transposed with respect to those coming from cctbx

      size_t index = 0;
      for (std::size_t j = 0; j < im->vpixels; j++) {
	for (std::size_t i = 0; i < im->hpixels; i++) {
	  im->slist[index] = xvectors_cctbx[i*im->vpixels + j];
	  index++;
	}
      }
    }

    inline void LunusProcimlt(int mode) {
      if (mode != 0 and mode != 1) {
	perror("LunusProcimlt: mode is not 0 or 1. Exiting.\n");
	exit(1);
      }
      lat->procmode = mode;
      lat->imdiff = imdiff;
      lprocimlt(lat);
    }

    inline void LunusProcimlt() {
      printf("LunusProcimlt: Processing using mode = %d\n",lat->procmode);
      lat->imdiff = imdiff;
      lprocimlt(lat);
    }

    inline void write_as_vtk(std::string fname) {
      FILE *f;
      if ((f = fopen(fname.c_str(),"wb")) == NULL) {
	perror("write_as_vtk: Can't open output file\n");
	exit(1);
      }
      lat->outfile = f;
      if (lwritevtk(lat) != 0) {
	perror("write_as_vtk: Couldn't write file\n");
	exit(1);
      }    
    }

    inline void write_as_hkl(std::string fname) {
      FILE *f;
      if ((f = fopen(fname.c_str(),"wb")) == NULL) {
	perror("write_as_hkl: Can't open output file\n");
	exit(1);
      }
      lat->outfile = f;
      if (lwritehkl(lat) != 0) {
	perror("write_as_hkl: Couldn't write file\n");
	exit(1);
      }    
    }

    inline void write_as_cube(std::string fname) {
      FILE *f;
      if ((f = fopen(fname.c_str(),"wb")) == NULL) {
	perror("write_as_cube: Can't open output file\n");
	exit(1);
      }
      lat->outfile = f;
      if (lwritecube(lat) != 0) {
	perror("write_as_cube: Couldn't write file\n");
	exit(1);
      }    
    }

    inline void write_as_lat(std::string fname) {
      FILE *f;
      if ((f = fopen(fname.c_str(),"wb")) == NULL) {
	perror("write_as_lat: Can't open output file\n");
	exit(1);
      }
      lat->outfile = f;
      if (lwritelt(lat) != 0) {
	perror("write_as_lat: Couldn't write file\n");
	exit(1);
      }    
    }

  };

  class LunusDIFFIMAGE {
  protected:
    DIFFIMAGE *imdiff, *imdiff_ref;

  public:
    inline  LunusDIFFIMAGE() {
      printf("in default XXX constr.\n");
      imdiff = linitim(1);
    }

    inline ~LunusDIFFIMAGE() {
      lfreeim(imdiff);
    }

    inline void LunusWindim(short cl, short rl, short cu, short ru) {
      printf("LunusWindim\n");
      imdiff->window_lower.c = cl;
      imdiff->window_lower.r = rl;
      imdiff->window_upper.c = cu;
      imdiff->window_upper.r = ru;
      lwindim(imdiff);
    }

    inline void LunusWindim() {
      printf("LunusWindim\n");
      lwindim(imdiff);
    }

    inline void LunusPunchim(short cl, short rl, short cu, short ru) {
      printf("LunusPunchim\n");
      imdiff->punchim_lower.c = cl;
      imdiff->punchim_lower.r = rl;
      imdiff->punchim_upper.c = cu;
      imdiff->punchim_upper.r = ru;
      lpunchim(imdiff);
    }

    inline void LunusPunchim() {
      printf("LunusPunchim\n");
      lpunchim(imdiff);
    }

    inline void LunusThrshim(int lower, int upper) {
      printf("LunusThrshim\n");
      imdiff->lower_threshold = (IMAGE_DATA_TYPE)lower;
      imdiff->upper_threshold = (IMAGE_DATA_TYPE)upper;
      lthrshim(imdiff);
    }

    inline void LunusThrshim() {
      printf("LunusThrshim\n");
      lthrshim(imdiff);
    }

    inline void LunusPolarim(float bx, float by, float dist, float polar, float offset, float px) {
      printf("LunusPolarim\n");
      imdiff->beam_mm.x = bx;
      imdiff->beam_mm.y = by;
      imdiff->distance_mm = dist;
      imdiff->polarization = polar;
      imdiff->polarization_offset = offset;
      imdiff->pixel_size_mm = px;
      lpolarim(imdiff);
    }

    inline void LunusNormim(float bx, float by, float dist, float cx, float cy, float px) {
      printf("LunusNormim\n");
      imdiff->beam_mm.x = bx;
      imdiff->beam_mm.y = by;
      imdiff->distance_mm = dist;
      imdiff->cassette.x = cx;
      imdiff->cassette.y = cy;
      imdiff->pixel_size_mm = px;
      lnormim(imdiff);
    }

    inline void LunusModeim(std::size_t w) {
      imdiff->mode_width = w;
      lmodeim(imdiff);
    }

    inline void LunusModeim() {
      lmodeim(imdiff);
    }

    inline scitbx::af::flex_double LunusScaleim(float bx, float by, float px, int inner_radius, int outer_radius) {
      imdiff_ref->mask_inner_radius = inner_radius;
      imdiff_ref->mask_outer_radius = outer_radius;
      imdiff_ref->beam_mm.x = bx;
      imdiff_ref->beam_mm.y = by;
      imdiff_ref->pixel_size_mm = px;
      imdiff->mask_inner_radius = inner_radius;
      imdiff->mask_outer_radius = outer_radius;
      imdiff->beam_mm.x = bx;
      imdiff->beam_mm.y = by;
      imdiff->pixel_size_mm = px;
      scitbx::af::flex_double scale_info(2);
      double* begin=scale_info.begin();
      printf("LunusScaleim\n");
      if (imdiff_ref == NULL) {
	printf("LunusScaleim: imdiff_ref not defined. Need to define reference using set_reference()\n");
	exit(1);
      }
      if (imdiff_ref->image_length != imdiff->image_length) {
	printf("LunusScaleim: imdiff_ref image is different length than imdiff. Exiting.\n");
	exit(1);
      }
      /*
      lscaleim(imdiff,imdiff_ref);
      begin[0] = imdiff->rfile[0];
      begin[1] = imdiff->rfile[1];
      */
      lscaleim(imdiff_ref,imdiff);
      begin[0] = imdiff_ref->rfile[0];
      begin[1] = imdiff_ref->rfile[1];
      //      printf("Scale, Error = %f, %f\n",imdiff->rfile[0],imdiff->rfile[1]);
      return scale_info;
    }
	
    inline void LunusSetparamsim(std::string deck) {
      deck += '\n';
      if (imdiff->params != NULL) free(imdiff->params);
      imdiff->params = (char *)calloc(deck.length()+1,sizeof(char));
      strcpy(imdiff->params,deck.c_str());
      lsetparamsim(imdiff);
    }

    inline void set_image(scitbx::af::flex_int data) {
      int* begin=data.begin();
      std::size_t size=data.size();
      std::size_t slow=data.accessor().focus()[0];
      std::size_t fast=data.accessor().focus()[1];
      imdiff->image_length = size;
      imdiff->hpixels = fast;
      imdiff->vpixels = slow;
      imdiff->image = (IMAGE_DATA_TYPE *)realloc(imdiff->image,imdiff->image_length*sizeof(IMAGE_DATA_TYPE));
      imdiff->value_offset = 0;
      std::size_t ct=0;
      for (int i = 0;i<imdiff->image_length;i++) {
	if (begin[i]<0) {
	  imdiff->image[i] = imdiff->ignore_tag;
	  ct++;
	} else {
	  imdiff->image[i] = (IMAGE_DATA_TYPE)begin[i];
	}
      }
    }

    inline void set_reference() {
      //      if (imdiff_ref != NULL) {
      //	lfreeim(imdiff_ref);
      //      }
      imdiff_ref = linitim(1);
      imdiff_ref->image_length = imdiff->image_length;
      imdiff_ref->hpixels = imdiff->hpixels;
      imdiff_ref->vpixels = imdiff->vpixels;
      imdiff_ref->pixel_size_mm = imdiff->pixel_size_mm;
      imdiff_ref->beam_mm.x = imdiff->beam_mm.x;
      imdiff_ref->beam_mm.y = imdiff->beam_mm.y;
      imdiff_ref->image = (IMAGE_DATA_TYPE *)realloc(imdiff_ref->image,imdiff_ref->image_length*sizeof(IMAGE_DATA_TYPE));
      memcpy((void *)imdiff_ref->image,(void *)imdiff->image,imdiff->image_length*sizeof(IMAGE_DATA_TYPE));
      int ct=0;
      for (int i = 0;i<imdiff_ref->image_length;i++) {
	if (imdiff_ref->image[i] == imdiff_ref->ignore_tag) {
	  ct++;
	}
      }
      printf("Number of ignored pixels = %d\n",ct);
    }


    inline scitbx::af::flex_double LunusRadialAvgim(float bx, float by, float px) {
      imdiff->beam_mm.x = bx;
      imdiff->beam_mm.y = by;
      imdiff->pixel_size_mm = px;
      lavgrim(imdiff);
      std::size_t rs = imdiff->rfile_length;
      scitbx::af::flex_double data(rs);
      double* begin=data.begin();
      std::size_t ct=0;
      imdiff->rfile = (RFILE_DATA_TYPE *)realloc(imdiff->rfile,imdiff->rfile_length*sizeof(RFILE_DATA_TYPE));
      for (int i = 0;i<imdiff->rfile_length;i++) {
	if (imdiff->rfile[i] == imdiff->ignore_tag) {
	  begin[i] = -1;
	  ct++;
	} else {
	  begin[i] = imdiff->rfile[i];
	}
      }
      printf("Calculated radial average for %ld bins, with %ld empty bins.\n",rs,ct);
      return data;
    }

    inline scitbx::af::flex_int get_image() {
      std::size_t fast = imdiff->hpixels;
      std::size_t slow = imdiff->vpixels;
      scitbx::af::flex_int data(scitbx::af::flex_grid<>(slow,fast));
      int* begin=data.begin();
      std::size_t ct=0;
      for (int i = 0;i<imdiff->image_length;i++) {
	if (imdiff->image[i] == imdiff->ignore_tag) {
	  begin[i] = -1;
	  ct++;
	} else {
	  begin[i] = imdiff->image[i];
	}
      }
      return data;
    }

    inline std::size_t get_image_data_type_size() {
      return sizeof(IMAGE_DATA_TYPE);
    }

  };

  class LunusLAT3D {
  protected:
    LAT3D *lat;

  public:
    inline  LunusLAT3D() {
      printf("in default XXX constr.\n");
      lat = linitlt();
    }

    inline ~LunusLAT3D() {
      lfreelt(lat);
    }

    inline void LunusSetparamslt(std::string deck) {
      deck += '\n';
      if (lat->params != NULL) free(lat->params);
      lat->params = (char *)calloc(deck.length()+1,sizeof(char));
      strcpy(lat->params,deck.c_str());
      lsetparamslt(lat);
    }

    inline void LunusReadlt(char f[]) {
      printf("Reading in file:\n%s\n",f);
      lat->filename = f;
      lat->infile = fopen(f,"rb");
      lreadlt(lat);
      fclose(lat->infile);
    }

    inline void LunusReadvtk(char f[]) {
      printf("Reading in file:\n%s\n",f);
      lat->filename = f;
      lat->infile = fopen(f,"rb");
      lreadvtk(lat);
      fclose(lat->infile);
    }

    inline void LunusWritelt(char f[]) {
      printf("Writing lattice to file:\n%s\n",f);
      lat->filename = f;
      lat->outfile = fopen(f,"wb");
      lwritelt(lat);
      fclose(lat->outfile);
    }

    inline void LunusWritevtk(char f[]) {
      printf("Writing lattice to file:\n%s\n",f);
      lat->filename = f;
      lat->outfile = fopen(f,"wb");
      lwritevtk(lat);
      fclose(lat->outfile);
    }

    inline void LunusWritehkl(char f[]) {
      printf("Writing lattice to file:\n%s\n",f);
      lat->filename = f;
      lat->outfile = fopen(f,"wb");
      lwritehkl(lat);
      fclose(lat->outfile);
    }

    inline void LunusSymlt(int sym) {
      printf("LunusSymlt\n");
      lat->symop_index = sym;
      lsymlt(lat);
    }

    inline void LunusAnisolt(float a, float b, float c, float alpha, float beta, float gamma) {
      printf("LunusAnisolt\n");
      lat->cell.a = a;
      lat->cell.b = b;
      lat->cell.c = c;
      lat->cell.alpha = alpha;
      lat->cell.beta = beta;
      lat->cell.gamma = gamma;
      lparsecelllt(lat);
      lanisolt(lat);
    }

    inline void set_lattice(scitbx::af::flex_int data) {
      int* begin=data.begin();
      std::size_t size=data.size();
      std::size_t xvox=data.accessor().focus()[0];
      std::size_t yvox=data.accessor().focus()[1];
      std::size_t zvox=data.accessor().focus()[2];
      lat->lattice_length = size;
      lat->xvoxels = xvox;
      lat->yvoxels = yvox;
      lat->zvoxels = zvox;
      lat->lattice = (LATTICE_DATA_TYPE *)realloc(lat->lattice,lat->lattice_length*sizeof(LATTICE_DATA_TYPE));
      std::size_t ct=0;
      for (int i = 0;i<lat->lattice_length;i++) {
	if (begin[i]<0) {
	  lat->lattice[i] = lat->mask_tag;
	  ct++;
	} else {
	  lat->lattice[i] = (LATTICE_DATA_TYPE)begin[i];
	}
      }
    }

    inline scitbx::af::flex_int get_lattice() {
      std::size_t xvox = lat->xvoxels;
      std::size_t yvox = lat->yvoxels;
      std::size_t zvox = lat->zvoxels;
      scitbx::af::flex_int data(scitbx::af::flex_grid<>(xvox,yvox,zvox));
      int* begin=data.begin();
      std::size_t ct=0;
      std::size_t latidx;
      std::size_t idx = 0;
      for (int i = 0;i<lat->xvoxels;i++) {
	for (int j = 0;j<lat->yvoxels;j++) {
	  for (int k = 0;k<lat->zvoxels;k++) {
	    latidx = k*lat->xyvoxels+j*lat->xvoxels+i;
	    if (lat->lattice[latidx] == lat->mask_tag) {
	      begin[idx] = lat->mask_tag;
	      ct++;
	    } else {
	      begin[idx] = lat->lattice[latidx];
	    }
	    idx++;
	  }
	}
      }
      return data;
    }

    inline std::size_t get_lattice_data_type_size() {
      return sizeof(LATTICE_DATA_TYPE);
    }
  };

}

using namespace boost::python;
namespace lunus{
namespace boost_python { namespace {

  void
  lunus_init_module() {
    using namespace boost::python;
    typedef return_value_policy<return_by_value> rbv;
    typedef default_call_policies dcp;

    void (lunus::Process::*LunusProcimlt1)() = &lunus::Process::LunusProcimlt;
    void (lunus::Process::*LunusProcimlt2)(int) = &lunus::Process::LunusProcimlt;
    void (lunus::Process::*LunusSetparamsim1)(std::string deck) = &lunus::Process::LunusSetparamsim;
    void (lunus::Process::*LunusSetparamsim2)(std::size_t n,std::string deck) = &lunus::Process::LunusSetparamsim;

    void (lunus::Process::*set_image1)(scitbx::af::flex_int data) = &lunus::Process::set_image;
    void (lunus::Process::*set_image2)(std::size_t n,scitbx::af::flex_int data) = &lunus::Process::set_image;

    void (lunus::Process::*set_xvectors1)(scitbx::af::flex_double xvectors_in) = &lunus::Process::set_xvectors;
    void (lunus::Process::*set_xvectors2)(std::size_t n,scitbx::af::flex_double xvectors_in) = &lunus::Process::set_xvectors;

    void (lunus::Process::*set_background1)(scitbx::af::flex_int data) = &lunus::Process::set_background;
    void (lunus::Process::*set_background2)(std::size_t n,scitbx::af::flex_int data) = &lunus::Process::set_background;

    class_<lunus::Process>("Process",init<>())
      .def(init<std::size_t>())
      .def("LunusSetmetim",&lunus::Process::LunusSetmetim)
      .def("LunusSetparamsim",LunusSetparamsim1)
      .def("LunusSetparamsim",LunusSetparamsim2)
      .def("LunusBkgsubim",&lunus::Process::LunusBkgsubim)
      .def("set_image",set_image1)
      .def("set_image",set_image2)
      .def("set_background",set_background1)
      .def("set_background",set_background2)
      .def("get_image",&lunus::Process::get_image)
      .def("get_lattice",&lunus::Process::get_lattice)
      .def("get_counts",&lunus::Process::get_counts)
      .def("divide_by_counts",&lunus::Process::divide_by_counts)
      .def("print_image_params",&lunus::Process::print_image_params)
      .def("write_as_vtk",&lunus::Process::write_as_vtk)
      .def("write_as_hkl",&lunus::Process::write_as_hkl)
      .def("write_as_cube",&lunus::Process::write_as_cube)
      .def("write_as_lat",&lunus::Process::write_as_lat)
      .def("set_lattice",&lunus::Process::set_lattice)
      .def("LunusSetparamslt",&lunus::Process::LunusSetparamslt)
      .def("set_amatrix",&lunus::Process::set_amatrix)
      .def("set_xvectors",set_xvectors1)
      .def("set_xvectors",set_xvectors2)
      .def("set_image_ref",&lunus::Process::set_image_ref)
      .def("LunusProcimlt",LunusProcimlt1)
      .def("LunusProcimlt",LunusProcimlt2)
    ;

    void (lunus::LunusDIFFIMAGE::*LunusWindim1)() = &lunus::LunusDIFFIMAGE::LunusWindim;
    void (lunus::LunusDIFFIMAGE::*LunusWindim2)(short, short, short, short) = &lunus::LunusDIFFIMAGE::LunusWindim;

    void (lunus::LunusDIFFIMAGE::*LunusPunchim1)() = &lunus::LunusDIFFIMAGE::LunusPunchim;
    void (lunus::LunusDIFFIMAGE::*LunusPunchim2)(short, short, short, short) = &lunus::LunusDIFFIMAGE::LunusPunchim;

    void (lunus::LunusDIFFIMAGE::*LunusThrshim1)() = &lunus::LunusDIFFIMAGE::LunusThrshim;
    void (lunus::LunusDIFFIMAGE::*LunusThrshim2)(int, int) = &lunus::LunusDIFFIMAGE::LunusThrshim;

    void (lunus::LunusDIFFIMAGE::*LunusModeim1)() = &lunus::LunusDIFFIMAGE::LunusModeim;
    void (lunus::LunusDIFFIMAGE::*LunusModeim2)(size_t) = &lunus::LunusDIFFIMAGE::LunusModeim;

    class_<lunus::LunusDIFFIMAGE>("LunusDIFFIMAGE",init<>())
      .def("LunusSetparamsim",&lunus::LunusDIFFIMAGE::LunusSetparamsim)
      .def("get_image_data_type_size",&lunus::LunusDIFFIMAGE::get_image_data_type_size)
      .def("set_image",&lunus::LunusDIFFIMAGE::set_image)
      .def("set_reference",&lunus::LunusDIFFIMAGE::set_reference)
      .def("get_image",&lunus::LunusDIFFIMAGE::get_image)
      .def("LunusPunchim",LunusPunchim1)
      .def("LunusPunchim",LunusPunchim2)
      .def("LunusWindim",LunusWindim1)
      .def("LunusWindim",LunusWindim2)
      .def("LunusThrshim",LunusThrshim1)
      .def("LunusThrshim",LunusThrshim2)
      .def("LunusPolarim",&lunus::LunusDIFFIMAGE::LunusPolarim)
      .def("LunusNormim",&lunus::LunusDIFFIMAGE::LunusNormim)
      .def("LunusModeim",LunusModeim1)
      .def("LunusModeim",LunusModeim2)
      .def("LunusRadialAvgim",&lunus::LunusDIFFIMAGE::LunusRadialAvgim)
      .def("LunusScaleim",&lunus::LunusDIFFIMAGE::LunusScaleim)
    ;

    class_<lunus::LunusLAT3D>("LunusLAT3D",init<>())
      .def("get_lattice_data_type_size",&lunus::LunusLAT3D::get_lattice_data_type_size)
      .def("get_lattice",&lunus::LunusLAT3D::get_lattice)
      .def("set_lattice",&lunus::LunusLAT3D::set_lattice)
      .def("LunusReadlt",&lunus::LunusLAT3D::LunusReadlt)
      .def("LunusReadvtk",&lunus::LunusLAT3D::LunusReadvtk)
      .def("LunusWritelt",&lunus::LunusLAT3D::LunusWritelt)
      .def("LunusWritevtk",&lunus::LunusLAT3D::LunusWritevtk)
      .def("LunusWritehkl",&lunus::LunusLAT3D::LunusWritehkl)
      .def("LunusSymlt",&lunus::LunusLAT3D::LunusSymlt)
      .def("LunusAnisolt",&lunus::LunusLAT3D::LunusAnisolt)
      .def("LunusSetparamslt",&lunus::LunusLAT3D::LunusSetparamslt)
    ;

    def("foo2",&lunus::foo2);
  }

}
}} // namespace lunus::boost_python::<anonymous>

//
BOOST_PYTHON_MODULE(lunus_ext)
{
  lunus::boost_python::lunus_init_module();
}
