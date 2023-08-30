/* File : example.i */
%module(directors="4") xapp_sdk
%include "std_string.i"
%include "std_vector.i"
%include "carrays.i"
%include <typemaps.i>

%{
  #include "swig_wrapper.h"

#ifdef E2AP_V1
  #include "../../lib/e2ap/v1_01/e2ap_types/common/e2ap_global_node_id.h"
  #include "../../lib/e2ap/v1_01/e2ap_types/common/e2ap_plmn.h"
  #include "../../lib/e2ap/v1_01/e2ap_types/common/e2ap_ran_function.h"
#elif E2AP_V2
  #include "../../lib/e2ap/v2_03/e2ap_types/common/e2ap_global_node_id.h"
  #include "../../lib/e2ap/v2_03/e2ap_types/common/e2ap_plmn.h"
  #include "../../lib/e2ap/v2_03/e2ap_types/common/e2ap_ran_function.h"
#elif E2AP_V3
  #include "../../lib/e2ap/v3_01/e2ap_types/common/e2ap_global_node_id.h"
  #include "../../lib/e2ap/v3_01/e2ap_types/common/e2ap_plmn.h"
  #include "../../lib/e2ap/v3_01/e2ap_types/common/e2ap_ran_function.h"
#endif

  #include "../../util/byte_array.h"
  #include "../../util/e2ap_ngran_types.h"

  #include "../../sm/mac_sm/ie/mac_data_ie.h"
  #include "../../sm/rlc_sm/ie/rlc_data_ie.h"
  #include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"
  #include "../../sm/slice_sm/ie/slice_data_ie.h"
  #include "../../sm/gtp_sm/ie/gtp_data_ie.h"
%}

#ifdef SWIGPYTHON
// convert list in python to std::vector<std::string>& in c++
%typemap(in) std::vector<std::string>& (std::vector<std::string> temp) {
  // Check if the input object is a list
  if (PyList_Check($input)) {
    int size = PyList_Size($input);
    temp.resize(size);
    for (int i = 0; i < size; ++i) {
      PyObject* item = PyList_GetItem($input, i);
      if (!PyUnicode_Check(item)) {
        PyErr_SetString(PyExc_TypeError, "Expected a list of strings");
        return NULL;
      }
      const char* str = PyUnicode_AsUTF8(item);
      temp[i] = str;
    }
    $1 = &temp;
  } else {
    PyErr_SetString(PyExc_TypeError, "Expected a list of strings");
    return NULL;
  }
}

/* uintXX_t mapping: Python -> C */
%typemap(in) uint8_t {
    $1 = (uint8_t) PyInt_AsLong($input);
}
%typemap(in) uint16_t {
    $1 = (uint16_t) PyInt_AsLong($input);
}
%typemap(in) uint32_t {
    $1 = (uint32_t) PyInt_AsLong($input);
}
%typemap(in) uint64_t {
    $1 = (uint64_t) PyInt_AsLong($input);
}

/* intXX_t mapping: Python -> C */
%typemap(in) int8_t {
    $1 = (int8_t) PyInt_AsLong($input);
}
%typemap(in) int16_t {
    $1 = (int16_t) PyInt_AsLong($input);
}
%typemap(in) int32_t {
    $1 = (int32_t) PyInt_AsLong($input);
}
%typemap(in) int64_t {
    $1 = (int64_t) PyInt_AsLong($input);
}

/* uintXX_t mapping: C -> Python */
%typemap(out) uint8_t {
    $result = PyInt_FromLong((long) $1);
}
%typemap(out) uint16_t {
    $result = PyInt_FromLong((long) $1);
}
%typemap(out) uint32_t {
    $result = PyInt_FromLong((long) $1);
}
%typemap(out) uint64_t {
    $result = PyInt_FromLong((long) $1);
}

/* intXX_t mapping: C -> Python */
%typemap(out) int8_t {
    $result = PyInt_FromLong((long) $1);
}
%typemap(out) int16_t {
    $result = PyInt_FromLong((long) $1);
}
%typemap(out) int32_t {
    $result = PyInt_FromLong((long) $1);
}
%typemap(out) int64_t {
    $result = PyInt_FromLong((long) $1);
}

#endif

#ifdef SWIGGO

%include "stdint.i"

%insert(cgo_comment_typedefs) %{
#cgo LDFLAGS: -L. -lxapp_sdk -Wl,-rpath=${SRCDIR}
%}

%insert(go_wrapper) %{
// Converts a slice of strings to a StringVector object
func SlToStrVec(slice []string) (_swig_ret StringVector) {
    vec := NewStringVector()
    for _, str := range slice {
        vec.Add(str)
    }
    return vec
}
%}

%typemap(in) uint8_t {
    $1 = (long)$input;
}
%typemap(in) uint16_t {
    $1 = (long)$input;
}
%typemap(in) uint32_t {
    $1 = (long)$input;
}
%typemap(in) uint64_t {
    $1 = (long)$input;
}

/* intXX_t mapping: GO -> C */
%typemap(in) int8_t {
    $1 = (long)$input;
}
%typemap(in) int16_t {
    $1 = (long)$input;
}
%typemap(in) int32_t {
    $1 = (long)$input;
}
%typemap(in) int64_t {
    $1 = (long)$input;
}

/* uintXX_t mapping: C -> GO */
%typemap(out) uint8_t {
    $result = (long) $1;
}
%typemap(out) uint16_t {
    $result = (long) $1;
}
%typemap(out) uint32_t {
    $result = (long) $1;
}
%typemap(out) uint64_t {
    $result = (long) $1;
}

/* intXX_t mapping: C -> GO */
%typemap(out) int8_t {
    $result = (long) $1;
}
%typemap(out) int16_t {
    $result = (long) $1;
}
%typemap(out) int32_t {
    $result = (long) $1;
}
%typemap(out) int64_t {
    $result = (long) $1;
}

#endif

%feature("director") mac_cb;
%feature("director") rlc_cb;
%feature("director") pdcp_cb;
%feature("director") slice_cb;
%feature("director") gtp_cb;

namespace std {
  %template(IntVector) vector<int>;
  %template(E2NodeVector) vector<E2Node>;
  %template(RANVector) vector<RanFunction>;
  %template(MACStatsVector) vector<mac_ue_stats_impl_t>;
  %template(RLC_RBStatsVector) vector<rlc_radio_bearer_stats_t>;
  %template(PDCP_RBStatsVector) vector<pdcp_radio_bearer_stats_t>;
  %template(StringVector) vector<std::string>;
  %template(SLICE_slicesStatsVector) vector<swig_fr_slice_t>;
  %template(SLICE_UEsStatsVector) vector<ue_slice_assoc_t>;
  %template(GTP_NGUTStatsVector) vector<gtp_ngu_t_stats_t>;
}


%array_class(ue_slice_assoc_t, ue_slice_assoc_array);
%array_class(fr_slice_t, slice_array);
%array_class(uint32_t, del_dl_array);
%array_class(uint32_t, del_ul_array);

%include "swig_wrapper.h"
%include "../../util/byte_array.h"
%include "../../util/e2ap_ngran_types.h"

%include "../../lib/e2ap/e2ap_global_node_id_wrapper.h"
%include "../../lib/e2ap/e2ap_plmn_wrapper.h"
%include "../../lib/e2ap/e2ap_ran_function_wrapper.h"


#ifdef E2AP_V1
  %include "../../lib/e2ap/v1_01/e2ap_types/common/e2ap_global_node_id.h"
  %include "../../lib/e2ap/v1_01/e2ap_types/common/e2ap_plmn.h"
  %include "../../lib/e2ap/v1_01/e2ap_types/common/e2ap_ran_function.h"
#elif defined E2AP_V2
  %include "../../lib/e2ap/v2_03/e2ap_types/common/e2ap_global_node_id.h"
  %include "../../lib/e2ap/v2_03/e2ap_types/common/e2ap_plmn.h"
  %include "../../lib/e2ap/v2_03/e2ap_types/common/e2ap_ran_function.h"
#elif defined E2AP_V3
  %include "../../lib/e2ap/v3_01/e2ap_types/common/e2ap_global_node_id.h"
  %include "../../lib/e2ap/v3_01/e2ap_types/common/e2ap_plmn.h"
  %include "../../lib/e2ap/v3_01/e2ap_types/common/e2ap_ran_function.h"
#endif

%include "../../sm/mac_sm/ie/mac_data_ie.h"
%include "../../sm/rlc_sm/ie/rlc_data_ie.h"
%include "../../sm/pdcp_sm/ie/pdcp_data_ie.h"
%include "../../sm/slice_sm/ie/slice_data_ie.h"
%include "../../sm/gtp_sm/ie/gtp_data_ie.h"

