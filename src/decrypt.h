#ifndef DECRYPT_H
#define DECRYPT_H

#include <string>
#include <vector>

#include "keyPair.h"

namespace core {
namespace decryptor {

    // Performs RSA decryption on a ciphertext byte vector
    [[nodiscard]] std::string decrypt(keyPair& keyPair, const std::vector<uint8_t>& ciphertext);
};
}  // namespace core

#endif