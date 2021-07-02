// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#ifndef TGVISD__TD__TD_HPP
#define TGVISD__TD__TD_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <mutex>
#include <thread>
#include <atomic>
#include <string>
#include <memory>
#include <cstdint>
#include <functional>
#include <unordered_map>

#include "Callback.hpp"

namespace tgvisd::Td {

using std::atomic;
using std::string;
using std::function;
using std::unique_ptr;
using std::unordered_map;

namespace td_api = td::td_api;
using Object = td_api::object_ptr<td_api::Object>;

class Td {
public:
	Td(uint32_t api_id, const char *api_hash, const char *data_path);
	void loop(int timeout);
	void close(void);
	void send_query(td_api::object_ptr<td_api::Function> f,
			std::function<void(Object)> handler);

	inline int64_t get_sess_user_id()
	{
		return user_id_;
	}

private:
	Callback		callback;
	uint32_t		api_id_;
	const char		*api_hash_;
	const char		*data_path_;
	bool			closed_ = false;
	bool			need_restart_ = false;
	bool			is_authorized_ = false;
	int64_t			user_id_ = 0;
	int64_t			client_id_ = 0;
	atomic<uint64_t>	current_query_id_{0};
	atomic<uint64_t>	authentication_query_id_{0};
	std::mutex		on_auth_update_mutex;

	using Object = td_api::object_ptr<td_api::Object>;

	unique_ptr<td::ClientManager>			client_manager_;
	td_api::object_ptr<td_api::AuthorizationState>	authorization_state_;

	unordered_map<int64_t, string> 				chat_title_;
	unordered_map<uint64_t, function<void(Object)>> 	handlers_;
	unordered_map<int32_t, td_api::object_ptr<td_api::user>>	users_;


	void restart(void);
	void on_authorization_state_update(void);
	void check_authentication_error(Object object);
	void process_response(td::ClientManager::Response response);
	void process_update(td_api::object_ptr<td_api::Object> update);
	std::function<void(Object object)>
		create_authentication_query_handler(void);
};

} /* namespace tgvisd::Td */


namespace detail {

template <class... Fs>
struct overload;

template <class F>
struct overload<F> : public F {
	explicit overload(F f) : F(f) {}
};

template <class F, class... Fs>
struct overload<F, Fs...>
	: public overload<F>
	, overload<Fs...> {
	overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {}
	using overload<F>::operator();
	using overload<Fs...>::operator();
};

}  /* namespace detail */


template <class... F>
auto overloaded(F... f) {
	return detail::overload<F...>(f...);
}

#endif /* #ifndef TGVISD__TD__TD_HPP */
