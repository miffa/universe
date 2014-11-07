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

#include "CharDecoderIterator.h"
#include "PullParser.h"
#include "Exception.h"

using namespace std;
using namespace xmlcpp;
using namespace boost;
using boost::unit_test::test_suite;

void testPullParserBasic()
{
  try {
    char const* filename = "note_encode_test.xml";

    fstream fs(filename);

    PullParser pp;
    pp.set_input(fs);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);
    BOOST_CHECK(pp.get_version() == L"1.0");
    BOOST_CHECK(pp.get_encoding() == L"UTF-8");
    BOOST_CHECK(pp.get_standalone() == L"yes");
    BOOST_CHECK_EQUAL(pp.get_depth(), 0);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::COMMENT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"note");
    BOOST_CHECK_EQUAL(pp.is_empty_elem_tag(), false);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"to");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::TEXT);
    BOOST_CHECK(pp.get_text() == L"Tove");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"to");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"from");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::TEXT);
    BOOST_CHECK(pp.get_text() == L"Jani");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"from");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"heading");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::TEXT);
    BOOST_CHECK(pp.get_text() == L"Reminder");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"heading");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"body");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::TEXT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"body");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"tag");
    BOOST_CHECK_EQUAL(pp.is_empty_elem_tag(), true);
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"tag");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"script");
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 1);
    BOOST_CHECK(pp.get_attr_name(0) == L"lang");
    BOOST_CHECK(pp.get_attr_value(0) == L"java");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::CDSECT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"script");
    BOOST_CHECK_EQUAL(pp.get_depth(), 2);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"note");
    BOOST_CHECK_EQUAL(pp.get_depth(), 1);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
    BOOST_CHECK_EQUAL(pp.get_depth(), 0);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_DOCUMENT);

    fs.close();
  }
  catch (std::exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testPullParserUnicode()
{
  try {
    char const* filename = "utf8test.xml";

    fstream fs(filename);

    PullParser pp;
    pp.set_input(fs);

    BOOST_CHECK(pp.get_version() == L"1.0");
    BOOST_CHECK(pp.get_encoding() == L"UTF-8");
    BOOST_CHECK(pp.get_standalone() == L"");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"document");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"English");
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"English");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"Russian");
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"Russian");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"Spanish");
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"Spanish");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"SimplifiedChinese");
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"SimplifiedChinese");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.get_text().length(), (size_t) 7);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"Heavy");
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.get_text().length(), (size_t) 8);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"document");

    fs.close();
  }
  catch (std::exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testPullParserMismatchedTag()
{
  try {
    wchar_t xmlfile[] = L"<a>xx<b>yy</a></b>";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"b");

    BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);

    BOOST_ERROR("exception must be thrown");
  }
  catch (Exception& ex) {
    BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_TAG_NOT_NESTED);
  }
}

void testPullParserDuplAttr()
{
  wchar_t *xmlfiles[] = {
    L"<a x=\"aa\" x=\"bb\" />",
    L"<a x=\"aa\" x=\"bb\"></a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);

      BOOST_ERROR("exception must be thrown");
    }
    catch (Exception& ex) {
      BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_ATTNAME_DUPL);
    }
  }
}

void testPullParserDuplAttrNS()
{
  wchar_t *xmlfiles[] = {
    L"<a xmlns:x=\"1234\" xmlns:y=\"1234\" x:q=\"aa\" y:q=\"bb\" />",
    L"<a xmlns:x=\"1234\" xmlns:y=\"1234\" x:q=\"aa\" y:q=\"bb\"></a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);

      BOOST_CHECK_EQUAL(pp.get_attr_size(), 4);
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
  // PROCESS_NAMESPACES
  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_feature(PullParser::PROCESS_NAMESPACES);
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);

      BOOST_ERROR("exception must be thrown");
    }
    catch (Exception& ex) {
      BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_ATTNAME_DUPL);
    }
  }
}

void testPullParserDuplAttrDefaultNS()
{
  wchar_t *xmlfiles[] = {
    L"<a xmlns=\"1234\" x=\"aa\" />",
    L"<a xmlns=\"1234\" x=\"aa\"></a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);

      BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
      BOOST_CHECK(pp.get_attr_namespace(0) == L"");
      BOOST_CHECK(pp.get_attr_name(0) == L"xmlns");
      BOOST_CHECK(pp.get_attr_value(0) == L"1234");
      BOOST_CHECK(pp.get_attr_namespace(1) == L"");
      BOOST_CHECK(pp.get_attr_name(1) == L"x");
      BOOST_CHECK(pp.get_attr_value(1) == L"aa");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
  // PROCESS_NAMESPACES
  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_feature(PullParser::PROCESS_NAMESPACES);
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);

      BOOST_CHECK_EQUAL(pp.get_attr_size(), 1);
      BOOST_CHECK(pp.get_attr_namespace(0) == L"");
      BOOST_CHECK(pp.get_attr_name(0) == L"x");
      BOOST_CHECK(pp.get_attr_value(0) == L"aa");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
}

void testPullParserNS()
{
  wchar_t *xmlfiles[] = {
    L"<x:a xmlns:x=\"1234\">"
    L"<x:b xmlns=\"\">"
    L"<c />"
    L"</x:b>"
    L"</x:a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"x:a");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"x:b");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"x:b");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"x:a");
      BOOST_CHECK(pp.get_namespace() == L"");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
  // PROCESS_NAMESPACES
  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_feature(PullParser::PROCESS_NAMESPACES);
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"a");
      BOOST_CHECK(pp.get_namespace() == L"1234");
      BOOST_CHECK_EQUAL(pp.get_attr_size(), 0);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"1234");
      BOOST_CHECK_EQUAL(pp.get_attr_size(), 0);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"1234");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"a");
      BOOST_CHECK(pp.get_namespace() == L"1234");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
  // PROCESS_NAMESPACES | REPORT_NAMESPACE_ATTRIBUTES
  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_feature(PullParser::PROCESS_NAMESPACES);
      pp.set_feature(PullParser::REPORT_NAMESPACE_ATTRIBUTES);
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"a");
      BOOST_CHECK(pp.get_namespace() == L"1234");
      BOOST_CHECK_EQUAL(pp.get_attr_size(), 1);
      BOOST_CHECK(pp.get_attr_prefix(0) == L"xmlns");
      BOOST_CHECK(pp.get_attr_name(0) == L"x");
      BOOST_CHECK(pp.get_attr_namespace(0) == L"");
      BOOST_CHECK(pp.get_attr_value(0) == L"1234");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"1234");
      BOOST_CHECK_EQUAL(pp.get_attr_size(), 1);
      BOOST_CHECK(pp.get_attr_prefix(0) == L"");
      BOOST_CHECK(pp.get_attr_name(0) == L"xmlns");
      BOOST_CHECK(pp.get_attr_namespace(0) == L"");
      BOOST_CHECK(pp.get_attr_value(0) == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"1234");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"a");
      BOOST_CHECK(pp.get_namespace() == L"1234");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
}

void testPullParserDefaultNS()
{
  wchar_t *xmlfiles[] = {
    L"<x:a xmlns=\"default\" xmlns:x=\"1234\">"
    L"<b>"
    L"<c xmlns=\"\"/>"
    L"</b>"
    L"</x:a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"x:a");
      BOOST_CHECK(pp.get_namespace() == L"");
      BOOST_CHECK_EQUAL(pp.get_attr_size(), 2);
      BOOST_CHECK(pp.get_attr_name(0) == L"xmlns");
      BOOST_CHECK(pp.get_attr_name(1) == L"xmlns:x");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"x:a");
      BOOST_CHECK(pp.get_namespace() == L"");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
  // PROCESS_NAMESPACES
  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_feature(PullParser::PROCESS_NAMESPACES);
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"a");
      BOOST_CHECK(pp.get_namespace() == L"1234");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"default");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"c");
      BOOST_CHECK(pp.get_namespace() == L"");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"");
      BOOST_CHECK(pp.get_name() == L"b");
      BOOST_CHECK(pp.get_namespace() == L"default");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::END_TAG);
      BOOST_CHECK(pp.get_prefix() == L"x");
      BOOST_CHECK(pp.get_name() == L"a");
      BOOST_CHECK(pp.get_namespace() == L"1234");
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
}

void testPullParserInvalidEntityRef()
{
  wchar_t *xmlfiles[] = {
    L"<a>34&name;</a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_name() == L"a");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);

      BOOST_ERROR("exception must be thrown");
    }
    catch (Exception& ex) {
      BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_NO_ENTITY_REF);
    }
  }
}

void testPullParserEntityReplaceText()
{
  wchar_t *xmlfiles[] = {
    L"<a>34&name;</a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      pp.define_entity_replacement_text(L"name", L"pullparser");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_name() == L"a");

      BOOST_CHECK_EQUAL(pp.next(), PullParser::TEXT);
      BOOST_CHECK(pp.get_text() == L"34pullparser");

    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
}

void testPullParserAttrNormalize()
{
  try {
    wchar_t xmlfile[] = L"<a b=\"&lt;&gt;\" />";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.get_attr_size(), 1);
    BOOST_CHECK(pp.get_attr_value(0) == L"<>");
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }

  try {
    wchar_t xmlfile[] = L"<a b=\"&amp;lt;&gt;\" />";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.get_attr_size(), 1);
    BOOST_CHECK(pp.get_attr_value(0) == L"&lt;>");
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testPullParserCharNormalize()
{
  try {
    wchar_t xmlfile[] = L"<a>&lt;&#x0A;</a>";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::ENTITY_REF);
    BOOST_CHECK(pp.get_raw_text() == L"&lt;");
    BOOST_CHECK(pp.get_text() == L"<");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::TEXT);
    BOOST_CHECK(pp.get_raw_text() == L"\n");
    BOOST_CHECK(pp.get_text() == L"\n");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_DOCUMENT);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }

  try {
    wchar_t xmlfile[] = L"<a>&amp;lt;&#x0A;</a>";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::ENTITY_REF);
    BOOST_CHECK(pp.get_raw_text() == L"&amp;");
    BOOST_CHECK(pp.get_text() == L"&");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::TEXT);
    BOOST_CHECK(pp.get_raw_text() == L"lt;\n");
    BOOST_CHECK(pp.get_text() == L"lt;\n");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_DOCUMENT);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testPullParserEndOfLine()
{
  wchar_t *xmlfiles[] = {
    L"<a>\r</a>",
    L"<a>\n</a>",
    L"<a>\r\n</a>"
  };

  for (size_t i = 0; i < sizeof(xmlfiles) / sizeof(*xmlfiles); ++i) {
    try {
      wchar_t* xmlfile = xmlfiles[i];

      PullParser pp;
      pp.set_input(xmlfile);

      BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);

      BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
      BOOST_CHECK(pp.get_name() == L"a");

      BOOST_CHECK_EQUAL(pp.next_token(), PullParser::IGNORABLE_WHITESPACE);
      BOOST_CHECK(pp.get_raw_text() == L"\n");
      BOOST_CHECK(pp.get_text() == L"\n");

      BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);

      BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_DOCUMENT);
    }
    catch (Exception& ex) {
      BOOST_ERROR(ex.what());
    }
  }
}

void testPullParserTwoRootElement()
{
  try {
    wchar_t xmlfile[] = L"<a></a><b></b>";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);

    BOOST_ERROR("exception must be thrown");
  }
  catch (Exception& ex) {
    BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_MULTIPLE_ROOT);
  }
  // MULTIPLE_ROOT feature
  try {
    wchar_t xmlfile[] = L"<a></a><b></b>";

    PullParser pp;
    pp.set_feature(PullParser::MULTIPLE_ROOT);
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"b");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"b");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_DOCUMENT);
  }
  catch (Exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testPullParserUnclosedElement()
{
  try {
    wchar_t xmlfile[] = L"<a><b></b>";

    PullParser pp;
    pp.set_input(xmlfile);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_DOCUMENT);

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"a");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::START_TAG);
    BOOST_CHECK(pp.get_name() == L"b");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);
    BOOST_CHECK(pp.get_name() == L"b");

    BOOST_CHECK_EQUAL(pp.next_token(), PullParser::END_TAG);

    BOOST_ERROR("exception must be thrown");
  }
  catch (Exception& ex) {
    BOOST_CHECK_EQUAL(ex.get_error_code(), Exception::E_PP_TAG_UNCLOSED);
  }
}

test_suite* init_TestPullParser()
{
  using boost::bind;
  using boost::function;

  test_suite* test = BOOST_TEST_SUITE("PullParser_test_suite");

  test->add(BOOST_TEST_CASE(testPullParserBasic));
  test->add(BOOST_TEST_CASE(testPullParserUnicode));
  test->add(BOOST_TEST_CASE(testPullParserMismatchedTag));
  test->add(BOOST_TEST_CASE(testPullParserDuplAttr));
  test->add(BOOST_TEST_CASE(testPullParserDuplAttrNS));
  test->add(BOOST_TEST_CASE(testPullParserDuplAttrDefaultNS));
  test->add(BOOST_TEST_CASE(testPullParserNS));
  test->add(BOOST_TEST_CASE(testPullParserDefaultNS));
  test->add(BOOST_TEST_CASE(testPullParserInvalidEntityRef));
  test->add(BOOST_TEST_CASE(testPullParserEntityReplaceText));
  test->add(BOOST_TEST_CASE(testPullParserAttrNormalize));
  test->add(BOOST_TEST_CASE(testPullParserCharNormalize));
  test->add(BOOST_TEST_CASE(testPullParserEndOfLine));
  test->add(BOOST_TEST_CASE(testPullParserTwoRootElement));
  test->add(BOOST_TEST_CASE(testPullParserUnclosedElement));

  return test;
}
