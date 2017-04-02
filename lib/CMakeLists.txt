cmake_minimum_required(VERSION 3.7)

project(libtweeteria)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Casablanca REQUIRED)
find_package(RapidJSON REQUIRED)

set(TWEETERIA_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(TWEETERIA_HEADERS
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/cursor.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/entities.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/error.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/exceptions.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/forward_decl.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/id_types.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/image_util.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/proxy_config.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/string_util.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/tweet.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/tweeteria.hpp
    ${TWEETERIA_INCLUDE_DIR}/tweeteria/user.hpp
)

set(TWEETERIA_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
set(TWEETERIA_SOURCES
    ${TWEETERIA_SOURCE_DIR}/cursor.cpp
    ${TWEETERIA_SOURCE_DIR}/entities.cpp
    ${TWEETERIA_SOURCE_DIR}/error.cpp
    ${TWEETERIA_SOURCE_DIR}/image_util.cpp
    ${TWEETERIA_SOURCE_DIR}/proxy_config.cpp
    ${TWEETERIA_SOURCE_DIR}/string_util.cpp
    ${TWEETERIA_SOURCE_DIR}/tweet.cpp
    ${TWEETERIA_SOURCE_DIR}/tweeteria.cpp
    ${TWEETERIA_SOURCE_DIR}/user.cpp
)

set(TWEETERIA_TEST_DIR ${PROJECT_SOURCE_DIR}/test)
set(TWEETERIA_TEST_SOURCES
    ${TWEETERIA_TEST_DIR}/tweeteria.t.cpp
    ${TWEETERIA_TEST_DIR}/entities.t.cpp
    ${TWEETERIA_TEST_DIR}/image_util.t.cpp
    ${TWEETERIA_TEST_DIR}/string_util.t.cpp
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
target_compile_definitions(tweeteria PUBLIC RAPIDJSON_HAS_STDSTRING=1)
target_compile_options(tweeteria PRIVATE $<$<STREQUAL:$<CXX_COMPILER_ID>,MSVC>:/W4>)

option(TWEETERIA_BUILD_TESTS "If enabled, builds unit tests along with the library." ON)
if(TWEETERIA_BUILD_TESTS)
    find_package(Catch REQUIRED)
    enable_testing()

    add_executable(test_tweeteria ${TWEETERIA_TEST_SOURCES})
    target_link_libraries(test_tweeteria PUBLIC tweeteria)
    target_include_directories(test_tweeteria PUBLIC ${CATCH_INCLUDE_DIR})
    
    add_test(NAME Test.Tweeteria COMMAND test_tweeteria)
endif()