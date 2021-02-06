
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot
 */

#include <TeaBot/TeaCurl.hpp>

#include <errno.h>
#include <cstring>
#include <stdexcept>

namespace TeaBot {

/**
 * @param char   *ptr
 * @param size_t size
 * @param size_t nmemb
 * @param void   *userdata
 * @throw std::runtime_error
 * @return size_t
 */
size_t TeaCurl::write_callback(char *ptr, size_t size, size_t nmemb,
							   void *userdata)
{
	TeaCurl *obj	  = (TeaCurl *)userdata;
	char	*buf	  = &(obj->output_[obj->outlen_]);
	size_t  realsiz   = size * nmemb;
	size_t  newoutlen = obj->outlen_ + realsiz;

	if (obj->output_alloc_ < (newoutlen + 1)) {
		obj->output_alloc_ = newoutlen + 4096;
		char *tmp = (char *)realloc(obj->output_, obj->output_alloc_);
		if (tmp == NULL)
			goto err;

		obj->output_ = tmp;
		buf		  = &(obj->output_[obj->outlen_]);
	}

	obj->outlen_ += realsiz;
	buf[realsiz]  = '\0';
	memcpy(buf, ptr, realsiz);

	return realsiz;
err:
	{
		int  err_code = errno;
		char err_str[1024];
		char *err_p = err_str;

		memcpy(err_p, "realloc error: ", 15);
		err_p += 15;
		strncpy(err_p, strerror(err_code), sizeof(err_str) - 16);
		throw std::runtime_error(std::string(err_str));
		return 0;
	}
}

/**
 * @throws std::runtime_error
 * @return void
 */
void TeaCurl::init()
{
	curl_ = curl_easy_init();
	if (!curl_) {
		throw std::runtime_error(
			"Cannot initialize TeaCurl handle: curl_easy_init()"
		);
	}
}


TeaCurl::~TeaCurl()
{
	if (curl_)
		curl_easy_cleanup(curl_);

	if (!release_heap_) {
		if (output_ != NULL)
			free(output_);
	}
}


/**
 * @throws std::runtime_error
 * @return void
 */
CURLcode TeaCurl::execute()
{
	CURLcode res;

	if (curl_) {

		output_alloc_ = 4096;
		output_ = (char *)malloc(output_alloc_);

		curl_easy_setopt(curl_, CURLOPT_URL, url_);
		curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl_, CURLOPT_WRITEDATA, (void *)this);
		res = curl_easy_perform(curl_);

		if (res != CURLE_OK)
			goto err;


		return res;
	} else {
		res = CURLE_NO_CONNECTION_AVAILABLE;
	}

err:
	{
		char err_curl[1024];
		char *err_p = err_curl;

		memcpy(err_p, "Curl error: ", 12);
		err_p += 12;
		strncpy(err_p, curl_easy_strerror(res), sizeof(err_curl) - 13);
		throw std::runtime_error(std::string(err_curl));
		return res;
	}
}

} /* namespace TeaBot */
