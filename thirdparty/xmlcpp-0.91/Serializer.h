/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_SERIALIZER_H
#define XMLCPP_SERIALIZER_H

#include <string>
#include <ostream>
#include <boost/shared_ptr.hpp>

#include "CStrCharEncoder.h"
#include "StreamCharEncoder.h"

namespace xmlcpp
{

/**
 * Serializer class. It writes XML files. Serializer implements
 * XmlPull API v1. (see http://www.xmlpull.org)
 */
class Serializer : private boost::noncopyable
{
 public:
  /**
   * typdef for std::string.
   */
  typedef std::basic_string<char> string_type;
  /**
   * typdef for std::wstring.
   */
  typedef std::basic_string<wchar_t> wstring_type;

  /**
   * feature_type enum. PullParser implements XmlPull v1 API feature
   * set. (see http://xmlpull.org/v1/doc/properties.html)
   *
   * @see set_feature()
   * @see get_feature()
   */
  enum feature_type
  {
    LINE_SEPARATOR,           //!< Write new line character after each
                              //!< element was written automatically.
    INDENTATION,              //!< Indent according to current
                              //!< depth. It should be set with
                              //!< LINE_SEPARATOR also.
    ATTRIBUTE_USE_APOSTROPHE, //!< Use apostrophe instead of quotation
                              //!< mark to write attribute value.
    SIZE_OF_FEATURE_TYPE      //!< Internal use only.
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
   * Constructor.
   */
  Serializer();

  /**
   * Set namespace prefix. If you want to use namespace, you should
   * set its prefix before using. Usually, it should be called just
   * before call start_tag().
   *
   * @param prefix prefix name for ns.
   * @param ns namespace.
   * @see start_tag()
   */
  void set_prefix(wstring_type const& prefix, wstring_type const& ns);

  /**
   * Returns namespace prefix.
   *
   * @param ns namespace.
   */
  wstring_type get_prefix(wstring_type const& ns) const;

  /**
   * Write &lt;?xml declaration with encoding (if encoding not null)
   * and standalone flag (if standalone not null) This method can only
   * be called just after set_output.
   *
   * @param encoding XML encoding type.
   * @param standalone XML standalone type; "yes" or "no".
   */
  void start_document(wstring_type const& encoding,
		      wstring_type const& standalone);

  /**
   * Finish writing.
   */
  void end_document();

  /**
   * Writes a start tag with the given namespace and name.
   *
   * @param ns namespace. (do not use prefix name.)
   * @param name tag name.
   */
  Serializer& start_tag(wstring_type const& ns,
			wstring_type const& name);

  /**
   * Write end tag. If it is called just after start_tag(), it write
   * empty element tag. (&lt;name />) If you do not want to do this,
   * set the noyseemptyelemtag parameter true.
   *
   * @param ns namespace. (do not use prefix name.)
   * @param name tag name.
   * @param nouseemptyelemtag if true, it'll close start tag first,
   * and write end tag.
   * @see start_tag()
   */
  Serializer& end_tag(wstring_type const& ns, wstring_type const& name,
		      bool nouseemptyelemtag = false);

  /**
   * Write an attribute.
   *
   * @param ns namespace. (do not use prefix name.)
   * @param name attribute name.
   * @param value attribute value.
   */
  Serializer& attribute(wstring_type const& ns,
			wstring_type const& name,
			wstring_type const& value);

  /**
   * Write cdata section. It does not escape special character unlike
   * text().
   *
   * @param text string to write.
   * @see text()
   */
  Serializer& cdsect(wstring_type const& text);

  /**
   * Write comment.
   *
   * @param text string to write.
   */
  Serializer& comment(wstring_type const& text);

  /**
   * Write docuement declaration.
   *
   * @param text string to write.
   */
  Serializer& docdecl(wstring_type const& text);

  /**
   * Write entity ref.
   *
   * @param text string to write.
   */
  Serializer& entity_ref(wstring_type const& text);

  /**
   * Write white space.
   *
   * @param text string to write.
   */
  Serializer& ignorable_whitespace(wstring_type const& text);

  /**
   * Write processing instruction.
   *
   * @param text string to write.
   */
  Serializer& processing_instruction(wstring_type const& text);

  /**
   * Write text.
   *
   * @param text string to write.
   */
  Serializer& text(wstring_type const& text);

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
   * Flush the buffer. If you flush after start_tag(), and call
   * end_tag(), it'll not write empty element tag.
   */
  void flush();

  /**
   * Sets the ostream that the parser is going to writes.
   *
   * @param encoding encoding type to write.
   * @param os output stream object.
   * @param writebom if true, BOM (Byte Order Maek) is written.
   */
  template <class T>
    void set_output(string_type const& encoding,
		    std::basic_ostream<T>& os,
		    bool writebom = true)
    {
      typedef std::basic_ostream<T> S;
      set_output(boost::shared_ptr<CharEncoder>(
        new BasicStreamCharEncoder<S>(encoding, os, 1024)
						),
		 writebom);
    }

  /**
   * Sets the C string (null terminated) that the parser is going to
   * writes.
   *
   * @param encoding encoding type to write.
   * @param s C string (null terminated) pointer to write.
   * @param length s's buffer size.
   * @param writebom if true, BOM (Byte Order Maek) is written.
   */
  template <class T>
    void set_output(string_type const& encoding, T* s, size_t length,
		    bool writebom = false)
    {
      set_output(boost::shared_ptr<CharEncoder>(
        new BasicCStrCharEncoder<T>(encoding, s, length, 1024)
						),
		 writebom);
    }

  /**
   * Sets the CharEncoder that the parser is going to writes. All
   * other set_output() functions use this internaly. If you make new
   * CharEncoder class, you can use your class with this function.
   *
   * @param encoder CharEncoder object.
   * @param writebom if true, BOM (Byte Order Maek) is written.
   */
  void set_output(boost::shared_ptr<CharEncoder> encoder,
		  bool writebom);

 private:
  boost::shared_ptr<struct SerializerImpl> pimpl_;
};

}

#endif
