cmake_minimum_required(VERSION 3.7)

project(tweeteria)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(Casablanca REQUIRED)
find_package(Boost REQUIRED)

set(TWEETERIA_SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

add_executable(tweeteria ${TWEETERIA_SOURCES})
target_include_directories(tweeteria PUBLIC
    ${CASABLANCA_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIR}
)
target_link_libraries(tweeteria PUBLIC
    ${CASABLANCA_LIBRARIES}
)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tweeteria)

file(COPY ${CASABLANCA_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
