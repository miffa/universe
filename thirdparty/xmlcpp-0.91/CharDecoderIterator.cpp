/**
 * xmlcpp PullParser library.
 *
 * Copyright Wongoo Lee 2006. Use, modification and distribution is
 * subject to the Boost Software License, Version 1.0. (See
 * accompanying file LICENSE_1_0.txt or copy at
 * http://www.boost.org/LICENSE_1_0.txt)
 */

#include "CharDecoderIterator.h"

#include <limits>

namespace xmlcpp
{

CharDecoderIterator::CharDecoderIterator(boost::shared_ptr<CharDecoder> decoder)
  : decoder_(decoder), curr_(0)
{
  value_ = decoder_->at(curr_);
}

CharDecoderIterator::CharDecoderIterator()
  : curr_(-1), value_(CharDecoder::END)
{

}

void CharDecoderIterator::release_before()
{
  decoder_->release_before(curr_);
}

void CharDecoderIterator::swap(CharDecoderIterator& o)
{
  std::swap(decoder_, o.decoder_);
  std::swap(curr_, o.curr_);
  std::swap(value_, o.value_);
}

void CharDecoderIterator::increment()
{
  CharDecoder::wint_type i = decoder_->at(++curr_);
  if (i == CharDecoder::END) curr_ = -1;

  value_ = i;
}
 
bool CharDecoderIterator::equal(CharDecoderIterator const& other) const
{
  if (curr_ == -1 || curr_ == other.curr_) return true;

  return decoder_ == other.decoder_ && curr_ == other.curr_;
}

CharDecoderIterator::value_type const& CharDecoderIterator::dereference() const
{
  return value_;
}

}
