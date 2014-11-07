/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "PullParser.h"

#include <map>
#include <list>
#include <bitset>
#include <vector>
#include <boost/spirit.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/utility.hpp>
#include <boost/operators.hpp>

#include "CodeConv.h"
#include "CharDecoderIterator.h"
#include "PullParserError.h"
#include "Token.h"

namespace xmlcpp
{
  
using namespace std;

struct PullParserImpl
{
  struct Token : detail::Token
  {
    typedef detail::Token super_t;
    typedef std::bitset<PullParser::SIZE_OF_FEATURE_TYPE> fset_t;

    fset_t fset;

    PullParser::event_type type;

    Token()
      : type(PullParser::START_DOCUMENT) {
      isemptyelemtag = false;
    }

    Token(fset_t const& fs)
      : fset(fs), type(PullParser::START_DOCUMENT) {
      isemptyelemtag = false;
    }

    void swap(Token& o) {
      std::swap(*this, o);
    }
  };

  typedef PullParser::wstring_type wstring_type;
  typedef detail::QName QName;

  typedef list<Token> tokenlist_t;
  typedef vector<Token::ns_t> nns_t; // nested ns_t
  typedef vector<QName> ntag_t; // nested tag
  typedef map<wstring_type, wstring_type> entityrepl_t;
  typedef Token::fset_t fset_t;

  boost::shared_ptr<CharDecoder> decoder;
  CharDecoderIterator first, last;

  wstring_type version;
  wstring_type encoding;
  wstring_type standalone;

  Token curr;

  tokenlist_t nexttokens;
  nns_t nns;
  ntag_t ntag;
  entityrepl_t entrepl;
  fset_t fset;

  bool rootclosed;

  PullParserImpl() : rootclosed(false)
  {
    // set each feature's default value.
    fset.reset(); // all false
  }

  // namespace
  wstring_type ns_find(wstring_type const& key) const
  {
    typedef nns_t::const_reverse_iterator cr_iterator;
    for(cr_iterator i = nns.rbegin(); i != nns.rend(); ++i) {
      Token::ns_t::const_iterator j = (*i).find(key);
      if (j != (*i).end()) return (*j).second;
    }

    return wstring_type();
  }

  wstring_type normalize_value(wstring_type const& str) const
  {
    wstring_type value(str);
    size_t first = 0;
    do {
      first = value.find(L'&', first);
      if (first == wstring_type::npos) break;

      size_t last = value.find(L';', first);
      if (last == wstring_type::npos) break;

      wstring_type::iterator beg = value.begin();

      wstring_type eref =
	get_entity_ref(wstring_type(beg + first, beg + last + 1));

      value =
	wstring_type(beg, beg + first) +
	eref +
	wstring_type(beg + last + 1, value.end());

      first = first + eref.length();
    } while (true);

    return value;
  }

  void normalize_attr_value(Token& t) const
  {
    // see http://www.w3.org/TR/REC-xml/#AVNormalize
    // this function do not normalize white spaces.
    typedef Token::attr_t::iterator iterator;
    for (iterator i = t.attr.begin(); i != t.attr.end(); ++i) {
      (*i).second = normalize_value((*i).second);
    }
  }

  void fill_attr_namespace(Token& t) const
  {
    typedef Token::attr_t::iterator iterator;
    for (iterator i = t.attr.begin(); i != t.attr.end(); ++i) {
      // default namespace is not applied to attribute.
      // see http://www.w3.org/TR/REC-xml-names/#defaulting
      if (!(*i).first.prefix.empty()) {
	fill_qname_namespace((*i).first);
      }
    }

    for (iterator i = t.attr.begin(); i != t.attr.end(); ++i) {
      if (t.attrindex.end() !=
	  t.attrindex.find(make_pair((*i).first.ns, (*i).first.localpart))) {
	throw PullParserError0(Exception::E_PP_ATTNAME_DUPL);
      }

      t.attrindex[make_pair((*i).first.ns, (*i).first.localpart)] = i;
    }
  }

  void fill_qname_namespace(QName& n) const
  {
    n.ns = ns_find(n.prefix);
  }

  int get_attr_size() const
  {
    if (curr.type != PullParser::START_TAG) return -1;
    return curr.attr.size();
  }

  wstring_type get_attr_value(wstring_type ns,
			      wstring_type name) const
  {
    Token::attr_index_t::const_iterator i =
      curr.attrindex.find(make_pair(ns, name));

    if (i == curr.attrindex.end())
      throw PullParserError0(Exception::E_PP_ATTNAME_NOT_FOUND);

    return (*((*i).second)).second;
  }

  bool is_empty_elem_tag() const
  {
    if (curr.type != PullParser::START_TAG)
      throw PullParserError0(Exception::E_PP_INV_CALL_IN_EVNT);

    return curr.isemptyelemtag;
  }

  int get_depth() const
  {
    assert(nns.size() == ntag.size());

    if (curr.type == PullParser::END_TAG) return nns.size() + 1;

    return nns.size();
  }

  wstring_type get_entity_ref(wstring_type const& text) const
  {
    assert(!text.empty());
    assert(text[0] == L'&');
    assert(text[text.length() - 1] == L';');

    wstring_type rtext(text);

    // &ref; or &#00; or &#x00;
    if (rtext[1] != L'#') {
      if (rtext == L"&amp;") return L"&";
      if (rtext == L"&lt;") return L"<";
      if (rtext == L"&gt;") return L">";
      if (rtext == L"&apos;") return L"\'";
      if (rtext == L"&quot;") return L"\"";

      entityrepl_t::const_iterator i = entrepl.find(rtext);
      if (i != entrepl.end()) return (*i).second;

      throw PullParserError0(Exception::E_PP_NO_ENTITY_REF);
    }
    else if (rtext[2] != L'x') {
      rtext.assign(rtext.begin() + 2, rtext.end() -1);
      int n = 0;
      swscanf(rtext.c_str(), L"%d", &n);
      return wstring_type() + wchar_t(n);
    }
    else {
      rtext.assign(rtext.begin() + 3, rtext.end() -1);
      int n = 0;
      swscanf(rtext.c_str(), L"%x", &n);
      return wstring_type() + wchar_t(n);
    }
  }

  wstring_type get_text() const
  {
    if (curr.type == PullParser::ENTITY_REF) {
      return get_entity_ref(curr.rawtext);
    }

    return curr.rawtext;
  }

  PullParser::event_type lookahead_event_type()
  {
    if (nexttokens.empty()) {
      Token t;
      make_token_available(t);
      nexttokens.push_front(t);
    }

    return nexttokens.front().type;
  }
  

  PullParser::event_type next_token()
  {
    if (nexttokens.empty()) {
      make_token_available(curr);
    }
    else {
      curr = nexttokens.front();
      nexttokens.pop_front();
    }

    if (curr.type == PullParser::TEXT) {
      curr.rawtext = normalize_value(curr.rawtext);
    }
    else if (curr.type == PullParser::SE_TAG) {
      curr.type = PullParser::START_TAG;
      curr.isemptyelemtag = true;

      Token t;
      t.type = PullParser::END_TAG;
      t.name = curr.name;
      nexttokens.push_front(t);
    }

    // check nested tag
    if (curr.type == PullParser::START_TAG) {
      if (rootclosed && !fset[PullParser::MULTIPLE_ROOT]) {
	throw PullParserError0(Exception::E_PP_MULTIPLE_ROOT);
      }
      // call order is important.
      // 1. make new namespace available.
      // 2. set name space value to tag and attribute.
      // 3. push tag to tag stack.
      nns.push_back(curr.ns);

      fill_attr_namespace(curr);
      normalize_attr_value(curr);
      fill_qname_namespace(curr.name);

      ntag.push_back(curr.name);
    }
    else if (curr.type == PullParser::END_TAG) {
      // call order is important.
      // 1. set name space value to tag.
      // 2. pop tag from tag stack.
      fill_qname_namespace(curr.name);

      QName stagname = ntag.back();
      ntag.pop_back();
      nns.pop_back();

      if (stagname != curr.name) {
	throw PullParserError0(Exception::E_PP_TAG_NOT_NESTED);
      }

      if (ntag.size() == 0) {
	rootclosed = true;
      }
    }

    return curr.type;
  }

  PullParser::event_type next()
  {
    do {
      next_token();
    } while (curr.type != PullParser::START_TAG &&
	     curr.type != PullParser::END_TAG &&
	     curr.type != PullParser::TEXT &&
	     curr.type != PullParser::CDSECT &&
	     curr.type != PullParser::ENTITY_REF &&
	     curr.type != PullParser::END_DOCUMENT
	     );

    if (curr.type == PullParser::TEXT ||
	curr.type == PullParser::CDSECT ||
	curr.type == PullParser::ENTITY_REF)
      {
	wstring_type text = get_text();
	PullParser::event_type type = lookahead_event_type();
	while (type == PullParser::TEXT ||
	       type == PullParser::CDSECT ||
	       type == PullParser::ENTITY_REF)
	  {
	    next_token();
	    text += get_text();
	    type = lookahead_event_type();
	  }

	Token().swap(curr);
	curr.type = PullParser::TEXT;
	curr.rawtext = text;
      }

    return curr.type;
  }

  void set_input(boost::shared_ptr<CharDecoder> dec,
		 bool ignorexmldeclencoding)
  {
    decoder = dec;

    CharDecoderIterator(decoder).swap(first);

    if (lookahead_event_type() == PullParser::START_DOCUMENT) {
      next_token();

      version = curr.version;
      encoding = curr.encoding;
      standalone = curr.standalone;

      if (!ignorexmldeclencoding && !encoding.empty()) {
	wstring_type enc = encoding;
	boost::to_upper(enc);
	string encoding = CodeConv::to_str(enc);
	if (decoder->get_fromcode() != encoding) {
	  decoder->reset_fromcode(encoding);
	}
      }

      nexttokens.push_front(curr);
    }
    else {
      Token t;
      t.type = PullParser::START_DOCUMENT;
      nexttokens.push_front(t); // push dummy START_DOCUMENT.
    }
  }

  void define_entity_replacement_text(wstring_type const& name,
				      wstring_type const& text)
  {
    if (get_feature(PullParser::PROCESS_DOCDECL) ||
	get_feature(PullParser::VALIDATION)) {
      throw PullParserError0(Exception::E_PP_INV_CALL_IN_FEAT);
    }

    wchar_t* predefined[] = {
      L"amp", L"lt", L"gt", L"apos", L"quot"
    };

    for (size_t i = 0; i < sizeof(predefined) / sizeof(*predefined); ++i) {
      if (name == predefined[i])
	throw PullParserError0(Exception::E_PP_PREDEF_ENTITY_REF);
    }

    wstring_type nname = L'&' + name + L';';
    entrepl[nname] = text;
  }

  void set_feature(PullParser::feature_type type, bool state)
  {
    if (type == PullParser::PROCESS_DOCDECL ||
	type == PullParser::VALIDATION) {
      throw PullParserError0(Exception::E_PP_NOT_IMPL);

    }
    if (type == PullParser::PROCESS_NAMESPACES) {
      if (curr.type != PullParser::START_DOCUMENT) {
	throw PullParserError0(Exception::E_PP_INV_CALL_IN_EVNT);
      }
    }

    fset.set(type, state);
  }

  bool get_feature(PullParser::feature_type type)
  {
    return fset.test(type);
  }

  void make_token_available(Token& t);
};

void PullParser::set_feature(feature_type type, bool state)
{
  pimpl_->set_feature(type, state);
}

bool PullParser::get_feature(feature_type type)
{
  return pimpl_->get_feature(type);
}

char const* PullParser::event_type_c_str(event_type type)
{
  switch (type) {
  case START_DOCUMENT: return "START_DOCUMENT";
  case END_DOCUMENT: return "END_DOCUMENT";
  case START_TAG: return "START_TAG";
  case END_TAG: return "END_TAG";
  case TEXT: return "TEXT";
  case IGNORABLE_WHITESPACE: return "IGNORABLE_WHITESPACE";
  case CDSECT: return "CDSECT";
  case PROCESSING_INSTRUCTION: return "PROCESSING_INSTRUCTION";
  case COMMENT: return "COMMENT";
  case ENTITY_REF: return "ENTITY_REF";
  case DOCDECL: return "DOCDECL";
  case SE_TAG: return "SE_TAG";
  case UNKNOWN_EVENT:
  default:
    return "UNKNOWN_EVENT";
  }
}

PullParser::PullParser()
  : pimpl_(new PullParserImpl)
{

}

PullParser::event_type PullParser::get_event_type() const
{
  return pimpl_->curr.type;
}

PullParser::wstring_type PullParser::get_version() const
{
  return pimpl_->version;
}

PullParser::wstring_type PullParser::get_encoding() const
{
  return pimpl_->encoding;
}

PullParser::wstring_type PullParser::get_standalone() const
{
  return pimpl_->standalone;
}

PullParser::wstring_type PullParser::get_prefix() const
{
  return pimpl_->curr.name.prefix;
}

PullParser::wstring_type PullParser::get_name() const
{
  return pimpl_->curr.name.localpart;
}

PullParser::wstring_type PullParser::get_namespace() const
{
  return pimpl_->curr.name.ns;
}

PullParser::wstring_type PullParser::get_text() const
{
  return pimpl_->get_text();
}

PullParser::wstring_type PullParser::get_raw_text() const
{
  return pimpl_->curr.rawtext;
}

int PullParser::get_attr_size() const
{
  return pimpl_->get_attr_size();
}

PullParser::wstring_type PullParser::get_attr_prefix(int index) const
{
  return pimpl_->curr.attr.at(index).first.prefix;
}

PullParser::wstring_type PullParser::get_attr_namespace(int index) const
{
  return pimpl_->curr.attr.at(index).first.ns;
}

PullParser::wstring_type PullParser::get_attr_name(int index) const
{
  return pimpl_->curr.attr.at(index).first.localpart;
}

PullParser::wstring_type PullParser::get_attr_value(int index) const
{
  return pimpl_->curr.attr.at(index).second;
}

PullParser::wstring_type PullParser::get_attr_value(wstring_type ns,
						    wstring_type name) const
{
  return pimpl_->get_attr_value(ns, name);
}

bool PullParser::is_empty_elem_tag() const
{
  return pimpl_->is_empty_elem_tag();
}

int PullParser::get_depth() const
{
  return pimpl_->get_depth();
}

PullParser::event_type PullParser::next()
{
  return pimpl_->next();
}

PullParser::event_type PullParser::next_token()
{
  return pimpl_->next_token();
}

void PullParser::set_input(boost::shared_ptr<CharDecoder> decoder,
			   bool ignorexmldeclencoding)
{
  pimpl_->set_input(decoder, ignorexmldeclencoding);
}

void PullParser::define_entity_replacement_text(wstring_type const& name,
						wstring_type const& text)
{
  pimpl_->define_entity_replacement_text(name, text);
}


// Spirit parser
namespace detail
{

bool in(wchar_t ch, wchar_t l, wchar_t r)
{
  return ch >= l && ch <= r;
}

bool in(wchar_t ch, wchar_t l)
{
  return ch == l;
}

struct BaseChar_p : boost::spirit::char_parser<BaseChar_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x0041, 0x005A) || in(ch, 0x0061, 0x007A) ||
      in(ch, 0x00C0, 0x00D6) || in(ch, 0x00D8, 0x00F6) ||
      in(ch, 0x00F8, 0x00FF) || in(ch, 0x0100, 0x0131) ||
      in(ch, 0x0134, 0x013E) || in(ch, 0x0141, 0x0148) ||
      in(ch, 0x014A, 0x017E) || in(ch, 0x0180, 0x01C3) ||
      in(ch, 0x01CD, 0x01F0) || in(ch, 0x01F4, 0x01F5) ||
      in(ch, 0x01FA, 0x0217) || in(ch, 0x0250, 0x02A8) ||
      in(ch, 0x02BB, 0x02C1) || in(ch, 0x0386) || in(ch, 0x0388, 0x038A) ||
      in(ch, 0x038C) || in(ch, 0x038E, 0x03A1) || in(ch, 0x03A3, 0x03CE) ||
      in(ch, 0x03D0, 0x03D6) || in(ch, 0x03DA) || in(ch, 0x03DC) ||
      in(ch, 0x03DE) || in(ch, 0x03E0) || in(ch, 0x03E2, 0x03F3) ||
      in(ch, 0x0401, 0x040C) || in(ch, 0x040E, 0x044F) ||
      in(ch, 0x0451, 0x045C) || in(ch, 0x045E, 0x0481) ||
      in(ch, 0x0490, 0x04C4) || in(ch, 0x04C7, 0x04C8) ||
      in(ch, 0x04CB, 0x04CC) || in(ch, 0x04D0, 0x04EB) ||
      in(ch, 0x04EE, 0x04F5) || in(ch, 0x04F8, 0x04F9) ||
      in(ch, 0x0531, 0x0556) || in(ch, 0x0559) || in(ch, 0x0561, 0x0586) ||
      in(ch, 0x05D0, 0x05EA) || in(ch, 0x05F0, 0x05F2) ||
      in(ch, 0x0621, 0x063A) || in(ch, 0x0641, 0x064A) ||
      in(ch, 0x0671, 0x06B7) || in(ch, 0x06BA, 0x06BE) ||
      in(ch, 0x06C0, 0x06CE) || in(ch, 0x06D0, 0x06D3) ||
      in(ch, 0x06D5) || in(ch, 0x06E5, 0x06E6) || in(ch, 0x0905, 0x0939) ||
      in(ch, 0x093D) || in(ch, 0x0958, 0x0961) || in(ch, 0x0985, 0x098C) ||
      in(ch, 0x098F, 0x0990) || in(ch, 0x0993, 0x09A8) ||
      in(ch, 0x09AA, 0x09B0) || in(ch, 0x09B2) || in(ch, 0x09B6, 0x09B9) ||
      in(ch, 0x09DC, 0x09DD) || in(ch, 0x09DF, 0x09E1) ||
      in(ch, 0x09F0, 0x09F1) || in(ch, 0x0A05, 0x0A0A) ||
      in(ch, 0x0A0F, 0x0A10) || in(ch, 0x0A13, 0x0A28) ||
      in(ch, 0x0A2A, 0x0A30) || in(ch, 0x0A32, 0x0A33) ||
      in(ch, 0x0A35, 0x0A36) || in(ch, 0x0A38, 0x0A39) ||
      in(ch, 0x0A59, 0x0A5C) || in(ch, 0x0A5E) || in(ch, 0x0A72, 0x0A74) ||
      in(ch, 0x0A85, 0x0A8B) || in(ch, 0x0A8D) || in(ch, 0x0A8F, 0x0A91) ||
      in(ch, 0x0A93, 0x0AA8) || in(ch, 0x0AAA, 0x0AB0) ||
      in(ch, 0x0AB2, 0x0AB3) || in(ch, 0x0AB5, 0x0AB9) ||
      in(ch, 0x0ABD) || in(ch, 0x0AE0) || in(ch, 0x0B05, 0x0B0C) ||
      in(ch, 0x0B0F, 0x0B10) || in(ch, 0x0B13, 0x0B28) ||
      in(ch, 0x0B2A, 0x0B30) || in(ch, 0x0B32, 0x0B33) ||
      in(ch, 0x0B36, 0x0B39) || in(ch, 0x0B3D) || in(ch, 0x0B5C, 0x0B5D) ||
      in(ch, 0x0B5F, 0x0B61) || in(ch, 0x0B85, 0x0B8A) ||
      in(ch, 0x0B8E, 0x0B90) || in(ch, 0x0B92, 0x0B95) ||
      in(ch, 0x0B99, 0x0B9A) || in(ch, 0x0B9C) || in(ch, 0x0B9E, 0x0B9F) ||
      in(ch, 0x0BA3, 0x0BA4) || in(ch, 0x0BA8, 0x0BAA) ||
      in(ch, 0x0BAE, 0x0BB5) || in(ch, 0x0BB7, 0x0BB9) ||
      in(ch, 0x0C05, 0x0C0C) || in(ch, 0x0C0E, 0x0C10) ||
      in(ch, 0x0C12, 0x0C28) || in(ch, 0x0C2A, 0x0C33) ||
      in(ch, 0x0C35, 0x0C39) || in(ch, 0x0C60, 0x0C61) ||
      in(ch, 0x0C85, 0x0C8C) || in(ch, 0x0C8E, 0x0C90) ||
      in(ch, 0x0C92, 0x0CA8) || in(ch, 0x0CAA, 0x0CB3) ||
      in(ch, 0x0CB5, 0x0CB9) || in(ch, 0x0CDE) || in(ch, 0x0CE0, 0x0CE1) ||
      in(ch, 0x0D05, 0x0D0C) || in(ch, 0x0D0E, 0x0D10) ||
      in(ch, 0x0D12, 0x0D28) || in(ch, 0x0D2A, 0x0D39) ||
      in(ch, 0x0D60, 0x0D61) || in(ch, 0x0E01, 0x0E2E) ||
      in(ch, 0x0E30) || in(ch, 0x0E32, 0x0E33) || in(ch, 0x0E40, 0x0E45) ||
      in(ch, 0x0E81, 0x0E82) || in(ch, 0x0E84) || in(ch, 0x0E87, 0x0E88) ||
      in(ch, 0x0E8A) || in(ch, 0x0E8D) || in(ch, 0x0E94, 0x0E97) ||
      in(ch, 0x0E99, 0x0E9F) || in(ch, 0x0EA1, 0x0EA3) ||
      in(ch, 0x0EA5) || in(ch, 0x0EA7) || in(ch, 0x0EAA, 0x0EAB) ||
      in(ch, 0x0EAD, 0x0EAE) || in(ch, 0x0EB0) || in(ch, 0x0EB2, 0x0EB3) ||
      in(ch, 0x0EBD) || in(ch, 0x0EC0, 0x0EC4) || in(ch, 0x0F40, 0x0F47) ||
      in(ch, 0x0F49, 0x0F69) || in(ch, 0x10A0, 0x10C5) ||
      in(ch, 0x10D0, 0x10F6) || in(ch, 0x1100) || in(ch, 0x1102, 0x1103) ||
      in(ch, 0x1105, 0x1107) || in(ch, 0x1109) || in(ch, 0x110B, 0x110C) ||
      in(ch, 0x110E, 0x1112) || in(ch, 0x113C) || in(ch, 0x113E) ||
      in(ch, 0x1140) || in(ch, 0x114C) || in(ch, 0x114E) || in(ch, 0x1150) ||
      in(ch, 0x1154, 0x1155) || in(ch, 0x1159) || in(ch, 0x115F, 0x1161) ||
      in(ch, 0x1163) || in(ch, 0x1165) || in(ch, 0x1167) || in(ch, 0x1169) ||
      in(ch, 0x116D, 0x116E) || in(ch, 0x1172, 0x1173) ||
      in(ch, 0x1175) || in(ch, 0x119E) || in(ch, 0x11A8) || in(ch, 0x11AB) ||
      in(ch, 0x11AE, 0x11AF) || in(ch, 0x11B7, 0x11B8) ||
      in(ch, 0x11BA) || in(ch, 0x11BC, 0x11C2) || in(ch, 0x11EB) ||
      in(ch, 0x11F0) || in(ch, 0x11F9) || in(ch, 0x1E00, 0x1E9B) ||
      in(ch, 0x1EA0, 0x1EF9) || in(ch, 0x1F00, 0x1F15) ||
      in(ch, 0x1F18, 0x1F1D) || in(ch, 0x1F20, 0x1F45) ||
      in(ch, 0x1F48, 0x1F4D) || in(ch, 0x1F50, 0x1F57) ||
      in(ch, 0x1F59) || in(ch, 0x1F5B) || in(ch, 0x1F5D) ||
      in(ch, 0x1F5F, 0x1F7D) || in(ch, 0x1F80, 0x1FB4) ||
      in(ch, 0x1FB6, 0x1FBC) || in(ch, 0x1FBE) || in(ch, 0x1FC2, 0x1FC4) ||
      in(ch, 0x1FC6, 0x1FCC) || in(ch, 0x1FD0, 0x1FD3) ||
      in(ch, 0x1FD6, 0x1FDB) || in(ch, 0x1FE0, 0x1FEC) ||
      in(ch, 0x1FF2, 0x1FF4) || in(ch, 0x1FF6, 0x1FFC) ||
      in(ch, 0x2126) || in(ch, 0x212A, 0x212B) || in(ch, 0x212E) ||
      in(ch, 0x2180, 0x2182) || in(ch, 0x3041, 0x3094) ||
      in(ch, 0x30A1, 0x30FA) || in(ch, 0x3105, 0x312C) ||
      in(ch, 0xAC00, 0xD7A3)
      ;
  }
};

struct CombiningChar_p : boost::spirit::char_parser<CombiningChar_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x0300, 0x0345) || in(ch, 0x0360, 0x0361) ||
      in(ch, 0x0483, 0x0486) || in(ch, 0x0591, 0x05A1) ||
      in(ch, 0x05A3, 0x05B9) || in(ch, 0x05BB, 0x05BD) ||
      in(ch, 0x05BF) || in(ch, 0x05C1, 0x05C2) || in(ch, 0x05C4) ||
      in(ch, 0x064B, 0x0652) || in(ch, 0x0670) || in(ch, 0x06D6, 0x06DC) ||
      in(ch, 0x06DD, 0x06DF) || in(ch, 0x06E0, 0x06E4) ||
      in(ch, 0x06E7, 0x06E8) || in(ch, 0x06EA, 0x06ED) ||
      in(ch, 0x0901, 0x0903) || in(ch, 0x093C) || in(ch, 0x093E, 0x094C) ||
      in(ch, 0x094D) || in(ch, 0x0951, 0x0954) || in(ch, 0x0962, 0x0963) ||
      in(ch, 0x0981, 0x0983) || in(ch, 0x09BC) || in(ch, 0x09BE) ||
      in(ch, 0x09BF) || in(ch, 0x09C0, 0x09C4) || in(ch, 0x09C7, 0x09C8) ||
      in(ch, 0x09CB, 0x09CD) || in(ch, 0x09D7) || in(ch, 0x09E2, 0x09E3) ||
      in(ch, 0x0A02) || in(ch, 0x0A3C) || in(ch, 0x0A3E) || in(ch, 0x0A3F) ||
      in(ch, 0x0A40, 0x0A42) || in(ch, 0x0A47, 0x0A48) ||
      in(ch, 0x0A4B, 0x0A4D) || in(ch, 0x0A70, 0x0A71) ||
      in(ch, 0x0A81, 0x0A83) || in(ch, 0x0ABC) || in(ch, 0x0ABE, 0x0AC5) ||
      in(ch, 0x0AC7, 0x0AC9) || in(ch, 0x0ACB, 0x0ACD) ||
      in(ch, 0x0B01, 0x0B03) || in(ch, 0x0B3C) || in(ch, 0x0B3E, 0x0B43) ||
      in(ch, 0x0B47, 0x0B48) || in(ch, 0x0B4B, 0x0B4D) ||
      in(ch, 0x0B56, 0x0B57) || in(ch, 0x0B82, 0x0B83) ||
      in(ch, 0x0BBE, 0x0BC2) || in(ch, 0x0BC6, 0x0BC8) ||
      in(ch, 0x0BCA, 0x0BCD) || in(ch, 0x0BD7) || in(ch, 0x0C01, 0x0C03) ||
      in(ch, 0x0C3E, 0x0C44) || in(ch, 0x0C46, 0x0C48) ||
      in(ch, 0x0C4A, 0x0C4D) || in(ch, 0x0C55, 0x0C56) ||
      in(ch, 0x0C82, 0x0C83) || in(ch, 0x0CBE, 0x0CC4) ||
      in(ch, 0x0CC6, 0x0CC8) || in(ch, 0x0CCA, 0x0CCD) ||
      in(ch, 0x0CD5, 0x0CD6) || in(ch, 0x0D02, 0x0D03) ||
      in(ch, 0x0D3E, 0x0D43) || in(ch, 0x0D46, 0x0D48) ||
      in(ch, 0x0D4A, 0x0D4D) || in(ch, 0x0D57) || in(ch, 0x0E31) ||
      in(ch, 0x0E34, 0x0E3A) || in(ch, 0x0E47, 0x0E4E) || in(ch, 0x0EB1) ||
      in(ch, 0x0EB4, 0x0EB9) || in(ch, 0x0EBB, 0x0EBC) ||
      in(ch, 0x0EC8, 0x0ECD) || in(ch, 0x0F18, 0x0F19) || in(ch, 0x0F35) ||
      in(ch, 0x0F37) || in(ch, 0x0F39) || in(ch, 0x0F3E) || in(ch, 0x0F3F) ||
      in(ch, 0x0F71, 0x0F84) || in(ch, 0x0F86, 0x0F8B) ||
      in(ch, 0x0F90, 0x0F95) || in(ch, 0x0F97) || in(ch, 0x0F99, 0x0FAD) ||
      in(ch, 0x0FB1, 0x0FB7) || in(ch, 0x0FB9) || in(ch, 0x20D0, 0x20DC) ||
      in(ch, 0x20E1) || in(ch, 0x302A, 0x302F) || in(ch, 0x3099) ||
      in(ch, 0x309A)
      ;
  }
};

struct Extender_p : boost::spirit::char_parser<Extender_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x00B7) || in(ch, 0x02D0) || in(ch, 0x02D1) || in(ch, 0x0387) ||
      in(ch, 0x0640) || in(ch, 0x0E46) || in(ch, 0x0EC6) || in(ch, 0x3005) ||
      in(ch, 0x3031, 0x3035) || in(ch, 0x309D, 0x309E) ||
      in(ch, 0x30FC, 0x30FE)
      ;
  }
};

struct Digit_p : boost::spirit::char_parser<Digit_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x0030, 0x0039) || in(ch, 0x0660, 0x0669) ||
      in(ch, 0x06F0, 0x06F9) || in(ch, 0x0966, 0x096F) ||
      in(ch, 0x09E6, 0x09EF) || in(ch, 0x0A66, 0x0A6F) ||
      in(ch, 0x0AE6, 0x0AEF) || in(ch, 0x0B66, 0x0B6F) ||
      in(ch, 0x0BE7, 0x0BEF) || in(ch, 0x0C66, 0x0C6F) ||
      in(ch, 0x0CE6, 0x0CEF) || in(ch, 0x0D66, 0x0D6F) ||
      in(ch, 0x0E50, 0x0E59) || in(ch, 0x0ED0, 0x0ED9) ||
      in(ch, 0x0F20, 0x0F29)
      ;
  }
};

struct Ideographic_p : boost::spirit::char_parser<Ideographic_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x4E00, 0x9FA5) || in(ch, 0x3007) || in(ch, 0x3021, 0x3029)
      ;
  }
};

struct Char_p : boost::spirit::char_parser<Char_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x09) || in(ch, 0x0A) || in(ch, 0x0D) || in(ch, 0x20, 0x0D7FF) ||
      in(ch, 0xE000, 0xFFFD) || in(ch, 0x10000, 0x10FFFF)
      ;
  }
};

struct PubidChar_p : boost::spirit::char_parser<PubidChar_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x20) || in(ch, 0x0A) || in(ch, 0x0D) || in(ch, L'a', L'z') ||
      in(ch, L'A', L'Z') || in(ch, L'0', L'9') ||
      in(ch, L'-') || in(ch, L'\'') || in(ch, L'(') || in(ch, L')') ||
      in(ch, L'+') || in(ch, L',') || in(ch, L'.') || in(ch, L'/') ||
      in(ch, L':') || in(ch, L'=') || in(ch, L'?') || in(ch, L';') ||
      in(ch, L'!') || in(ch, L'*') || in(ch, L'#') || in(ch, L'@') ||
      in(ch, L'$') || in(ch, L'_') || in(ch, L'%')
      ;
  }
};

struct S_p : boost::spirit::char_parser<S_p>
{
  bool test(wchar_t ch) const {
    return
      in(ch, 0x20) || in(ch, 0x09) || in(ch, 0x0D) || in(ch, 0x0A)
      ;
  }
};


template <class action>
struct xmlparser : boost::spirit::grammar<xmlparser<action> >
{
  action& act;

  xmlparser(action& a)
    : act(a)
  {
  }

  template <class Tokenizer>
  struct definition
  {
    definition(xmlparser const& self)
    {
      using namespace boost::spirit;

      action& act = self.act;

      S =
	S_p()
	;

      WhiteSpaces =
	+S
	;

      Char =
	Char_p()
	;

      BaseChar =
	BaseChar_p()
	;

      Ideographic =
	Ideographic_p()
	;

      CombiningChar =
	CombiningChar_p()
	;

      Extender =
	Extender_p()
	;

      Digit =
	Digit_p()
	;

      Eq =
	!S >> L'=' >> !S
	;

      VersionNum =
	str_p(L"1.0")
	;

      EncName =
	alpha_p >> *(alnum_p | L'.' | L'_' | L'-')
	;

      SDDecl =
	S >> L"standalone" >> Eq
	  >> (
	      (ch_p(L'\'') >> (str_p(L"yes") | L"no")[act.standalone] >> L'\'' )
	      |
	      (ch_p(L'\"') >> (str_p(L"yes") | L"no")[act.standalone] >> L'\"' )
	      )
	;

      EncodingDecl =
	S >> L"encoding" >> Eq
	  >> (
	      (ch_p(L'\'') >> EncName[act.encoding] >> L'\'' )
	      |
	      (ch_p(L'\"') >> EncName[act.encoding] >> L'\"' )
	      )
	;

      VersionInfo =
	S >> L"version" >> Eq
	  >> (
	      (ch_p(L'\'') >> VersionNum[act.version] >> L'\'' )
	      |
	      (ch_p(L'\"') >> VersionNum[act.version] >> L'\"' )
	      )
	;

      XMLDecl =
	confix_p(
		 L"<?xml",
		 VersionInfo >> !EncodingDecl >> !SDDecl >> !S,
		 L"?>"
		 )
	;

      Comment =
	confix_p(
		 L"<!--",
		 ( *( ~ch_p(L'-') | (ch_p(L'-') >> ~ch_p(L'-'))) )[act.rawtext],
		 L"-->"
		 )
	;

      NameChar =
	Letter | Digit | L'.' | L'-' | L'_' | L':' |
	CombiningChar | Extender
	;

      Letter =
	BaseChar | Ideographic
	;

      Name =
	(Letter | L'_' | L':') >> *(NameChar)
	;

      EntityRef =
	confix_p(L'&', Name, L';')
	;

      CharRef =
	confix_p(L"&#", +digit_p, L';')
	|
	confix_p(L"&#x", +xdigit_p, L';')
	;

      Reference = EntityRef | CharRef
	;

      AttValue =
	confix_p(
		 L'\'',
		 (*((Char - (ch_p(L'<') | L'&')) | Reference))[act.att],
		 L'\''
		 )
	|
	confix_p(
		 L'\"',
		 (*((Char - (ch_p(L'<') | L'&')) | Reference))[act.att],
		 L'\"'
		 )
	;

      Attribute =
	Name[act.attname] >> Eq >> AttValue
	;

      CharData =
	(
	 +(+(Char - (ch_p(L'<') | L'&')) | CharRef)// ommit ']]>' handling
	 )[act.rawtext]
	;

      STag =
	ch_p(L'<')
	>> Name[act.name] >> *(S >> Attribute) >> !S
	>> (str_p(L">")[act.stag] | str_p(L"/>")[act.setag])
	;

      ETag =
	confix_p(
		 L"</",
		 Name[act.name] >> !S,
		 L'>'
		 )
	;

      CDSect =
	confix_p(
		 L"<![CDATA[",
		 (*Char)[act.rawtext],
		 L"]]>"
		 )
	;

      PITarget =
	Name - (
		(ch_p(L'X') | L'x') >>
		(ch_p(L'M') | L'm') >>
		(ch_p(L'L') | L'l')
		)
	;

      PI =
	confix_p(
		 str_p(L"<?"),
		 (PITarget >> !(S >> *Char))[act.rawtext],
		 str_p(L"?>")
		 )
	;

      SystemLiteral =
	confix_p(L'\"', Char, L'\"')
	|
	confix_p(L'\'', Char, L'\'')
	;

      PubidLiteral =
	confix_p(L'\"', PubidChar, L'\"')
	|
	confix_p(L'\'', PubidChar, L'\'')
	;

      PubidChar =
	PubidChar_p()
	;

      ExternalID =
	(str_p("SYSTEM") >> S >> SystemLiteral)
	|
	(str_p("PUBLIC") >> S >> PubidLiteral >>  S >> SystemLiteral)
	;

      PEReference =
	ch_p('%') >> Name >> L';'
	;

      DeclSep =
	PEReference | S
	;

      Mixed =
	confix_p(L'(',
		 !S >> L"#PCDATA" >> *(!S >> L'|' >> !S >> Name) >> !S,
		 L")*"
		 )
	|
	confix_p(L'(',
		 !S >> L"#PCDATA" >> !S,
		 L')'
		 )
	;

      Cp =
	(Name | Choice | Seq) >> !(ch_p(L'?') | L'*' | L'+')
	;

      Choice =
	confix_p(L'(',
		 !S >> Cp >> +(!S >> L'|' >> !S >> Cp) >> !S,
		 L')'
		 )
	;

      Seq =
	confix_p(L'(',
		 !S >> Cp >> *(!S >> L',' >> !S >> Cp) >> !S,
		 L')'
		 )
	;

      Children =
	(Choice | Seq) >> !(ch_p(L'?') | L'*' | L'+')
	;

      ContentSpec =
	str_p(L"EMPTY") | L"ANY" | Mixed | Children
	;

      ElementDecl =
	str_p(L"<!ELEMENT") >> S >> Name >> S >> ContentSpec >> !S >> L'>'
	;

      StringType =
	str_p(L"CDATA")
	;

      TokenizedType =
	str_p(L"ID") | L"IDREF" | L"IDREFS" | L"ENTITY" | L"ENTITIES" |
	L"NMTOKEN" | L"NMTOKENS"
	;

      NotationType =
	str_p(L"NOTATION") >> S >> L'(' >> !S >> Name
			   >> *(!S >> L'|' >> !S >> Name) >> !S >> L')'
	;

      Nmtoken =
	+NameChar
	;

      Nmtokens =
	Nmtoken >> *(ch_p(0x20) >> Nmtoken)
	;

      Enumeration =
	ch_p(L'(') >> !S >> Nmtoken
		   >> *(!S >> L'|' >> !S >> Nmtoken)
		   >> !S >> L')'
	;

      EnumeratedType =
	NotationType | Enumeration  
	;

      AttType =
	StringType | TokenizedType | EnumeratedType
	;

      DefaultDecl =
	str_p(L"#REQUIRED") | L"#IMPLIED" |
	(!(str_p(L"#FIXED") >> S) >> AttValue)
	;

      AttDef =
	S >> Name >> S >> AttType >> S >> DefaultDecl  
	;

      AttlistDecl =
	confix_p(L"<!ATTLIST",
		 S >> Name >> *AttDef >> !S,
		 L'>'
		 )
	;

      EntityValue =
	confix_p(L'\"',
		 *(Char - (ch_p(L'%') | L'&') | PEReference | Reference),
		 L'\"'
		 )
	|
	confix_p(L'\'',
		 *(Char - (ch_p(L'%') | L'&') | PEReference | Reference),
		 L'\''
		 )
	;

      NDataDecl =
	S >> L"NDATA" >> S >> Name  
	;

      EntityDef =
	EntityValue | (ExternalID >> !NDataDecl)
	;

      GEDecl =
	confix_p(L"<!ENTITY",
		 S >> Name >> S >> EntityDef >> !S,
		 L'>'
		 )
	;

      PEDef =
	EntityValue | ExternalID
	;

      PEDecl =
	confix_p(L"<!ENTITY",
		 S >> L'%' >> S >> Name >> S >> PEDef >> !S,
		 L'>'
		 )
	;

      EntityDecl =
	GEDecl | PEDecl
	;

      PublicID =
	str_p(L"PUBLIC") >> S >> PubidLiteral
	;

      NotationDecl =
	confix_p(L"<!NOTATION",
		 S >> Name >> S >> (ExternalID | PublicID) >> !S,
		 L'>'
		 )
	;

      MarkupDecl =
	ElementDecl | AttlistDecl | EntityDecl | NotationDecl | PI | Comment
	;

      IntSubset =
	*(MarkupDecl | DeclSep)
	;

      DocTypeDecl =
	str_p(L"<!DOCTYPE") >>
	(S >> Name
	 >> !(S >> ExternalID)
	 >> !S
	 >> !(L'[' >> IntSubset >> L']' >> !S)
	 )[act.rawtext]
	 >> L'>'
	;

      top =
	XMLDecl[act.xmldecl]
	| WhiteSpaces[act.rawtext][act.ws]
	| Comment[act.comment]
	| CharData[act.text]
	| STag
	| ETag[act.etag]
	| CDSect[act.cdsect]
	| PI[act.pi]
	| EntityRef[act.rawtext][act.entityref]
	| DocTypeDecl[act.doctypedecl]
	;
      /*
      BOOST_SPIRIT_DEBUG_NODE(STag);
      BOOST_SPIRIT_DEBUG_NODE(ETag);
      BOOST_SPIRIT_DEBUG_NODE(Name);
      BOOST_SPIRIT_DEBUG_NODE(Attribute);
      BOOST_SPIRIT_DEBUG_NODE(AttValue);
      BOOST_SPIRIT_DEBUG_NODE(Reference);
      BOOST_SPIRIT_DEBUG_NODE(S);
      */
    }

    boost::spirit::rule<Tokenizer> top;
    boost::spirit::rule<Tokenizer> XMLDecl, Comment, CharData,
      STag, ETag, CDSect, PI, DocTypeDecl,
      Name, Attribute, Letter, NameChar, AttValue, PITarget,
      Reference, EntityRef, CharRef, BaseChar, Ideographic,
      ExternalID, IntSubset, MarkupDecl, DeclSep,
      PubidLiteral, SystemLiteral, PubidChar, PEReference,
      ElementDecl, AttlistDecl, EntityDecl, NotationDecl,
      CombiningChar, Extender, Digit,
      ContentSpec, Mixed, Children, Choice, Seq, Cp,
      AttDef, AttType, DefaultDecl,
      NotationType, Enumeration, Nmtoken, Nmtokens,
      GEDecl, PEDecl,
      EntityDef, EntityValue, NDataDecl, PEDef, PublicID,
      StringType, TokenizedType, EnumeratedType,
      VersionInfo, EncodingDecl, VersionNum, EncName, SDDecl, WhiteSpaces, Eq,
      S, Char;

    boost::spirit::rule<Tokenizer> EntityCharRef; // It's not in the
						  // XML InfoSet. It's
						  // exist for
						  // PullParser's
						  // XML_ROUNDTRIP
						  // feature.


    boost::spirit::rule<Tokenizer> const& start() const { return top; }
  };
};

struct Tokenizer
{
  typedef PullParserImpl::Token Token;
  typedef PullParserImpl::QName QName;

  template <class T>
  struct SetType
  {
    SetType(T& t, T value) : t_(t), value_(value) {
    }
    template <typename Iter>
    void operator()(Iter , Iter ) const {
      t_ = value_;
    }
    T& t_;
    T value_;
  };

  struct CopyStr
  {
    CopyStr(std::wstring& s_)
      : s(s_) {
    }
    template <typename Iter>
    void operator()(Iter first, Iter last) const {
      s.assign(first, last);
    }

    std::wstring& s;
  };

  struct SetName
  {
    SetName(Token& t_)
      : t(t_) {
    }
    template <typename Iter>
    void operator()(Iter first, Iter last) const {
      std::wstring name(first, last);
      if (t.fset[PullParser::PROCESS_NAMESPACES]) {
	size_t i = name.find(L':');
	if (i == std::wstring::npos) {
	  t.name.localpart = name;
	}
	else {
	  t.name.prefix =
	    std::wstring(name.begin(), name.begin() + i);
	  t.name.localpart =
	    std::wstring(name.begin() + i + 1, name.end());
	}
      }
      else {
	t.name.localpart = name;
      }
    }

    Token& t;
  };

  struct SetAtt
  {
    SetAtt(Token& t_)
      : t(t_) {
    }
    template <typename Iter>
    void operator()(Iter first, Iter last) const {
      std::wstring attvalue(first, last);

      if (t.fset[PullParser::PROCESS_NAMESPACES]) {
	if (t.attname.find(L"xmlns:") == 0) {
	  std::wstring NCName(t.attname.begin() + 6, t.attname.end());
	  t.ns[NCName] = attvalue;
	  if (t.fset[PullParser::REPORT_NAMESPACE_ATTRIBUTES]) {
	    QName qname;
	    qname.prefix = L"xmlns";
	    qname.localpart = NCName;
	    t.attr.push_back(make_pair(qname, attvalue));
	  }
	}
	else if (t.attname == L"xmlns") {
	  t.ns[L""] = attvalue;
	  if (t.fset[PullParser::REPORT_NAMESPACE_ATTRIBUTES]) {
	    QName qname;
	    qname.localpart = L"xmlns";
	    t.attr.push_back(make_pair(qname, attvalue));
	  }
	}
	else {
	  QName qname;
	  size_t i = t.attname.find(L':');
	  if (i == std::wstring::npos) {
	    qname.localpart = t.attname;
	  }
	  else {
	    qname.prefix =
	      std::wstring(t.attname.begin(), t.attname.begin() + i);
	    qname.localpart =
	      std::wstring(t.attname.begin() + i + 1, t.attname.end());
	  }
	  t.attr.push_back(make_pair(qname, attvalue));
	}
      }
      else {
	QName qname;
	qname.localpart = t.attname;
	t.attr.push_back(make_pair(qname, attvalue));
      }
    }

    Token& t;
  };

  // ref
  Token& t;

  // action
  SetType<PullParser::event_type> xmldecl;
  SetType<PullParser::event_type> comment;
  SetType<PullParser::event_type> text;
  SetType<PullParser::event_type> stag;
  SetType<PullParser::event_type> etag;
  SetType<PullParser::event_type> setag;
  SetType<PullParser::event_type> cdsect;
  SetType<PullParser::event_type> pi;
  SetType<PullParser::event_type> entityref;
  SetType<PullParser::event_type> doctypedecl;
  SetType<PullParser::event_type> ws;
  CopyStr version;
  CopyStr encoding;
  CopyStr standalone;
  SetName name;
  CopyStr rawtext;
  CopyStr attname;
  SetAtt att;

  // ctor
  Tokenizer(Token& t_)
    : t(t_),
      xmldecl(t.type, PullParser::START_DOCUMENT),
      comment(t.type, PullParser::COMMENT),
      text(t.type, PullParser::TEXT),
      stag(t.type, PullParser::START_TAG),
      etag(t.type, PullParser::END_TAG),
      setag(t.type, PullParser::SE_TAG),
      cdsect(t.type, PullParser::CDSECT),
      pi(t.type, PullParser::PROCESSING_INSTRUCTION),
      entityref(t.type, PullParser::ENTITY_REF),
      doctypedecl(t.type, PullParser::DOCDECL),
      ws(t.type, PullParser::IGNORABLE_WHITESPACE),
      version(t.version),
      encoding(t.encoding),
      standalone(t.standalone),
      name(t),
      rawtext(t.rawtext),
      attname(t.attname),
      att(t)
  {
  }

};

} // namespace detail

void PullParserImpl::make_token_available(Token& t)
{
  using namespace boost::spirit;
  using namespace detail;

  // set feature-set to Token.
  Token(fset).swap(t);
  t.type = PullParser::UNKNOWN_EVENT;

  Tokenizer act(t);

  xmlparser<Tokenizer> xp(act);

  parse_info<CharDecoderIterator> info = parse(first, last, xp, nothing_p);

  if (first == info.stop) {
    if (first == last) {
      t.type = PullParser::END_DOCUMENT;
      if (ntag.size() != 0)
	throw PullParserError0(Exception::E_PP_TAG_UNCLOSED);
    }
    else {
      throw PullParserError0(Exception::E_PP_MATCH_NOTHING);
    }
  }

  if (t.type == PullParser::UNKNOWN_EVENT) {
    /*
      for (; first_ != info.stop; ++first_) {
      using namespace std;
      wcout << hex << (int) *first_ << endl << dec;
      }
    */
    throw PullParserError0(Exception::E_PP_MATCH_FAIL);
  }

  first = info.stop;

  first.release_before();
}

}
