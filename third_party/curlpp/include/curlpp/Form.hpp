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

#ifndef CURLPP_FORM_HPP
#define CURLPP_FORM_HPP


#include <utilspp/clone_ptr.hpp>

#include <curl/curl.h>

#include <list>
#include <string>


namespace curlpp
{


	class FormPart;


	typedef std::list<utilspp::clone_ptr<curlpp::FormPart> > Forms;


	/**
	* This class is used internally to wrap over curl_httppost
	* class.
	*/

	class HttpPost
	{

	public:

		HttpPost(const Forms & posts);
		HttpPost();
		~HttpPost();

		/**
		* initialize the HTTP post with the list of forms. The Forms 
		* will be cloned.
		*/
		HttpPost & operator=(const Forms & posts);

		operator Forms() { return getList(); }


		/**
		* return the curl_httppost representation of this HTTP Post.
		* Be aware that the memory return is owned by the current 
		* instance, so don't try to delete it.
		*/
		::curl_httppost * cHttpPost() const;

		/**
		* Free all HTTP posts.
		*/
		void clear();

		/**
		* Get the list. 
		*/
		Forms getList();

	private:

		::curl_httppost * mFirst;
		::curl_httppost * mLast;
		Forms mForms;

	};


	/**
	* This class is the base representation of a post. You need
	* to inherit from it to define a type of post.
	*/

	class FormPart
	{
		friend class HttpPost;

	public:

		/**
		* initialize the FormPart. "name" is the name of the field.
		*/
		FormPart(const char * name);

		/**
		* initialize the FormPart. "name" is the name of the field.
		*/
		FormPart(const std::string & name);

		virtual ~FormPart();

		virtual FormPart * clone() const = 0;

	protected:

		/**
		* it will add himself to the curl_httppost * first.
		*/
		virtual void add(::curl_httppost ** first, 
			::curl_httppost ** last) = 0;

		/**
		* Contain the name of the field.
		*/
		const std::string mName;

	};


namespace FormParts
{


	/**
	* This class is a file post. It will send a file in the
	* HTTP post.
	*/

	class File : public FormPart
	{

	public:

		/**
		* initialize a File part. "name" is the name of the field. 
		* "filename" is the string that holds the filename.
		*/
		File(const char * name, 
			const char * filename);

		/**
		* initialize a File part. "name" is the name of the field. 
		* "filename" is the string that holds the filename. 
		* "contentType" is the MIME type of the file.
		*/
		File(const char * name, 
			const char * filename, 
			const char * contentType);

		/**
		* initialize a File part. "name" is the name of the field. 
		* "filename" is the string that holds the filename.
		*/
		File(const std::string & name, 
			const std::string & filename);

		/**
		* initialize a File part. "name" is the name of the field. 
		* "filename" is the string that holds the filename. 
		* "contentType" is the MIME type of the file.
		*/
		File(const std::string & name, 
			const std::string & filename,
			const std::string & contentType);

		virtual ~File();

		/**
		* This function will return a copy of the instance.
		*/
		virtual File * clone() const;

	private:

		void add(::curl_httppost ** first, 
			::curl_httppost ** last);

	private:

		const std::string mFilename; 
		const std::string mContentType;

	};


	/**
	* This class is a file post. It will send a file in the
	* HTTP post.
	*/

	class Content : public FormPart
	{

	public:

		/**
		* initialize a Content part. "name" is the name of the field. 
		* "content" is the string that holds the filename.
		*/
		Content(const char * name, 
			const char * content);

		/**
		* initialize a Content part. "name" is the name of the field. 
		* "content" is the string that holds the filename. 
		* "contentType" is the MIME type of the file.
		*/
		Content(const char * name, 
			const char * content, 
			const char * contentType);

		/**
		* initialize a Content part. "name" is the name of the field. 
		* "content" is the string that holds the content.
		*/
		Content(const std::string & name, 
			const std::string & content);

		/**
		* initialize a Content part. "name" is the name of the field. 
		* "content" is the string that holds the content. 
		* "content_type" is the MIME type of the file.
		*/
		Content(const std::string & name, 
			const std::string & content,
			const std::string & content_type);

		virtual ~Content();

		/**
		* This function will return a copy of the instance.
		*/
		virtual Content * clone() const;

	private:

		void add(::curl_httppost ** first, 
			::curl_httppost ** last);

	private:

		const std::string mContent; 
		const std::string mContentType;

	};


} // namespace FormParts


} // namespace curlpp

namespace cURLpp = curlpp;


#endif //CURLPP_FORM_HPP
