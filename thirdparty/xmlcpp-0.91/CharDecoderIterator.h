/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_CHARDECODERITERATOR_H
#define XMLCPP_CHARDECODERITERATOR_H

#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/iterator/iterator_facade.hpp>
#include "CharDecoder.h"

namespace xmlcpp
{

/**
 * CharDecoderIterator class. Its iterator_category is forward
 * traversal. To be forward traversal iterator, it caches the
 * converted buffer. If its buffer size can be problem, you can
 * release the front of the cache buffer with 'releas_before'.
 */
class CharDecoderIterator
  : public boost::iterator_facade<
      CharDecoderIterator,
      CharDecoder::wchar_type const,
      boost::forward_traversal_tag
    >
{
 public:
  explicit CharDecoderIterator(boost::shared_ptr<CharDecoder> decoder);
  CharDecoderIterator();

  void release_before();

  void swap(CharDecoderIterator& o);

 private:
  friend class boost::iterator_core_access;

  void increment();
 
  bool equal(CharDecoderIterator const& other) const;

  value_type const& dereference() const;

 private:
  boost::shared_ptr<CharDecoder> decoder_;
  int curr_; // index
  value_type value_;
};

}

#endif
