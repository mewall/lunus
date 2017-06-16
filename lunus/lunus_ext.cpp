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
    DIFFIMAGE *imdiff;

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

    inline void LunusModeim(std::size_t w) {
      imdiff->mode_width = w;
      lmodeim(imdiff);
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
      .def("get_image",&lunus::LunusDIFFIMAGE::get_image)
      .def("LunusPunchim",&lunus::LunusDIFFIMAGE::LunusPunchim)
      .def("LunusWindim",&lunus::LunusDIFFIMAGE::LunusWindim)
      .def("LunusThrshim",&lunus::LunusDIFFIMAGE::LunusThrshim)
      .def("LunusModeim",&lunus::LunusDIFFIMAGE::LunusModeim)
    ;

    def("foo2",&lunus::foo2);
  }

}
}} // namespace lunus::boost_python::<anonymous>

BOOST_PYTHON_MODULE(lunus_ext)
{
  lunus::boost_python::lunus_init_module();
}
