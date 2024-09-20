#include "curlpp/cURLpp.hpp"
#include "curlpp/Exception.hpp"

#include <cstdlib>


namespace 
{

	
  struct libcURLInstance
  {
    libcURLInstance();

    ~libcURLInstance()
      { curlpp::terminate(); }
  };
}


void curlpp::initialize(long flags)
{
  CURLcode code;
  code = curl_global_init(flags);
  libcurlRuntimeAssert("cannot initialize curlpp", code);
}


void curlpp::terminate()
{
  curl_global_cleanup();
}


curlpp::Cleanup::Cleanup()
{
  curlpp::initialize();
}


curlpp::Cleanup::~Cleanup()
{}


std::string 
curlpp::escape(const std::string & url)
{
   std::string buffer;
   char* p = curl_escape(url.c_str(), (int)url.size());
   if(!p) {
      throw std::runtime_error("unable to escape the string"); //we got an error
   }
   else {
     buffer = p;
     curl_free(p);
   }
   return buffer;
}


std::string
curlpp::unescape(const std::string & url)
{
   std::string buffer;
   char* p = curl_unescape(url.c_str(), (int)url.size());
   if (!p)
   {
      throw RuntimeError("unable to escape the string"); //we got an error
   }
   else
   {
      buffer = p;
      curl_free(p);
   }
   return buffer;
}


std::string
curlpp::getenv(const std::string & name)
{
   std::string buffer;
   char* p = curl_getenv(name.c_str());
   if (!p)
   {
      throw RuntimeError("unable to get the environnement string"); //we got an error
   }
   else
   {
      buffer = p;
      free(p);
   }
   return buffer;
}


std::string
curlpp::libcurlVersion()
{
  char* p = curl_version();
   if (!p)
   {
      throw RuntimeError("unable to get the libcurl version"); //we got an error
   }
      
   return std::string(p);
}


time_t 
curlpp::getdate(const std::string & date, time_t * now)
{
  time_t return_value = curl_getdate(date.c_str(), now);
   if(!return_value)
   {
      throw RuntimeError("unable to get the date");
   }

   return return_value;
}

