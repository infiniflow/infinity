/*
 *    Copyright (c) <2002-2006> <Jean-Philippe Barrette-LaPierre>
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

#ifndef CURLPP_OPTION_INL
#define CURLPP_OPTION_INL

#include "internal/OptionSetter.hpp"
#include "Exception.hpp"

#include <iostream>


namespace curlpp
{


// Option
	
template<typename OptionType>
Option<OptionType>::Option(CURLoption option, typename Option<OptionType>::ParamType value)
: OptionBase(option),
mContainer(NULL)
{
	setValue(value);
}


template<typename OptionType>
Option<OptionType>::Option(CURLoption option)
: OptionBase(option),
mContainer(NULL)
{}

template<typename OptionType>
Option<OptionType>::Option(const Option<OptionType> & other)
  : OptionBase(other)
  , mContainer(NULL)
{
	if(other.mContainer != NULL)	
	{
		setValue(other.getValue());
	}
}


template<typename OptionType>
Option<OptionType>::~Option()
{
	delete mContainer;
	mContainer = NULL;
}


template<typename OptionType>
void
Option<OptionType>::setValue(typename Option<OptionType>::ParamType value)
{
	if(mContainer == NULL)
	{	
		mContainer = new internal::OptionContainer<OptionType>(value);
	}
	else
	{
		mContainer->setValue(value);
	}
}


template<typename OptionType>
void
Option<OptionType>::updateMeToOption(const OptionBase & other)
{
	const Option<OptionType> * option = dynamic_cast<const Option<OptionType> *>(&other);
	if(option == NULL)
	{
		throw UnsetOption("You are trying to update an option to an incompatible option");
	}
	setValue(option->getValue());
}

template<typename OptionType>
void
Option<OptionType>::clear()
{
  delete mContainer;
  mContainer = NULL;
}

template<typename OptionType>
typename Option<OptionType>::ReturnType
Option<OptionType>::getValue() const
{
  if(mContainer == NULL)
    throw UnsetOption("You are trying to retreive the value of an unset option");

  return mContainer->getValue();
}


// OptionTrait

template<typename OptionType, CURLoption option>
OptionTrait<OptionType, option>::OptionTrait(typename Option<OptionType>::ParamType value)
: Option<OptionType>(option, value)
{}


template<typename OptionType, CURLoption option>
OptionTrait<OptionType, option>::OptionTrait()
: Option<OptionType>(option)
{}


template<typename OptionType, CURLoption option>
OptionTrait<OptionType, option> *
OptionTrait<OptionType, option>::clone() const
{
  return new OptionTrait<OptionType, option>(this->getValue());
}


template<typename OptionType, CURLoption option>
void
OptionTrait<OptionType, option>::updateHandleToMe(internal::CurlHandle * handle) const
{
	if(this->mContainer == NULL)
	{
		throw UnsetOption("You are trying to set an unset option to a handle");
	}	

	internal::OptionSetter<OptionType, option>::setOpt(handle, this->mContainer->getHandleOptionValue());
}


// NoValueOptionTrait

template<CURLoption option>
NoValueOptionTrait<option>::NoValueOptionTrait() 
  : OptionTrait<bool, option>(true)
{}


template<CURLoption option>
NoValueOptionTrait<option> *
NoValueOptionTrait<option>::clone() const
{
  return new NoValueOptionTrait<option>();
}


// NotAvailableOptionTrait

template<typename OptionType>
NotAvailableOptionTrait<OptionType>::NotAvailableOptionTrait(typename Option<OptionType>::ParamType )
  : Option<OptionType>(CURLOPT_URL)
{
  throw NotAvailable();
}

template<typename OptionType>
NotAvailableOptionTrait<OptionType>::NotAvailableOptionTrait()
  : Option<OptionType>(CURLOPT_URL)
{
  throw NotAvailable();
}


template<typename OptionType>
NotAvailableOptionTrait<OptionType> *
NotAvailableOptionTrait<OptionType>::clone() const
{
  throw NotAvailable();
}


template<typename OptionType>
void
NotAvailableOptionTrait<OptionType>::updateHandleToMe(internal::CurlHandle *) const
{
  throw NotAvailable();
}


} // namespace curlpp

#endif
