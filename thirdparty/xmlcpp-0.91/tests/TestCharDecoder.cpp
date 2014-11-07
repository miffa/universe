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

#include "StreamCharDecoder.h"
#include "CStrCharDecoder.h"
#include "IterPairCharDecoder.h"
#include "CharDecoderIterator.h"

using namespace std;
using namespace xmlcpp;
using namespace boost;
using boost::unit_test::test_suite;

void testCharDecoderIterator(shared_ptr<CharDecoder> decoder)
{
  try {
    CharDecoderIterator first(decoder);
    CharDecoderIterator last;

    wstring docstr;

    for (; first != last; ++first) {
      docstr += *first;
    }

    BOOST_CHECK_EQUAL(decoder->get_ilseq_cnt(), (size_t) 0);

    wstring goal(L"<?xml version=\"1.0\"");
    wstring doc(docstr.begin(), docstr.begin() + goal.length());
    if (doc != goal) {
      BOOST_ERROR("doc != goal");
    }
  }
  catch (exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testCharDecoder(shared_ptr<CharDecoder> decoder)
{
  try {
    wstring docstr;

    CharDecoder::wint_type c = decoder->next();
    docstr += c;
    while (c != CharDecoder::END) {
      c = decoder->next();
      docstr += c;
    }

    BOOST_CHECK_EQUAL(decoder->get_ilseq_cnt(), (size_t) 0);

    wstring first(L"<?xml version=\"1.0\"");
    wstring doc(docstr.begin(), docstr.begin() + first.length());
    if (doc != first) {
      BOOST_ERROR("doc != first");
    }
  }
  catch (exception& ex) {
    BOOST_ERROR(ex.what());
  }
}

void testStreamCharDecoder(char const* filename,
			   char const* encoding)
{
  fstream fs(filename);

  shared_ptr<CharDecoder> decoder(new StreamCharDecoder(fs, encoding));

  testCharDecoder(decoder);

  fs.close();
}

void testCStrCharDecoder(char const* filename,
			 char const* encoding)
{
  string doc;

  fstream fs(filename);

  while (fs) {
    char buf[1024] = { '\0' };
    fs.read(buf, sizeof(buf));
    doc += buf;
  }

  fs.close();

  shared_ptr<CharDecoder> decoder(new CStrCharDecoder(doc.c_str(), encoding));

  testCharDecoder(decoder);
}

void testIterPairCharDecoder(char const* filename,
			     char const* encoding)
{
  deque<char> doc;

  fstream fs(filename);

  while (fs) {
    char buf[1024] = { '\0' };
    fs.read(buf, sizeof(buf));
    doc.insert(doc.end(), buf, buf + sizeof(buf));
  }

  fs.close();

  shared_ptr<CharDecoder> decoder(
    newIterPairCharDecoder(doc.begin(),doc.end(),  encoding)
				  );

  testCharDecoder(decoder);
}

void testStreamCharDecoderIterator(char const* filename,
				   char const* encoding)
{
  fstream fs(filename);

  shared_ptr<CharDecoder> decoder(new StreamCharDecoder(fs, encoding));

  testCharDecoderIterator(decoder);

  fs.close();
}

void testCStrCharDecoderIterator(char const* filename,
				 char const* encoding)
{
  string doc;

  fstream fs(filename);

  while (fs) {
    char buf[1024] = { '\0' };
    fs.read(buf, sizeof(buf));
    doc += buf;
  }

  fs.close();

  shared_ptr<CharDecoder> decoder(new CStrCharDecoder(doc.c_str(), encoding));

  testCharDecoderIterator(decoder);
}

void testIterPairCharDecoderIterator(char const* filename,
				     char const* encoding)
{
  deque<char> doc;

  fstream fs(filename);

  while (fs) {
    char buf[1024] = { '\0' };
    fs.read(buf, sizeof(buf));
    doc.insert(doc.end(), buf, buf + sizeof(buf));
  }

  fs.close();

  shared_ptr<CharDecoder> decoder(
    newIterPairCharDecoder(doc.begin(),doc.end(),  encoding)
				  );

  testCharDecoderIterator(decoder);
}

test_suite* init_TestCharDecoder()
{
  using boost::bind;
  using boost::function;


  test_suite* test = BOOST_TEST_SUITE("CharDecoder_test_suite");

  {
    struct Filecode {
      char const* file;
      char const* code;
    } filecode[] = {
      { "note_encode_utf8_u.xml", "UTF-8", },
      { "note_encode_utf8_u.xml", 0, },
      { "note_encode_utf16_u.xml", "UTF-16", },
      { "note_encode_utf16_u.xml", 0, },
      { "note_encode_8859_u.xml", "ISO-8859-1", },
      { "note_encode_1252_u.xml", "CP1252", },
      { "note_encode_none_u.xml", 0 }
    };

    for (size_t i = 0; i < sizeof(filecode) / sizeof(*filecode); ++i) {
      test->add(
        BOOST_TEST_CASE(
          bind(&testStreamCharDecoder, filecode[i].file, filecode[i].code)
	)
      );
    }

    for (size_t i = 0; i < sizeof(filecode) / sizeof(*filecode); ++i) {
      test->add(
        BOOST_TEST_CASE(
          bind(&testIterPairCharDecoder, filecode[i].file, filecode[i].code)
        )
      );
    }

    for (size_t i = 0; i < sizeof(filecode) / sizeof(*filecode); ++i) {
      test->add(
        BOOST_TEST_CASE(
          bind(&testStreamCharDecoderIterator, filecode[i].file, filecode[i].code)
	)
      );
    }

    for (size_t i = 0; i < sizeof(filecode) / sizeof(*filecode); ++i) {
      test->add(
        BOOST_TEST_CASE(
          bind(&testIterPairCharDecoderIterator, filecode[i].file, filecode[i].code)
        )
      );
    }
  }

  {
    // CStrCharDecoder can't handle UTF-16, because it has 8-bit null
    // value.
    struct Filecode {
      char const* file;
      char const* code;
    } filecode[] = {
      { "note_encode_utf8_u.xml", "UTF-8", },
      { "note_encode_utf8_u.xml", 0, },
      //{ "note_encode_utf16_u.xml", "UTF-16", },
      //{ "note_encode_utf16_u.xml", 0, },
      { "note_encode_8859_u.xml", "ISO-8859-1", },
      { "note_encode_1252_u.xml", "CP1252", },
      { "note_encode_none_u.xml", 0 }
    };

    for (size_t i = 0; i < sizeof(filecode) / sizeof(*filecode); ++i) {
      test->add(
        BOOST_TEST_CASE(
          bind(&testCStrCharDecoder, filecode[i].file, filecode[i].code)
        )
      );
    }

    for (size_t i = 0; i < sizeof(filecode) / sizeof(*filecode); ++i) {
      test->add(
        BOOST_TEST_CASE(
          bind(&testCStrCharDecoderIterator, filecode[i].file, filecode[i].code)
        )
      );
    }
  }

  return test;
}
