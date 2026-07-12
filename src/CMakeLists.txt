add_library(RSA
        decrypt.cpp
        encrypt.cpp
        keyPair.cpp
        signature.cpp
)

# Add src directory to path for header includes
target_include_directories(RSA
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

if (WIN32)
        target_link_libraries(RSA PRIVATE bcrypt)
endif ()
