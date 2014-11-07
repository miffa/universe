#ifndef XMLCPP_STREAMCHARENCODER_H
#define XMLCPP_STREAMCHARENCODER_H

#include <istream>
#include <iostream>
#include <algorithm>

#include "CharEncoder.h"
#include "Exception.h"

namespace xmlcpp
{

/**
 * StreamStreamCharEncoder class. It's a CharEncoder for C-string output
 * targets. It can be used with char const* or wchar const*.
 */
template <class S>
class BasicStreamCharEncoder : public CharEncoder
{
 public:
  typedef S stream_type;
  typedef typename stream_type::char_type char_type;

  BasicStreamCharEncoder(std::string const& encoding,
			 stream_type& outstr,
			 int bytescacheSize = 1024 / sizeof(char_type))
    : CharEncoder(encoding, bytescacheSize),
    outstr_(outstr)
  {
  }

  virtual int write_impl(char const* cstr, int byteslen)
  {
    assert(byteslen % sizeof(char_type) == 0);

    if (!outstr_.good())
      throw Exception0(Exception::E_FILE_WRITE_FAIL);

    char_type const* s = reinterpret_cast<char_type const*>(cstr);
    int len = byteslen / sizeof(char_type);
    outstr_.write(s, len);
    return byteslen;
  }

 private:
  stream_type& outstr_;
};

/**
 * StreamCharEncoder typedef. It's the specialization of
 * BasicStreamCharEncoder with char.
 *
 * It can be used with any encoding type.
 */
typedef BasicStreamCharEncoder<char> StreamCharEncoder;

/**
 * WStreamCharEncoder typedef. It's the specialization of
 * BasicStreamCharEncoder with wchar_t.
 *
 * It can be used with any encoding type.
 */
typedef BasicStreamCharEncoder<wchar_t> WStreamCharEncoder;

}

#endif

