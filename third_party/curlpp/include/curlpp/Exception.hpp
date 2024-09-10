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

#ifndef CURLPP_EXCEPTION_HPP
#define CURLPP_EXCEPTION_HPP



#include <curl/curl.h>

#include <string>
#include <stdexcept>


namespace curlpp
{


	/**
	* This class is a parent to all curlpp's RuntimeErrors.
	*
	* This class takes a const std::string & as argument for it's parent: std::runtime_errors. 
	* This class is thrown when curlpp is encountering an error, but for runtime 
	* considerations, "unpredictable" by the library user.
	*/

	class RuntimeError : public std::runtime_error
	{
	
	public:
	
		RuntimeError(const char * reason);
		RuntimeError(const std::string & string);
		virtual ~RuntimeError() throw();

	};


	/**
	* This class is a parent to all curlpp's RuntimeErrors.
	*
	* This class takes a const std::string & as argument for it's parent: std::runtime_errors. 
	* This class is thrown when curlpp is encountering an error, but for logic 
	* considerations, "predictable" by the library user. Predictable means 
	* that the library user is missusing the library.
	*/

	class LogicError : public std::logic_error
	{

	public:

		LogicError(const char * reason);
		LogicError(const std::string & string);
		virtual ~LogicError() throw();

	};


	/**
	* This is a class derived from curlpp::RuntimeError.
	*
	* It takes a const char * and a CURLcode as arguments. This class is thrown when libcurl is
	* returning an error with a CURLcode, but for runtime considerations, 
	* "unpredictable" by the library user.
	*/

	class LibcurlRuntimeError : public curlpp::RuntimeError
	{

	public:

		LibcurlRuntimeError(const std::string & reason, CURLcode code);
		LibcurlRuntimeError(const char * reason, CURLcode code);

		/**
		* Returns the CURLcode that libcurl returned.
		*/
		CURLcode whatCode() const throw();

	private:

		CURLcode mCode;

	};


	/*
	* This is a class derived from curlpp::LogicError, that takes a const 
	* char * and a CURLcode as arguments. This class is thrown when libcurl is
	* returning an error with a CURLcode,  but for logic considerations, 
	* "predictable" by the library user. Predictable means that the library 
	* user is missusing the library.
	*/

	class LibcurlLogicError : public curlpp::LogicError
	{

	public:

		LibcurlLogicError(const std::string & reason, CURLcode code);
		LibcurlLogicError(const char * reason, CURLcode code);

		/* 
		* return the CURLcode that libcurl returned
		*/
		CURLcode whatCode() const throw();

	private:

		CURLcode mCode;

	};


	/**
	* This exception is thrown when you try to retreive a value for an
	* unset option.
	*/

	class UnsetOption : public curlpp::RuntimeError
	{

	public:

		UnsetOption(const std::string & reason);
		UnsetOption(const char * reason);

	};


	/**
	* This exception is thrown when you try to instantiate an option 
	* that isn't available for your current libcURL version.
	*/

	class NotAvailable : public curlpp::LogicError
	{

	public:

		NotAvailable();

	};


	/**
	* This exception is thrown when an exception is thrown within
	* a callback without the curlpp::raiseException function.
	*/

	class UnknowException : public curlpp::RuntimeError
	{

	public:

		UnknowException();

	};


	/**
	* This exception is thrown by the curlpp::raiseException function.
	* It's used to throw exceptions within callbacks
	*/

	class CallbackExceptionBase : public curlpp::RuntimeError
	{

	protected:

		CallbackExceptionBase();
		CallbackExceptionBase(const CallbackExceptionBase & other);

	public:

		virtual void throwMe() = 0;
		virtual CallbackExceptionBase * clone() = 0;

	};


	/**
	* This exception is thrown by the curlpp::raiseException function.
	* It's used to throw exceptions within callbacks
	*/

	template<typename ExceptionType>
	class CallbackException : public CallbackExceptionBase
	{

	public: 

		typedef CallbackException<ExceptionType> _CE;

		CallbackException(const ExceptionType & e) 
			: mException(e)
		{}

		virtual void throwMe() 
		{
			throw mException;
		}

		virtual _CE * clone() 
		{
			return new _CE(*this);
		}

	private:

		ExceptionType mException;

	};


	/**
	* This function is the function to be called within callbacks
	* if you want an exception to be thrown at the 
	* curlpp::Easy::perform function call level.
	*/

	template<typename T>
	void raiseException(const T & e) 
	{
		throw (CallbackExceptionBase *)(new CallbackException<T>(e));
	}


	template<typename T>
	CallbackException<T> * createCallbackException(const T & e)
	{
		return CallbackException<T>(e);
	}


	/**
	* if CURLcode is not equal to CURLE_OK, it throws a 
	* curlpp::LibcurlRuntimeError with the reason and the code. It's used 
	* in inline function, because throwing an exception is heavy in binary 
	* code, something we don't want in inline functions.
	*/

	void libcurlRuntimeAssert(const std::string & reason, CURLcode code);
	void libcurlRuntimeAssert(const char * reason, CURLcode code);


	/**
	* if CURLcode is not equal to CURLE_OK, it throws a 
	* curlpp::LibcurlLogicError with the reason and the code. It's used 
	* in inline function, because throwing an exception is heavy in binary 
	* code, something we don't want in inline functions.
	*/

	void libcurlLogicAssert(const std::string & reason, CURLcode code);
	void libcurlLogicAssert(const char * reason, CURLcode code);


	/**
	* if isOkay is false, it throws a curlpp::RuntimeError 
	* with the reason. It's used in inline function, because throwing 
	* an exception is heavy in binary code, something we don't want in 
	* an inline function.
	*/

	void runtimeAssert(const std::string & reason, bool isOkay);
	void runtimeAssert(const char * reason, bool isOkay);


	/**
	* if is_true is false, it throws a curlpp::LogicError with 
	* the reason. It's used in inline function, because throwing 
	* an exception is heavy in binary code, something we don't 
	* want in an inline function.
	*/

	void logicAssert(const std::string & reason, bool isOkay);
	void logicAssert(const char * reason, bool isOkay);


} // namespace curlpp

namespace cURLpp = curlpp;


#endif // #ifndef CURLPP_EXCEPTION_HPP
