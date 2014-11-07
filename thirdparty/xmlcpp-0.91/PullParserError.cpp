/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "PullParserError.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace xmlcpp
{

PullParserError::PullParserError(char const* file, int line, char const* func,
				 error_code code, ...) throw()
  : Exception(code)
{
  using namespace std;

  char format[128] = { '\0' };
  snprintf(format, sizeof(format), "%s:%d: %s: %s",
	   file, line, func, error_code_c_str(code));

  va_list params;
  va_start(params, code);
  vsnprintf(msg_, sizeof(msg_), format, params);
  va_end(params);
}

}
