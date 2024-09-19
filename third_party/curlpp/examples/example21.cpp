/*
*    Copyright (c) <2002-2005> <Jean-Philippe Barrette-LaPierre>
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

/**
* \file
* Using ReadStream option.
* 
*/


#include <sstream>

#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
 
int main(int argc, char *argv[])
{
  if(argc != 3) {
    std::cerr << "Example 2: Missing argument" << std::endl 
	      << "Example 2: Usage: example02 url string-to-send" 
	      << std::endl;
    return EXIT_FAILURE;
  }
  char *url = argv[1];

  std::istringstream myStream(argv[2]);
  int size = myStream.str().size();
     
  char buf[50];
   try
   {
      curlpp::Cleanup cleaner;
      curlpp::Easy request;

      std::list< std::string > headers;
      headers.push_back("Content-Type: text/*"); 
      sprintf(buf, "Content-Length: %d", size); 
      headers.push_back(buf);
      
      using namespace curlpp::Options;
      request.setOpt(new Verbose(true));
      request.setOpt(new ReadStream(&myStream));
      request.setOpt(new InfileSize(size));
      request.setOpt(new Upload(true));
      request.setOpt(new HttpHeader(headers));
      request.setOpt(new Url(url));
       
      request.perform();
   }
   catch ( curlpp::LogicError & e )
     {
       std::cout << e.what() << std::endl;
     }
   catch ( curlpp::RuntimeError & e )
     {
       std::cout << e.what() << std::endl;
     }
   
   return 0;
}

