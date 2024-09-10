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

#ifndef CURLPP_CURL_HANDLE_HPP
#define CURLPP_CURL_HANDLE_HPP



#include "../Exception.hpp"
#include "../Types.hpp"

#include <curl/curl.h>

#include <memory>


namespace curlpp
{


namespace internal
{


	/**
	* Wrapper for CURL * handle.
	*/

	class CurlHandle
	{

	public:

		CurlHandle();
		CurlHandle(CURL * handle);

		std::unique_ptr<CurlHandle> clone() const;

		/**
		* Calls curl_easy_perform on the handle and throws exceptions on errors.
		*/
		void perform();

		/**
		* Simply calls curl_easy_reset on the handle.
		*/
		void reset();

		virtual ~CurlHandle();

		/**
		* Calls curl_easy_setopt on the option.
		*/
		template<typename OptionType>
		void option(CURLoption option, OptionType value);

		/**
		* Calls curl_easy_setopt on the option.
		*/
		template<typename OptionType, CURLoption optionType>
		void option(OptionType value);

		/**
		* This function will return the CURL * handle.
		*
		* DO NOT use this, unless you REALLY know what you
		* are doing.
		*/
		CURL * getHandle() const;

		/**
		* Request internal information from the curl session with this function.
		*
		* The third argument MUST be a pointer to a long, a pointer to a char *, 
		* a pointer to a struct curl_slist * or a pointer to a double.
		*/
		template<typename T>
		void getInfo(CURLINFO info, T & value) const;


		template<typename FunctorType>
		typename FunctorType::ResultType execute(FunctorType functor, typename FunctorType::ParamList params);


		size_t executeWriteFunctor(char * buffer, size_t size, size_t nitems);

		void setWriteFunctor(curlpp::types::WriteFunctionFunctor functor)
		{
			mWriteFunctor = functor;
		}


		size_t executeHeaderFunctor(char * buffer, size_t size, size_t nitems);

		void setHeaderFunctor(curlpp::types::WriteFunctionFunctor functor)
		{
			mHeaderFunctor = functor;
		}


		size_t executeReadFunctor(char * buffer, size_t size, size_t nitems);

		void setReadFunctor(curlpp::types::ReadFunctionFunctor functor)
		{
			mReadFunctor = functor;
		}


		int executeProgressFunctor(double dltotal, 
																double dlnow, 
																double ultotal, 
																double ulnow);

		void setProgressFunctor(curlpp::types::ProgressFunctionFunctor functor)
		{
			mProgressFunctor = functor;
		}


		int executeDebugFunctor(curl_infotype, char *, size_t);

		void setDebugFunctor(curlpp::types::DebugFunctionFunctor functor)
		{
			mDebugFunctor = functor;
		}


		CURLcode executeSslCtxFunctor(void * ssl_ctx);

		void setSslCtxFunctor(curlpp::types::SslCtxFunctionFunctor functor)
		{
			mSslFunctor = functor;
		}

		void setException(curlpp::CallbackExceptionBase * e);
		void throwException();

	private:

		CurlHandle(const CurlHandle & other);
		CurlHandle & operator=(const CurlHandle & other);

		/**
		* Provides libcURL a space to store error messages.
		*
		* Pass a char * to a buffer that the libcURL may store
		* human readable error messages in. This may  be  more
		* helpful  than just the return code from the library.
		* The buffer must be at least CURL_ERROR_SIZE big.
		* Note: if the library does not return an  error,  the
		* buffer may not have been touched. Do not rely on the
		* contents in those cases.
		*/
		void errorBuffer(char* buffer);

	private:

		CURL * mCurl;

		char mErrorBuffer[CURL_ERROR_SIZE + 1];

		curlpp::types::WriteFunctionFunctor mWriteFunctor;
		curlpp::types::WriteFunctionFunctor mHeaderFunctor;
		curlpp::types::ReadFunctionFunctor mReadFunctor;
		curlpp::types::ProgressFunctionFunctor mProgressFunctor;
		curlpp::types::DebugFunctionFunctor mDebugFunctor;
		curlpp::types::SslCtxFunctionFunctor mSslFunctor;
		curlpp::CallbackExceptionBase * mException;

	};


} // namespace internal


} // namespace curlpp

namespace cURLpp = curlpp;


#include "CurlHandle.inl"

#endif // #ifndef CURLPP_CURL_HANDLE_HPP
