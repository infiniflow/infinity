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

#ifndef CURLPP_OPTION_LIST_HPP
#define CURLPP_OPTION_LIST_HPP


#include "../OptionBase.hpp"

#include <map>


namespace curlpp
{


namespace internal
{


	/**
	* This class can keep a set of option_container it's used to keep all the
	* option set on a handle.
	*/

	class OptionList
	{

	public:

		typedef std::map<CURLoption, curlpp::OptionBase *> mapType;

		/**
		* This construction initialize an empty list of options.
		*/
		OptionList();

		/**
		* This contructor initialize his list by calling the
		* option_container<T>::clone() function.
		*/
		OptionList(const OptionList & rhs);

		virtual ~OptionList();

		/**
		* This function will add an option to the list. Note that if the
		* option is allready contained by the list, the option value of this
		* option will be overrided by the new value. 
		*
		* Note: The option memory will be owned by the request, so when the 
		* request will be deleted, the option will be deleted too. It is 
		* suggested to not referer at all further in the code to the option 
		* instance passed as an argument.
		*/
		virtual void setOpt(OptionBase * option);

		/**
		* This function will add an option to the list. Note that if the
		* option is allready contained by the list, the option value of this
		* option will be overrided by the new value. 
		*/
		virtual void setOpt(const OptionBase & option);

		/**
		* This function will call the setOpt on each options
		* contained by * the option list passed in argument.
		*/
		virtual void setOpt(const OptionList & options);

		/**
		* This function will get the current option value of the corresponding 
		* OptionBase. Note that if the option is not set, the option passed in
		* parameter will be cleared. (See Option::getOpt for more details)
		*/
		virtual void getOpt(OptionBase * option) const;

		/**
		* all the options contained by the class will be deleted and we will
		* clone the rhs option_container and store them in the class.
		*/
		//OptionList & operator=(const OptionList & rhs);


	private:

		void insert(const OptionList::mapType & other);

		OptionList::mapType mOptions;

	};


} // namespace internal


} // namespace curlpp

namespace cURLpp = curlpp;


#endif // #ifndef CURLPP_OPTION_LIST_HPP
