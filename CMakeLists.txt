
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(TeaBot VERSION 8.3 LANGUAGES C CXX)

include_directories(src)
include_directories(src/ext)

set(TGVISD_MAIN_SOURCE
	src/tgvisd/main.cpp
	src/tgvisd/print.c
	src/tgvisd/print.h
)

## Build the main binary (static)
add_executable(tgvisd ${TGVISD_MAIN_SOURCE})
set_property(TARGET tgvisd PROPERTY C_STANDARD 11)
set_property(TARGET tgvisd PROPERTY CXX_STANDARD 20)
target_link_libraries(tgvisd pthread)
target_compile_options(tgvisd PRIVATE
	-ggdb3
	-Wall
	-Wextra
	-Wpedantic
	-Wno-unused-parameter
	-fno-omit-frame-pointer
	-fstack-protector-strong
)
