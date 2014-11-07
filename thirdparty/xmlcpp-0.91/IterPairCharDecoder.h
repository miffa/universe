/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#ifndef XMLCPP_ITERPAIRCHARDECODER_H
#define XMLCPP_ITERPAIRCHARDECODER_H

#include <istream>
#include <iterator>
#include <boost/shared_ptr.hpp>

#include "CharDecoder.h"

namespace xmlcpp
{

/**
 * IterPairCharDecoder class. It's a CharDecoder for iterator
 * pair input sources.
 */
template <class BidirectionalIterator>
class IterPairCharDecoder : public CharDecoder
{
 public:
  typedef BidirectionalIterator iterator;
  typedef std::iterator_traits<iterator> iter_traits;
  typedef typename iter_traits::value_type char_type;
  typedef typename iter_traits::iterator_category iter_tag;

  IterPairCharDecoder(iterator first,
		      iterator last,
		      char const* fromcode,
		      bool ignoreilseq = false,
		      size_t bytescacheSize = 1024 / sizeof(char_type),
		      bool isxmlfile = false)
    : CharDecoder(ignoreilseq, bytescacheSize, isxmlfile),
    first_(first), last_(last), curr_(first), tmpsize_(0)
  {
    set_fromcode(fromcode);
  }

 private:
  virtual char* get_inbuf(size_t& bytesleft, size_t bytessizehint)
  {
    if (curr_ == last_) return 0;

    if (bytessizehint < 128) bytessizehint = 128;
    size_t sizehint = bytessizehint / sizeof(char_type);

    if (tmpsize_ < sizehint) {
      tmpsize_ = sizehint;
      tmp_.reset(new char_type[tmpsize_]);
    }

    size_t left = 0;
    for (; left < tmpsize_ && curr_ != last_; ++left) {
      tmp_[left] = *curr_;
      ++curr_;
    }

    bytesleft = left * sizeof(char_type);

    return reinterpret_cast<char*>(tmp_.get());
  }

  virtual void seek_inbuf(int bytesoffset)
  {
    assert((bytesoffset % sizeof(char_type)) == 0);

    int offset = bytesoffset / (int)sizeof(char_type);

    std::advance(curr_, offset);
  }

 private:
  iterator first_, last_;
  iterator curr_;
  boost::scoped_array<char_type> tmp_;
  size_t tmpsize_;
};

/**
 * newIterPairCharDecoder function.
 */
template <class BidirectionalIterator>
  boost::shared_ptr<CharDecoder>
  newIterPairCharDecoder(BidirectionalIterator first,
			 BidirectionalIterator last,
			 char const* fromcode,
			 bool ignoreilseq = true,
			 size_t bytescacheSize = 1024 / sizeof(typename std::iterator_traits<BidirectionalIterator>::value_type),
			 bool isxmlfile = false)
  {
    return boost::shared_ptr<CharDecoder>(
      new IterPairCharDecoder<BidirectionalIterator>(first,
						     last,
						     fromcode,
						     ignoreilseq,
						     bytescacheSize));
  }

}

#endif
