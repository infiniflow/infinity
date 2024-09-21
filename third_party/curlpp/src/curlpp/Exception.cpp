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

#include "curlpp/Exception.hpp"

curlpp::RuntimeError::~RuntimeError() throw()
{}

curlpp::RuntimeError::RuntimeError(const char * reason)
  : std::runtime_error(std::string(reason))
{}

curlpp::RuntimeError::RuntimeError(const std::string & reason)
  : std::runtime_error(reason)
{}

curlpp::LogicError::~LogicError() throw()
{}

curlpp::LogicError::LogicError(const char * reason)
    : std::logic_error(std::string(reason))
{}

curlpp::LogicError::LogicError(const std::string & reason)
    : std::logic_error(reason)
{}

curlpp::LibcurlRuntimeError::LibcurlRuntimeError(const std::string & reason, CURLcode code)
    : curlpp::RuntimeError(reason)
    , mCode(code)
{}

curlpp::LibcurlRuntimeError::LibcurlRuntimeError(const char * reason, CURLcode code)
    : curlpp::RuntimeError(reason)
    , mCode(code)
{}

CURLcode curlpp::LibcurlRuntimeError::whatCode() const throw()
{
   return mCode;
}

curlpp::LibcurlLogicError::LibcurlLogicError(const std::string & reason, CURLcode code)
    : curlpp::LogicError(reason)
    , mCode(code)
{}

curlpp::LibcurlLogicError::LibcurlLogicError(const char * reason, CURLcode code)
    : curlpp::LogicError(reason)
    , mCode(code)
{}

curlpp::UnsetOption::UnsetOption(const std::string & reason)
    : curlpp::RuntimeError(reason)
{};

curlpp::UnsetOption::UnsetOption(const char * reason)
    : curlpp::RuntimeError(reason)
{};

curlpp::NotAvailable::NotAvailable()
    : curlpp::LogicError("This option was not available at compile time.")
{};

curlpp::UnknowException::UnknowException() 
  : curlpp::RuntimeError("An unknown exception was thrown within a callback")
{}

CURLcode curlpp::LibcurlLogicError::whatCode() const throw()
{
   return mCode;
}

curlpp::CallbackExceptionBase::CallbackExceptionBase() 
  : RuntimeError("Exception thrown within a callback")
{}
    
curlpp::CallbackExceptionBase::CallbackExceptionBase(const CallbackExceptionBase & other) 
  : RuntimeError(other)
{}


void curlpp::libcurlRuntimeAssert(const std::string & reason, CURLcode code)
{
  curlpp::libcurlRuntimeAssert(reason.c_str(), code);
}

void curlpp::libcurlRuntimeAssert(const char * reason, CURLcode code)
{
  if (code != CURLE_OK)
    throw curlpp::LibcurlRuntimeError(reason, code);
}

void curlpp::libcurlLogicAssert(const std::string & reason, CURLcode code)
{
  libcurlLogicAssert(reason.c_str(), code);
}

void curlpp::libcurlLogicAssert(const char * reason, CURLcode code)
{
  if (code != CURLE_OK)
    throw curlpp::LibcurlLogicError(reason, code);
}

void curlpp::runtimeAssert(const std::string & reason, bool isOkay)
{
  runtimeAssert(reason.c_str(), isOkay);
}

void curlpp::runtimeAssert(const char * reason, bool isOkay)
{
  if (!isOkay)
    throw curlpp::RuntimeError(reason);
}

void curlpp::logicAssert(const std::string & reason, bool isOkay)
{
  logicAssert(reason.c_str(), isOkay);
}

void curlpp::logicAssert(const char * reason, bool isOkay)
{
   if (!isOkay)
     throw curlpp::LogicError(reason);
}

