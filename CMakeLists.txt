cmake_minimum_required(VERSION 2.8)
project(suicideTg)

set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall")
set(Boost_USE_MULTITHREADED ON)

###
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)

include_directories(/usr/local/include ${OPENSSL_INCLUDE_DIR} ${Boost_INCLUDE_DIR})

if (CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIRS})
  add_definitions(-DHAVE_CURL)
endif()

set(LOCAL_SRC src/main.cpp)
set(TGBOT_LIBRARIES /usr/local/lib/libTgBot.a)

add_executable(suicideTg ${LOCAL_SRC})

target_link_libraries(suicideTg ${TGBOT_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} ${OPENSSL_LIBRARIES} ${Boost_LIBRARIES} ${CURL_LIBRARIES})
