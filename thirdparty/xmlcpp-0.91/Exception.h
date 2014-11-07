/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_EXCEPTION_H
#define XMLCPP_EXCEPTION_H

#include <exception>
#include <boost/current_function.hpp>

namespace xmlcpp
{

/**
 * Exception class. Every exceptions in this library should be derived
 * this class.
 */
class Exception : public std::exception
{
 public:
  enum error_code
  {
    S_OK,
    E_CONV_OPEN,
    E_CONV_CLOSE,
    E_CONV_INV_SEQ,
    E_CONV_IS_NULL,
    E_FILE_TOO_SMALL,
    E_FILE_WRITE_FAIL,
    E_BUFF_TOO_SMALL,
    E_PP_MATCH_NOTHING,
    E_PP_MATCH_FAIL,
    E_PP_ATTNAME_DUPL,
    E_PP_TAG_NOT_NESTED,
    E_PP_INV_CALL_IN_EVNT,
    E_PP_INV_CALL_IN_FEAT,
    E_PP_ATTNAME_NOT_FOUND,
    E_PP_NO_ENTITY_REF,
    E_PP_PREDEF_ENTITY_REF,
    E_PP_NOT_IMPL,
    E_PP_TAG_UNCLOSED,
    E_PP_MULTIPLE_ROOT,
    E_PP_NS_NOT_SET,
    E_UNKNOWN
  };

  char const* error_code_c_str(error_code code);

  Exception(char const* file, int line, char const* func,
	    error_code code, ...) throw();
  virtual ~Exception() throw();

  virtual char const* what() const throw();

  error_code get_error_code() const throw();

 protected:
  Exception(error_code code) throw();

  error_code code_;
  char msg_[512];
};

#define Exception0(code) \
  Exception(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code)

#define Exception1(code, arg1) \
  Exception(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code, arg1)

#define Exception2(code, arg1, arg2) \
  Exception(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code, arg1, arg2)

#define Exception3(code, arg1, arg2, arg3) \
  Exception(__FILE__, __LINE__, BOOST_CURRENT_FUNCTION, code, arg1, arg2, arg3)

}

#endif
