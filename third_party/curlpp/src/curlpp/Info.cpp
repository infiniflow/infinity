#include "curlpp/internal/SList.hpp"

#include "curlpp/Info.hpp"

#include <string>
#include <list>


namespace curlpp 
{


template<>
void
InfoTypeConverter<std::string>::get(const curlpp::Easy & handle, 
				      CURLINFO info,
				      std::string & value)
{
  char * tmp;
  InfoGetter::get(handle, info, tmp);
  value = tmp;
}


template<>
void 
InfoTypeConverter<std::list<std::string> >::get(const curlpp::Easy & handle,
						   CURLINFO info,
						   std::list<std::string> & value)
{ 
  curl_slist * tmpList = NULL;
  InfoGetter::get(handle, info, tmpList);
	internal::SList slist(tmpList);
  value = slist.list();
}


template<>
void
InfoTypeConverter<long>::get(const curlpp::Easy & handle, 
			       CURLINFO info,
			       long & value)
{
  InfoGetter::get(handle, info, value);
}


template<>
void
InfoTypeConverter<double>::get(const curlpp::Easy & handle, 
				 CURLINFO info,
				 double & value)
{
  curl_off_t tmp;
  InfoGetter::get(handle, info, tmp);
  value = (double)tmp;
}


} // namespace curlpp
