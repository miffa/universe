/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "CharEncoder.h"

#include <cerrno>
#include <iostream>

#include "PullParserError.h"

namespace xmlcpp
{

CharEncoder::CharEncoder(string_type const& encoding, int bytescachesize)
  : encoding_(encoding), conv_(new CodeConv(encoding_, "WCHAR_T")),
    size_(bytescachesize), buf_(new wchar_type[size_]), len_(0), pos_(0)
{
  
}

CharEncoder::~CharEncoder()
{

}

int CharEncoder::write(wstring_type const& str)
{
  size_t inbytesleft = str.length() * sizeof(wchar_type);
  char* inbuf = const_cast<char*>(reinterpret_cast<char const*>(str.c_str()));

  while (inbytesleft) {
    size_t outbytesleft = sizeof(wchar_type) * size_;
    char* outbuf = reinterpret_cast<char*>(buf_.get());

    size_t outbytes = outbytesleft;
    char* prebuf = outbuf;
    try {
      conv_->convert(&inbuf, &inbytesleft, &outbuf, &outbytesleft);
      write_impl(prebuf, outbytes - outbytesleft);
    }
    catch (Exception& ex) {
      throw;
    }
  }

  return str.length();
}

void CharEncoder::flush()
{
  flush_impl();
}

void CharEncoder::flush_impl()
{
}

}


