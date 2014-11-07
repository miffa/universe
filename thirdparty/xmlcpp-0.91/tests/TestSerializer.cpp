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
#include <boost/lexical_cast.hpp>
#include <string>
#include <deque>
#include <iostream>
#include <fstream>

#include "Serializer.h"

using namespace std;
using namespace xmlcpp;
using namespace boost;
using boost::unit_test::test_suite;

void testSerializerBasic()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<name/>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"name");
    sr.end_tag(L"", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }

  try {
    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"name");
    sr.end_tag(L"", L"oops");

    BOOST_ERROR("exception must be thrown");
  }
  catch (Exception& ex) {
    BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_TAG_NOT_NESTED);
  }
}

void testSerializerDefaultNS()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<name xmlns=\"http://ideathinking.com\"/>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.set_prefix(L"", L"http://ideathinking.com");

    sr.start_tag(L"http://ideathinking.com", L"name");
    sr.end_tag(L"http://ideathinking.com", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerNS()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<ns:name xmlns:ns=\"http://ideathinking.com\"/>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.set_prefix(L"ns", L"http://ideathinking.com");

    sr.start_tag(L"http://ideathinking.com", L"name");
    sr.end_tag(L"http://ideathinking.com", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerAttr()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<ns:name xmlns:ns=\"http://ideathinking.com\" "
      "id=\"10\" ns:id=\"20\""
      "/>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.set_prefix(L"ns", L"http://ideathinking.com");

    sr.start_tag(L"http://ideathinking.com", L"name");
    sr.attribute(L"", L"id", L"10");
    sr.attribute(L"http://ideathinking.com", L"id", L"20");
    sr.end_tag(L"http://ideathinking.com", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }

  try {
    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.set_prefix(L"ns", L"http://ideathinking.com");

    sr.start_tag(L"http://ideathinking.com", L"name");
    sr.attribute(L"", L"id", L"10");
    sr.attribute(L"", L"id", L"20");

    BOOST_ERROR("exception must be thrown");
  }
  catch (Exception& ex) {
    BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_ATTNAME_DUPL);
  }
}

void testSerializerDepth()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<!-- outside -->"  // 0
      "<root>"            // 1
      "<foobar>"          // 2
      "sometext"          // 2
      "</foobar>"         // 2
      "</root>"           // 1
      "<!-- outside -->"; // 0

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.comment(L" outside ");
    BOOST_CHECK_EQUAL(sr.get_depth(), 0);

    sr.start_tag(L"", L"root");
    BOOST_CHECK_EQUAL(sr.get_depth(), 1);

    sr.start_tag(L"", L"foobar");
    BOOST_CHECK_EQUAL(sr.get_depth(), 2);

    sr.text(L"sometext");
    BOOST_CHECK_EQUAL(sr.get_depth(), 2);

    sr.end_tag(L"", L"foobar");
    BOOST_CHECK_EQUAL(sr.get_depth(), 2);

    sr.end_tag(L"", L"root");
    BOOST_CHECK_EQUAL(sr.get_depth(), 1);

    sr.comment(L" outside ");
    BOOST_CHECK_EQUAL(sr.get_depth(), 0);

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerAttrEscape()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<name qty=\"&lt;&quot;\'10\'&quot;&gt;\"/>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"name");
    sr.attribute(L"", L"qty", L"<\"\'10\'\">");
    sr.end_tag(L"", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
  // ATTRIBUTE_USE_APOSTROPHE
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<name qty=\'&lt;\"&apos;10&apos;\"&gt;\'/>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_feature(Serializer::ATTRIBUTE_USE_APOSTROPHE);
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"name");
    sr.attribute(L"", L"qty", L"<\"\'10\'\">");
    sr.end_tag(L"", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerEscape()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
      "<name>"
      "&lt;&apos;&quot;&amp;&gt;"
      "</name>";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"name");
    sr.text(L"<\'\"&>");
    sr.end_tag(L"", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerLineSeparator()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<name>\n"
      "&lt;&apos;&quot;&amp;&gt;\n"
      "</name>\n";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_feature(Serializer::LINE_SEPARATOR);
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"name");
    sr.text(L"<\'\"&>");
    sr.end_tag(L"", L"name");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerIndentation()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<a>\n"
      "\ttext\n"
      "\t<b>\n"
      "\t\ttext\n"
      "\t\t<c>\n"
      "\t\t</c>\n"
      "\t</b>\n"
      "</a>\n";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_feature(Serializer::LINE_SEPARATOR);
    sr.set_feature(Serializer::INDENTATION);
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"a");
    sr.text(L"text");
    sr.start_tag(L"", L"b");
    sr.text(L"text");
    sr.start_tag(L"", L"c");
    sr.end_tag(L"", L"c", true);
    sr.end_tag(L"", L"b");
    sr.end_tag(L"", L"a");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }

  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<a>\n"
      "\ttext\n"
      "\t<b>\n"
      "\t\ttext\n"
      "\t\t<c/>\n"
      "\t</b>\n"
      "</a>\n";

    char buff[1024] = { '\0' };

    Serializer sr;
    sr.set_feature(Serializer::LINE_SEPARATOR);
    sr.set_feature(Serializer::INDENTATION);
    sr.set_output("UTF-8", buff, sizeof(buff));

    sr.start_document(L"UTF-8", L"");

    sr.start_tag(L"", L"a");
    sr.text(L"text");
    sr.start_tag(L"", L"b");
    sr.text(L"text");
    sr.start_tag(L"", L"c");
    sr.end_tag(L"", L"c");
    sr.end_tag(L"", L"b");
    sr.end_tag(L"", L"a");

    sr.end_document();

    BOOST_CHECK_EQUAL(string(buff), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testSerializerStream()
{
  try {
    char goal[] =
      "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
      "<!-- you can delete this file -->\n"
      "<a/>\n";

    char filename[] = "__test_delete_me__.xml";

    ofstream os(filename);

    Serializer sr;
    sr.set_feature(Serializer::LINE_SEPARATOR);
    sr.set_output("UTF-8", os);

    sr.start_document(L"UTF-8", L"");
    sr.comment(L" you can delete this file ");
    sr.start_tag(L"", L"a");
    sr.end_tag(L"", L"a");

    sr.end_document();

    os.close();

    // read
    ifstream is(filename);

    char buff[1024] = { '\0' };

    is.read(buff, sizeof(buff));

    is.close();

    // check BOM
    BOOST_CHECK_EQUAL((unsigned char) buff[0], 0xEF);
    BOOST_CHECK_EQUAL((unsigned char) buff[1], 0xBB);
    BOOST_CHECK_EQUAL((unsigned char) buff[2], 0xBF);
    BOOST_CHECK_EQUAL(string(buff + 3), goal);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

test_suite* init_TestSerializer()
{
  using boost::bind;
  using boost::function;

  test_suite* test = BOOST_TEST_SUITE("Serializer_test_suite");

  test->add(BOOST_TEST_CASE(testSerializerBasic));
  test->add(BOOST_TEST_CASE(testSerializerDefaultNS));
  test->add(BOOST_TEST_CASE(testSerializerNS));
  test->add(BOOST_TEST_CASE(testSerializerAttr));
  test->add(BOOST_TEST_CASE(testSerializerDepth));
  test->add(BOOST_TEST_CASE(testSerializerAttrEscape));
  test->add(BOOST_TEST_CASE(testSerializerEscape));
  test->add(BOOST_TEST_CASE(testSerializerLineSeparator));
  test->add(BOOST_TEST_CASE(testSerializerIndentation));
  test->add(BOOST_TEST_CASE(testSerializerStream));

  return test;
}
