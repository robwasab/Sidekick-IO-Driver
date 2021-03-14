/* File: sidekickio.i */

%module sidekickio
%{
    #define SWIG_FILE_WITH_INIT
    #include "sidekickio.h"
%}

%include "stdint.i"
%include "carrays.i"

%typemap(in) (uint8_t * data, size_t len) {
  if (!PyByteArray_Check($input)) {
    SWIG_exception_fail(SWIG_TypeError, "in method '" "$symname" "', argument "
                       "$argnum"" of type '" "$type""'");
  }
  $1 = (uint8_t*) PyByteArray_AsString($input);
  $2 = (size_t) PyByteArray_Size($input);
}

%apply (uint8_t * data, size_t len) {
(const uint8_t * data, size_t len)
};


%include "..\..\..\sidekickio.h"
