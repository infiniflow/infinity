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

#ifndef CURLPP_OPTION_HPP
#define CURLPP_OPTION_HPP


#include "internal/OptionContainer.hpp"

#include "OptionBase.hpp"


namespace curlpp
{


	class Easy;

	/**
	* This class is representing an option that you set on a class. 
	*
	* We use utilspp::pointer_on_member_function and utilspp::type_trait to simplify
	* the declaration of an option object.
	*/

        template<typename OT>
	class Option : public curlpp::OptionBase
	{

	public:

		typedef OT OptionType;

		typedef typename internal::OptionContainer<OptionType>::ParamType ParamType;
		typedef typename internal::OptionContainer<OptionType>::ValueType ValueType;
		typedef typename internal::OptionContainer<OptionType>::ReturnType ReturnType;
		typedef typename internal::OptionContainer<OptionType>::HandleOptionType HandleOptionType;

	protected:

		/**
		* The constructor takes the a value to set a handle.
		*/
		Option(CURLoption option, typename Option<OptionType>::ParamType value);

		/**
		* The construction will copy the value of the Option passed in argument.
		*/
		Option(const Option<OptionType> & other);

		/**
		* The constructor will contain an unset option value.
		* Note that if you try to retreive the value of this option
		* before calling the curlpp::Option::setValue function it will
		* throw a UnsetOption exception.
		*/
		Option(CURLoption option);

	public:

		/**
		* What can I say? Everyone is dying one time or another...
		*/
		virtual ~Option();


		/**
		* This function will set the value that will be set on handle when we
		* will call the "update" function on a handle.
		*/
		void setValue(typename Option<OptionType>::ParamType value);

		/** 
		* This function will return the value that this option was set to.
		*
		* Note: if you didn't set any value by the curlpp::Option::setValue function,
		* or the handle option value, retreived by the curlpp::Option::updateMeToHandle
		* function, is a unset value, it will throw a UnsetOption exception.
		*/
		typename Option<OptionType>::ReturnType getValue() const;

		/**
		* This function will reset the option value. That means that if you try to
		* retreive the value of this option, or if you try to set this option to a
		* handle, it will throw an UnsetOption exception.
		*/
		virtual void clear();

		/**
		* Will update the value of the option with the value of the
		* option passed is argument.
		*/
		virtual void updateMeToOption(const OptionBase & other);

	private:

		/**
		* This function will update the given handle to the value previously set,
		* by the curlpp::Option::setValue function.
		*/
		void setOpt(curlpp::Easy * handle) const;

		/**
		* This function will update the current value of the option to the handle
		* option value.
		*/
		void getOpt(curlpp::Easy * handle);

	protected:

		/**
		* the class that actually have the value.
		*/
		typename curlpp::internal::OptionContainer<OptionType> * mContainer;

	};


	/**
	* This class is just a wrapper around curlpp::Option, in order to
	* be able to typedef Options. 
	*/

        template<typename OptionType, CURLoption opt>
	class OptionTrait : public Option<OptionType>
	{
		
		friend class Easy;

	public:

		static const CURLoption option = opt;

		/**
		* The constructor takes the a value to set a handle.
		*/
		OptionTrait(typename Option<OptionType>::ParamType value);

		/**
		* The constructor will contain an unset option value.
		* Note that if you try to retreive the value of this option
		* before calling the curlpp::Option::setValue function it will
		* throw a UnsetOption exception.
		*/
		OptionTrait();

		/**
		* Return a copy of the current option. 
		* Note that the option value is copied too.
		*/
		virtual OptionTrait * clone() const;

	private:

		/**
		* will call the actual libcurl option function with the value we got 
		* on the handle.
		*/
		virtual void updateHandleToMe(internal::CurlHandle * handle) const;

	};


	/**
	* This class is just a wrapper around curlpp::OptionTrait, in order to
	* be able to have "No value" option, like SslDefaultEngine.
	*/

	template<CURLoption option>
	class NoValueOptionTrait : public OptionTrait<bool, option>
	{

	public:

		NoValueOptionTrait();

		/**
		* Return a copy of the current option. 
		* Note that the option value is copied too.
		*/
		virtual NoValueOptionTrait * clone() const;

	};


	/**
	* This class is used when the option is not implemented.
	*/

	template<typename OptionType>
	class NotAvailableOptionTrait : public Option<OptionType>
	{

	public:

		/**
		* The constructor takes the a value to set a handle.
		*/
		NotAvailableOptionTrait(typename Option<OptionType>::ParamType value);

		/**
		* The constructor will contain an unset option value.
		* Note that if you try to retreive the value of this option
		* before calling the curlpp::Option::setValue function it will
		* throw a UnsetOption exception.
		*/
		NotAvailableOptionTrait();

		/**
		* Return a copy of the current option. 
		* Note that the option value is copied too.
		*/
		virtual NotAvailableOptionTrait * clone() const;

	private:

		/**
		* will call the actual libcurl option function with the value we got 
		* on the handle.
		*/
		virtual void updateHandleToMe(internal::CurlHandle * handle) const;

	};


} // namespace curlpp

namespace cURLpp = curlpp;


#include "Option.inl"

#endif // #ifndef CURLPP_OPTION_HPP
