/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_CODECONV_H
#define XMLCPP_CODECONV_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

namespace xmlcpp
{

/**
 * CodeConv class.
 */
class CodeConv : private boost::noncopyable
{
 public:
  CodeConv(std::string const& to, std::string const& from);
  ~CodeConv();

  size_t convert(char** inbuf, size_t* inbytesleft,
		 char** outbuf, size_t* outbytesleft);

  static std::string to_str(std::wstring const& in, char oob = 0);
  static std::wstring to_str(std::string const& in);

  static std::string to_utf8(std::wstring const& in);

  void init();

 private:
  boost::shared_ptr<struct CodeConvImpl> pimpl_;
};

}

#endif
