
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(TdExample VERSION 1.0 LANGUAGES CXX)

find_package(Td 1.7.0 REQUIRED)


set(TEABOT_SOURCE
  src/main.cpp
  src/TeaBot/TeaBot.cpp
)


add_library(bbteabot SHARED src/TeaBot/TdLibHandler.cpp)
target_link_libraries(bbteabot PRIVATE Td::TdStatic)
target_include_directories(bbteabot PRIVATE td/include)

add_executable(teabot ${TEABOT_SOURCE})
target_link_libraries(teabot bbteabot pthread)
set_property(TARGET teabot PROPERTY CXX_STANDARD 14)
target_include_directories(teabot PRIVATE td/include)
