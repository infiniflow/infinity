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

#ifndef CURLPP_INFO_INL
#define CURLPP_INFO_INL

template<CURLINFO info, typename T>
void
curlpp::Info<info, T>::get(const curlpp::Easy & handle, T & value)
{
   curlpp::InfoTypeConverter<T>::get(handle, info, value);
}

template<CURLINFO info, typename T>
T
curlpp::Info<info, T>::get(const curlpp::Easy & handle)
{
  T value;
  curlpp::InfoTypeConverter<T>::get(handle, info, value);
  return value;
}

template<CURLINFO info, typename T>
void
curlpp::NotAvailableInfo<info, T>::get(const curlpp::Easy &, T &)
{
  throw curlpp::NotAvailable();
}

template<CURLINFO info, typename T>
T
curlpp::NotAvailableInfo<info, T>::get(const curlpp::Easy &)
{
  throw curlpp::NotAvailable();
}



template<typename T>
void
curlpp::InfoGetter::get(const curlpp::Easy & handle, 
	                CURLINFO info,
 			T & value)
{
   handle.getInfo(info, value);
}


template<typename T>
void
curlpp::InfoTypeConverter<T>::get(const curlpp::Easy & handle, 
				    CURLINFO info,
				    T & value)
{
  InfoGetter::get(handle, info, value);
}


#endif
