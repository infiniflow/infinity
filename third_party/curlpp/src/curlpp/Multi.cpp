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


#include "curlpp/Easy.hpp"
#include "curlpp/Exception.hpp"
#include "curlpp/Multi.hpp"


curlpp::Multi::Multi()
{
  mMultiHandle = curl_multi_init();
  runtimeAssert("Error when trying to curl_multi_init() a handle", mMultiHandle != NULL);
}

curlpp::Multi::~Multi()
{
  // remove all the remaining easy handles
  while (!mHandles.empty()) 
  {
    std::map<CURL *, const curlpp::Easy *>::iterator handle = mHandles.begin();
    curl_multi_remove_handle(mMultiHandle, handle->second->getHandle());
    mHandles.erase(handle);
  }

  curl_multi_cleanup(mMultiHandle);
}

void
curlpp::Multi::add(const curlpp::Easy * handle)
{
  CURLMcode code = curl_multi_add_handle(mMultiHandle, handle->getHandle());
  if(code != CURLM_CALL_MULTI_PERFORM) {
    if(code != CURLM_OK) {
      throw curlpp::RuntimeError(curl_multi_strerror(code));
    }
  }
  mHandles.insert(std::make_pair(handle->getHandle(),handle));
}

void
curlpp::Multi::remove(const curlpp::Easy * handle)
{
  CURLMcode code = curl_multi_remove_handle(mMultiHandle, handle->getHandle());
  if(code != CURLM_CALL_MULTI_PERFORM) {
    if(code != CURLM_OK) {
      throw curlpp::RuntimeError(curl_multi_strerror(code));
    }
  }
  mHandles.erase(handle->getHandle());
}

bool
curlpp::Multi::perform(int * nbHandles)
{
  CURLMcode code = curl_multi_perform(mMultiHandle, nbHandles);
  if(code == CURLM_CALL_MULTI_PERFORM) {
    return false;
  }

  if(code != CURLM_OK) {
    throw curlpp::RuntimeError(curl_multi_strerror(code));
  }

  return true;
}

void
curlpp::Multi::fdset(fd_set * read, fd_set * write, fd_set * exc, int * max)
{
  CURLMcode code = curl_multi_fdset(mMultiHandle, read, write, exc, max);
  if(code != CURLM_CALL_MULTI_PERFORM) {
    if(code != CURLM_OK) {
      throw curlpp::RuntimeError(curl_multi_strerror(code));
    }
  }
}

curlpp::Multi::Msgs
curlpp::Multi::info()
{
  CURLMsg * msg; /* for picking up messages with the transfer status */
  
  int msgsInQueue;
  Msgs result;
  while ((msg = curl_multi_info_read(mMultiHandle, &msgsInQueue)) != NULL) {
    Multi::Info inf;
    inf.msg = msg->msg;
    inf.code = msg->data.result;
    result.push_back(std::make_pair(mHandles[msg->easy_handle],inf));
  }

  return result;
}

