// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Td
 *
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

#include <chrono>
#include <iostream>
#include <condition_variable>

namespace td_api = td::td_api;
using Object = td_api::object_ptr<td_api::Object>;

#include "Callback.hpp"

namespace tgvisd::Td {

using std::atomic;
using std::string;
using std::function;
using std::unique_ptr;
using std::unordered_map;
using namespace std::chrono_literals;

class Td {
public:
	Callback	callback;

	Td(uint32_t api_id, const char *api_hash, const char *data_path);
	void loop(int timeout);
	void close(void);
	void send_query(td_api::object_ptr<td_api::Function> f,
			std::function<void(Object)> handler);

	inline void send_query(td_api::object_ptr<td_api::Function> f)
	{
		return send_query(std::move(f), {});
	}

	template <typename METHOD, typename RETURN>
	td_api::object_ptr<RETURN> send_query_sync(
		td_api::object_ptr<METHOD> method
	);

	inline int64_t get_sess_user_id()
	{
		return user_id_;
	}

private:
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


	inline uint64_t next_query_id()
	{
		return atomic_fetch_add(&current_query_id_, 1);
	}
};


template <typename METHOD, typename RETURN>
td_api::object_ptr<RETURN> Td::send_query_sync(td_api::object_ptr<METHOD> method)
{
	std::mutex mut;
	bool finished = false;
	std::condition_variable cond;
	std::unique_lock<std::mutex> lock(mut, std::defer_lock);
	td_api::object_ptr<RETURN> retVal;

	uint32_t warnCounter = 0;
	const uint32_t warnAtCounter = 60;

	auto callback = [&](td_api::object_ptr<td_api::Object> obj) {

		if (obj->get_id() == td_api::error::ID) {
			auto err = td::move_tl_object_as<td_api::error>(obj);
			printf("Error on doQuerySync: %s\n",
			       err->message_.c_str());
			goto out;
		}

		if (obj->get_id() != RETURN::ID) {
			puts("Invalid object returned on send_query_sync");
			goto out;
		}

		retVal = td::move_tl_object_as<RETURN>(obj);
	out:
		finished = true;
		cond.notify_one();
	};

	send_query(std::move(method), callback);

	lock.lock();
	while (!finished) {
		cond.wait_for(lock, 1000ms, [&finished](void){
			return finished;
		});

		if (++warnCounter > warnAtCounter) {
			printf("Warning: Td::send_query_sync stall for %us\n",
			       warnCounter);
		}
	}

	return retVal;
}

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
