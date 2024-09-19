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

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <curlpp/Infos.hpp>

#include <cstdlib>


struct MethodClass
{
private:
  MethodClass();

public:
  MethodClass(std::ostream * stream)
    : mStream(stream)
    , writeRound(0)
  {}

  // Helper Class for reading result from remote host
  size_t debug(curlpp::Easy *handle, curl_infotype type, char* ptr, size_t size)
  {
    ++writeRound;

    curlpp::options::Url url;
    handle->getOpt(url);

    // Calculate the real size of the incoming buffer
    std::cerr << "write round: " << writeRound << ", url: " << url.getValue() << ", type: " << type << std::endl;
    mStream->write(ptr, size);

    // return the real size of the buffer...
    return size;
  };

  // Public member vars
  std::ostream * mStream;
  unsigned writeRound;
};


int main(int argc, char *argv[])
{
  if(argc != 2) {
    std::cerr << argv[0] << ": Wrong number of arguments" << std::endl 
	      << argv[0] << ": Usage: " << " url " 
	      << std::endl;
    return EXIT_FAILURE;
  }

  char *url = argv[1];
  
  try {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;

    MethodClass mObject(&std::cerr);
    
    // Set the debug callback to enable cURL 
    // to write result in a stream
    using namespace std::placeholders;
    curlpp::options::DebugFunction * test = new curlpp::options::DebugFunction(std::bind(&MethodClass::debug, &mObject, &request, _1, _2, _3));
    request.setOpt(test);
    
    // Setting the URL to retrive.
    request.setOpt(new curlpp::options::Url(url));
    request.setOpt(new curlpp::options::Verbose(true));
    request.perform();

    return EXIT_SUCCESS;
  }
  catch ( curlpp::LogicError & e ) {
    std::cout << e.what() << std::endl;
  }
  catch ( curlpp::RuntimeError & e ) {
    std::cout << e.what() << std::endl;
  }

  return EXIT_FAILURE;
}
