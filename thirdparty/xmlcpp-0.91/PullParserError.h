/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_PULLPARSERERROR_H
#define XMLCPP_PULLPARSERERROR_H

#include "Exception.h"

namespace xmlcpp
{

/**
 * PullParserError class.
 */
class PullParserError : public Exception
{
 public:
  PullParserError(char const* file, int line, char const* func,
		  error_code code, ...) throw();
};

#define PullParserError0(code) \
  PullParserError(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code)

#define PullParserError1(code, arg1) \
  PullParserError(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code, arg1)

#define PullParserError2(code, arg1, arg2) \
  PullParserError(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code, arg1, arg2)

#define PullParserError3(code, arg1, arg2, arg3) \
  PullParserError(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code, arg1, arg2, arg3)

}

#endif
