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
    DIFFIMAGE *imdiff, *imdiff_ref;
    LAT3D *lat;

  public:
    inline Process() {
      imdiff = linitim();
      imdiff_ref = linitim();
      lat = linitlt();
    }

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
    }

    inline void set_imdiff_ref() {
      lcloneim(imdiff_ref,imdiff);
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
      imdiff->correction = (float *)realloc(imdiff->correction,imdiff->image_length*sizeof(float));
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
	  if (isnan(lat->lattice[i])) lat->lattice[i] = lat->mask_tag;
	} else {
	  lat->lattice[i] = lat->mask_tag;
	}
      }
    }

    inline void set_amatrix(scitbx::af::flex_double amatrix_in) {
      double* begin=amatrix_in.begin();
      imdiff->amatrix.xx = (float)begin[0];
      imdiff->amatrix.yx = (float)begin[1];
      imdiff->amatrix.zx = (float)begin[2];
      imdiff->amatrix.xy = (float)begin[3];
      imdiff->amatrix.yy = (float)begin[4];
      imdiff->amatrix.zy = (float)begin[5];
      imdiff->amatrix.xz = (float)begin[6];
      imdiff->amatrix.yz = (float)begin[7];
      imdiff->amatrix.zz = (float)begin[8];
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
      */    }

    inline void set_xvectors(scitbx::af::flex_double xvectors_in) {
      double* begin=xvectors_in.begin();
      std::size_t size=xvectors_in.size();

      struct xyzcoords *xvectors_cctbx = (struct xyzcoords *)malloc(size*sizeof(struct xyzcoords));

      if (imdiff->xvectors == NULL) free(imdiff->xvectors);
      imdiff->xvectors = (struct xyzcoords *)malloc(size*sizeof(float));

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
	  imdiff->xvectors[index] = xvectors_cctbx[i*imdiff->vpixels + j];
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
  };

  class LunusDIFFIMAGE {
  protected:
    DIFFIMAGE *imdiff, *imdiff_ref;

  public:
    inline  LunusDIFFIMAGE() {
      printf("in default XXX constr.\n");
      imdiff = linitim();
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
      imdiff_ref = linitim();
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

    class_<lunus::Process>("Process",init<>())
      .def("LunusSetparamsim",&lunus::Process::LunusSetparamsim)
      .def("set_image",&lunus::Process::set_image)
      .def("get_image",&lunus::Process::get_image)
      .def("get_lattice",&lunus::Process::get_lattice)
      .def("get_counts",&lunus::Process::get_counts)
      .def("divide_by_counts",&lunus::Process::divide_by_counts)
      .def("write_as_vtk",&lunus::Process::write_as_vtk)
      .def("set_lattice",&lunus::Process::set_lattice)
      .def("LunusSetparamslt",&lunus::Process::LunusSetparamslt)
      .def("set_amatrix",&lunus::Process::set_amatrix)
      .def("set_xvectors",&lunus::Process::set_xvectors)
      .def("set_imdiff_ref",&lunus::Process::set_imdiff_ref)
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

BOOST_PYTHON_MODULE(lunus_ext)
{
  lunus::boost_python::lunus_init_module();
}
