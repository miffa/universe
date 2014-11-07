/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Exception.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

namespace xmlcpp
{

char const* Exception::error_code_c_str(error_code code)
{
  switch (code) {
  case S_OK:
    return "ok";
  case E_CONV_OPEN:
    return "converter open error";
  case E_CONV_CLOSE:
    return "converter close error";
  case E_CONV_INV_SEQ:
    return "invalid multibytes sequence";
  case E_CONV_IS_NULL:
    return "converter is not initialized";
  case E_FILE_TOO_SMALL:
    return "too small file size";
  case E_FILE_WRITE_FAIL:
    return "writing to file failed.";
  case E_BUFF_TOO_SMALL:
    return "too small buffer size";
  case E_PP_MATCH_NOTHING:
    return "match nothing";
  case E_PP_MATCH_FAIL:
    return "match failed";
  case E_PP_ATTNAME_DUPL:
    return "attribute name appear more than once";
  case E_PP_TAG_NOT_NESTED:
    return "tag is not properly nested";
  case E_PP_INV_CALL_IN_EVNT:
    return "invalid call for event";
  case E_PP_INV_CALL_IN_FEAT:
    return "invalid call for feature";
  case E_PP_ATTNAME_NOT_FOUND:
    return "attribute name not found";
  case E_PP_NO_ENTITY_REF:
    return "expand entity ref failed";
  case E_PP_PREDEF_ENTITY_REF:
    return "predefined entity ref name";
  case E_PP_NOT_IMPL:
    return "not implemented";
  case E_PP_TAG_UNCLOSED:
    return "unclosed tag";
  case E_PP_MULTIPLE_ROOT:
    return "only one root element allowed";
  case E_PP_NS_NOT_SET:
    return "namespace prefix is not set";
  case E_UNKNOWN:
  default:
    return "unknown";
  }
}

Exception::Exception(error_code code) throw()
  : code_(code)
{
}

Exception::Exception(char const* file, int line, char const* func,
		     error_code code, ...) throw()
  : code_(code)
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

Exception::~Exception() throw()
{

}

char const* Exception::what() const throw()
{
  return msg_;
}

Exception::error_code Exception::get_error_code() const throw()
{
  return code_;
}

}
