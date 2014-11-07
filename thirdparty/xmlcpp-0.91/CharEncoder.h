/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_CHARENCODER_H
#define XMLCPP_CHARENCODER_H

#include <string>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

#include "CodeConv.h"

namespace xmlcpp
{

/**
 * CharEncoder class. It returns the wchar_type one by one from any
 * input source. It uses Template Design Pattern. So the derived class
 * should implement its own 'getInbuf' and 'seekInbuf' functions.
 *
 * Its wchar_type is just platform's wchar_t type. Its width is 32bits
 * in Linux, and 16bits in Windows. So in Windows, it can present only
 * plane-0 unicode.
 *
 * If you want to make another CharEncoder source class, derive your
 * class from CharDecode and implement two functions; 'get_inbuf' and
 * 'seek_inbuf'. You can call 'set_fromcode' only after your class
 * prepared for these functions call because set_fromcode use them. If
 * you pass null(0) for fromcode parameter, CharDecode try to find
 * encoding of the source with BOM (Byte Order Mark).
 */
class CharEncoder : private boost::noncopyable
{
 public:
  typedef char char_type;
  typedef std::basic_string<char_type> string_type;

  typedef wchar_t wchar_type; ///< 32bit in linux, 16bit in windows
  typedef std::basic_string<wchar_type> wstring_type;

  CharEncoder(string_type const& encoding, int bytescachesize);
  virtual ~CharEncoder();

  int write(wstring_type const& str);
  void flush();

 private:
  virtual int write_impl(char const* cstr, int byteslen) = 0;
  virtual void flush_impl();

 private:
  std::string encoding_;

  boost::scoped_ptr<CodeConv> conv_;

  int size_;
  boost::scoped_array<wchar_type> buf_;
  int len_;
  int pos_;
};

}

#endif
