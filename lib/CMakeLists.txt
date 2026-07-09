if(NOT EXISTS "${CMAKE_CURRENT_LIST_DIR}/BigInt/CMakeLists.txt")
  message(FATAL_ERROR "BigInt submodule not initialized. Run: git submodule update --init --recursive")
endif()
add_subdirectory(BigInt)