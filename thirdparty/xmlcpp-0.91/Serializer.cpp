/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Serializer.h"

#include <set>

#include "PullParserError.h"
#include "Token.h"

namespace xmlcpp
{
  
using namespace std;

struct SerializerImpl
{
  typedef Serializer::string_type string_type;
  typedef Serializer::wstring_type wstring_type;
  typedef detail::QName QName;

  typedef bitset<Serializer::SIZE_OF_FEATURE_TYPE> fset_t;
  typedef map<wstring_type, wstring_type> ns_t;
  typedef vector<ns_t> nns_t; // nested ns_t
  typedef vector<QName> ntag_t; // nested tag
  typedef set<QName> attr_t; // nested tag

  enum event_type
  {
    NONE,
    START_TAG,
    END_TAG
  };

  boost::shared_ptr<CharEncoder> encoder;

  fset_t fset;
  nns_t nns;
  ntag_t ntag;

  ns_t nextns;
  wstring_type indent;
  wstring_type newline;

  event_type prevtype;
  wstring_type currbuff;
  attr_t attr;

  SerializerImpl()
  {
    fset.reset();
    nns.push_back(ns_t());

    newline = L"\n";
    indent = L"\t";

    prevtype = NONE;
    currbuff.clear();
    attr.clear();
  }

  void write(wstring_type const& str,
	     bool nolinesep = false,
	     int indentoffset = 0)
  {
    wstring_type doc(str);
    if (fset[Serializer::LINE_SEPARATOR] && !nolinesep) {
      doc += newline;
      if (fset[Serializer::INDENTATION]) {
	int depth = get_depth() + indentoffset;
	if (depth > 0) {
	  wstring_type ind;
	  for (int i = 0; i < depth; ++i) {
	    ind += indent;
	  }
	  doc = ind + doc;
	}
      }
    }

    encoder->write(doc);
  }

  void end_prev(int indentoffset = -1)
  {
    if (prevtype == START_TAG) {
      currbuff += L">";
      write(currbuff, false, indentoffset);
    }
    prevtype = NONE;
    currbuff.clear();
    attr.clear();
  }

  void set_feature(Serializer::feature_type type, bool state)
  {
    fset[type] = state;
  }

  bool get_feature(Serializer::feature_type type)
  {
    return fset[type];
  }

  void set_prefix(wstring_type const& prefix, wstring_type const& ns)
  {
    assert(!ns.empty());

    nextns[ns] = prefix;
  }

  wstring_type get_prefix(wstring_type const& ns) const
  {
    return find_prefix(ns);
  }

  wstring_type escape_text(wstring_type const& text) const
  {
    wstring_type str;
    for (wstring_type::const_iterator i = text.begin(); i != text.end(); ++i) {
      if (*i == L'&') str += L"&amp;";
      else if (*i == L'<') str+= L"&lt;";
      else if (*i == L'>') str+= L"&gt;";
      else if (*i == L'\'') str+= L"&apos;";
      else if (*i == L'\"') str+= L"&quot;";
      else str += *i;
    }
    return str;
  }

  wstring_type escape_attr_text(wstring_type const& text) const
  {
    bool apos = fset[Serializer::ATTRIBUTE_USE_APOSTROPHE];

    wstring_type str;
    for (wstring_type::const_iterator i = text.begin(); i != text.end(); ++i) {
      if (*i == L'&') str += L"&amp;";
      else if (*i == L'<') str += L"&lt;";
      else if (*i == L'>') str += L"&gt;";
      else if (*i == L'\'') {
	if (apos) str += L"&apos;";
	else str += *i;
      }
      else if (*i == L'\"') {
	if (!apos) str += L"&quot;";
	else str += *i;
      }
      else str += *i;
    }
    return str;
  }

  wstring_type find_prefix(wstring_type const& ns) const
  {
    if (ns.empty()) return wstring_type();

    for (nns_t::const_reverse_iterator i = nns.rbegin(); i != nns.rend(); ++i) {
      ns_t::const_iterator j = (*i).find(ns);
      if (j != (*i).end()) return (*j).second;
    }

    throw PullParserError0(Exception::E_PP_NS_NOT_SET);
  }

  void start_document(wstring_type const& encoding,
		      wstring_type const& standalone)
  {
    wstring_type doc(L"<?xml version=\"1.0\"");
    if (!encoding.empty())
      doc += L" encoding=\"" + encoding + L"\"";
    if (!standalone.empty())
      doc += L" standalone=\"" + standalone + L"\"";
    doc += L"?>";

    write(doc);
  }

  void end_document()
  {
    end_prev();
    encoder->flush();
  }

  void start_tag(wstring_type const& ns, wstring_type const& name)
  {
    end_prev();

    nns.push_back(nextns);
    ns_t().swap(nextns);

    QName qname(ns, L"", name);
    ntag.push_back(qname);

    currbuff = L"<";
    wstring_type pfx = find_prefix(ns);
    if (pfx.empty())
      currbuff += name;
    else
      currbuff += pfx + L':' + name;

    ns_t& currns = nns.back();
    for (ns_t::iterator i = currns.begin(); i != currns.end(); ++i) {
      if ((*i).second.empty()) {
	currbuff += L" xmlns=\"" + (*i).first + L"\"";;
      }
      else {
	currbuff += L" xmlns:" + (*i).second + L"=\"" + (*i).first + L"\"";
      }
    }

    prevtype = START_TAG;
  }

  void end_tag(wstring_type const& ns,
	       wstring_type const& name,
	       bool nouseemptyelemtag)
  {
    QName qname(ns, L"", name);

    if (ntag.back() != qname)
      throw PullParserError0(Exception::E_PP_TAG_NOT_NESTED);
    ntag.pop_back();

    if (prevtype == START_TAG && !nouseemptyelemtag) {
      currbuff += L"/"; // L">" is appended in end_prev().
      end_prev(0);
    }
    else {
      wstring_type doc(L"</");
      wstring_type pfx = find_prefix(ns);
      if (pfx.empty())
	doc += name;
      else
	doc += pfx + L':' + name;
      doc += L'>';

      nns.pop_back();

      if (prevtype == START_TAG) {
	end_prev(0);
	write(doc, false, 0);
      }
      else {
	write(doc, false, -1);
      }
    }

    prevtype = END_TAG;
  }

  void attribute(wstring_type const& ns,
		 wstring_type const& name,
		 wstring_type const& value)
  {
    if (prevtype != START_TAG)
      throw PullParserError0(Exception::E_PP_INV_CALL_IN_EVNT);

    QName qname(ns, L"", name);
    if (attr.find(qname) != attr.end())
      throw PullParserError0(Exception::E_PP_ATTNAME_DUPL);
    attr.insert(qname);

    currbuff += L" ";

    wstring_type quot(L"\"");
    if (fset[Serializer::ATTRIBUTE_USE_APOSTROPHE]) {
      quot = L"\'";
    }

    wstring_type pfx = find_prefix(ns);
    if (pfx.empty())
      currbuff += name + L"="
	+ quot + escape_attr_text(value) + quot;
    else
      currbuff += pfx + L':' + name + L"="
	+ quot + escape_attr_text(value) + quot;
  }

  void cdsect(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(L"<![CDATA[");
    doc += text;
    doc += L"]]>";
    write(doc);
  }

  void comment(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(L"<!--");
    doc += text;
    doc += L"-->";
    write(doc);
  }

  void docdecl(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(L"<!DOCTYPE");
    doc += text;
    doc += L">";
    write(doc);
  }

  void entity_ref(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(text);
    write(doc, true);
  }

  void ignorable_whitespace(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(text);
    write(doc, true);
  }

  void processing_instruction(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(L"<?");
    doc += text;
    doc += L"?>";
    write(doc);
  }

  void text(wstring_type const& text)
  {
    end_prev();

    wstring_type doc(escape_text(text));
    write(doc);
  }

  wstring_type get_name() const
  {
    if (ntag.empty()) return wstring_type();

    QName const& name = ntag.back();
    return name.localpart;
  }

  wstring_type get_namespace() const
  {
    if (ntag.empty()) return wstring_type();

    QName const& name = ntag.back();
    return name.ns;
  }

  int get_depth() const
  {
    if (prevtype == END_TAG) return ntag.size() + 1;

    return ntag.size();
  }

  void flush()
  {
    encoder->flush();
  }

  void set_output(boost::shared_ptr<CharEncoder> enc, bool writebom)
  {
    encoder = enc;
    if (writebom) {
      wstring_type bom(1, 0xFEFF);
      write(bom, true);
    }
  }
};

void Serializer::set_feature(feature_type type, bool state)
{
  return pimpl_->set_feature(type, state);
}

bool Serializer::get_feature(feature_type type)
{
  return pimpl_->get_feature(type);
}

Serializer::Serializer()
  : pimpl_(new SerializerImpl)
{
}

void Serializer::set_prefix(wstring_type const& prefix, wstring_type const& ns)
{
  pimpl_->set_prefix(prefix, ns);
}

Serializer::wstring_type Serializer::get_prefix(wstring_type const& ns) const
{
  return pimpl_->get_prefix(ns);
}

void Serializer::start_document(wstring_type const& encoding,
				wstring_type const& standalone)
{
  pimpl_->start_document(encoding, standalone);
}

void Serializer::end_document()
{
  pimpl_->end_document();
}

Serializer& Serializer::start_tag(wstring_type const& ns,
				  wstring_type const& name)
{
  pimpl_->start_tag(ns, name);
  return *this;
}

Serializer& Serializer::end_tag(wstring_type const& ns,
				wstring_type const& name,
				bool nouseemptyelemtag)
{
  pimpl_->end_tag(ns, name, nouseemptyelemtag);
  return *this;
}

Serializer& Serializer::attribute(wstring_type const& ns,
				  wstring_type const& name,
				  wstring_type const& value)
{
  pimpl_->attribute(ns, name, value);
  return *this;
}

Serializer& Serializer::cdsect(wstring_type const& text)
{
  pimpl_->cdsect(text);
  return *this;
}

Serializer& Serializer::comment(wstring_type const& text)
{
  pimpl_->comment(text);
  return *this;
}

Serializer& Serializer::docdecl(wstring_type const& text)
{
  pimpl_->docdecl(text);
  return *this;
}

Serializer& Serializer::entity_ref(wstring_type const& text)
{
  pimpl_->entity_ref(text);
  return *this;
}

Serializer& Serializer::ignorable_whitespace(wstring_type const& text)
{
  pimpl_->ignorable_whitespace(text);
  return *this;
}

Serializer& Serializer::processing_instruction(wstring_type const& text)
{
  pimpl_->processing_instruction(text);
  return *this;
}

Serializer& Serializer::text(wstring_type const& text)
{
  pimpl_->text(text);
  return *this;
}

Serializer::wstring_type Serializer::get_name() const
{
  return pimpl_->get_name();
}

Serializer::wstring_type Serializer::get_namespace() const
{
  return pimpl_->get_namespace();
}

int Serializer::get_depth() const
{
  return pimpl_->get_depth();
}

void Serializer::flush()
{
  pimpl_->flush();
}


void Serializer::set_output(boost::shared_ptr<CharEncoder> encoder,
			    bool writebom)
{
  pimpl_->set_output(encoder, writebom);
}

}

