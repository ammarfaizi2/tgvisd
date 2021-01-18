
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(TeaBot VERSION 8.3 LANGUAGES CXX)

find_package(Td 1.7.0 REQUIRED)

include_directories(src)

set(TEABOT_SOURCE
  src/TeaBot/main.cpp
  src/TeaBot/TeaBot.cpp
  src/TeaBot/TeaBot.hpp
  src/TeaBot/helpers.cpp
  src/TeaBot/helpers.hpp
  src/TeaBot/Response.cpp
  src/TeaBot/Response.hpp
  src/TeaBot/Responses/MyMessage.cpp
  src/TeaBot/Responses/MyMessage.hpp
)

set(TDTEABOT_SOURCE
  src/TdTeaBot/TdHandler.cpp
  src/TdTeaBot/TdHandler.hpp
  src/TdTeaBot/Callback.hpp
  src/TdTeaBot/detail.hpp
)

add_library(tdteabot SHARED ${TDTEABOT_SOURCE})
set_property(TARGET tdteabot PROPERTY CXX_STANDARD 14)
target_link_libraries(tdteabot PRIVATE Td::TdStatic)
target_include_directories(tdteabot PRIVATE td/include)
target_compile_options(tdteabot PRIVATE -Wall -Wextra -Wno-unused-parameter)

add_executable(teabot ${TEABOT_SOURCE})
set_property(TARGET teabot PROPERTY CXX_STANDARD 14)
target_link_libraries(teabot tdteabot pthread curl)
target_include_directories(teabot PRIVATE td/include)
target_compile_options(teabot PRIVATE -Wall -Wextra -Wno-unused-parameter)