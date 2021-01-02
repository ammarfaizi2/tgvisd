
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(TdExample VERSION 1.0 LANGUAGES CXX)

find_package(Td 1.7.0 REQUIRED)


set(TEABOT_SOURCE
  src/main.cpp
  src/TeaBot.cpp
)

add_executable(teabot ${TEABOT_SOURCE})
target_link_libraries(teabot PRIVATE Td::TdStatic)
set_property(TARGET teabot PROPERTY CXX_STANDARD 14)
