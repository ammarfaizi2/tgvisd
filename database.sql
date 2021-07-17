-- Adminer 4.7.6 MySQL dump

SET NAMES utf8;
SET time_zone = '+00:00';
SET foreign_key_checks = 0;
SET sql_mode = 'NO_AUTO_VALUE_ON_ZERO';

SET NAMES utf8mb4;

DROP TABLE IF EXISTS `tg_files`;
CREATE TABLE `tg_files` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `tg_file_id` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `tg_uniq_id` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `md5_sum` binary(16) NOT NULL,
  `sha1_sum` binary(20) NOT NULL,
  `file_type` varchar(64) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT 'unknown',
  `ext` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `size` bigint unsigned DEFAULT NULL,
  `hit_count` bigint unsigned NOT NULL DEFAULT '1',
  `description` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci,
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `tg_file_id` (`tg_file_id`),
  KEY `md5_sum` (`md5_sum`),
  KEY `sha1_sum` (`sha1_sum`),
  KEY `file_type` (`file_type`),
  KEY `ext` (`ext`),
  KEY `size` (`size`),
  KEY `hit_count` (`hit_count`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`),
  FULLTEXT KEY `description` (`description`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `tg_group_admins`;
CREATE TABLE `tg_group_admins` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `user_id` bigint unsigned NOT NULL,
  `group_id` bigint unsigned NOT NULL,
  `role` enum('creator','administrator','member','restricted','left','kicked') NOT NULL DEFAULT 'administrator',
  `info` json DEFAULT NULL,
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `role` (`role`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`),
  KEY `user_id` (`user_id`),
  KEY `group_id` (`group_id`),
  CONSTRAINT `tg_group_admins_ibfk_3` FOREIGN KEY (`user_id`) REFERENCES `tg_users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_group_admins_ibfk_4` FOREIGN KEY (`group_id`) REFERENCES `tg_groups` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;


DROP TABLE IF EXISTS `tg_group_history`;
CREATE TABLE `tg_group_history` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `group_id` bigint unsigned NOT NULL,
  `username` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL,
  `link` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `photo` bigint unsigned DEFAULT NULL,
  `created_at` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `name` (`name`),
  KEY `username` (`username`),
  KEY `link` (`link`),
  KEY `created_at` (`created_at`),
  KEY `group_id` (`group_id`),
  KEY `photo` (`photo`),
  CONSTRAINT `tg_group_history_ibfk_6` FOREIGN KEY (`group_id`) REFERENCES `tg_groups` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_group_history_ibfk_7` FOREIGN KEY (`photo`) REFERENCES `tg_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `tg_group_message_data`;
CREATE TABLE `tg_group_message_data` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `msg_id` bigint unsigned NOT NULL,
  `text` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci,
  `text_entities` json DEFAULT NULL,
  `file` bigint unsigned DEFAULT NULL,
  `is_edited` enum('0','1') CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '0',
  `tg_date` datetime DEFAULT NULL,
  `created_at` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `msg_id` (`msg_id`),
  KEY `file` (`file`),
  KEY `is_edited` (`is_edited`),
  KEY `created_at` (`created_at`),
  KEY `tg_date` (`tg_date`),
  FULLTEXT KEY `text` (`text`),
  CONSTRAINT `tg_group_message_data_ibfk_3` FOREIGN KEY (`msg_id`) REFERENCES `tg_group_messages` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_group_message_data_ibfk_4` FOREIGN KEY (`file`) REFERENCES `tg_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `tg_group_message_fwd`;
CREATE TABLE `tg_group_message_fwd` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `user_id` bigint unsigned NOT NULL,
  `msg_id` bigint unsigned NOT NULL,
  `tg_forwarded_date` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `tg_forwarded_date` (`tg_forwarded_date`),
  KEY `user_id` (`user_id`),
  KEY `msg_id` (`msg_id`),
  CONSTRAINT `tg_group_message_fwd_ibfk_3` FOREIGN KEY (`user_id`) REFERENCES `tg_users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_group_message_fwd_ibfk_4` FOREIGN KEY (`msg_id`) REFERENCES `tg_group_messages` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;


DROP TABLE IF EXISTS `tg_group_messages`;
CREATE TABLE `tg_group_messages` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `group_id` bigint unsigned DEFAULT NULL,
  `user_id` bigint unsigned DEFAULT NULL,
  `tg_msg_id` bigint unsigned DEFAULT NULL,
  `reply_to_tg_msg_id` bigint unsigned DEFAULT NULL,
  `msg_type` varchar(255) DEFAULT NULL,
  `has_edited_msg` enum('0','1') NOT NULL DEFAULT '0',
  `is_forwarded_msg` enum('0','1') NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `tg_msg_id` (`tg_msg_id`),
  KEY `reply_to_tg_msg_id` (`reply_to_tg_msg_id`),
  KEY `msg_type` (`msg_type`),
  KEY `is_edited` (`has_edited_msg`),
  KEY `created_at` (`created_at`),
  KEY `group_id` (`group_id`),
  KEY `user_id` (`user_id`),
  KEY `updated_at` (`updated_at`),
  CONSTRAINT `tg_group_messages_ibfk_1` FOREIGN KEY (`group_id`) REFERENCES `tg_groups` (`id`) ON DELETE SET NULL ON UPDATE CASCADE,
  CONSTRAINT `tg_group_messages_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `tg_users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;


DROP TABLE IF EXISTS `tg_groups`;
CREATE TABLE `tg_groups` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `tg_group_id` bigint NOT NULL,
  `username` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL,
  `link` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `photo` bigint unsigned DEFAULT NULL,
  `msg_count` bigint unsigned NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `tg_group_id` (`tg_group_id`),
  KEY `name` (`name`),
  KEY `username` (`username`),
  KEY `link` (`link`),
  KEY `msg_count` (`msg_count`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`),
  KEY `photo` (`photo`),
  CONSTRAINT `tg_groups_ibfk_1` FOREIGN KEY (`photo`) REFERENCES `tg_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `tg_private_message_data`;
CREATE TABLE `tg_private_message_data` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `msg_id` bigint unsigned NOT NULL,
  `text` text CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci,
  `text_entities` json DEFAULT NULL,
  `file` bigint unsigned DEFAULT NULL,
  `is_edited` enum('0','1') CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL DEFAULT '0',
  `tg_date` datetime DEFAULT NULL,
  `created_at` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `msg_id` (`msg_id`),
  KEY `file` (`file`),
  KEY `is_edited` (`is_edited`),
  KEY `created_at` (`created_at`),
  KEY `tg_date` (`tg_date`),
  FULLTEXT KEY `text` (`text`),
  CONSTRAINT `tg_private_message_data_ibfk_3` FOREIGN KEY (`msg_id`) REFERENCES `tg_private_messages` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_private_message_data_ibfk_4` FOREIGN KEY (`file`) REFERENCES `tg_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `tg_private_message_fwd`;
CREATE TABLE `tg_private_message_fwd` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `user_id` bigint unsigned NOT NULL,
  `msg_id` bigint unsigned NOT NULL,
  `tg_forwarded_date` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `msg_id` (`msg_id`),
  KEY `tg_forwarded_date` (`tg_forwarded_date`),
  CONSTRAINT `tg_private_message_fwd_ibfk_3` FOREIGN KEY (`user_id`) REFERENCES `tg_users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_private_message_fwd_ibfk_4` FOREIGN KEY (`msg_id`) REFERENCES `tg_private_messages` (`id`) ON DELETE CASCADE ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;


DROP TABLE IF EXISTS `tg_private_messages`;
CREATE TABLE `tg_private_messages` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `user_id` bigint unsigned DEFAULT NULL,
  `tg_msg_id` bigint unsigned DEFAULT NULL,
  `reply_to_tg_msg_id` bigint unsigned DEFAULT NULL,
  `msg_type` varchar(255) NOT NULL,
  `has_edited_msg` enum('0','1') NOT NULL DEFAULT '0',
  `is_forwarded_msg` enum('0','1') NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL ON UPDATE CURRENT_TIMESTAMP,
  `updated_at` datetime DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `tg_msg_id` (`tg_msg_id`),
  KEY `reply_to_tg_msg_id` (`reply_to_tg_msg_id`),
  KEY `msg_type` (`msg_type`),
  KEY `has_edited_msg` (`has_edited_msg`),
  KEY `created_at` (`created_at`),
  CONSTRAINT `tg_private_messages_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `tg_users` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb3;


DROP TABLE IF EXISTS `tg_user_history`;
CREATE TABLE `tg_user_history` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `user_id` bigint unsigned NOT NULL,
  `username` varchar(72) CHARACTER SET latin1 COLLATE latin1_swedish_ci DEFAULT NULL,
  `first_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL,
  `last_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci DEFAULT NULL,
  `photo` bigint unsigned DEFAULT NULL,
  `created_at` datetime NOT NULL,
  PRIMARY KEY (`id`),
  KEY `user_id` (`user_id`),
  KEY `photo` (`photo`),
  KEY `username` (`username`),
  KEY `first_name` (`first_name`),
  KEY `last_name` (`last_name`),
  KEY `created_at` (`created_at`),
  CONSTRAINT `tg_user_history_ibfk_2` FOREIGN KEY (`user_id`) REFERENCES `tg_users` (`id`) ON DELETE CASCADE ON UPDATE CASCADE,
  CONSTRAINT `tg_user_history_ibfk_3` FOREIGN KEY (`photo`) REFERENCES `tg_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


DROP TABLE IF EXISTS `tg_users`;
CREATE TABLE `tg_users` (
  `id` bigint unsigned NOT NULL AUTO_INCREMENT,
  `tg_user_id` bigint unsigned NOT NULL,
  `username` varchar(255) CHARACTER SET utf8 COLLATE utf8_general_ci DEFAULT NULL,
  `first_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci NOT NULL,
  `last_name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_520_ci DEFAULT NULL,
  `photo` bigint unsigned DEFAULT NULL,
  `group_msg_count` bigint unsigned NOT NULL DEFAULT '0',
  `private_msg_count` bigint unsigned NOT NULL DEFAULT '0',
  `is_bot` enum('0','1') CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL DEFAULT '0',
  `created_at` datetime NOT NULL,
  `updated_at` datetime DEFAULT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `tg_user_id` (`tg_user_id`),
  KEY `username` (`username`),
  KEY `first_name` (`first_name`),
  KEY `last_name` (`last_name`),
  KEY `group_msg_count` (`group_msg_count`),
  KEY `private_msg_count` (`private_msg_count`),
  KEY `created_at` (`created_at`),
  KEY `updated_at` (`updated_at`),
  KEY `photo` (`photo`),
  CONSTRAINT `tg_users_ibfk_1` FOREIGN KEY (`photo`) REFERENCES `tg_files` (`id`) ON DELETE SET NULL ON UPDATE CASCADE
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_520_ci;


-- 2021-07-17 06:12:32
