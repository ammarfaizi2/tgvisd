

/**
 * @return const char *
 */
inline static std::string shell_exec(const char *cmd)
{
    char *p;
    char out[10240];
    size_t total_len = 0;

    FILE *handle = popen(cmd, "r");
    if (handle == NULL) {
        sprintf(out, "Error: %s", strerror(errno));
        goto ret;
    }

    p = out;
    while ((total_len < sizeof(out)) && (fgets(p, 4096, handle) != NULL)) {
        size_t len  = strlen(p);
        p          += len;
        total_len  += len;
    }

    pclose(handle);

ret:
    return std::string(out);
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

    char c = ctext[0];
    if (!(c == '!' || c == '/' || c == '.'))
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
