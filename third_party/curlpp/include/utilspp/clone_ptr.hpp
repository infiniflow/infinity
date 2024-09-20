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

#ifndef UTILSPP_CLONEPTR_HPP
#define UTILSPP_CLONEPTR_HPP

#include <cassert>
#include <stdexcept>

namespace utilspp
{
  // This class is meant to manage a pointer. This class will 
  // ensure that when we go out of scope, it will delete the 
  // pointer.
  // 
  // However, contrary to the std::unique_ptr, instead of 
  // transfering the ownership on copy construction, it clones 
  // the content. This means that we can have STL containers
  // that uses that class for managing the pointers.
  //
  // So, it means that the class we stores, needs a "clone"
  // member.
  template <typename T> 
  class clone_ptr
  {
  public:
    clone_ptr() : value_(NULL) {}

    // This constructor takes ownership of the pointer.
    //
    // Note that it isn't explicit. This might be a 
    // problem.
    clone_ptr(T * value) : value_(value) {}

    ~clone_ptr() {if (value_) delete value_;}
    
    // This is the default constructor that takes his 
    // value from cloning the content of the other 
    // clone_ptr.
    clone_ptr(const clone_ptr & other)
    {value_ = other->clone();}
 
    T * operator->() 
    {
      if (value_)
	return value_;
      
      throw std::runtime_error("using a null clone_ptr");
    }

    const T * operator->() const
    {
      assert(value_);

      return value_;
    }

    T * get() { return value_; }
    const T * get() const { return value_; }
    
    // This just releases the pointer. It means that the
    // pointer is no longer owned by the smart pointer.
    T * release() 
    { 
      T * r = value_; 
      value_ = NULL;

      return r;
    }

  private:
    T * value_;
  };
}

#endif
