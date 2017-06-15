#include <cctbx/boost_python/flex_fwd.h>
#include <boost/python/module.hpp>
#include <boost/python/class.hpp>
#include <boost/python/def.hpp>
#include <scitbx/array_family/flex_types.h>
#include <scitbx/array_family/shared.h>

#include <c/include/mwmask.h>

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
    inline LunusIMAd() {
      printf("in default constr.");
      imdiff = linitim();
    }

    inline ~LunusIMAd() {
      lfreeim(imdiff);
    }

    /*
    inline IMAGE_DATA_TYPE set_image(void *buf,) {
      return imdiff->image;
    }
    */
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

    inline void set_modeim_width(size_t w) {
      imdiff->mode_width = w;
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
    ;
    
    def("foo2",&lunus::foo2);
  }

}
}} // namespace lunus::boost_python::<anonymous>

BOOST_PYTHON_MODULE(lunus_ext)
{
  lunus::boost_python::lunus_init_module();
}
