# RSA 

RSA is an educational C++20 library for RSA-style encryption and
decryption primitives. 

> [!IMPORTANT]
> This project is experimental and intended for learning/library exploration.
> It is not production-ready cryptography.

## Features

- RSA-style encryption and decryption APIs
- Public and private key structs with serialization helpers
- Base64 helper functions for encoded key or byte-vector data

## Integration

Clone the repository with its submodules:

```sh
git clone --recurse-submodules https://github.com/ParallelEngineering/RSA-Encryptor.git
```

To integrate the library into another CMake project, add this repository as a
subdirectory and link against the `RSA` target:

```cmake
add_subdirectory(path/to/RSA-Encryptor)
target_link_libraries(YourTarget PRIVATE RSA)
```

You can also build the library directly with CMake:

```sh
cmake -S . -B build
cmake --build build
```

## Basic Usage

```cpp
#include "decrypt.h"
#include "encrypt.h"
#include "keyPair.h"

#include <cstdint>
#include <string>
#include <vector>

int main() {
    keyPair keys;

    PublicKey publicKey = keys.getPublicKey();
    PrivateKey privateKey = keys.getPrivateKey();

    core::Encryptor encryptor(publicKey);
    core::Decryptor decryptor(privateKey);

    const std::string message = "hello";
    std::vector<uint8_t> ciphertext = encryptor.encrypt(message);
    std::string plaintext = decryptor.decrypt(ciphertext);
}
```
