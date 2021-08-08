#!/bin/sh
export TGVISD_DB_HOST=127.0.0.1
export TGVISD_DB_PORT=33060
export TGVISD_DB_USER=username
export TGVISD_DB_PASS=password
export TGVISD_DB_NAME=database
export TGVISD_API_ID=123123
export TGVISD_API_HASH=aaaaaa
export TGVISD_DATA_PATH=accounts/6281123123123
build/tgvisd
