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

#include "curlpp/internal/CurlHandle.hpp"

#include "curlpp/cURLpp.hpp"
#include "curlpp/Exception.hpp"

#include <string>
#include <iostream>
#include <memory>
#include <cstring>


using std::memset;
using std::unique_ptr;


namespace curlpp
{


namespace internal
{


void CurlHandle::perform()
{
	CURLcode code;

	code = curl_easy_perform(mCurl);
	throwException();
	libcurlRuntimeAssert(mErrorBuffer, code); //if we got an error
}


void CurlHandle::reset()
{
	curl_easy_reset(mCurl);
	memset(mErrorBuffer,0,CURL_ERROR_SIZE + 1);
	errorBuffer(mErrorBuffer);
}


CURL * CurlHandle::getHandle() const
{
	return mCurl;
}


CurlHandle::CurlHandle() 
	: mException(nullptr)
{
	memset(mErrorBuffer,0,CURL_ERROR_SIZE + 1);
	mCurl = curl_easy_init();
	runtimeAssert("Error when trying to curl_easy_init() a handle", mCurl != nullptr);
	errorBuffer(mErrorBuffer);
}


CurlHandle::CurlHandle(CURL * handle) 
	: mException(nullptr)
{
	memset(mErrorBuffer,0,CURL_ERROR_SIZE + 1);
	mCurl = handle;
	runtimeAssert("Error when trying to curl_easy_init() a handle", mCurl != nullptr);
	errorBuffer(mErrorBuffer);
}


std::unique_ptr<CurlHandle> 
CurlHandle::clone() const
{
	CURL * cHandle = curl_easy_duphandle(mCurl);
	runtimeAssert("Error when trying to curl_easy_duphandle() a handle", cHandle != nullptr);
	unique_ptr<CurlHandle> newHandle(new CurlHandle(cHandle));

	return newHandle;
}


CurlHandle::~CurlHandle()
{
	if (mException)
	{
		delete mException;
		mException = nullptr;
	}
	curl_easy_cleanup(mCurl);
}

void
CurlHandle::setException(curlpp::CallbackExceptionBase * e) 
{
	if(mException)
	{
		delete mException;
	}

	mException = e;
}

void
CurlHandle::errorBuffer(char * buffer)
{
	option(CURLOPT_ERRORBUFFER, (void *)buffer);
}


template<typename FunctorType>
typename FunctorType::ResultType 
CurlHandle::execute(FunctorType functor, typename FunctorType::ParamList params)
{
	if (!functor)
	{
	 	setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return functor(params);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


size_t 
CurlHandle::executeWriteFunctor(char * buffer, size_t size, size_t nitems)
{
	if (!mWriteFunctor)
	{
		setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null write functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return mWriteFunctor(buffer, size, nitems);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


size_t 
CurlHandle::executeHeaderFunctor(char * buffer, size_t size, size_t nitems)
{
	if (!mHeaderFunctor)
	{
		setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null write functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return mHeaderFunctor(buffer, size, nitems);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


size_t 
CurlHandle::executeReadFunctor(char * buffer, size_t size, size_t nitems)
{
	if(!mReadFunctor)
	{
		setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null write functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return mReadFunctor(buffer, size, nitems);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


int
CurlHandle::executeProgressFunctor(double dltotal, 
																	 double dlnow, 
																	 double ultotal, 
																	 double ulnow)
{
	if (!mProgressFunctor)
	{
		setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null write functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return mProgressFunctor(dltotal, dlnow, ultotal, ulnow);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


int 
CurlHandle::executeDebugFunctor(curl_infotype info, char * buffer, size_t size)
{
	if (!mDebugFunctor)
	{
		setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null write functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return mDebugFunctor(info, buffer, size);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


CURLcode 
CurlHandle::executeSslCtxFunctor(void * ssl_ctx)
{
	if (!mSslFunctor)
	{
		setException(new CallbackException<curlpp::LogicError>(curlpp::LogicError("Null write functor")));
		return CURLE_ABORTED_BY_CALLBACK;
	}

	try
	{
		return mSslFunctor(ssl_ctx);
	}

	catch(curlpp::CallbackExceptionBase * e)
	{
		setException(e);
	}

	catch(...)
	{
		setException(new CallbackException<curlpp::UnknowException>(curlpp::UnknowException()));
	}

	return CURLE_ABORTED_BY_CALLBACK;
}


void
CurlHandle::throwException()
{
  if(mException) 
  {
    std::unique_ptr< cURLpp::CallbackExceptionBase > e(mException);
    mException = nullptr;
    e->throwMe();
  }
}


} // namespace internal


} // namespace curlpp
