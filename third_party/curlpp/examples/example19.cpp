/*
 *    Copyright (c) <2002-2005> <Gazihan Alankus>
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
* Forms demo.
* 
*/


#include <fstream>

#include <cstdlib>
#include <cerrno>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>

int main(int argc, char *argv[])
{
  if(argc < 2) {
    std::cerr << argv[0] << ": Wrong number of arguments" << std::endl 
	      << "Usage: " << argv[0] << " url"
	      << std::endl;
    return EXIT_FAILURE;
  }
  
  char *url = argv[1];
  
  try {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;
    
    request.setOpt(new curlpp::options::Url(url)); 
    //request.setOpt(new curlpp::options::Verbose(true)); 
    
    {
      // Forms takes ownership of pointers!
      curlpp::Forms formParts;
      formParts.push_back(new curlpp::FormParts::Content("name1", "value1"));
      formParts.push_back(new curlpp::FormParts::Content("name2", "value2"));
      
      request.setOpt(new curlpp::options::HttpPost(formParts)); 
    }

    // The forms have been cloned and are valid for the request, even
    // if the original forms are out of scope.
    std::ofstream myfile("/dev/null");
    myfile << request << std::endl << request << std::endl;
  }
  catch ( curlpp::LogicError & e ) {
    std::cout << e.what() << std::endl;
  }
  catch ( curlpp::RuntimeError & e ) {
    std::cout << e.what() << std::endl;
  }

  return EXIT_SUCCESS;
}
