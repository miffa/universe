/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "CharDecoder.h"

#include <cerrno>
#include <iostream>

#include "Exception.h"

namespace xmlcpp
{

CharDecoder::CharDecoder(bool ignoreilseq,
			 size_t bytescachesize,
			 bool isxmlfile)
  : size_(bytescachesize), buf_(new wchar_type[size_]), len_(0), pos_(0),
    ignoreilseq_(ignoreilseq),
    ilseqcnt_(0),
    isxmlfile_(isxmlfile),
    lastchar_(0),
    cachefirst_(0)
{
}

CharDecoder::~CharDecoder()
{

}

void CharDecoder::reset_fromcode(std::string& fromcode)
{
  conv_.reset(new CodeConv("WCHAR_T", fromcode_.c_str())); // to wchar_t
  fromcode_ = fromcode;
}

void CharDecoder::set_fromcode(char const* fromcode)
{
  if (fromcode == 0) {
    // use BOM (Byte Order Mark) to detect encoding type. see
    // http://unicode.org/unicode/faq/utf_bom.html#25 or
    // http://www.w3.org/TR/REC-xml/#sec-guessing
    size_t bytesleft = 4;

    char* buf = get_inbuf(bytesleft, bytesleft);

    size_t bytesread = bytesleft;
    
    if (bytesleft < 4) {
      throw Exception0(Exception::E_FILE_TOO_SMALL);
    }

    fromcode = check_bom(reinterpret_cast<unsigned char*>(buf), bytesleft);
    if (bytesleft != 0) {
      int offset = bytesleft;
      seek_inbuf(-offset);
    }

    if (fromcode == 0) {
      if (isxmlfile_) {
	bytesleft = bytesread;
	fromcode = check_without_bom(reinterpret_cast<unsigned char*>(buf),
				     bytesleft);
	assert(bytesleft == bytesread);
      }
    }

    // assume UTF-8 if it can not be determined.
    if (fromcode == 0) fromcode = "UTF-8";
  }

  fromcode_ = fromcode;

  conv_.reset(new CodeConv("WCHAR_T", fromcode_.c_str())); // to wchar_t
}

char const* CharDecoder::check_bom(unsigned char const* buf, size_t& bytes)
{
  assert(bytes >= 4);

  // 00 00 FE FF  UTF-32, big-endian
  // FF FE 00 00  UTF-32, little-endian
  // FE FF        UTF-16, big-endian
  // FF FE 	  UTF-16, little-endian
  // EF BB BF 	  UTF-8
  if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0xFE && buf[3] == 0xFF) {
    bytes -= 4;
    return "UTF-32BE";
  }
  if (buf[0] == 0xFF && buf[1] == 0xFE && buf[2] == 0x00 && buf[3] == 0x00) {
    bytes -= 4;
    return "UTF-32LE";
  }
  if (buf[0] == 0xFE && buf[1] == 0xFF) {
    bytes -= 2;
    return "UTF-16BE";
  }
  if (buf[0] == 0xFF && buf[1] == 0xFE) {
    bytes -= 2;
    return "UTF-16LE";
  }
  if (buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
    bytes -= 3;
    return "UTF-8";
  }

  return 0;
}

char const* CharDecoder::check_without_bom(unsigned char const* buf,
					   size_t& bytes)
{
  assert(bytes >= 4);

  // 00 00 FE FF  UTF-32, big-endian
  // FF FE 00 00  UTF-32, little-endian
  // FE FF        UTF-16, big-endian
  // FF FE 	  UTF-16, little-endian
  // EF BB BF 	  UTF-8
  if (buf[0] == 0x00 && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x3C) {
    return "UTF-32BE";
  }
  if (buf[0] == 0x3C && buf[1] == 0x00 && buf[2] == 0x00 && buf[3] == 0x00) {
    return "UTF-32LE";
  }
  if (buf[0] == 0x00 && buf[1] == 0x3C && buf[2] == 0x00 && buf[3] == 0x3F) {
    return "UTF-16BE";
  }
  if (buf[0] == 0x3C && buf[1] == 0x00 && buf[2] == 0x3F && buf[3] == 0x00) {
    return "UTF-16LE";
  }
  if (buf[0] == 0x3C && buf[1] == 0x3F && buf[2] == 0x78 && buf[3] == 0x6D) {
    return "UTF-8";
  }
  if (buf[0] == 0x4C && buf[1] == 0x6F && buf[2] == 0xA7 && buf[3] == 0x94) {
    return "EBCDIC";
  }

  return 0;
}

size_t CharDecoder::get_ilseq_cnt() const
{
  return ilseqcnt_;
}

std::string CharDecoder::get_fromcode() const
{
  return fromcode_;
}

CharDecoder::wint_type CharDecoder::next()
{
  wint_type c = 0;

  if (isxmlfile_) {
    // see http://www.w3.org/TR/REC-xml/#sec-line-ends
    if (lastchar_ != 0) {
      c = lastchar_;
      lastchar_ = 0;
    }
    else {
      c = next_impl();
      if (c == 0x0D) {
	c = next_impl();
	if (c != 0x0A) {
	  lastchar_ = c;
	  c = 0x0A;
	}
      }
    }
  }
  else {
    c = next_impl();
  }

  cache_ += c; // append to cache.

  return c;

}

CharDecoder::wint_type CharDecoder::next_impl()
{
  if (conv_.get() == 0) {
    throw Exception0(Exception::E_CONV_IS_NULL);
  }

  if (pos_ >= len_) {
    size_t inbytes = 0;
    size_t outbytesleft = sizeof(wchar_type) * size_;

    char* inbuf = get_inbuf(inbytes, outbytesleft);
    if (inbuf == 0) return END; // eof

    char* outbuf = reinterpret_cast<char*>(buf_.get());

    bool ilseq_continue = false;
    do {
      ilseq_continue = false;
      size_t inbytesleft = inbytes;
      try {
	conv_->convert(&inbuf, &inbytesleft, &outbuf, &outbytesleft);
	if (inbytes > inbytesleft) { // for the case that the last
				     // bytes is invalid
	  int offset = inbytesleft;
	  seek_inbuf(-offset);
	}
      }
      catch (Exception& ex) {
	if (ex.get_error_code() == Exception::E_CONV_INV_SEQ) {
	  ++ilseqcnt_;
	  if (!ignoreilseq_) throw;

	  ilseq_continue = true;
	}
	else {
	  throw;
	}
      }

      if (ilseq_continue) {
	++inbuf;
	--inbytesleft;
	if (inbytesleft == 0) {
	  inbuf = get_inbuf(inbytesleft, outbytesleft);
	  if (inbuf == 0) return END; // eof
	}
      }

    } while (ilseq_continue);

    len_ = (sizeof(wchar_type) * size_ - outbytesleft) / sizeof(wchar_type);
    pos_ = 0;
  }

  wchar_type c = buf_[pos_];
  ++pos_;

  return c;
}

CharDecoder::wint_type CharDecoder::at(int index)
{
  if (index == -1) return END;

  index -= cachefirst_;

  while (index >= (int) cache_.length()) {
    if (next() == END) return END;
  }

  return cache_.at(index); // throw exception on boundary error.
}

void CharDecoder::release_before(int index)
{
  if (index == -1) index = cache_.length() + cachefirst_;

  index -= cachefirst_;

  if (index > (int) cache_.length()) index = cache_.length();

  if (index > 0) {
    cache_.erase(0, index);
    cachefirst_ += index;
  }
}

}
