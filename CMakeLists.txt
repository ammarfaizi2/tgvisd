
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(TeaBot VERSION 8.3 LANGUAGES CXX)

find_package(Td 1.7.5 REQUIRED)

include_directories(src)
include_directories(src/ext)


# TD core (low level)
set(TGVISD_TD_SOURCE
	src/tgvisd/Td/Callback.hpp
	src/tgvisd/Td/Td.hpp
	src/tgvisd/Td/Td.cpp
)


# Modules
set(TGVISD_MODULES_SOURCE
  src/tgvisd/Modules/BaseEnumerate.cpp
  src/tgvisd/Modules/BaseEnumerate.hpp
  src/tgvisd/Modules/BasePreload.cpp
  src/tgvisd/Modules/BasePreload.hpp
  src/tgvisd/Modules/Enumerate/Shell.cpp
  src/tgvisd/Modules/Enumerate/Shell.hpp
  src/tgvisd/Modules/Preload/HistoryScraper.cpp
  src/tgvisd/Modules/Preload/HistoryScraper.hpp
)


# Main
set(TGVISD_MAIN_SOURCE
  src/tgvisd/common.hpp
  src/tgvisd/DB.cpp
  src/tgvisd/DB.hpp
  src/tgvisd/DBCommon.hpp
  src/tgvisd/DBt/PreparedStatement.cpp
  src/tgvisd/DBt/PreparedStatement.hpp
  src/tgvisd/main.cpp
  src/tgvisd/Main/Main.cpp
  src/tgvisd/Main/Main.hpp
  src/tgvisd/Main/Module.cpp
  src/tgvisd/Main/Module.hpp
  src/tgvisd/Main/Worker.cpp
  src/tgvisd/Main/Worker.hpp
  ${TGVISD_MODULES_SOURCE}
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

link_directories(${CMAKE_SOURCE_DIR}/src/mysql_cpp/build)
include_directories(${CMAKE_SOURCE_DIR}/src/mysql_cpp/include)

## Build the main binary (static)
add_executable(tgvisd ${TGVISD_MAIN_SOURCE})

## Build the main binary (shared)
# add_library(tgvisd SHARED ${TGVISD_MAIN_SOURCE})

set_property(TARGET tgvisd PROPERTY CXX_STANDARD 20)
target_link_libraries(tgvisd tgvisd-td pthread curl mysqlcppconn8)
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
