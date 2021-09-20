# SPDX-License-Identifier: GPL-2.0-only

cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(tgvisd VERSION 0.2.0 LANGUAGES C CXX)
find_package(Td 1.7.5 REQUIRED)

include_directories(src)

# tdlib wrapper (low level)
set(TGVISD_TDLIB_WRAPPER_SOURCES
  src/tdlib/Td/Callback.hpp
  src/tdlib/Td/Td.hpp
  src/tdlib/Td/Td.cpp
)

# tgvisd core
set(TGVISD_CORE_SOURCES
  src/tgvisd/Core/main.cpp
  src/tgvisd/Core/ModuleTable.hpp
  src/tgvisd/Core/Worker.cpp
  src/tgvisd/Core/Worker.hpp
)

# tgvisd modules
set(TGVISD_MODULES_SOURCES
  
)
