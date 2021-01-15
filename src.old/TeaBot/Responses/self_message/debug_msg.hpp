

/**
 * @param const char                                *ctext
 * @param size_t                                    len
 * @param TdLibHandler                              *handler
 * @param td_api::object_ptr<td::td_api::message>   &msg
 * @param int64_t                                   chat_id
 * @return bool
 */
inline static bool handle_debug_msg(const char *ctext, size_t len, TdLibHandler *handler,
                                    td_api::object_ptr<td::td_api::message> &msg,
                                    int64_t chat_id)
{
    if (len < 6)
        return false;

    ctext++;

    if (memcmp(ctext, "debug", 5) != 0)
        return false;


    ctext += 5;

    auto emsg = td_api::make_object<td_api::editMessageText>();
    auto imt  = td_api::make_object<td_api::inputMessageText>();

    imt->text_ = td_api::make_object<td_api::formattedText>();
    imt->text_->text_ = to_string(msg);
    emsg->chat_id_    = chat_id;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);

    handler->send_query(std::move(emsg), {});

    return true;
}
