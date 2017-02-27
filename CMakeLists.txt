cmake_minimum_required(VERSION 3.7)

project(tweeteria)

add_subdirectory(lib)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
find_package(Qt5BaseDir)
if(QT5_BASE_DIR)
    list(APPEND CMAKE_PREFIX_PATH ${QT5_BASE_DIR})
endif()
find_package(Qt5Widgets 5.8 REQUIRED)
find_package(Boost REQUIRED)
find_package(gbBase)

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


set(CLIENT_QT_SOURCE_FILES
    ${PROJECT_SOURCE_DIR}/client/ui/main_window.cpp
    ${PROJECT_SOURCE_DIR}/client/ui/user_widget.cpp
)
set(CLIENT_QT_MOC_HEADER_FILES
    ${PROJECT_SOURCE_DIR}/client/ui/main_window.hpp
    ${PROJECT_SOURCE_DIR}/client/ui/user_widget.hpp
)
qt5_wrap_cpp(CLIENT_QT_MOC_SOURCE_FILES ${CLIENT_QT_MOC_HEADER_FILES})
source_group(moc FILES
    ${CLIENT_QT_MOC_SOURCE_FILES}
)

add_executable(tweeteria_client WIN32
    ${PROJECT_SOURCE_DIR}/client/main.cpp
    ${CLIENT_QT_SOURCE_FILES}
    ${CLIENT_QT_MOC_HEADER_FILES}
    ${CLIENT_QT_MOC_SOURCE_FILES}
)
target_include_directories(tweeteria_client PUBLIC ${PROJECT_SOURCE_DIR}/client)
target_link_libraries(tweeteria_client PUBLIC tweeteria Qt5::Widgets Qt5::WinMain Ghulbus::gbBase)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT tweeteria_test)

file(COPY ${CASABLANCA_DLLS} DESTINATION ${CMAKE_BINARY_DIR})
get_property(GBBASE_DLL TARGET Ghulbus::gbBase PROPERTY IMPORTED_LOCATION_RELWITHDEBINFO)
add_custom_command(TARGET tweeteria_test POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:Ghulbus::gbBase> $<TARGET_FILE_DIR:tweeteria_test>
)

getQt5Dlls(Qt5::Widgets qt_DLLS)
file(COPY ${qt_DLLS} ${QT5_ADDITIONAL_DLLS} DESTINATION ${PROJECT_BINARY_DIR})
