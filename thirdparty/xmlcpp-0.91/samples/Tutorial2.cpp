#include <string>
#include <fstream>

#include <Serializer.h>

using namespace std;
using namespace xmlcpp;

int main(int argc, char* argv[])
{
  // test indentation
  {
    char* filename = "__test_delete_me1__.xml";
    if (argc > 2)
      filename = argv[1];

    ofstream fs(filename);

    Serializer sr;
    sr.set_feature(Serializer::LINE_SEPARATOR);
    sr.set_feature(Serializer::INDENTATION);

    sr.set_output("UTF-8", fs);

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

    fs.close();
  }

  // test attribute and namespace
  {
    char* filename = "__test_delete_me2__.xml";
    if (argc > 3)
      filename = argv[2];

    ofstream fs(filename);

    Serializer sr;
    sr.set_feature(Serializer::LINE_SEPARATOR);
    sr.set_feature(Serializer::INDENTATION);

    sr.set_output("UTF-8", fs);

    sr.start_document(L"UTF-8", L"");

    sr.set_prefix(L"ns", L"http://ideathinking.com");

    sr.start_tag(L"http://ideathinking.com", L"name");
    sr.attribute(L"", L"id", L"10");
    sr.attribute(L"http://ideathinking.com", L"id", L"20");
    sr.end_tag(L"http://ideathinking.com", L"name");

    sr.end_document();

    fs.close();
  }
}
