cmake_minimum_required(VERSION 3.7)

project(tweeteria)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(lib)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Qt5BaseDir)
if(QT5_BASE_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${QT5_BASE_DIR})
endif()
find_package(Sqlpp11BaseDir)
if(SQLPP11_BASE_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${SQLPP11_BASE_DIR})
endif()
find_package(Qt5Widgets 5.8 REQUIRED)
find_package(Qt5Svg 5.8 REQUIRED)
find_package(Boost REQUIRED)
find_package(gbBase REQUIRED)
find_package(sqlpp11 REQUIRED)
find_package(sqlpp11-connector-sqlite3)

set(TWEETERIA_SOURCES
    ${PROJECT_SOURCE_DIR}/src/main.cpp
)

add_executable(tweeteria_test ${TWEETERIA_SOURCES})
target_include_directories(tweeteria_test PUBLIC
    ${Boost_INCLUDE_DIR}
)
target_link_libraries(tweeteria_test PUBLIC
    tweeteria
)

set(CLIENT_SOURCE_DIR ${PROJECT_SOURCE_DIR}/client)

set(CLIENT_SOURCE_FILES
    ${CLIENT_SOURCE_DIR}/image_provider.cpp
    ${CLIENT_SOURCE_DIR}/version.cpp
    ${CLIENT_SOURCE_DIR}/web_resource_provider.cpp
    ${CLIENT_SOURCE_DIR}/db/client_database.cpp
)

set(CLIENT_DATABASE_TABLE_HEADERS
    ${CLIENT_SOURCE_DIR}/db/tables/friends.hpp
    ${CLIENT_SOURCE_DIR}/db/tables/sqlite_master.hpp
    ${CLIENT_SOURCE_DIR}/db/tables/tweeteria_client_properties.hpp
)
source_group("Table Headers" FILES ${CLIENT_DATABASE_TABLE_HEADERS})

set(CLIENT_HEADER_FILES
    ${CLIENT_SOURCE_DIR}/exceptions.hpp
    ${CLIENT_SOURCE_DIR}/image_provider.hpp
    ${CLIENT_SOURCE_DIR}/metatype_declarations.hpp
    ${CLIENT_SOURCE_DIR}/qt_begin_disable_warnings.hpp
    ${CLIENT_SOURCE_DIR}/qt_end_disable_warnings.hpp
    ${CLIENT_SOURCE_DIR}/user_sort_order.hpp
    ${CLIENT_SOURCE_DIR}/version.hpp
    ${CLIENT_SOURCE_DIR}/web_resource_provider.hpp
    ${CLIENT_SOURCE_DIR}/db/client_database.hpp
    ${CLIENT_SOURCE_DIR}/db/table_layouts.hpp
    ${CLIENT_DATABASE_TABLE_HEADERS}
)

set(CLIENT_UI_DIR ${CLIENT_SOURCE_DIR}/ui)
set(CLIENT_QT_SOURCE_FILES
    ${CLIENT_UI_DIR}/main_window.cpp
    ${CLIENT_UI_DIR}/central_widget.cpp
    ${CLIENT_UI_DIR}/data_model.cpp
    ${CLIENT_UI_DIR}/opening_dialog.cpp
    ${CLIENT_UI_DIR}/proxy_config_dialog.cpp
    ${CLIENT_UI_DIR}/svg_icon.cpp
    ${CLIENT_UI_DIR}/tweet_widget.cpp
    ${CLIENT_UI_DIR}/tweets_list.cpp
    ${CLIENT_UI_DIR}/user_widget.cpp
    ${CLIENT_UI_DIR}/users_list.cpp
)
set(CLIENT_QT_MOC_HEADER_FILES
    ${CLIENT_UI_DIR}/main_window.hpp
    ${CLIENT_UI_DIR}/central_widget.hpp
    ${CLIENT_UI_DIR}/data_model.hpp
    ${CLIENT_UI_DIR}/opening_dialog.hpp
    ${CLIENT_UI_DIR}/proxy_config_dialog.hpp
    ${CLIENT_UI_DIR}/svg_icon.hpp
    ${CLIENT_UI_DIR}/tweet_widget.hpp
    ${CLIENT_UI_DIR}/tweets_list.hpp
    ${CLIENT_UI_DIR}/user_widget.hpp
    ${CLIENT_UI_DIR}/users_list.hpp
)
qt5_wrap_cpp(CLIENT_QT_MOC_SOURCE_FILES ${CLIENT_QT_MOC_HEADER_FILES})
source_group(moc FILES
    ${CLIENT_QT_MOC_SOURCE_FILES}
)

qt5_add_resources(CLIENT_QT_RESOURCE_FILES ${CLIENT_SOURCE_DIR}/resources/tweeteria_client.qrc)
source_group(resources FILES
    ${CLIENT_QT_RESOURCE_FILES}
)

add_executable(tweeteria_client WIN32
    ${CLIENT_SOURCE_DIR}/main.cpp
    ${CLIENT_SOURCE_FILES}
    ${CLIENT_HEADER_FILES}
    ${CLIENT_QT_SOURCE_FILES}
    ${CLIENT_QT_SOURCE_FILES}
    ${CLIENT_QT_MOC_HEADER_FILES}
    ${CLIENT_QT_MOC_SOURCE_FILES}
    ${CLIENT_QT_RESOURCE_FILES}
)
target_include_directories(tweeteria_client PUBLIC ${CLIENT_SOURCE_DIR})
target_link_libraries(tweeteria_client PUBLIC tweeteria Qt5::Widgets Qt5::Svg Ghulbus::gbBase sqlpp11-connector-sqlite3)
if(WIN32)
    target_link_libraries(tweeteria_client PUBLIC Qt5::WinMain)
endif()
target_compile_options(tweeteria_client PRIVATE $<$<STREQUAL:$<CXX_COMPILER_ID>,MSVC>:/W4>)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tweeteria_test)

file(COPY ${CASABLANCA_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
get_property(GBBASE_DLL TARGET Ghulbus::gbBase PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO)
add_custom_command(TARGET tweeteria_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Ghulbus::gbBase> $<TARGET_FILE_DIR:tweeteria_test>
)

if(WIN32)
    getQt5Dlls(Qt5::Widgets qt_DLLS)
    getQt5Dlls(Qt5::Svg qt_DLLS)
    file(COPY ${qt_DLLS} ${QT5_ADDITIONAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR})
endif()

option(TWEETERIA_BUILD_TESTS "If enabled, builds unit tests along with the library." ON)
if(TWEETERIA_BUILD_TESTS)
    enable_testing()
endif()