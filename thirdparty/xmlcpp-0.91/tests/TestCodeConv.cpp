/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/test/unit_test.hpp>

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string>

#include "CodeConv.h"

using namespace std;
using namespace xmlcpp;
using namespace boost;
using boost::unit_test::test_suite;

void testCodeConvToStr()
{
  string s("abcdefghijklmnopqrstuvwxyz01234567890");
  wstring ws(CodeConv::to_str(s));
  BOOST_CHECK_EQUAL(s.length(), ws.length());
  BOOST_CHECK_EQUAL(ws[0], L'a');

  s = CodeConv::to_str(ws);
  BOOST_CHECK_EQUAL(s.length(), ws.length());
  BOOST_CHECK_EQUAL(s[0], 'a');
}

void testCodeConvToUTF8()
{
  wstring ws(L"abcdefghijklmnopqrstuvwxyz01234567890");
  string s(CodeConv::to_utf8(ws));
  BOOST_CHECK_EQUAL(s.length(), ws.length());
  BOOST_CHECK_EQUAL(s[0], 'a');
}

test_suite* init_TestCodeConv()
{
  using boost::bind;
  using boost::function;

  test_suite* test = BOOST_TEST_SUITE("CodeConv_test_suite");

  test->add(BOOST_TEST_CASE(&testCodeConvToStr));
  test->add(BOOST_TEST_CASE(&testCodeConvToUTF8));

  return test;
}
