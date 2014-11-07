/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_CHARDECODER_H
#define XMLCPP_CHARDECODER_H

#include <string>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/scoped_array.hpp>

#include "CodeConv.h"

namespace xmlcpp
{

/**
 * CharDecoder class. It returns the wchar_type one by one from any
 * input source. It uses Template Design Pattern. So the derived class
 * should implement its own 'getInbuf' and 'seekInbuf' functions.
 *
 * Its wchar_type is just platform's wchar_t type. Its width is 32bits
 * in Linux, and 16bits in Windows. So in Windows, it can present only
 * plane-0 unicode.
 *
 * If you want to make another CharDecoder source class, derive your
 * class from CharDecode and implement two functions; 'get_inbuf' and
 * 'seek_inbuf'. You can call 'set_fromcode' only after your class
 * prepared for these functions call because set_fromcode use them. If
 * you pass null(0) for fromcode parameter, CharDecode try to find
 * encoding of the source with BOM (Byte Order Mark).
 */
class CharDecoder : private boost::noncopyable
{
 public:
  typedef wchar_t wchar_type; ///< 32bit in linux, 16bit in windows
  typedef std::basic_string<wchar_type> wstring_type;

  typedef std::char_traits<wchar_type> wchar_traits;
  typedef wchar_traits::int_type wint_type; //< wint_t

  static wint_type const END = WEOF;

  CharDecoder(bool ignoreilseq, size_t bytescachesize, bool isxmlfile);
  virtual ~CharDecoder();

  wint_type next();

  wint_type at(int index);
  void release_before(int index);

  size_t get_ilseq_cnt() const;

  std::string get_fromcode() const;
  void reset_fromcode(std::string& fromcode);

 protected:
  void set_fromcode(char const* fromcode);

 private:
  wint_type next_impl();

  char const* check_bom(unsigned char const* buf, size_t& bytes);
  char const* check_without_bom(unsigned char const* buf, size_t& bytes);

 private:
  /**
   * This function should be implemented by derived classes. It
   * returns 0 for eof situation.
   */
  virtual char* get_inbuf(size_t& bytesleft, size_t bytessizehint) = 0;

  /**
   * This function should be implemented by derived classes. The
   * bytesoffset can be negative value.
   */
  virtual void seek_inbuf(int bytesoffset) = 0;

 private:
  std::string fromcode_;

  boost::scoped_ptr<CodeConv> conv_;

  int size_;
  boost::scoped_array<wchar_type> buf_;
  int len_;
  int pos_;

  bool ignoreilseq_;
  size_t ilseqcnt_;

  bool isxmlfile_;
  wint_type lastchar_;

 private:
  wstring_type cache_;
  int cachefirst_;
};

}

#endif
