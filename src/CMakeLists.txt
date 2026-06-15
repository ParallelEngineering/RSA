add_library(RSA
        decrypt.cpp
        encrypt.cpp
        keyPair.cpp
        key_fwd.h
)

# Add src directory to path for header includes
target_include_directories(RSA
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)