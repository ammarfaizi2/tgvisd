
# SPDX-License-Identifier: GPL-2.0

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(TeaBot VERSION 8.3 LANGUAGES C CXX)
find_package(Td 1.7.5 REQUIRED)

include_directories(src)
include_directories(src/ext)

# TD core (low level)
set(TGVISD_TD_SOURCE
  src/tgvisd/Td/Callback.hpp
  src/tgvisd/Td/Td.hpp
  src/tgvisd/Td/Td.cpp
)

# tgvisd core
set(TGVISD_CORE_SOURCE
  src/tgvisd/common.hpp
  src/tgvisd/entry.cpp
  src/tgvisd/Main.cpp
  src/tgvisd/Main.hpp
  src/tgvisd/print.c
  src/tgvisd/print.h
  src/tgvisd/Scraper.cpp
  src/tgvisd/Scraper.hpp
)

# Build the shared library that contains tdlib core.
add_library(tgvisd-td SHARED ${TGVISD_TD_SOURCE})
set_property(TARGET tgvisd-td PROPERTY CXX_STANDARD 20)
target_link_libraries(tgvisd-td PRIVATE Td::TdStatic)
target_include_directories(tgvisd-td PRIVATE td/include)
target_compile_options(tgvisd-td PRIVATE
  -ggdb3
  -Wall
  -Wextra
  -Wpedantic
  -Wno-unused-parameter
  -fno-omit-frame-pointer
  -fstack-protector-strong
)


## Build the main binary (static)
add_executable(tgvisd ${TGVISD_CORE_SOURCE})
set_property(TARGET tgvisd PROPERTY CXX_STANDARD 20)
set_property(TARGET tgvisd PROPERTY C_STANDARD 11)
target_link_libraries(tgvisd tgvisd-td pthread curl)
target_include_directories(tgvisd PRIVATE td/include)
target_compile_options(tgvisd PRIVATE
  -ggdb3
  -Wall
  -Wextra
  -Wpedantic
  -Wno-unused-parameter
  -fno-omit-frame-pointer
  -fstack-protector-strong
)
