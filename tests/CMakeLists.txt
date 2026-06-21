# Prevent CMake from re-downloading Catch2 every time CLion reloads
set(FETCHCONTENT_UPDATES_DISCONNECTED ON CACHE BOOL "Don't update FetchContent on every config" FORCE)

# Download Catch2
include(FetchContent)
FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG v3.11.0
)
FetchContent_MakeAvailable(Catch2)

# Find Catch 2 CMakeList
list(APPEND CMAKE_MODULE_PATH ${Catch2_SOURCE_DIR}/extras)
include(Catch)

add_executable(RSA-Tests
        test_rsa.cpp
)
target_link_libraries(RSA-Tests
        PRIVATE
        RSA
        Catch2::Catch2WithMain
)

# Catch2 integration in CTest
catch_discover_tests(RSA-Tests
        TEST_PREFIX rsa:
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
