#ifndef DECRYPT_H
#define DECRYPT_H

#include <string>
#include <vector>

#include "keyPair.h"

namespace core {
class Decryptor {
private:
    PrivateKey key;

public:
    // Constructor binds the decryption process to a specific Private Key
    explicit Decryptor(PrivateKey  privKey);

    // Performs RSA decryption on a ciphertext byte vector
    [[nodiscard]] std::string decrypt(const std::vector<uint8_t>& ciphertext) const;
};
}

#endif