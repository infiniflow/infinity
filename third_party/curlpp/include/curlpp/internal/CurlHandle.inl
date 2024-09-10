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


#ifndef CURLPP_CURLHANDLE_INL
#define CURLPP_CURLHANDLE_INL


#include "../Exception.hpp"


namespace curlpp
{


namespace internal
{


template<typename OptionType>
void 
CurlHandle::option(CURLoption optionType, 
OptionType value)
{
	CURLcode code;
	code = curl_easy_setopt(mCurl, optionType, value);
	libcurlRuntimeAssert(mErrorBuffer, code);
}


template<typename OptionType, CURLoption optionType>
void 
CurlHandle::option(OptionType value)
{
	option(optionType, value);
}


template <typename T>
void 
CurlHandle::getInfo(CURLINFO info, T & value) const
{
	CURLcode code;
	code = curl_easy_getinfo(mCurl, info, & value);
	libcurlRuntimeAssert(mErrorBuffer, code);
}


} // namespace internal


} // namespace curlpp


#endif 

