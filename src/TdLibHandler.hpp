
#ifndef __TdLibHandler_HPP
#define __TdLibHandler_HPP

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

#include <map>
#include <string>
#include <vector>
#include <limits>
#include <memory>
#include <cstdint>
#include <sstream>
#include <iostream>
#include <functional>

namespace detail {

template <class... Fs>
struct overload;

template <class F>
struct overload<F> : public F {
  explicit overload(F f) : F(f) {
  }
};

template <class F, class... Fs>
struct overload<F, Fs...>
    : public overload<F>
    , overload<Fs...> {
  overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {
  }
  using overload<F>::operator();
  using overload<Fs...>::operator();
};

}  /* namespace detail */

template <class... F>
auto overloaded(F... f) {
  return detail::overload<F...>(f...);
}

namespace td_api = td::td_api;

namespace TeaBot {

class TdLibHandler
{
private:
  using Object = td_api::object_ptr<td_api::Object>;

  int32_t  client_id_{0};
  uint64_t current_query_id_{0};
  uint64_t authentication_query_id_{0};

  bool need_restart_{false};
  bool are_authorized_{false};

  std::unique_ptr<td::ClientManager> client_manager_;

  std::map<int64_t, std::string> chat_title_;
  std::map<uint64_t, std::function<void(Object)>> handlers_;
  std::map<int32_t, td_api::object_ptr<td_api::user>> users_;

  td_api::object_ptr<td_api::AuthorizationState> authorization_state_;

  void
  restart();

  uint64_t
  next_query_id();

  void
  process_response(td::ClientManager::Response response);

  void
  process_update(td_api::object_ptr<td_api::Object> update);

  void
  send_query(td_api::object_ptr<td_api::Function> f,
             std::function<void(Object)> handler);

  std::string
  get_user_name(int32_t user_id) const;

  std::string
  get_chat_title(int64_t chat_id) const;

  void
  on_authorization_state_update();

  std::function<void(Object object)>
  create_authentication_query_handler();

  void
  check_authentication_error(Object object);

  bool
  handle_loop();
public:
  TdLibHandler(const char *storage_path);

  void
  loop();
};

} /* namespace TeaBot */

#endif /* #ifndef __TdLibHandler_HPP */
