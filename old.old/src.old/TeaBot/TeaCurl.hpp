
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#ifndef TEABOT__TEACURL_HPP
#define TEABOT__TEACURL_HPP

#include <cstdlib>
#include <curl/curl.h>

namespace TeaBot {

class TeaCurl
{
private:
	CURLcode res_;
	CURL *curl_ = NULL;
	char *output_ = NULL;
	const char *url_ = NULL;
	size_t output_alloc_ = 0;
	size_t outlen_ = 0;
	bool   release_heap_ = false;

	static size_t write_callback(char *ptr, size_t size, size_t nmemb,
				     void *userdata);
public:
	inline TeaCurl()
	{
		init();
	}

	~TeaCurl();

	inline TeaCurl(const char *url): url_(url)
	{
		init();
	}

	inline void setUrl(const char *url) noexcept
	{
		url_ = url;
	}

	inline char *get_output()
	{
		release_heap_ = true;
		return output_;
	}

	inline size_t get_outlen()
	{
		return outlen_;
	}

	void init();

	CURLcode execute();
};

} /* namespace TeaBot */

#endif /* #ifndef TEABOT__TEACURL_HPP */
