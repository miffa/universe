#include <string>
#include <fstream>

#include <PullParser.h>

using namespace std;
using namespace xmlcpp;

void process_start_document(PullParser& pp);
void process_start_tag(PullParser& pp);
void process_end_tag(PullParser& pp);
void process_text(PullParser& pp);

int main(int argc, char* argv[])
{
  if (argc != 2) {
    cout << "usage: " << argv[0] << " xmlfilename\n";
    cout << "    more pretty printout, try 'xmlprinter -p' in this directory\n";
    return 1;
  }
  ifstream fs(argv[1]);

  PullParser pp;
  pp.set_feature(PullParser::PROCESS_NAMESPACES);

  pp.set_input(fs);

  PullParser::event_type evt = pp.get_event_type();
  do {
    switch (evt) {
    case PullParser::START_DOCUMENT:
      process_start_document(pp);
      break;
    case PullParser::START_TAG:
      process_start_tag(pp);
      break;
    case PullParser::END_TAG:
      process_end_tag(pp);
      break;
    case PullParser::TEXT:
      process_text(pp);
      break;
    default:
      break;
    }
    evt = pp.next();
  } while (evt != PullParser::END_DOCUMENT);
}

void process_start_document(PullParser& pp)
{
  wcout << L"<?xml";
  wcout << L" version=\"" << pp.get_version() << L"\"";
 
  if (!pp.get_encoding().empty())
    wcout << L" encoding=\"" << pp.get_encoding() << L"\"";
 
  if (!pp.get_standalone().empty())
    wcout << L" standalone\"" << pp.get_standalone() << L"\"";
 
  wcout << L" ?>";
}

void process_start_tag(PullParser& pp)
{
  wcout << L"<" << pp.get_name();
  for (int i = 0; i < pp.get_attr_size(); ++i) {
    wcout << L" " << pp.get_attr_name(i)
	  << L"=\"" << pp.get_attr_value(i) << L"\"";
  }
 
  if (pp.is_empty_elem_tag()) {
    wcout << L" />";
    pp.next(); // consume next END_TAG token.
  }
  else {
    wcout << L">";
  }
}
 
void process_end_tag(PullParser& pp)
{
  wcout << L"</" << pp.get_name() << L">";
}

void process_text(PullParser& pp)
{
  wcout << pp.get_text();
}
