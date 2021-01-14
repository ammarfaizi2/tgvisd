

/**
 * @return const char *
 */
inline static std::string shell_exec(const char *cmd)
{
    char out[8096];
    size_t outlen = 0;

    FILE *handle = popen(cmd, "r");
    if (handle == NULL) {
        outlen = (size_t)snprintf(out, sizeof(out), "Error: %s", strerror(errno));
        goto ret;
    }

    outlen = fread(out, sizeof(char), sizeof(out), handle);
    pclose(handle);

ret:
    {
        std::string ret;
        ret.assign(out, outlen);
        return ret;
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
inline static bool handle_shell_exec(const char *ctext, size_t len, TdLibHandler *handler,
                                     td_api::object_ptr<td::td_api::message> &msg,
                                     int64_t chat_id)
{
    if (len < 5)
        return false;

    ctext++;

    if (memcmp(ctext, "sh ", 3) != 0)
        return false;

    ctext += 3;

    auto emsg = td_api::make_object<td_api::editMessageText>();
    auto imt  = td_api::make_object<td_api::inputMessageText>();

    imt->text_ = td_api::make_object<td_api::formattedText>();
    imt->text_->text_ = shell_exec(ctext);
    emsg->chat_id_    = chat_id;
    emsg->message_id_ = msg->id_;
    emsg->input_message_content_ = std::move(imt);

    handler->send_query(std::move(emsg), {});

    return true;
}
