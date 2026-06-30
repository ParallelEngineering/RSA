# RSA

RSA is an educational C++20 library for RSA-style encryption and
decryption primitives. It exposes `core::Encryptor`, `core::Decryptor`,
`PublicKey`, `PrivateKey`, and `keyPair`, and uses the custom `Base256`
big-integer library for modular arithmetic beyond native integer sizes.

> [!IMPORTANT]
> This project is experimental and intended for learning/library exploration.
> It is not production-ready cryptography.

## Installation and Integration

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

## Architecture

- `src/` contains the RSA-facing library target, exposed as `RSA`.
- `core::Encryptor` encrypts byte data with the RSA operation `C = M^e mod n`.
- `core::Decryptor` decrypts ciphertext blocks with `M = C^d mod n`.
- `keyPair` owns the current public/private key data and provides serialization
  helpers.

