project ("UtilitySrc")

add_executable (UtilitySrc "Utility.cpp" "Utility.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  SET_TARGET_PROPERTIES(UtilitySrc PROPERTIES LINKER_LANGUAGE CXX)
  SET_PROPERTY(TARGET UtilitySrc PROPERTY CXX_STANDARD 23)
endif()
