/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_STREAMCHARDECODER_H
#define XMLCPP_STREAMCHARDECODER_H

#include <istream>
#include "CharDecoder.h"

namespace xmlcpp
{

/**
 * BasicStreamCharDecoder class. It's a CharDecoder for istream input
 * sources. It can be used with fstream, stringstream, or any other
 * istream.
 */
template <class S>
class BasicStreamCharDecoder : public CharDecoder
{
 public:
  typedef S stream_type;
  typedef typename stream_type::char_type char_type;

  BasicStreamCharDecoder(stream_type& instr,
			 char const* fromcode,
			 bool ignoreilseq = false,
			 size_t bytescacheSize = 1024 / sizeof(char_type),
			 bool isxmlfile = false)
    : CharDecoder(ignoreilseq, bytescacheSize, isxmlfile),
    instr_(instr), tmpsize_(0)
  {
    set_fromcode(fromcode);
  }

 private:
  virtual char* get_inbuf(size_t& bytesleft, size_t bytessizehint)
  {
    if (instr_.eof()) return 0;

    if (bytessizehint < 128) bytessizehint = 128;
    size_t sizehint = bytessizehint / sizeof(char_type);

    if (tmpsize_ < sizehint) {
      tmpsize_ = sizehint;
      tmp_.reset(new char_type[tmpsize_]);
    }

    bytesleft = instr_.readsome(tmp_.get(), tmpsize_);

    if (bytesleft <= tmpsize_) {
      // readsome do not set eof bit even though it reachs the end of
      // stream.  so we call get to set eof bit.
      instr_.peek();
    }

    bytesleft *= sizeof(char_type);

    return tmp_.get();
  }

  virtual void seek_inbuf(int bytesoffset)
  {
    assert((bytesoffset % sizeof(char_type)) == 0);

    int offset = bytesoffset / (int)sizeof(char_type);

    instr_.seekg(offset, std::istream::cur);
    if (offset < 0 && instr_.eof()) {
      instr_.clear(); // to clear eof flag.
    }
  }

 private:
  stream_type& instr_;
  boost::scoped_array<char_type> tmp_;
  size_t tmpsize_;
};

/**
 * StreamCharDecoder typedef. It's the specialization of
 * BasicStreamCharDecoder with std::istream.
 *
 * It can be used with any encoding type.
 */
typedef BasicStreamCharDecoder<std::istream> StreamCharDecoder;

/**
 * WStreamCharDecoder typedef. It's the specialization of
 * BasicStreamCharDecoder with std::wistream.
 *
 * It can be used only with UTF-32BE or UTF-32LE if sizeof wchar_t is
 * 4. And UTF-32BE, UTF-32LE, UTF-16BE, or UTF-16LE if sizeof wchar_t
 * is 2.
 */
typedef BasicStreamCharDecoder<std::wistream> WStreamCharDecoder;

}

#endif
