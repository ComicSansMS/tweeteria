find_path(SQLPP11_BASE_DIR
    NAMES "lib/cmake/Sqlpp11"
    HINTS ENV SQLPP11_ROOT
    DOC "Sqlpp11 root directory"
)

find_path(DATE_DIR
    NAMES date.h
    HINTS ENV DATE_ROOT
    DOC "Hinnant date library"
)
if(DATE_DIR)
    set(HinnantDate_ROOT_DIR ${DATE_DIR} CACHE PATH "Root directory of Howard Hinnant's date library")
endif()
