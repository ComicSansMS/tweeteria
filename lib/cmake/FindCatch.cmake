find_path(CATCH_INCLUDE_DIR
  catch.hpp
  HINTS $ENV{CATCH_ROOT}
  PATH_SUFFIXES single_include
)
