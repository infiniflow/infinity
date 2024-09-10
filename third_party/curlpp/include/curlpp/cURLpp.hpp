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


#ifndef CURLPP_CURLPP_HPP
#define CURLPP_CURLPP_HPP

#define LIBCURLPP_VERSION "0.8.1"
#define LIBCURLPP_VERSION_NUM 0x000801



#include <curl/curl.h>

#include <string>
#include <cstdlib>

namespace curlpp
{

	/**
	* This function takes care of initializing curlpp (also libcURL).
	*
	* If you want to cleanup cURL before your application quits just call curlpp::terminate().
	* This function should only be called once (no matter how many threads or 
	* libcurl sessions that'll be used) by every application that uses libcurl, 
	* it will throw a logic_error if you call it twice.
	*
	* The flags option is a bit pattern that tells  libcurl  exact what  features
	* to init, as described below. Set the desired bits by ORing the values together.
	*
	* CURL_GLOBAL_ALL
	* Initialize  everything  possible.  This  sets all known bits.
	*
	* CURL_GLOBAL_SSL
	* Initialize SSL
	*  
	* CURL_GLOBAL_WIN32
	* Initialize  the  Win32  socket  libraries.
	*
	* CURL_GLOBAL_NOTHING
	* Initialise nothing extra. This sets no bit.
	*
	* NOTE: you should never call this function twice.
	*/

	void initialize(long flags = CURL_GLOBAL_ALL);

	/**
	* This function takes care of cleaning up curlpp (also libcURL). 
	* 
	* See curlpp::initialize(long flags) for more documentation.
	* NOTE: you cannot call this function if curlpp is not loaded, or if you already
	* called in once; it will throw a logic_error if you do otherwise.
	*/

	void terminate();

	/**
	* \depreciated This is an obsolete class. DO NOT use it. 
	*
	* The only reason it's still here,
	* is to be sure that it is backward compatible. This class was taking care of 
	* initialization and cleaning up curlpp (also libcURL) (it was calling 
	* curlpp:terminate() in his destructor). However, from now on, you do not need
	* this class. Note that the removal of this class was done because it was 
	* raising some threading issues. 
	*
	* Old documentation of that class: 
	*
	* If you want to be sure that curlpp is cleaned up after you reached 
	* the end of scope of a specific function using curlpp, instantiate 
	* this class. This function call curlpp::initialize() in his 
	* constructor, so you don't have to call it by yourself, when you have 
	* decided to use it.
	*
	* See curlpp::initialize(long flags) and curlpp:terminate() for more documentation.
	*/

	class Cleanup
	{

	public:

		Cleanup();
		~Cleanup();

	};

	/**
	* URL encodes string. 
	*
	* This  function  will  convert  the given input string to an URL encoded
	* string and return that as a new allocated string. All input  characters
	* that  are  not a-z, A-Z or 0-9 will be converted to their "URL escaped"
	* version (%NN where NN is a two-digit hexadecimal number).
	*/

	std::string escape(const std::string & url);

	/**
	* Decodes URL encoded string. 
	*
	* This  function  will  convert  the  given  URL encoded input string to a
	* "plain string" and return that as a new allocated string. All input
	* characters that are URL encoded (%XX) where XX is a two-digit
	* hexadecimal number, or +) will be converted to their plain text versions
	* (up to a ? letter, no + letters to the right of a ? letter will be
	* converted).
	*/

	std::string unescape(const std::string & url);

	/**
	* Portable wrapper for the getenv() function.
	*
	* It's meant to emulate 
	* its behaviour and provide an identical interface for all operating
	* systems libcurl builds on (including win32). Under unix operating
	* systems, there isn't any point in returning an allocated memory,
	* although other systems won't work  properly if this isn't done. The unix
	* implementation thus have to suffer slightly from the drawbacks of other
	* systems.
	*/

	std::string getenv(const std::string & name);

	/**
	* Returns  a  human readable string with the version number of libcurl and
	* some of its important components  (like  OpenSSL version).
	*
	* Note:  this  returns  the  actual running lib's version, you might have
	* installed a newer lib's include files in your system  which may turn
	* your LIBCURL_VERSION #define value to differ from this result.
	*/
	std::string libcurlVersion();

	/**
	* This function returns the number of  seconds  since  January 1st  1970,
	* for the date and time that the datestring parameter specifies. The now
	* parameter is there and should hold the current time to allow the
	* datestring to specify relative dates/times. Read further in the date
	* string parser section below.
	*
	* PARSING DATES AND TIMES
	* A  "date" is a string, possibly empty, containing many items separated
	* by whitespace.  The whitespace may be omitted when no ambiguity
	* arises.  The empty string means the beginning of today (i.e., midnight).
	* Order of the items is immaterial. A date string may contain many
	* flavors of items:
	*
	* calendar date items
	* This can be specified in a number of different ways. Including
	* 1970-09-17, 70-9-17, 70-09-17, 9/17/72, 24 September 1972, 24 Sept 72,
	* 24 Sep 72, Sep 24, 1972, 24-sep-72, 24sep72.  The year can also  be
	* omitted, for example: 9/17 or "sep 17".
	* 
	* time of the day items
	* This  string specifies the time on a given day. Syntax supported
	* includes: 18:19:0, 18:19, 6:19pm, 18:19-0500 (for specifying the time
	* zone as well).
	*
	* time zone items
	* Specifies international time zone. There are a few acronyms
	* supported, but in general you should instead use the specific
	* realtive time compared to UTC. Supported formats include: -1200, MST,
	* +0100.
	*
	* day of the week items
	* Specifies a day of the week. If this is mentioned alone it means that
	* day of the week in the future. Days of the week may be spelled out in
	* full: `Sunday', `Monday', etc or they may be abbreviated to their
	* first three letters, optionally followed by a period. The special
	* abbreviations `Tues' for `Tuesday', `Wednes' for `Wednesday' and `Thur'
	* or `Thurs' for `Thursday' are also allowed. A number may precede a day
	* of the week item to move forward supplementary weeks. It is best
	* used in expression like `third monday'. In this context, `last DAY'
	* or `next DAY' is also acceptable; they move one week before or after
	* the day that DAY by itself would represent.
	*
	* relative items
	* A relative item adjusts a date (or the current date if none) forward
	* or backward. Example syntax includes: "1 year", "1 year ago",
	* "2 days", "4 weeks". The string `tomorrow' is worth one day in the
	* future (equivalent to `day'), the string `yesterday' is worth one
	* day in the past (equivalent to `day ago').
	*
	* pure numbers
	* If the decimal number is of the form YYYYMMDD and no other calendar date
	* item appears before it in the date string, then YYYY is read as
	* the year, MM as the month number and DD as the day of the month, for the
	* specified calendar date.
	*/

	time_t getdate(const std::string & date, time_t * now = 0);


} // namespace curlpp

namespace cURLpp = curlpp;


#endif // #ifndef CURLPP_CURLPP_HPP
