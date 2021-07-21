
CREATE VIEW v_msg_group_simple AS SELECT
  a.tg_user_id,
  b.tg_msg_id,
  CONCAT(
    a.first_name,
    IF (a.last_name != '' AND a.last_name IS NOT NULL,
      CONCAT(' ', a.last_name),
      ''
    )
  ) as name,
  SUBSTR(c.`text`, 1, 30) AS text_in_msg,
  b.msg_type,
  c.tg_date
FROM gw_users AS a 
INNER JOIN gw_group_messages AS b ON a.id = b.user_id
INNER JOIN gw_group_message_data AS c ON b.id = c.msg_id
INNER JOIN gw_groups AS d ON d.id = b.group_id
ORDER BY c.tg_date;

