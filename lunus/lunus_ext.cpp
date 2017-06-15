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

  class LunusIMAd {
  protected:
    DIFFIMAGE *imdiff;

  public:
    inline  LunusIMAd() {
      printf("in default XXX constr.\n");
      imdiff = linitim();
    }

    inline ~LunusIMAd() {
      lfreeim(imdiff);
    }

    inline void LunusModeim() {
      printf("LunusModeim\n");
      lmodeim(imdiff);
    }

    inline void LunusWindim() {
      printf("LunusWindim\n");
      lwindim(imdiff);
    }

    inline void LunusPunchim() {
      printf("LunusPunchim\n");
      lpunchim(imdiff);
    }

    inline void LunusThrshim() {
      printf("LunusThrshim\n");
      lthrshim(imdiff);
    }

    inline void set_raw_data(scitbx::af::flex_int data) {
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

    inline scitbx::af::flex_int get_lunus_data() {
      std::size_t fast = imdiff->hpixels;
      std::size_t slow = imdiff->vpixels;
      scitbx::af::flex_int data(scitbx::af::flex_grid<>(slow,fast));
      int* begin=data.begin();
      std::size_t size=data.size();
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

    inline void set_windim_lower(short c, short r) {
      imdiff->window_lower.c = c;
      imdiff->window_lower.r = r;
    }

    inline void set_windim_upper(short c, short r) {
      imdiff->window_upper.c = c;
      imdiff->window_upper.r = r;
    }

    inline void set_punchim_lower(short c, short r) {
      imdiff->punchim_lower.c = c;
      imdiff->punchim_lower.r = r;
    }

    inline void set_punchim_upper(short c, short r) {
      imdiff->punchim_upper.c = c;
      imdiff->punchim_upper.r = r;
    }

    inline void set_modeim_width(std::size_t w) {
      imdiff->mode_width = w;
    }

    inline int get_modeim_width() {
      //      return std::reinterpret_cast<int>(imdiff->mode_width);
      return (int)(imdiff->mode_width);
    }

    inline void set_thresholds(int lower, int upper) {
      imdiff->lower_threshold = (IMAGE_DATA_TYPE)lower;
      imdiff->upper_threshold = (IMAGE_DATA_TYPE)upper;
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

    class_<lunus::LunusIMAd>("LunusIMAd",init<>())
      .def("get_image_data_type_size",&lunus::LunusIMAd::get_image_data_type_size)
      .def("set_punchim_lower",&lunus::LunusIMAd::set_punchim_lower)
      .def("set_punchim_upper",&lunus::LunusIMAd::set_punchim_upper)
      .def("set_windim_lower",&lunus::LunusIMAd::set_windim_lower)
      .def("set_windim_upper",&lunus::LunusIMAd::set_windim_upper)
      .def("set_thresholds",&lunus::LunusIMAd::set_thresholds)
      .def("set_modeim_width",&lunus::LunusIMAd::set_modeim_width)
      .def("get_modeim_width",&lunus::LunusIMAd::get_modeim_width)
      .def("set_raw_data",&lunus::LunusIMAd::set_raw_data)
      .def("get_lunus_data",&lunus::LunusIMAd::get_lunus_data)
      .def("LunusPunchim",&lunus::LunusIMAd::LunusPunchim)
      .def("LunusWindim",&lunus::LunusIMAd::LunusWindim)
      .def("LunusThrshim",&lunus::LunusIMAd::LunusThrshim)
      .def("LunusModeim",&lunus::LunusIMAd::LunusModeim)
    ;

    def("foo2",&lunus::foo2);
  }

}
}} // namespace lunus::boost_python::<anonymous>

BOOST_PYTHON_MODULE(lunus_ext)
{
  lunus::boost_python::lunus_init_module();
}
