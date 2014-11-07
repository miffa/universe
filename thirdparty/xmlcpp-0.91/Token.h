/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_TOKEN_H
#define XMLCPP_TOKEN_H

#include <map>
#include <bitset>
#include <vector>
#include <boost/operators.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_comparison.hpp>

namespace xmlcpp
{

namespace detail
{
  struct QName : boost::equality_comparable<QName>,
		 boost::less_than_comparable<QName>
  {
    typedef std::wstring wstring_type;

    wstring_type ns;
    wstring_type prefix;
    wstring_type localpart;

    QName();
    QName(wstring_type const& n, wstring_type const& p, wstring_type const& l);

    bool operator==(QName const& o) const;
    bool operator<(QName const& o) const;
  };

  struct Token
  {
    typedef std::wstring wstring_type;

    typedef std::vector<std::pair<QName, wstring_type> > attr_t;
    typedef std::map<std::pair<wstring_type, wstring_type>, attr_t::iterator>
      attr_index_t;
    typedef std::map<wstring_type, wstring_type> ns_t;

    wstring_type version;
    wstring_type encoding;
    wstring_type standalone;

    QName name;
    wstring_type rawtext;

    attr_t attr;
    attr_index_t attrindex;

    ns_t ns;

    bool isemptyelemtag;

    wstring_type attname;
  };
}

}

#endif

