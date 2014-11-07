#ifndef XMLCPP_CSTRCHARDECODER_H
#define XMLCPP_CSTRCHARDECODER_H

#include <istream>
#include <iostream>
#include "CharDecoder.h"

namespace xmlcpp
{

/**
 * CStrStreamCharDecoder class. It's a CharDecoder for C-string
 * (null-terminated) input sources. It can be used with char const* or
 * wchar const*.
 */
template <class C>
class BasicCStrCharDecoder : public CharDecoder
{
 public:
  typedef C char_type;

  BasicCStrCharDecoder(char_type const* instr,
		       char const* fromcode,
		       bool ignoreilseq = false,
		       size_t bytescacheSize = 1024 / sizeof(char_type),
		       bool isxmlfile = false)
    : CharDecoder(ignoreilseq, bytescacheSize, isxmlfile),
    instr_(instr), curr_(instr)
  {
    set_fromcode(fromcode);
  }

 private:
  virtual char* get_inbuf(size_t& bytesleft, size_t bytessizehint)
  {
    if (*curr_ == 0) return 0;

    char_type const* loc = curr_;

    if (bytessizehint < 128) bytessizehint = 128;
    //size_t sizehint = bytessizehint / sizeof(char_type);

    bytesleft = 0;
    while (*curr_ != 0) {
      ++curr_;
      ++bytesleft;
    }

    bytesleft *= sizeof(char_type);

    return const_cast<char*>(reinterpret_cast<char const*>(loc));
  }

  virtual void seek_inbuf(int bytesoffset)
  {
    assert((bytesoffset % sizeof(char_type)) == 0);

    if (bytesoffset == 0) return;

    bytesoffset /= (int)sizeof(char_type);

    if (bytesoffset > 0) {
      while (bytesoffset > 0 && *curr_ != 0) {
	--bytesoffset;
	++curr_;
      }
    }
    else {
      while (bytesoffset < 0 && curr_ >= instr_) {
	++bytesoffset;
	--curr_;
      }
    }
  }

 private:
  char_type const* instr_;
  char_type const* curr_;
};

/**
 * CStrCharDecoder typedef. It's the specialization of
 * BasicCStrCharDecoder with char.
 *
 * It can be used with any encoding type.
 */
typedef BasicCStrCharDecoder<char> CStrCharDecoder;

/**
 * WCStrCharDecoder typedef. It's the specialization of
 * BasicCStrCharDecoder with wchar_t.
 *
 * It can be used with any encoding type.
 */
typedef BasicCStrCharDecoder<wchar_t> WCStrCharDecoder;

}

#endif
