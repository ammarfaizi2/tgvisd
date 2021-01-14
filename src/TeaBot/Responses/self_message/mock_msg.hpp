
/**
 * @param char x
 * @return char
 */
inline static char tolower(char x)
{
  return ('A' <= x && x <= 'Z') ? x + 32 : x;
}

/**
 * @param char x
 * @return char
 */
inline static char toupper(char x)
{
  return ('a' <= x && x <= 'z') ? x - 32 : x;
}

/**
 * @param char   *str
 * @param size_t len
 * @return void
 */
inline static void mock_string(char *str, size_t len)
{
  bool c = true;
  while (len--) {
    *str = c ? tolower(*str) : toupper(*str);
    str++;
    c = !c;
  }
}

/**
 * @param const char                                *ctext
 * @param size_t                                    len
 * @param TdLibHandler                              *handler
 * @param td_api::object_ptr<td::td_api::message>   &msg
 * @param int64_t                                   chat_id
 * @return bool
 */
inline static bool handle_mock_msg(const char *ctext, size_t len, TdLibHandler *handler,
                                   td_api::object_ptr<td::td_api::message> &msg,
                                   int64_t chat_id)
{
    if (len < 5)
        return false;

    ctext++;

    if (memcmp(ctext, "mock", 4) != 0)
        return false;

    bool action = false;
    std::string replied_text;
    pthread_mutex_t query_mutex = PTHREAD_MUTEX_INITIALIZER;


    /* Get the replied text message. */
    auto getMsg = td_api::make_object<td_api::getMessage>(
        msg->reply_in_chat_id_,
        msg->reply_to_message_id_
    );

    pthread_mutex_lock(&query_mutex);
    handler->send_query(std::move(getMsg),
        [&action, &replied_text, &query_mutex] (auto object) {

        if (object->get_id() == td_api::error::ID)
            goto ret;

        {
            auto msg = td::move_tl_object_as<td_api::message>(object);
            if (msg->content_->get_id() != td_api::messageText::ID)
                goto ret; /* Skip non text message. */

            replied_text = static_cast<td_api::messageText &>(*(msg->content_)).text_->text_;
            action = true;
        }

        ret:
        pthread_mutex_unlock(&query_mutex);
    });
    pthread_mutex_lock(&query_mutex);

    if (!action)
        return false;

    mock_string((char *)replied_text.c_str(), replied_text.length());

    /* Edit message. */
    auto emsg = td_api::make_object<td_api::editMessageText>();
    auto imt  = td_api::make_object<td_api::inputMessageText>();

    imt->text_ = td_api::make_object<td_api::formattedText>();
    imt->text_->text_ = std::move(replied_text);
    emsg->chat_id_    = chat_id;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);

    handler->send_query(std::move(emsg), {});

    return true;
}
