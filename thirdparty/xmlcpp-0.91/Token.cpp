/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "Token.h"

namespace xmlcpp
{

namespace detail
{
  QName::QName()
  {
  }

  QName::QName(wstring_type const& n,
	       wstring_type const& p,
	       wstring_type const& l)
    : ns(n), prefix(p), localpart(l)
  {
  }

  bool QName::operator==(QName const& o) const
  {
    if (ns.empty() && o.ns.empty())
      return prefix == o.prefix && localpart == o.localpart;

    return ns == o.ns && localpart == o.localpart;
  }

  bool QName::operator<(QName const& o) const
  {
    using boost::make_tuple;
    return
      make_tuple(ns, prefix, localpart) <
      make_tuple(o.ns, o.prefix, o.localpart);
  }

}

}

