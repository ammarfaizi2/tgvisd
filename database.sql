-- Adminer 4.7.6 MySQL dump

SET NAMES utf8;
SET time_zone = '+00:00';
SET foreign_key_checks = 0;
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

SET NAMES utf8mb4;

DROP TABLE IF EXISTS `gw_files`;
CREATE TABLE `gw_files` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `tg_file_id` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `tg_uniq_id` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
  `md5_sum` binary(16) NOT NULL,
  `sha1_sum` binary(20) NOT NULL,
  `file_type` varchar(64) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL DEFAULT 'unknown',
  `ext` varchar(32) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
  `size` bigint unsigned NOT NULL DEFAULT '0',
  `hit_count` bigint unsigned NOT NULL DEFAULT '1',
  `description` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci DEFAULT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `tg_file_id` (`tg_file_id`),
  KEY `tg_uniq_id` (`tg_uniq_id`),
  KEY `md5_sum` (`md5_sum`),
  KEY `sha1_sum` (`sha1_sum`),
  KEY `file_type` (`file_type`),
  KEY `ext` (`ext`),
  KEY `size` (`size`),
  KEY `hit_count` (`hit_count`),
  KEY `description` (`description`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `gw_group_message_data`;
CREATE TABLE `gw_group_message_data` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `msg_id` bigint unsigned NOT NULL,
  `text` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci,
  `text_entities` text CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci,
  `file` bigint unsigned DEFAULT NULL,
  `is_edited` enum('0','1') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '0',
  `tg_date` datetime DEFAULT NULL,
  `created_at` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `msg_id` (`msg_id`),
  KEY `is_edited` (`is_edited`),
  KEY `tg_date` (`tg_date`),
  KEY `created_at` (`created_at`),
  KEY `file` (`file`),
  FULLTEXT KEY `text` (`text`),
  CONSTRAINT `gw_group_message_data_ibfk_1` FOREIGN KEY (`msg_id`) REFERENCES `gw_group_messages` (`id`) ON DELETE RESTRICT,
  CONSTRAINT `gw_group_message_data_ibfk_2` FOREIGN KEY (`file`) REFERENCES `gw_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `gw_group_message_forward`;
CREATE TABLE `gw_group_message_forward` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `msg_id` bigint unsigned NOT NULL,
  `origin_type` enum('channel','chat','hidden_user','user') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `orig_date` date DEFAULT NULL,
  `pas_at` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci COMMENT 'public_service_announcement_type_ The type of a public service announcement for the forwarded message. https://core.telegram.org/tdlib/docs/classtd_1_1td__api_1_1message_forward_info.html#aca7885f90a22bfba54b42c4a1ffc7b47',
  `orig_id` bigint unsigned DEFAULT NULL,
  `origin_text` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci DEFAULT NULL,
  `from_chat_id` bigint unsigned NOT NULL,
  `from_msg_id` bigint unsigned NOT NULL,
  PRIMARY KEY (`id`),
  KEY `orig_date` (`orig_date`),
  KEY `from_chat_id` (`from_chat_id`),
  KEY `from_msg_id` (`from_msg_id`),
  KEY `orig_user_id` (`orig_id`),
  KEY `origin_text` (`origin_text`),
  KEY `msg_id` (`msg_id`),
  CONSTRAINT `gw_group_message_forward_ibfk_1` FOREIGN KEY (`msg_id`) REFERENCES `gw_group_messages` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `gw_group_messages`;
CREATE TABLE `gw_group_messages` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `group_id` bigint unsigned NOT NULL,
  `user_id` bigint unsigned NOT NULL,
  `tg_msg_id` bigint unsigned NOT NULL,
  `reply_to_tg_msg_id` bigint unsigned DEFAULT NULL,
  `msg_type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `has_edited_msg` enum('0','1') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `is_forwarded_msg` enum('0','1') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `group_id` (`group_id`),
  KEY `user_id` (`user_id`),
  KEY `tg_msg_id` (`tg_msg_id`),
  KEY `reply_to_tg_msg_id` (`reply_to_tg_msg_id`),
  KEY `msg_type` (`msg_type`),
  KEY `has_edited_msg` (`has_edited_msg`),
  KEY `is_forwarded_msg` (`is_forwarded_msg`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`),
  CONSTRAINT `gw_group_messages_ibfk_1` FOREIGN KEY (`group_id`) REFERENCES `gw_groups` (`id`) ON DELETE RESTRICT,
  CONSTRAINT `gw_group_messages_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `gw_users` (`id`) ON DELETE RESTRICT
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `gw_groups`;
CREATE TABLE `gw_groups` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `tg_group_id` bigint NOT NULL,
  `username` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL,
  `link` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `tg_group_id` (`tg_group_id`),
  KEY `username` (`username`),
  KEY `name` (`name`),
  KEY `link` (`link`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `gw_users`;
CREATE TABLE `gw_users` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `tg_user_id` bigint unsigned NOT NULL,
  `username` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci DEFAULT NULL,
  `first_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL,
  `last_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci DEFAULT NULL,
  `is_bot` enum('0','1') CHARACTER SET utf8mb4 COLLATE utf8mb4_general_ci NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `tg_user_id` (`tg_user_id`),
  KEY `username` (`username`),
  KEY `first_name` (`first_name`),
  KEY `last_name` (`last_name`),
  KEY `is_bot` (`is_bot`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `track_event_id`;
CREATE TABLE `track_event_id` (
  `id` bigint unsigned NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


-- 2021-07-21 22:31:14
