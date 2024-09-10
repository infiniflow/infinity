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


#ifndef CURLPP_INFO_HPP
#define CURLPP_INFO_HPP



#include "Easy.hpp"


namespace curlpp
{


	/**
	* This class is responsible of retreiving the Info from
	* a handle. This is the class you use when you want to do
	* so.
	*/

	template<CURLINFO info, typename T>
	struct Info 
	{
		static void get(const curlpp::Easy & handle, T & value);
		static T get(const curlpp::Easy & handle);
	};


	/**
	* This class is used when an info is not available for the
	* current libcURL version.
	*/

	template<CURLINFO info, typename T>
	struct NotAvailableInfo : Info<info, T>
	{
		static void get(const curlpp::Easy & handle, T & value);
		static T get(const curlpp::Easy & handle);
	};


	/**
	* This is the class you need to specialize if you use
	* a special type that libcURL is not aware of. This class
	* need to call curlpp::InfoGetter::get function. See 
	* curlpp::InfoGetter for more information.
	*/

	template<typename T>
	struct InfoTypeConverter
	{
		static void get(const curlpp::Easy & handle, CURLINFO info, T & value);
	}; 


	template<>
	void InfoTypeConverter<std::string>
		::get(const curlpp::Easy & handle, CURLINFO info, std::string & value);


	template<>
	void InfoTypeConverter<std::list<std::string> >
		::get(const curlpp::Easy & handle, CURLINFO info, std::list<std::string> & value);


	template<>
	void InfoTypeConverter<long>
		::get(const curlpp::Easy & handle, CURLINFO info, long & value);


	template<>
	void InfoTypeConverter<double>
		::get(const curlpp::Easy & handle, CURLINFO info, double & value);


	/**
	* This is the only class that is authorized to retreive 
	* info from a curlpp::Easy class. So, this is the class
	* you need to use when you specialize the class
	* curlpp::InfoTypeConverter. This class is in fact used
	* as a proxy, just to be sure that nobody access curlpp::Easy's 
	* private data.
	*/

	struct InfoGetter
	{
		template<typename T>
		static void get(const curlpp::Easy & handle, CURLINFO info, T & value);
	};


} // namespace curlpp

namespace cURLpp = curlpp;


#include "Info.inl"

#endif // #ifndef CURLPP_INFO_HPP
