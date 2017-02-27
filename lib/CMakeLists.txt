cmake_minimum_required(VERSION 3.7)

project(libtweeteria)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Casablanca REQUIRED)
find_package(RapidJSON REQUIRED)

set(TWEETERIA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(TWEETERIA_HEADERS
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/tweeteria.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/user.hpp
)

set(TWEETERIA_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(TWEETERIA_SOURCES
    ${TWEETERIA_SOURCE_DIR}/tweeteria.cpp
)

add_library(tweeteria ${TWEETERIA_HEADERS} ${TWEETERIA_SOURCES})
target_include_directories(tweeteria PUBLIC
    ${TWEETERIA_INCLUDE_DIR}
    ${CASABLANCA_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
)
target_link_libraries(tweeteria PUBLIC
    ${CASABLANCA_LIBRARIES}
)
