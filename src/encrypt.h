#ifndef ENCRYPT_H
#define ENCRYPT_H

#include <string>
#include <vector>

#include "keyPair.h"

namespace core {
class Encryptor {
   private:
    PublicKey key;

   public:
    // Constructor binds the encryption process to a specific Public Key
    explicit Encryptor(PublicKey pubKey);

    // Performs RSA encryption on a plaintext string
    [[nodiscard]] std::vector<uint8_t> encrypt(const std::string& plaintext) const;
};
}  // namespace core

#endif