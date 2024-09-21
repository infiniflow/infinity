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

#ifndef CURLPP_EASY_HPP
#define CURLPP_EASY_HPP


#include "internal/CurlHandle.hpp"
#include "internal/OptionList.hpp"

#include "Option.hpp"

#include <memory>


namespace curlpp
{


	/**
	* Easy class.
	* Detailed description.
	*/

	class Easy
	{

	public: 

		friend struct InfoGetter;

		Easy();

		/**
		* This allow to have a handle, which might have
		* some option set, but we don't care about them.
		*/
		Easy(std::unique_ptr<internal::CurlHandle> handle);
		virtual ~Easy();

		/**
		* it will call the curl_easy_perform function will all the options
		* previously set for this handle.
		*/
		void perform();

		/**
		* This function will set the option value of the OptionBase
		* to the handle.
		*/
		virtual void setOpt(const OptionBase & option);

		/**
		* This function will set the option value of the OptionBase to the 
		* handle. 
		*/
		virtual void setOpt(std::unique_ptr<OptionBase> option);

		/**
		* This function will set the option value of the OptionBase to the 
		* handle. 
		*
		* Note: be carefull when using this function, see 
		* curlpp::OptionList::setOpt(OptionBase * option) function for more
		* details.
		*/
		virtual void setOpt(OptionBase * option);

		/**
		* This function will create OptionTrait class with the value given and call
		* virtual void setOpt(const OptionBase & option) with it.
		*/
		template<typename OptionTrait>
		void setOpt(typename OptionTrait::ParamType);

		/**
		* Setting options from custom container with curlpp options.
		*/
		template<typename InputIterator>
		void setOpt(InputIterator first, InputIterator last);

		/**
		* This function will get the current option value of the corresponding 
		* OptionBase. Note that if the option is not set, the option passed in
		* parameter will be cleared. (See Option::getOpt for more details)
		*/
		void getOpt(OptionBase * option) const;

		/**
		* This function will get the current option value of the corresponding 
		* OptionBase. Note that if the option is not set, the option passed in
		* parameter will be cleared. (See Option::getOpt for more details)
		*/
		void getOpt(OptionBase & option) const;

		/**
		* Get all options.
		*/
		//template<typename OutputIterator>
		//void getOptions(OutputIterator out);

		/*
		* This function empties the option collection and reset all options
		* to their default value
 		*/
		virtual void reset ();

 		/**
		* This function will return the cURL * handle.
		* DO NOT use this, unless you REALLY know what you
		* are doing.
		*/
		CURL * getHandle() const;

		internal::CurlHandle & getCurlHandle() { return *mCurl; }
		const internal::CurlHandle & getCurlHandle() const { return *mCurl; }

	private:

		/**
		* This function will call the setOpt on each options
		* contained by * the option list passed in argument.
		*/
		virtual void setOpt(const internal::OptionList & options);

		/**
		* This is the function that curlpp::InfoGetter will call
		* to retreive option.
		*/
		template<typename T>
		void getInfo(CURLINFO info, T & value) const;

		std::unique_ptr<internal::CurlHandle> mCurl;

		internal::OptionList mOptions;

	};


} // namespace curlpp

namespace cURLpp = curlpp;

#include "Easy.inl"

// Not quite sure if we shouldn't pass a const handle and clone it instead.
std::ostream & operator<<(std::ostream & stream, const curlpp::Easy & handle);


#endif // #ifndef CURLPP_EASY_HPP
