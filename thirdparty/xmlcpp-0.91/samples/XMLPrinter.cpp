/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include <fstream>
#include <iostream>

#include "PullParser.h"

using namespace std;

void prettyPrint(char const* filename, char const* encoding);
void debugPrint(char const* filename, char const* encoding);

void usage(char* self)
{
  cout << "usage: " << self << " [-p] xmlfilename [encoding]" << endl;
  cout << "\t-p\tpretty print" << endl;
  exit(1);
}

int main(int argc, char* argv[])
{
  if (argc < 2) usage(argv[0]);

  bool pretty = false;
  if (strcmp(argv[1], "-p") == 0) {
    pretty = true;
    if (argc < 3) usage(argv[0]);
  }

  if (pretty) prettyPrint(argv[2], argc == 4 ? argv[3] : 0);
  else debugPrint(argv[1], argc == 3 ? argv[2] : 0);
}

void debugPrint(char const* filename, char const* encoding)
{
  using namespace xmlcpp;

  try {
    fstream fs(filename);
    fs.exceptions (ios_base::badbit);

    PullParser pp;
    pp.set_input(fs, encoding);

    while (pp.next_token() != PullParser::END_DOCUMENT) {
      wcout << PullParser::event_type_c_str(pp.get_event_type()) << L": " << endl;
      switch (pp.get_event_type()) {
      case PullParser::START_DOCUMENT:
	wcout << L"\tversion=\"" << pp.get_version() << L"\"" << endl;
	wcout << L"\tencoding=\"" << pp.get_encoding() << L"\"" << endl;
	wcout << L"\tstandalone\"" << pp.get_standalone() << L"\"" << endl;
	break;
      case PullParser::START_TAG:
	wcout << L"\tname=\"" << pp.get_name() << L"\"" << endl;
	for (int i = 0; i < pp.get_attr_size(); ++i) {
	  wcout << L"\t\t" << pp.get_attr_name(i)
		<< L"=\"" << pp.get_attr_value(i) << L"\"" << endl;
	}
	break;
      case PullParser::END_TAG:
	wcout << L"\tname=\"" << pp.get_name() << L"\"" << endl;
	break;
      case PullParser::TEXT:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      case PullParser::IGNORABLE_WHITESPACE:
	break;
      case PullParser::CDSECT:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      case PullParser::PROCESSING_INSTRUCTION:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      case PullParser::COMMENT:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      case PullParser::ENTITY_REF:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      case PullParser::DOCDECL:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      case PullParser::UNKNOWN_EVENT:
      default:
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      }
    }

    wcout << L"END_DOCUMENT: " << endl;
  }
  catch (exception& ex) {
    wcout << ex.what() << endl;
  }
}

void prettyPrint(char const* filename, char const* encoding)
{
  using namespace xmlcpp;

  try {
    fstream fs(filename);
    fs.exceptions (ios_base::badbit);

    PullParser pp;
    pp.set_input(fs, encoding);

    while (pp.next_token() != PullParser::END_DOCUMENT) {
      switch (pp.get_event_type()) {
      case PullParser::START_DOCUMENT:
	wcout << L"<?xml";
	wcout << L" version=\"" << pp.get_version() << L"\"";
	if (!pp.get_encoding().empty())
	  wcout << L" encoding=\"" << pp.get_encoding() << L"\"";
	if (!pp.get_standalone().empty())
	  wcout << L" standalone\"" << pp.get_standalone() << L"\"";
	wcout << L" ?>";
	break;
      case PullParser::START_TAG:
	wcout << L"<" << pp.get_name();
	for (int i = 0; i < pp.get_attr_size(); ++i) {
	  wcout << L" " << pp.get_attr_name(i)
		<< L"=\"" << pp.get_attr_value(i) << L"\"";
	}
	if (pp.is_empty_elem_tag()) {
	  wcout << L" />";
	  pp.next_token(); // consume next END_TAG token.
	}
	else {
	  wcout << L">";
	}
	break;
      case PullParser::END_TAG:
	wcout << L"</" << pp.get_name() << L">";
	break;
      case PullParser::TEXT:
	wcout << pp.get_raw_text();
	break;
      case PullParser::IGNORABLE_WHITESPACE:
	wcout << pp.get_raw_text();
	break;
      case PullParser::CDSECT:
	wcout << L"<![CDATA[" << pp.get_raw_text() << L"]]>";
	break;
      case PullParser::PROCESSING_INSTRUCTION:
	wcout << L"<?" << pp.get_raw_text() << L"?>";
	break;
      case PullParser::COMMENT:
	wcout << L"<--" << pp.get_raw_text() << L"-->";
	break;
      case PullParser::ENTITY_REF:
	wcout << pp.get_raw_text();
	break;
      case PullParser::DOCDECL:
	wcout << L"<!DOCTYPE" << pp.get_raw_text() << L">";
	break;
      case PullParser::UNKNOWN_EVENT:
      default:
	wcout << L"UNKNOWN_EVENT: " << endl;
	wcout << L"\ttext=\"" << pp.get_raw_text() << L"\"" << endl;
	break;
      }
    }
  }
  catch (exception& ex) {
    wcout << ex.what() << endl;
  }
}
