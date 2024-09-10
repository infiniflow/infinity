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

#ifndef CURLPP_OPTION_CONTAINER_HPP
#define CURLPP_OPTION_CONTAINER_HPP


#include "OptionContainerType.hpp"


namespace curlpp
{


namespace internal
{


	template<typename OptionType>
	class Option;

	/**
	* This class is used to set an option to a handle and to keep it's value.
	*/
	template<class OptionType>
	class OptionContainer
	{

	public:

		typedef typename OptionContainerType<OptionType>::ParamType ParamType;
		typedef typename OptionContainerType<OptionType>::ReturnType ReturnType;
		typedef typename OptionContainerType<OptionType>::ValueType ValueType;
		typedef typename OptionContainerType<OptionType>::HandleOptionType HandleOptionType;

		/**
		* Contructor. We pass the value of the option.
		*/
		OptionContainer(typename OptionContainer<OptionType>::ParamType value);

		OptionContainer(OptionContainer<OptionType> & other);

		/**
		* This function set the argument that will be passed to the 
		* option call for a handle. It will use the argument passed to
		* this function.
		*/
		void setValue(typename OptionContainer<OptionType>::ParamType value);

		/**
		* This function get the argument that is set on the handle.
		*/
		typename OptionContainer<OptionType>::ReturnType getValue();

		/** 
		* We call this function to have the value passed to the curl_easy_setopt.
		* 
		* Note: DO NOT USE THIS FUNCTION! It's for internal use only.
		*/
		typename OptionContainer<OptionType>::HandleOptionType getHandleOptionValue();


	private:         
		/**
		* We cannot call this constructor. We absolutely need an initial value.
		*/
		OptionContainer();

		/**
		* Current value of the option.
		*/
		typename OptionContainer<OptionType>::ValueType mValue;
	};


} // namespace internal


} // namespace curlpp

namespace cURLpp = curlpp;

#include "OptionContainer.inl"

#endif // #ifndef CURLPP_OPTION_CONTAINER_HPP

