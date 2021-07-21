
/**
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-v3
 * @package TeaBot::Modules::Debug
 */

#ifndef TEABOT__MODULES__DEBUG__MODULE_HPP
#define TEABOT__MODULES__DEBUG__MODULE_HPP

#include <cstring>
#include <TeaBot/TeaBotModule.hpp>

using TeaBot::TeaBotModule;

namespace TeaBot::Modules::Debug {

class Module : public TeaBotModule
{
public:
	inline static bool match(std::shared_ptr<Response> &res)
	{
		const std::string &text = res->getText();
		const char *ctext = text.c_str();
		size_t len = text.size();

		if (len < 6)
			return false;

		ctext++;
		if (memcmp(ctext, "debug", 5) != 0)
			return false;

		Module mod(res);
		mod.run();

		return true;
	}

	void run();
	inline Module(std::shared_ptr<Response> &res):
		TeaBotModule(res)
	{}
};

} /* namespace TeaBot::Modules::Debug */

#endif
