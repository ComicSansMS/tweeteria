find_path(SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT
    NAMES "include/sqlpp11/sqlite3/sqlite3.h"
    HINTS ENV SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT
    DOC "Sqlpp11 Connector Sqlite3 source directory"
)

find_path(SQLITE3_INCLUDE_DIR sqlite3.h
    HINTS ENV SQLITE3_ROOT
    DOC "Sqlite3 include directory"
)

find_library(SQLITE3_LIBRARY_RELEASE sqlite3
    HINTS ENV SQLITE3_ROOT
    PATH_SUFFIXES "build/RelWithDebInfo" "build/Release"
    DOC "Sqlite3 library"
)

find_library(SQLITE3_LIBRARY_DEBUG sqlite3
    HINTS ENV SQLITE3_ROOT
    PATH_SUFFIXES "build/Debug"
    DOC "Sqlite3 library"
)
set(SQLITE3_LIBRARY
    $<$<CONFIG:Debug>:${SQLITE3_LIBRARY_DEBUG}>
    $<$<NOT:$<CONFIG:Debug>>:${SQLITE3_LIBRARY_RELEASE}>
)

if(SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT)
    add_library(sqlpp11-connector-sqlite3
        ${SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT}/src/connection.cpp
        ${SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT}/src/bind_result.cpp
        ${SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT}/src/prepared_statement.cpp
        ${SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT}/src/detail/connection_handle.cpp
    )
    if(NOT TARGET sqlpp11)
        message(FATAL_ERROR "Sqlpp11 not found.")
    endif()
    target_link_libraries(sqlpp11-connector-sqlite3 PUBLIC sqlpp11 ${SQLITE3_LIBRARY})
    target_include_directories(sqlpp11-connector-sqlite3 PUBLIC ${SQLITE3_INCLUDE_DIR})
    target_include_directories(sqlpp11-connector-sqlite3 PUBLIC ${SQLPP11_CONNECTOR_SQLITE3_SOURCE_ROOT}/include)
endif()