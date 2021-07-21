// SPDX-License-Identifier: GPL-2.0
/*
 * @author Ammar Faizi <ammarfaizi2@gmail.com> https://www.facebook.com/ammarfaizi2
 * @license GPL-2.0
 * @package tgvisd::Main::Preload
 *
 * Copyright (C) 2021 Ammar Faizi <ammarfaizi2@gmail.com>
 */

#if defined(__linux__)
#  include <pthread.h>
#endif

#include <stack>
#include <cstdio>
#include <mutex>
#include <chrono>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <atomic>
#include <unordered_map>
#include <condition_variable>

#include <errno.h>
#include <cstring>
#include <md5_php.h>
#include <sha1_php.h>

#include <tgvisd/DB.hpp>

#include "HistoryScraper.hpp"

namespace tgvisd::Modules::Preload {

using tgvisd::DBt::PreparedStatement;
using namespace std::chrono_literals;

HistoryScraper::HistoryScraper(void)
{
}

class Worker
{
public:
	inline ~Worker(void)
	{
		if (db_)
			delete db_;
	}

	Worker(tgvisd::Main::Main *main);
	void run(void);
	inline bool stopUpdate(void)
	{
		return main_->stopUpdate();
	}

	inline tgvisd::Td::Td *getTd(void)
	{
		return td_;
	}


	// void gatherMsg(void);
	// void gatherMsgByChatId(int64_t chat_id);
	td_api::object_ptr<td_api::user> getUser(int64_t user_id);
	td_api::object_ptr<td_api::chat> getChat(int64_t chat_id);
	td_api::object_ptr<td_api::chats> getChats(void);
	td_api::object_ptr<td_api::messages> getChatHistory(int64_t chat_id);
	td_api::object_ptr<td_api::file> downloadFile(int32_t file_id);

	void gatherChatEventLog(int64_t chat_id);
	int64_t processChatEventLog(td_api::object_ptr<td_api::chatEvents> events);
	void processMessage(td_api::message &msg);

	uint64_t resolveUser(td_api::user &user, td_api::user **user_p);
	int64_t resolveGroup(td_api::chat &chat, td_api::chat **chat_p);
	uint64_t resolveMessage(td_api::message &msg, uint64_t db_user_id,
				uint64_t db_group_id);
	uint64_t resolveFile(tgvisd::DB *db, td_api::file &file);
	uint64_t lastInsertId(void);
private:
	tgvisd::DB *db_ = nullptr;
	tgvisd::Td::Td *td_ = nullptr;
	tgvisd::Main::Main *main_ = nullptr;

	std::unordered_map<std::int64_t, std::uint64_t> userTgToDb_;
	std::unordered_map<std::int64_t, std::uint64_t> groupTgToDb_;
	std::unordered_map<std::uint64_t, td_api::user> userMap_;
	std::unordered_map<std::int64_t, td_api::chat> groupMap_;

	void insertMsgDataText(uint64_t db_msg_id, td_api::message &msg);
	void insertMsgDataPhoto(uint64_t db_msg_id, td_api::message &msg);
	void _insertMsgDataPhoto(uint64_t db_msg_id, td_api::message &msg);
	bool trackEventId(int64_t event_id);
};


mbp_ret_t HistoryScraper::initPreload(tgvisd::Main::Main *main)
{
	main->incRef();
	std::thread worker = std::thread([main](void){
		Worker *wrk = new Worker(main);
		wrk->run();
		delete wrk;
		main->decRef();
	});

#if defined(__linux__)
	{
		char trname[32];
		pthread_t pt = worker.native_handle();
		snprintf(trname, sizeof(trname), "history-scraper");
		pthread_setname_np(pt, trname);
	}
#endif
	worker.detach();
	return MBP_CONTINUE;
}


Worker::Worker(tgvisd::Main::Main *main):
	main_(main)
{
	td_ = main->getTd();
	assert(td_);
}


// td_api::object_ptr<td_api::chats> Worker::getChats(void)
// {
// 	return td_->send_query_sync<td_api::getChats, td_api::chats>(
// 		td_api::make_object<td_api::getChats>(
// 			nullptr,
// 			std::numeric_limits<std::int64_t>::max(),
// 			0,
// 			100
// 		)
// 	);
// }


// td_api::object_ptr<td_api::messages> Worker::getChatHistory(int64_t chat_id)
// {
// 	int64_t from_message_id = 0;
// 	int32_t offset = 0;
// 	int32_t limit = 100;
// 	bool only_local = false;
// 	return td_->send_query_sync<td_api::getChatHistory, td_api::messages>(
// 		td_api::make_object<td_api::getChatHistory>(
// 			chat_id,
// 			from_message_id,
// 			offset,
// 			limit,
// 			only_local
// 		)
// 	);
// }


td_api::object_ptr<td_api::chat> Worker::getChat(int64_t chat_id)
{
	return td_->send_query_sync<td_api::getChat, td_api::chat>(
		td_api::make_object<td_api::getChat>(chat_id),
		30
	);
}


td_api::object_ptr<td_api::user> Worker::getUser(int64_t user_id)
{
	printf("In getUser(): %ld\n", user_id);
	return td_->send_query_sync<td_api::getUser, td_api::user>(
		td_api::make_object<td_api::getUser>(user_id)
		, 30
	);
}


// void Worker::gatherMsg(void)
// {
// 	td_api::object_ptr<td_api::chats> chats = getChats();

// 	if (unlikely(this->stopUpdate()))
// 		return;

// 	if (!chats) {
// 		std::cout << "Chat is null on gatherMsg" << std::endl;
// 		return;
// 	}

// 	for (auto chat_id: chats->chat_ids_) {
// 		if (chat_id > 0)
// 			continue;

// 		gatherMsgByChatId(chat_id);
// 	}

// 	sleep(1);
// }


// void Worker::gatherMsgByChatId(int64_t chat_id)
// {
// 	td_api::object_ptr<td_api::messages> msgs = getChatHistory(chat_id);

// 	if (unlikely(this->stopUpdate()))
// 		return;

// 	if (!msgs) {
// 		std::cout
// 			<< "Messages is null on gatherMsgByChatId"
// 			<< std::endl;
// 		return;
// 	}

// 	if (msgs->total_count_ < 5)
// 		return;

// 	td_api::object_ptr<td_api::chat> chat = getChat(chat_id);


// 	for (auto &msg: msgs->messages_) {

// 		if (unlikely(!msg))
// 			continue;

// 		auto &content = msg->content_;
// 		if (content->get_id() != td_api::messageText::ID)
// 			continue;

// 		if (chat->title_ != "GNU/Weeb")
// 			continue;

// 		std::string &text = static_cast<td_api::messageText &>(*content).text_->text_;
// 		printf("[chat_id: %ld] [title: %s] [msg_id: %ld]: \"%s\"\n",
// 			chat->id_,
// 			chat->title_.c_str(),
// 			(msg->id_ >> 20u),
// 			text.c_str());
// 	}
// }


td_api::object_ptr<td_api::file> Worker::downloadFile(int32_t file_id)
{
	int32_t prio = 32;
	int32_t limit = 0;
	int32_t offset = 0;
	bool sync = true;
	
	return td_->send_query_sync<td_api::downloadFile, td_api::file>(
		td_api::make_object<td_api::downloadFile>(
			file_id,
			prio,
			limit,
			offset,
			sync
		),
		30
	);
}


const char *dbhost, *dbuser, *dbpass, *dbname;

static inline tgvisd::DB *createDB(void)
{
	tgvisd::DB *db_ = new tgvisd::DB(dbhost, 0, dbuser, dbpass, dbname);
	db_->connect();
	return db_;
}

void Worker::run(void)
{
	dbhost = getenv("DB_HOST");
	if (!dbhost) {
		puts("Missing DB_HOST");
		abort();
	}
	dbuser = getenv("DB_USER");
	if (!dbuser) {
		puts("Missing DB_USER");
		abort();
	}
	dbpass = getenv("DB_PASS");
	if (!dbpass) {
		puts("Missing DB_PASS");
		abort();
	}
	dbname = getenv("DB_NAME");
	if (!dbname) {
		puts("Missing DB_NAME");
		abort();
	}
	

	try {
		sleep(2);
		db_ = createDB();
		// gatherChatEventLog(-1001483770714); // GNU/Weeb
		gatherChatEventLog(-1001226735471); // Private Cloud
	} catch (std::string &err) {
		std::cout << "Error: " << err << std::endl;
		throw err;
	}
}


void Worker::gatherChatEventLog(int64_t chat_id)
{
	int64_t min_ev = 0;
	std::vector<int32_t> user_ids;
	printf("In gatherChatEventLog\n");

	// auto st = db_->prepare("SELECT MIN(id) FROM track_event_id;");
	// st->execute();
	// if (auto row = st->fetch()) {
	// 	uint64_t t_min = 0;
	// 	IS_OK(mysqlx_get_uint(row, 0, &t_min), st->getStmt());
	// 	min_ev = t_min;
	// 	pr_debug("min_ev from DB: %ld", min_ev);
	// }

	while (min_ev != -1) {
		auto events = td_->send_query_sync<
			td_api::getChatEventLog,
			td_api::chatEvents
		>(
			td_api::make_object<td_api::getChatEventLog>(
				chat_id,
				"",
				min_ev,
				100,
				nullptr,
				std::move(user_ids)
			),
			30
		);

		if (!events)
			break;
		
		min_ev = processChatEventLog(std::move(events));
		pr_debug("min_ev = %ld", min_ev);
	}
}


bool Worker::trackEventId(int64_t event_id)
{
	try {
		auto st = db_->prepare("INSERT INTO track_event_id VALUES (?)");
		st->bind(
			PARAM_UINT(event_id),
			PARAM_END
		);
		st->execute();
	} catch (std::string &err) {
		pr_debug("skip dup: %ld", event_id);
		return false;
	}
	return true;
}


int64_t Worker::processChatEventLog(td_api::object_ptr<td_api::chatEvents> events)
{
	printf("In processChatEventLog\n");
	int64_t min_event = -1;
	size_t i = 0;
	for (auto &ev: events->events_) {
		if (i == 0) {
			min_event = ev->id_;
		} else if (ev->id_ < min_event) {
			min_event = ev->id_;
		}
		i++;

		if (!trackEventId(ev->id_))
			continue;

		if (ev->action_->get_id() != td_api::chatEventMessageDeleted::ID)
			continue;
		auto &msg = static_cast<td_api::chatEventMessageDeleted &>(*ev->action_).message_;
		processMessage(*msg);
	}
	return min_event;
}


void Worker::processMessage(td_api::message &msg)
{
	printf("In processMessage\n");
	if (msg.sender_->get_id() != td_api::messageSenderUser::ID)
		return;

	auto user_ = getUser(
		static_cast<td_api::messageSenderUser &>(*msg.sender_).user_id_
	);
	td_api::user *user;
	auto chat_ = getChat(msg.chat_id_);
	td_api::chat *chat;


	// std::cout << to_string(msg) << std::endl;

	// std::cout << to_string(user) << std::endl;
	uint64_t db_user_id = resolveUser(*user_, &user);
	uint64_t db_group_id = resolveGroup(*chat_, &chat);
	// printf("db_user_id = %lu\n", db_user_id);
	// printf("db_group_id = %lu\n", db_group_id);
	resolveMessage(msg, db_user_id, db_group_id);
}


static char *getDateByUnixTM(time_t epoch, char *buffer, size_t len)
{
	strftime(buffer, len, "%Y-%m-%d %H:%I:%S", gmtime(&epoch));
	return buffer;
}


static char *getDateNow(char *buffer, size_t len)
{
	time_t epoch_now = std::time(nullptr);
	return getDateByUnixTM(epoch_now, buffer, len);
}


static uint64_t lastInsertId_static(tgvisd::DB *db_)
{
	uint64_t ret = 0;
	auto st = db_->prepare("SELECT LAST_INSERT_ID();");
	st->execute();
	if (auto row = st->fetch()) {
		IS_OK(mysqlx_get_uint(row, 0, &ret), st->getStmt());
	}
	return ret;
}


uint64_t Worker::lastInsertId(void)
{
	return lastInsertId_static(db_);
}


uint64_t Worker::resolveUser(td_api::user &user, td_api::user **user_p)
{
	uint64_t ret = 0;
	std::unique_ptr<PreparedStatement> st;
	const char query[] = 
		"INSERT INTO `gw_users` "		\
		"(" 					\
			"`tg_user_id`,"			\
			"`username`," 			\
			"`first_name`,"			\
			"`last_name`,"			\
			"`is_bot`,"			\
			"`created_at`,"			\
			"`updated_at`"			\
		")" 					\
		" VALUES (?, ?, ?, ?, '0', ?, NULL);";

	std::unordered_map<int64_t, uint64_t>::const_iterator got =
		userTgToDb_.find(user.id_);

	if (!(got == userTgToDb_.end())) {
		ret = got->second;
		goto out_no_move;
	}

	st = db_->prepare(query);
	try {
		char dateBuf[64];
		const char *dateNow = getDateNow(dateBuf, sizeof(dateBuf));
		st->bind(
			PARAM_UINT((uint64_t)user.id_),
			PARAM_STRING(user.username_.c_str()),
			PARAM_STRING(user.first_name_.c_str()),
			PARAM_STRING(user.last_name_.c_str()),
			PARAM_STRING(dateNow),
			PARAM_END
		);
		st->execute();
	} catch (std::string &err) {
		goto do_fetch;
	}

	if ((ret = lastInsertId()))
		goto out;

do_fetch:
	pr_debug("Fetching from DB (user)...");
	st = db_->prepare("SELECT id FROM gw_users WHERE tg_user_id = ?");
	st->bind(
		PARAM_SINT((uint64_t)user.id_),
		PARAM_END
	);
	st->execute();

	if (auto row = st->fetch()) {
		IS_OK(mysqlx_get_uint(row, 0, &ret), st->getStmt());
		printf("fetched = %lu\n", ret);
		if (ret)
			goto out;
	}

out_no_move:
	*user_p = &user;
	return ret;

out:
	userTgToDb_.insert_or_assign(user.id_, ret);
	userMap_.insert_or_assign(ret, std::move(user));
	*user_p = &userMap_[ret];
	return ret;
}


int64_t Worker::resolveGroup(td_api::chat &chat, td_api::chat **chat_p)
{
	uint64_t ret = 0;
	std::unique_ptr<PreparedStatement> st;
	const char query[] = 
		"INSERT INTO `gw_groups` "		\
		"(" 					\
			"`tg_group_id`,"		\
			"`username`," 			\
			"`name`,"			\
			"`link`,"			\
			"`created_at`,"			\
			"`updated_at`"			\
		")" 					\
		" VALUES (?, ?, ?, NULL, ?, NULL);";

	std::unordered_map<int64_t, uint64_t>::const_iterator got =
		groupTgToDb_.find(chat.id_);

	if (!(got == groupTgToDb_.end())) {
		ret = got->second;
		goto out_no_move;
	}

	st = db_->prepare(query);
	try {
		char dateBuf[64];
		const char *dateNow = getDateNow(dateBuf, sizeof(dateBuf));
		st->bind(
			PARAM_SINT((uint64_t)chat.id_),
			PARAM_STRING("GNUWeeb"),
			PARAM_STRING(chat.title_.c_str()),
			PARAM_STRING(dateNow),
			PARAM_END
		);
		st->execute();
	} catch (std::string &err) {
		goto do_fetch;
	}

	if ((ret = lastInsertId()))
		goto out;

do_fetch:
	pr_debug("Fetching from DB (group)...");
	st = db_->prepare("SELECT id FROM gw_groups WHERE tg_group_id = ?");
	st->bind(
		PARAM_SINT((uint64_t)chat.id_),
		PARAM_END
	);
	st->execute();

	if (auto row = st->fetch()) {
		IS_OK(mysqlx_get_uint(row, 0, &ret), st->getStmt());
		printf("fetched = %lu\n", ret);
		if (ret)
			goto out;
	}

out_no_move:
	*chat_p = &chat;
	return ret;

out:
	groupTgToDb_.insert_or_assign(chat.id_, ret);
	groupMap_.insert_or_assign(chat.id_, std::move(chat));
	*chat_p = &groupMap_[ret];
	return ret;
}


uint64_t Worker::resolveMessage(td_api::message &msg, uint64_t db_user_id,
				uint64_t db_group_id)
{
	uint64_t ret = 0;
	const char query[] =
		"INSERT INTO `gw_group_messages`"		\
		"("						\
			"`group_id`,"				\
			"`user_id`,"				\
			"`tg_msg_id`,"				\
			"`reply_to_tg_msg_id`,"			\
			"`msg_type`,"				\
			"`has_edited_msg`,"			\
			"`is_forwarded_msg`,"			\
			"`created_at`,"				\
			"`updated_at`"				\
		") VALUES (?, ?, ?, ?, ?, '0', '0', ?, NULL);";

	const char *msg_type = NULL;

	switch (msg.content_->get_id()) {
	case td_api::messageText::ID:
		msg_type = "text";
		break;
	case td_api::messagePhoto::ID:
		msg_type = "photo";
		break;
	case td_api::messageVideo::ID:
		msg_type = "video";
		break;
	case td_api::messageSticker::ID:
		msg_type = "sticker";
		break;
	default:
		std::cout << "Got default case, skipping...\n" << std::endl;
		return 0;
	}


	printf("Saving %s\n", msg_type);
	char dateBuf[64];
	const char *dateNow = getDateNow(dateBuf, sizeof(dateBuf));
	auto st = db_->prepare(query);
	st->execute(
		PARAM_UINT(db_group_id),
		PARAM_UINT(db_user_id),
		PARAM_UINT(msg.id_ >> 20u),
		PARAM_UINT(msg.reply_to_message_id_ >> 20u),
		PARAM_STRING(msg_type),
		PARAM_STRING(dateNow),
		PARAM_END
	);

	ret = lastInsertId();
	switch (msg.content_->get_id()) {
	case td_api::messageText::ID:
		insertMsgDataText(ret, msg);
		break;
	case td_api::messagePhoto::ID:
		insertMsgDataPhoto(ret, msg);
		break;
	case td_api::messageVideo::ID:
		break;
	case td_api::messageSticker::ID:
		break;
	}

	return 0;
}


void Worker::insertMsgDataText(uint64_t db_msg_id, td_api::message &msg)
{
	const char query[] =
		"INSERT INTO `gw_group_message_data`"	\
		"("					\
			"`msg_id`,"			\
			"`text`,"			\
			"`text_entities`,"		\
			"`file`,"			\
			"`is_edited`,"			\
			"`tg_date`,"			\
			"`created_at`"			\
		") VALUES (?, ?, NULL, NULL, '0', ?, ?);";

	auto &msgText = static_cast<td_api::messageText &>(*msg.content_);
	auto st = db_->prepare(query);
	char dateBuf1[64], dateBuf2[64];
	const char *msgDate = getDateByUnixTM(msg.date_, dateBuf1, sizeof(dateBuf1));
	const char *dateNow = getDateNow(dateBuf2, sizeof(dateBuf2));
	st->bind(
		PARAM_UINT(db_msg_id),
		PARAM_STRING(msgText.text_->text_.c_str()),
		PARAM_STRING(msgDate),
		PARAM_STRING(dateNow),
		PARAM_END
	);
	st->execute();
}


static const char *get_filename_ext(const char *filename)
{
	const char *dot = strrchr(filename, '.');
	if (!dot || dot == filename)
		return "";
	return dot + 1;
}


uint64_t Worker::resolveFile(tgvisd::DB *db, td_api::file &file)
{
	unsigned char md5_buf[16], sha1_buf[20];
	uint64_t ret = 0;
	pr_debug("Downloading file...");
	auto f = downloadFile(file.id_);
	if (!f) {
		pr_debug("Download fail?");
		return ret;
	}
	pr_debug("Downloaded OK!");
	std::cout << to_string(f) << std::endl;

	auto &loc = f->local_;
	if (loc->path_ == "") {
		pr_debug("Cannot find file path");
		return ret;
	}

	if (!md5_file(loc->path_.c_str(), md5_buf)) {
		pr_debug("md5_file failed: %s", strerror(errno));
		return ret;
	}

	if (!sha1_file(loc->path_.c_str(), sha1_buf)) {
		pr_debug("sha1_file failed: %s", strerror(errno));
		return ret;
	}

	const char *file_ext = get_filename_ext(loc->path_.c_str());
	std::cout << "File ex: " << file_ext << std::endl;

	try {
		const char query[] =
			"INSERT INTO `gw_files` "	\
			"("				\
				"`tg_file_id`,"		\
				"`tg_uniq_id`,"		\
				"`md5_sum`,"		\
				"`sha1_sum`,"		\
				"`ext`,"		\
				"`size`,"		\
				"`created_at`"		\
			") VALUES (?, ?, ?, ?, ?, ?, ?)";

		char dateBuf[64];
		const char *dateNow = getDateNow(dateBuf, sizeof(dateBuf));
		auto st = db->prepare(query);
		st->execute(
			PARAM_STRING(file.remote_->id_.c_str()),
			PARAM_STRING(file.remote_->unique_id_.c_str()),
			PARAM_BYTES(md5_buf, sizeof(md5_buf)),
			PARAM_BYTES(sha1_buf, sizeof(sha1_buf)),
			PARAM_STRING(file_ext),
			PARAM_UINT((uint64_t)loc->downloaded_size_),
			PARAM_STRING(dateNow),
			PARAM_END
		);
		ret = lastInsertId_static(db);
	} catch (std::string &err) {
		std::cout << err << std::endl;
		throw err;
	}
	return ret;
}


#define MAX_WRK (100u)

static std::mutex wrk_mut;
static std::condition_variable wrk_cond;
static std::atomic<uint32_t> wrk_online = 0;
static bool init = false;


struct db_pool {
	tgvisd::DB *db;
	uint16_t idx;
};

std::mutex pool_mutex;
static std::vector<struct db_pool> wrk_db_pool;
static std::stack<uint16_t> wrk_db_pstack;


static void putWrkDb(struct db_pool *pl)
{
	pool_mutex.lock();
	wrk_db_pstack.push(pl->idx);
	pool_mutex.unlock();
}


static struct db_pool *getWrkDb(void)
{
	struct db_pool *ret = nullptr;
	uint16_t idx;
	pool_mutex.lock();
	if (!wrk_db_pstack.empty()) {
		idx = wrk_db_pstack.top();
		wrk_db_pstack.pop();

		if (!wrk_db_pool[idx].db) {
			wrk_db_pool[idx].db = createDB();
		}
		ret = &wrk_db_pool[idx];
	}
	pool_mutex.unlock();
	return ret;
}



static void initWrkDb(void)
{
	if (!init) {
		init = true;
		wrk_db_pool.reserve(200);
		for (uint16_t i = 200; i--;) {
			struct db_pool pl;
			pl.db = nullptr;
			pl.idx = i;
			wrk_db_pool.push_back(pl);
			putWrkDb(&pl);
		}
	}
}



static void wrk_wait(void)
{
	pr_debug("wrk acquiring mutex...");
	std::unique_lock<std::mutex> lock(wrk_mut);
	while (atomic_load(&wrk_online) >= MAX_WRK) {
		pr_debug("Waiting for workers to finish (count=%u)...",
			 atomic_load(&wrk_online));
		wrk_cond.wait_for(lock, 1000ms, [](){
			return !(atomic_load(&wrk_online) >= MAX_WRK);
		});
	}
	pr_debug("Wrk go! (count=%u)", atomic_load(&wrk_online));
	initWrkDb();
}



void Worker::insertMsgDataPhoto(uint64_t db_msg_id, td_api::message &msg)
{
	wrk_wait();
	std::thread worker([this, db_msg_id, &msg](void){
		atomic_fetch_add(&wrk_online, 1);
		try {
			this->_insertMsgDataPhoto(db_msg_id, msg);
		} catch (std::string &err) {
			std::cout << err << std::endl;
		}
		atomic_fetch_sub(&wrk_online, 1);
	});
#if defined(__linux__)
	{
		char trname[32];
		pthread_t pt = worker.native_handle();
		snprintf(trname, sizeof(trname), "photo-download");
		pthread_setname_np(pt, trname);
	}
#endif
	worker.detach();
}


void Worker::_insertMsgDataPhoto(uint64_t db_msg_id, td_api::message &msg)
{
	uint64_t file_id;
	auto &msgPhoto = static_cast<td_api::messagePhoto &>(*msg.content_);
	const char *text = msgPhoto.caption_->text_.c_str();
	td_api::file *taken_file = nullptr;

	auto &photos = msgPhoto.photo_->sizes_;
	for (auto &photo: photos) {
		auto &pfile = photo->photo_;
		if (!taken_file) {
			taken_file = pfile.get();
		} else {
			if (taken_file->size_ < pfile->size_)
				taken_file = pfile.get();
		}
		std::cout << to_string(pfile) << std::endl;
	}

	if (!taken_file)
		return;

	struct db_pool *dbp = getWrkDb();
	if (!dbp) {
		puts("DB pool is full!");
		abort();
	}
	auto db = dbp->db;
	file_id = resolveFile(db, *taken_file);

	{
		const char query[] =
			"INSERT INTO `gw_group_message_data`"	\
			"("					\
				"`msg_id`,"			\
				"`text`,"			\
				"`text_entities`,"		\
				"`file`,"			\
				"`is_edited`,"			\
				"`tg_date`,"			\
				"`created_at`"			\
			") VALUES (?, ?, NULL, ?, '0', ?, ?);";
		auto st = db_->prepare(query);
		char dateBuf1[64], dateBuf2[64];
		const char *msgDate = getDateByUnixTM(msg.date_, dateBuf1, sizeof(dateBuf1));
		const char *dateNow = getDateNow(dateBuf2, sizeof(dateBuf2));
		st->execute(
			PARAM_UINT(db_msg_id),
			PARAM_STRING(text),
			PARAM_UINT(file_id),
			PARAM_STRING(msgDate),
			PARAM_STRING(dateNow),
			PARAM_END
		);
	}
	putWrkDb(dbp);
}


} /* namespace tgvisd::Modules::Preload */
