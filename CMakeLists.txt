cmake_minimum_required(VERSION 2.8)
project(HTTP1_server)
set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES src/main.cc)

add_compile_options(-g -Wall -Wextra -pedantic -Werror)
add_executable(server ${SOURCE_FILES})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(server PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(server "${CMAKE_THREAD_LIBS_INIT}")
endif()
