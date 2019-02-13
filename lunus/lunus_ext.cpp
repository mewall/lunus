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

    inline void LunusPunchim(short cl, short rl, short cu, short ru) {
      printf("LunusPunchim\n");
      imdiff->punchim_lower.c = cl;
      imdiff->punchim_lower.r = rl;
      imdiff->punchim_upper.c = cu;
      imdiff->punchim_upper.r = ru;
      lpunchim(imdiff);
    }

    inline void LunusThrshim(int lower, int upper) {
      printf("LunusThrshim\n");
      imdiff->lower_threshold = (IMAGE_DATA_TYPE)lower;
      imdiff->upper_threshold = (IMAGE_DATA_TYPE)upper;
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
      printf("Converted image size %ld,%ld with %ld negative pixel values.\n",fast,slow,ct);
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
      printf("Converted image size %ld,%ld with %ld negative pixel values.\n",fast,slow,ct);
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
      printf("Converted lattice of size (%ld,%ld,%ld) with %ld negative pixel values.\n",xvox,yvox,zvox,ct);
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
      printf("Converted lattice of size (%ld,%ld,%ld) with %ld negative pixel values.\n",xvox,yvox,zvox,ct);
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

    class_<lunus::LunusDIFFIMAGE>("LunusDIFFIMAGE",init<>())
      .def("get_image_data_type_size",&lunus::LunusDIFFIMAGE::get_image_data_type_size)
      .def("set_image",&lunus::LunusDIFFIMAGE::set_image)
      .def("set_reference",&lunus::LunusDIFFIMAGE::set_reference)
      .def("get_image",&lunus::LunusDIFFIMAGE::get_image)
      .def("LunusPunchim",&lunus::LunusDIFFIMAGE::LunusPunchim)
      .def("LunusWindim",&lunus::LunusDIFFIMAGE::LunusWindim)
      .def("LunusThrshim",&lunus::LunusDIFFIMAGE::LunusThrshim)
      .def("LunusPolarim",&lunus::LunusDIFFIMAGE::LunusPolarim)
      .def("LunusNormim",&lunus::LunusDIFFIMAGE::LunusNormim)
      .def("LunusModeim",&lunus::LunusDIFFIMAGE::LunusModeim)
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
    ;

    def("foo2",&lunus::foo2);
  }

}
}} // namespace lunus::boost_python::<anonymous>

BOOST_PYTHON_MODULE(lunus_ext)
{
  lunus::boost_python::lunus_init_module();
}
