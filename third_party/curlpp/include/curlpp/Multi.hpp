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

#ifndef CURLPP_MULTI_HPP
#define CURLPP_MULTI_HPP


#include <curl/multi.h>

#include <list>
#include <map>


namespace curlpp
{


	class Easy;

	class Multi
	{

	public:

		struct Info 
		{
			CURLcode code;
			CURLMSG msg;
		};

	public:

		Multi();
		~Multi();

		void add(const curlpp::Easy * handle);
		void remove(const curlpp::Easy * handle);

		bool perform(int * nbHandles);
		void fdset(fd_set * read_fd_set,
								fd_set * write_fd_set,
								fd_set * exc_fd_set,
								int * max_fd);

		typedef std::list<std::pair<const curlpp::Easy *, Multi::Info> >
			Msgs;
		
		Msgs info();

	private:

		CURLM * mMultiHandle;
		std::map<CURL *, const curlpp::Easy *> mHandles;
	};


} // namespace curlpp

namespace cURLpp = curlpp;


#endif // #ifndef CURLPP_MULTI_HPP
