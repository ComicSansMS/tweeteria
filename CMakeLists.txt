cmake_minimum_required(VERSION 3.7)

project(tweeteria)

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
    ${CLIENT_UI_DIR}/tweet_widget.cpp
    ${CLIENT_UI_DIR}/tweets_list.cpp
    ${CLIENT_UI_DIR}/user_widget.cpp
)
set(CLIENT_QT_MOC_HEADER_FILES
    ${CLIENT_UI_DIR}/main_window.hpp
    ${CLIENT_UI_DIR}/central_widget.hpp
    ${CLIENT_UI_DIR}/tweet_widget.hpp
    ${CLIENT_UI_DIR}/tweets_list.hpp
    ${CLIENT_UI_DIR}/user_widget.hpp
)
qt5_wrap_cpp(CLIENT_QT_MOC_SOURCE_FILES ${CLIENT_QT_MOC_HEADER_FILES})
source_group(moc FILES
    ${CLIENT_QT_MOC_SOURCE_FILES}
)

add_executable(tweeteria_client WIN32
    ${CLIENT_SOURCE_DIR}/main.cpp
    ${CLIENT_SOURCE_FILES}
    ${CLIENT_HEADER_FILES}
    ${CLIENT_QT_SOURCE_FILES}
    ${CLIENT_QT_SOURCE_FILES}
    ${CLIENT_QT_MOC_HEADER_FILES}
    ${CLIENT_QT_MOC_SOURCE_FILES}
)
target_include_directories(tweeteria_client PUBLIC ${CLIENT_SOURCE_DIR})
target_link_libraries(tweeteria_client PUBLIC tweeteria Qt5::Widgets Qt5::WinMain Ghulbus::gbBase sqlpp11-connector-sqlite3)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tweeteria_test)

file(COPY ${CASABLANCA_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
get_property(GBBASE_DLL TARGET Ghulbus::gbBase PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO)
add_custom_command(TARGET tweeteria_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Ghulbus::gbBase> $<TARGET_FILE_DIR:tweeteria_test>
)

getQt5Dlls(Qt5::Widgets qt_DLLS)
file(COPY ${qt_DLLS} ${QT5_ADDITIONAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR})

option(TWEETERIA_BUILD_TESTS "If enabled, builds unit tests along with the library." ON)
if(TWEETERIA_BUILD_TESTS)
    enable_testing()
endif()