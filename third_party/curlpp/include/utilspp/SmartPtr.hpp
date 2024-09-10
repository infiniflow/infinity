/*
 *    Copyright (c) <2002-2009> <Jean-Philippe Barrette-LaPierre>
 *    
 *    Permission is hereby granted, free of charge, to any person obtaining
 *    a copy of this software and associated documentation files 
 *    (curlpp), to deal in the Software without restriction, 
 *    including without limitation the rights to use, copy, modify, merge,
 *    publish, distribute, sublicense, and/or sell copies of the Software,
 *    and to permit persons to whom the Software is furnished to do so, 
 *    subject to the following conditions:
 *    
 *    The above copyright notice and this permission notice shall be included
 *    in all copies or substantial portions of the Software.
 *    
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 *    OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY 
 *    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, 
 *    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef UTILSPP_SMARTPTR_HPP
#define UTILSPP_SMARTPTR_HPP

#include <stdexcept>
#include "NonCopyable.hpp"

#define NULL_BODY_ERROR "the smart pointer contain a NULL pointer"

namespace utilspp
{

  template <typename Type = unsigned int>
  class FastCount
  {
  public:
    FastCount(Type count = 1) : mCount(count)
    {}

    FastCount & operator++()
    {
      mCount++;
      return *this;
    }

    FastCount & operator--()
    {
      mCount--;
      return *this;
    }

    operator Type()
    {
      return mCount;
    }

    Type useCount()
    {
      return mCount;
    }

  private:
    Type mCount;
  };

  
  template <typename ContentType, typename CountPolicy = FastCount>
  class CountingBody : public utilspp::NonCopyable
  {
  public:
    CountingBody(ContentType * body) : mBody(body)
    {}

    void inc()
    {
      ++mCount;
    }

    void dec()
    {
      --mCount;
      if (mCount <= 0) {
	delete this;
      }
    }

    ContentType * get()
    {
      return mBody;
    }

  protected:
    ~CountingBody()
    {
      if (mBody != NULL) {
	delete mBody;
	mBody = NULL;
      }
    }

  private:
    CountPolicy mCount;
    ContentType * mBody;
  };


  template <typename ContentType, typename CountingBodyPolicy = CountingBody>
  class SharedPtr
  {
  public:
    SharedPtr() : mContent(new CountingPolicy<ContentType>(NULL))
    {}

    explicit SharedPtr(ContentType * content) : mContent(new CountingBodyPolicy<ContentType>(content))
    {}

    ~SharedPtr()
    {
      mContent->dec();
    }

    SharedPtr(const SharedPtr & other) : mContent(other.mContent)
    {
      mContent->inc();
    }

    SharedPtr & operator=(const SharedPtr & other)
    {
      if(mContent->get() != other.mContent->get()) {
	mContent->dec();
	mContent = other.mContent;
	mContent->inc();
      }
      return (*this);
    }

    SharedPtr & operator=(ContentType * content)
    {
      mContent--;
      mContent = new CountingBodyPolicy<ContentType>(content);
    }

    bool operator==(const SharedPtr & other) const
    {
      return (mContent->get() == other.mContent->get());
    }

    bool operator!=(const SharedPtr & other) const
    {
      return (mContent->get() != other.mContent->get());
    }

    bool operator<(const SharedPtr & other) const
    {
      return (mContent->get() < other.mContent->get());
    }

    operator ContentType*()
    {
      return mContent->get();
    }

    ContentType & operator*()
    {
      if(mContent->get() == NULL) {
	throw std::runtime_error(NULL_BODY_ERROR);
      }
      return *mContent->get();
    }

    ContentType* operator->()
    { 
      if(mContent->get() == NULL) {
	throw std::runtime_error(NULL_BODY_ERROR);
      }
      return mContent->get();
    }

  private:
    CountingBodyPolicy * mContent;
  };
}

#endif
