/**
* \file
* Cookies.
* 
*/

#include <string>
#include <sstream>
#include <vector>
#include <iostream>

#include <cstdlib>
#include <ctime>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>

#define CURLPP_ALLOW_NOT_AVAILABLE

#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>


class YesNo
{
public:
    explicit YesNo(bool yn) : yesno(yn) {}
		std::string operator()() const {
        return yesno ? "Yes" : "No";
    }
		friend std::ostream &operator<<(std::ostream &strm, const YesNo &yn) {
        strm << yn();
        return strm;
    }
private:
    bool yesno;
};

struct MyCookie
{
	std::string name;
	std::string value;
	std::string domain;
	std::string path;
	time_t expires;
	bool tail;
	bool secure;
};

std::ostream &
operator<<(std::ostream &strm, const MyCookie &cook)
{
	strm << "Cookie: '" << cook.name << "' (secure: " << YesNo(cook.secure) << ", tail: "
		<< YesNo(cook.tail) << ") for domain: '" << cook.domain << "', "
		<< "path: '" << cook.path << "'.\n";
	strm << "Value: '" << cook.value << "'.\n";
	strm << "Expires: '" << ctime(&cook.expires) << "'.\n";

	return strm;
}

std::vector<std::string> &
split_cookie_str(const std::string &str, std::vector<std::string> &in)
{
	std::string part;

	std::istringstream strm(str);
	while (getline(strm, part, '\t'))
		in.push_back(part);

	return in;
}

std::vector<std::string>
splitCookieStr(const std::string &str)
{
	std::vector<std::string> split;
	split_cookie_str(str, split);
	return split;
}

std::vector<std::string> &
splitCookieStr(const std::string &str, std::vector<std::string> &in)
{
	return split_cookie_str(str, in);
}

int StrToInt(const std::string &str)
{
	std::istringstream strm(str);
	int i = 0;
	if (!(strm >> i)) {
		throw curlpp::RuntimeError("Unable to convert string '" + str + "' to integer!");
	}
	return i;
}

MyCookie
MakeCookie(const std::string &str_cookie)
{
	std::vector<std::string> vC = splitCookieStr(str_cookie);
	MyCookie cook;

	cook.domain = vC[0];
	cook.tail = vC[1] == "TRUE";
	cook.path = vC[2];
	cook.secure = vC[3] == "TRUE";
	cook.expires = StrToInt(vC[4]);
	cook.name = vC[5];
	cook.value = vC[6];

	return cook;
}

int
main(void)
{
	try
	{
		curlpp::Cleanup myCleanup;
		curlpp::Easy exEasy;
		std::vector<std::string> cookieList;

		// a cookie as in HTTP header
		cookieList.push_back("Set-Cookie: GMAIL_AT=EXPIRED;expires=Sun, 17-Jan-2038 19:14:07 GMT; path=/; domain=.google.com");

		// a Netscape style cookie with \t
		cookieList.push_back(".google.com\tTRUE\t/\tFALSE\t2147483647\tLSID\tI like you GOOGLE");

		// a Netscape style cookie with tabs in string
		cookieList.push_back(".yahoo.com	TRUE	/	FALSE	0	YAHOO_COOKIE	I like you yahoo, too");

		exEasy.setOpt(new curlpp::options::Url("http://www.google.com"));
		exEasy.setOpt(new curlpp::options::FileTime(true));
		exEasy.setOpt(new curlpp::options::Verbose(true));

		// loop throught the cookies and add one by one
		//
		for (std::vector<std::string>::iterator it = cookieList.begin();
			it != cookieList.end();
			++it)
		{
			exEasy.setOpt(curlpp::options::CookieList(*it));
		}
		exEasy.perform();

		// see what cookies we got
		//
		std::cout << "\nCookies from cookie engine:" << std::endl;
		std::list<std::string> cookies;
		curlpp::infos::CookieList::get(exEasy, cookies);
		int i = 1;
		for (std::list<std::string>::const_iterator it = cookies.begin();
			it != cookies.end();
			++it, i++)
		{
			std::cout << "[" << i << "]: " << MakeCookie(*it) << std::endl;
		}

		exit(EXIT_SUCCESS);
	}

	catch(curlpp::RuntimeError &e)
	{
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}

	catch(curlpp::LogicError &e)
	{
		std::cout << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}
