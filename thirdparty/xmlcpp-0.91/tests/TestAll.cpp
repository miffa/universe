/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

test_suite* init_TestCodeConv();
test_suite* init_TestCharDecoder();
test_suite* init_TestPullParser();
test_suite* init_TestSerializer();

test_suite* init_unit_test_suite(int , char* [])
{
  test_suite* test = BOOST_TEST_SUITE("xmlcpp_test_suite");

  test->add(init_TestCodeConv());
  test->add(init_TestCharDecoder());
  test->add(init_TestPullParser());
  test->add(init_TestSerializer());

  return test;
}


