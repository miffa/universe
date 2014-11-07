/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "CodeConv.h"

#include <cerrno>
#include <limits>
#include <algorithm>
#include <boost/lambda/lambda.hpp>
#include <boost/lambda/casts.hpp>
#include <boost/lambda/if.hpp>

#include "Exception.h"

#include <iconv.h>

namespace xmlcpp
{

struct CodeConvImpl
{
  iconv_t desc_;

  void ctor(std::string const& to, std::string const& from)
  {
    desc_ = iconv_open(to.c_str(), from.c_str());
    if (desc_ == (iconv_t)-1)
      throw Exception0(Exception::E_CONV_OPEN);
  }

  void dtor()
  {
    if (iconv_close(desc_) == -1)
      throw Exception0(Exception::E_CONV_CLOSE);
  }

  void init()
  {
    size_t in, out;
    iconv(desc_,
	  static_cast<char**>(0), &in,
	  static_cast<char**>(0), &out);
  }

  size_t convert(char** inbuf, size_t* inbytesleft,
		 char** outbuf, size_t* outbytesleft)
  {
    size_t ret = iconv(desc_,
		       inbuf, inbytesleft,
		       outbuf, outbytesleft);

    if (ret == (size_t) -1) {
      if (errno == EILSEQ)
	throw Exception0(Exception::E_CONV_INV_SEQ);
    }
    return ret;	
  }
};

CodeConv::CodeConv(std::string const& to, std::string const& from)
  : pimpl_(new CodeConvImpl)
{
  pimpl_->ctor(to, from);
}

CodeConv::~CodeConv()
{
  pimpl_->dtor();
}

void CodeConv::init()
{
  pimpl_->init();
}

size_t CodeConv::convert(char** inbuf, size_t* inbytesleft,
		      char** outbuf, size_t* outbytesleft)
{
  return pimpl_->convert(inbuf, inbytesleft, outbuf, outbytesleft);;
}

std::string CodeConv::to_str(std::wstring const& in, char oob)
{
  // If your boost version is equal or greater than 1.34, you can use
  // lexical_cast<string>(). But boost 1.33's lexical_cast has bug.

  std::string str;
  for (std::wstring::const_iterator i = in.begin(); i != in.end(); ++i) {
    if (*i > std::numeric_limits<char>::max()) {
      if (oob == 0)
	throw Exception0(Exception::E_CONV_INV_SEQ);
      str += oob;
    }
    else {
      str += static_cast<char>(*i);
    }
  }

  return str;
}

std::wstring CodeConv::to_str(std::string const& in)
{
  // If your boost version is equal or greater than 1.34, you can use
  // lexical_cast<wstring>(). But boost 1.33's lexical_cast has bug.

  using namespace boost::lambda;

  std::wstring str;
  std::transform(in.begin(), in.end(),
		 back_inserter(str),
		 _1);

  return str;
}

std::string CodeConv::to_utf8(std::wstring const& in)
{
  CodeConv ic("UTF-8", "WCHAR_T");

  size_t inbytesleft = in.length() * sizeof(wchar_t);
  char* inbuf = const_cast<char*>(reinterpret_cast<char const*>(in.c_str()));

  std::string ret;
  while (inbytesleft != 0) {
    char buf[1024] = { '\0' };
    size_t outbytesleft = sizeof(buf);

    char* outbuf = reinterpret_cast<char*>(buf);

    ic.convert(&inbuf, &inbytesleft,
	       &outbuf, &outbytesleft);
    ret += buf;
  }

  return ret;
}

}

