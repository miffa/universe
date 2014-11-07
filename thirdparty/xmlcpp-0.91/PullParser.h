/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_PULLPARSER_H
#define XMLCPP_PULLPARSER_H

#include <string>
#include <istream>

#include "StreamCharDecoder.h"
#include "CStrCharDecoder.h"
#include "IterPairCharDecoder.h"

namespace xmlcpp
{

/**
 * PullParser class. It parses XML files with pull style unlike push
 * style like SAX. It implements XmlPull API v1. (see
 * http://www.xmlpull.org) You can parse XML input element by element,
 * and you can stop parsing at anytime. It support XML namespace. But
 * to use this feature, you should set the PROCESS_NAMESPACES feature
 * first. It uses std::wstring_type for elemenets' name and value to
 * support full unicode character set.
 */
class PullParser : private boost::noncopyable
{
 public:
  /**
   * typdef for std::wstring.
   */
  typedef std::basic_string<wchar_t> wstring_type;

  /**
   * feature_type enum. PullParser implements XmlPull v1 API feature
   * set. But PROCESS_DOCDECL and VALIDATION are not supported
   * yet. (see http://www.xmlpull.org/v1/doc/features.html)
   *
   * @see set_feature()
   * @see get_feature()
   */
  enum feature_type
  {
    PROCESS_NAMESPACES,  //!< Set namespace awareness flag.
    PROCESS_DOCDECL,     //!< Not supported yet.
    VALIDATION,          //!< Not supported yet.
    REPORT_NAMESPACE_ATTRIBUTES, //!< Report namespace decl as an attribute.
    MULTIPLE_ROOT,       //!< Allow multiple root element.
    SIZE_OF_FEATURE_TYPE //!< Internal use only.
  };

  /**
   * Set or reset feature.
   *
   * @param type feature_type.
   * @param state true or false.
   */
  void set_feature(feature_type type, bool state = true);

  /**
   * Get feature.
   *
   * @param type feature_type.
   */
  bool get_feature(feature_type type);

  /**
   * event_type enum. PullParser's state is one of these types. At
   * start, it's in START_DOCUMENT state (before the first next(), but
   * after the first enxt_token()). You can change PullParser's state
   * with using next() or next_token() functions.
   *
   * @see next()
   * @see next_token()
   * @see http://www.w3.org/TR/REC-xml/
   */
  enum event_type
  {
    START_DOCUMENT,         //!< Signalize that parser is at the very
                            //!< beginning of the document and nothing
                            //!< was read yet.
    END_DOCUMENT,           //!< Logical end of the xml document. 
    START_TAG,              //!< STag
    END_TAG,                //!< ETag
    TEXT,                   //!< Text
    IGNORABLE_WHITESPACE,   //!< Space
    CDSECT,                 //!< CDSect
    PROCESSING_INSTRUCTION, //!< PI
    COMMENT,                //!< Comment
    ENTITY_REF,             //!< EntityRef
    DOCDECL,                //!< DocumentTypeDeclaration
    SE_TAG,                 //!< EmptyElemTag - This type is not
                            //!< reported directly. It reported as two
                            //!< types; START_TAG and END_TAG.
    UNKNOWN_EVENT           //!< Internal use only.
  };

  /**
   * Returns C string representation of event_type enum.
   *
   * @param type event_type.
   */
  static char const* event_type_c_str(event_type type);

  /**
   * Constructor.
   */
  PullParser();

  /**
   * Returns the version of XML file.
   */
  wstring_type get_version() const;

  /**
   * Returns the encoding type of XML file.
   */
  wstring_type get_encoding() const;

  /**
   * Returns the standalone option of XML file.
   */
  wstring_type get_standalone() const;

  /**
   * Returns the type of the current event.
   */
  event_type get_event_type() const;

  /**
   * Returns the prefix of the current element.
   */
  wstring_type get_prefix() const;

  /**
   * For START_TAG or END_TAG events, the (local) name of the current
   * element is returned when namespaces are enabled.
   */
  wstring_type get_name() const;

  /**
   * Returns the namespace URI of the current element.
   */
  wstring_type get_namespace() const;

  /**
   * Returns the text content of the current event. It expands
   * EntityRef.
   *
   * @see get_raw_text()
   */
  wstring_type get_text() const;

  /**
   * Returns the text content of the current event.
   *
   * @see get_text()
   */
  wstring_type get_raw_text() const;

  /**
   * Returns the number of attributes of the current start tag, or -1
   * if the current event type is not START_TAG
   */
  int get_attr_size() const;

  /**
   * Returns the namespace URI of the attribute with the given index
   * (starts from 0). If index is out of bound, it throw exception.
   *
   * @param index 0-based index of attributes.
   */
  wstring_type get_attr_namespace(int index) const;

  /**
   * Returns the prefix of the specified attribute Returns null string
   * if the element has no prefix. If index is out of bound, it throw
   * exception.
   *
   * @param index 0-based index of attributes.
   */
  wstring_type get_attr_prefix(int index) const;

  /**
   * Returns the local name of the specified attribute if namespaces
   * are enabled or just attribute name if namespaces are disabled. If
   * index is out of bound, it throw exception.
   *
   * @param index 0-based index of attributes.
   */
  wstring_type get_attr_name(int index) const;

  /**
   * Returns the given attributes value. If index is out of bound, it
   * throw exception.
   *
   * @param index 0-based index of attributes.
   */
  wstring_type get_attr_value(int index) const;

  /**
   * Returns the attributes value identified by namespace URI and
   * namespace local name.
   *
   * @param ns attribute's namespace.
   * @param name attribute's name.
   */
  wstring_type get_attr_value(wstring_type ns, wstring_type name) const;

  /**
   * Returns true if the current event is START_TAG and the tag is
   * degenerated (&lt;name />).
   */
  bool is_empty_elem_tag() const;

  /**
   * Returns the current depth of the element.
   * <pre>
   * &lt;!-- outside -->     0
   * &lt;root>               1
   *   sometext           1
   *   &lt;foobar>           2
   *   &lt;/foobar>          2
   * &lt;/root>              1
   * &lt;!-- outside -->     0
   * </pre>
   */
  int get_depth() const;

  /**
   * Get next parsing event. Element content will be coalesced and
   * only one TEXT event must be returned for whole element content
   * (comments and processing instructions will be ignored and entity
   * references must be expanded or exception must be thrown if entity
   * reference can not be exapnded).
   */
  event_type next();

  /**
   * This function works similarly to next() but will expose
   * additional event types (COMMENT, CDSECT, DOCDECL, ENTITY_REF,
   * PROCESSING_INSTRUCTION, or IGNORABLE_WHITESPACE) if they are
   * available in input.
   *
   * @see next_token()
   */
  event_type next_token();

  /**
   * Set new value for entity replacement text as defined in XML 1.0
   * Section 4.5 Construction of Internal Entity Replacement Text. If
   * you define 'copyright' to 'Copyright (a) Wongoo Lee', you can use
   * '&amp;copylight;' instead of long text.
   *
   * @param name entity ref name.
   * @param text entity ref replacement text.
   */
  void define_entity_replacement_text(wstring_type const& name,
				      wstring_type const& text);

  /**
   * Sets the istream that the parser is going to process.
   *
   * @param is input stream object.
   * @param fromcode if null, PullParser try to find the encoding.
   * @param ignorexmldeclencoding if true, the encoding type in the
   * XML input will be ignored.
   */
  template <class T>
    void set_input(std::basic_istream<T>& is,
		   char const* fromcode = 0,
		   bool ignorexmldeclencoding = false)
    {
      typedef std::basic_istream<T> S;
      set_input(boost::shared_ptr<CharDecoder>(
        new BasicStreamCharDecoder<S>(is,
				      fromcode,
				      false, 1024, true)
					       ),
		ignorexmldeclencoding);
    }

  /**
   * Sets the C string (null terminated) that the parser is going to
   * process.
   *
   * @param s input C string (null terminated) pointer to read.
   * @param fromcode if null, PullParser try to find the encoding.
   * @param ignorexmldeclencoding if true, the encoding type in the
   * XML input will be ignored.
   */
  template <class T>
    void set_input(T const* s,
		   char const* fromcode = 0,
		   bool ignorexmldeclencoding = false)
    {
      set_input(boost::shared_ptr<CharDecoder>(
        new BasicCStrCharDecoder<T>(s,
				    fromcode,
				    false, 1024, true)
					       ),
		ignorexmldeclencoding);
    }

  /**
   * Sets the iterator pair that the parser is going to process.
   *
   * @param first the first position of input iterator pair.
   * @param last the last position of input iterator pair.
   * @param fromcode if null, PullParser try to find the encoding.
   * @param ignorexmldeclencoding if true, the encoding type in the
   * XML input will be ignored.
   */
  template <class Iter>
    void set_input(Iter first, Iter last,
		   char const* fromcode = 0,
		   bool ignorexmldeclencoding = false)
    {
      set_input(boost::shared_ptr<CharDecoder>(
        new IterPairCharDecoder<Iter>(first, last,
				      fromcode,
				      false, 1024, true)
					       ),
		ignorexmldeclencoding);
    }

  /**
   * Sets the CharDecoder that the parser is going to process. All
   * other set_input() functions use this internaly. If you make new
   * CharDecoder class, you can use your class with this function.
   *
   * @param decoder CharDecoder object.
   * @param ignorexmldeclencoding if true, the encoding type in the
   * XML input will be ignored.
   */
  void set_input(boost::shared_ptr<CharDecoder> decoder, 
		 bool ignorexmldeclencoding);

 private:
  boost::shared_ptr<struct PullParserImpl> pimpl_;
};

}

#endif

