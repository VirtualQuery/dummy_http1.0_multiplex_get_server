cmake_minimum_required(VERSION 2.8)
project(HTTP1_server)

set(SOURCE_FILES src/main.cc)

add_compile_options(-std=c++11 -g -Wall -Wextra -pedantic -Werror)
add_executable(final ${SOURCE_FILES})

find_package(Threads REQUIRED)
if(THREADS_HAVE_PTHREAD_ARG)
  target_compile_options(final PUBLIC "-pthread")
endif()
if(CMAKE_THREAD_LIBS_INIT)
  target_link_libraries(final "${CMAKE_THREAD_LIBS_INIT}")
endif()
