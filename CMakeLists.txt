
cmake_minimum_required(VERSION 3.1 FATAL_ERROR)

project(TeaBot VERSION 8.3 LANGUAGES CXX)

find_package(Td 1.7.0 REQUIRED)

include_directories(src)

set(TDTEABOT_SOURCE
  src/TdTeaBot/TdHandler.cpp
  src/TdTeaBot/TdHandler.hpp
  src/TdTeaBot/Callback.hpp
  src/TdTeaBot/detail.hpp
)

set(TEABOT_MOD_SHELLEXEC
  src/TeaBot/Modules/ShellExec/Module.hpp
  src/TeaBot/Modules/ShellExec/Module.cpp
)

set(TEABOT_MOD_SED
  src/TeaBot/Modules/Sed/Module.hpp
  src/TeaBot/Modules/Sed/Module.cpp
)

set(TEABOT_SOURCE
  src/TeaBot/Responses/Message.cpp
  src/TeaBot/Responses/Message.hpp
  src/TeaBot/Responses/MyMessage.cpp
  src/TeaBot/Responses/MyMessage.hpp

  src/TeaBot/helpers.cpp
  src/TeaBot/helpers.hpp
  src/TeaBot/main.cpp
  src/TeaBot/Response.cpp
  src/TeaBot/Response.hpp
  src/TeaBot/TeaBot.cpp
  src/TeaBot/TeaBot.hpp
  src/TeaBot/TeaBotModule.hpp

  ${TEABOT_MOD_SHELLEXEC}
  ${TEABOT_MOD_SED}
)

set(TEABOT_TEST
  src/TeaBot/helpers.cpp
  src/TeaBot/helpers.hpp

  src/tests/helpers.cpp
)

add_library(tdteabot SHARED ${TDTEABOT_SOURCE})
set_property(TARGET tdteabot PROPERTY CXX_STANDARD 20)
target_link_libraries(tdteabot PRIVATE Td::TdStatic)
target_include_directories(tdteabot PRIVATE td/include)
target_compile_options(tdteabot PRIVATE
  -pedantic
  -Wall
  -Wextra
  -Wno-unused-parameter
  -fstack-protector-strong
)

add_executable(teabot ${TEABOT_SOURCE})
set_property(TARGET teabot PROPERTY CXX_STANDARD 20)
target_link_libraries(teabot tdteabot pthread curl)
target_include_directories(teabot PRIVATE td/include)
target_compile_options(teabot PRIVATE
  -pedantic
  -Wall
  -Wextra
  -Wno-unused-parameter
  -fstack-protector-strong
)

add_executable(teabot_test ${TEABOT_TEST})
set_property(TARGET teabot_test PROPERTY CXX_STANDARD 20)
target_link_libraries(teabot_test)
target_include_directories(teabot_test PRIVATE td/include)
target_compile_options(teabot_test PRIVATE
  -pedantic
  -Wall
  -Wextra
  -Wno-unused-parameter
  -fstack-protector-strong
)
