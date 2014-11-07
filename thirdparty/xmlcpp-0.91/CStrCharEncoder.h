#ifndef XMLCPP_CSTRCHARENCODER_H
#define XMLCPP_CSTRCHARENCODER_H

#include <istream>
#include <iostream>
#include <algorithm>

#include "CharEncoder.h"
#include "Exception.h"

namespace xmlcpp
{

/**
 * CStrStreamCharEncoder class. It's a CharEncoder for C-string output
 * targets. It can be used with char const* or wchar const*.
 */
template <class C>
class BasicCStrCharEncoder : public CharEncoder
{
 public:
  typedef C char_type;

  BasicCStrCharEncoder(std::string const& encoding,
		       char_type* instr,
		       int length,
		       int bytescacheSize = 1024 / sizeof(char_type))
    : CharEncoder(encoding, bytescacheSize),
    first_(instr), last_(instr + length)
  {
  }

  virtual int write_impl(char const* cstr, int byteslen)
  {
    assert(byteslen % sizeof(char_type) == 0);

    if ((last_ - first_) * (int) sizeof(char_type) < byteslen)
      throw Exception0(Exception::E_BUFF_TOO_SMALL);

    char_type const* s = reinterpret_cast<char_type const*>(cstr);
    int len = byteslen / sizeof(char_type);
    std::copy(s, s + len, first_);
    first_ += len;
    return byteslen;
  }

 private:
  char_type* first_;
  char_type* const last_;
};

/**
 * CStrCharEncoder typedef. It's the specialization of
 * BasicCStrCharEncoder with char.
 *
 * It can be used with any encoding type.
 */
typedef BasicCStrCharEncoder<char> CStrCharEncoder;

/**
 * WCStrCharEncoder typedef. It's the specialization of
 * BasicCStrCharEncoder with wchar_t.
 *
 * It can be used with any encoding type.
 */
typedef BasicCStrCharEncoder<wchar_t> WCStrCharEncoder;

}

#endif

