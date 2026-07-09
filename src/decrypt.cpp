#include "decrypt.h"

#include <iostream>

#include "helper.h"
#include "math_utils.h"

using namespace operations::math;

namespace core::decryptor {
std::string decrypt(keyPair& keyPair, const std::vector<uint8_t>& ciphertext) {
    std::string plaintext;

    // The block size in bytes is determined by the size of modulus n multiplied by 8 (64 bits per
    // limb)
    const size_t blockSize = keyPair.getPrivateKey().n.getBytes().size() * 8;
    if (blockSize == 0 || ciphertext.size() % blockSize != 0) {
        std::cerr << "Decryption error: Invalid ciphertext block size alignment." << std::endl;
        return plaintext;
    }

    // Process the ciphertext block-by-block using the fixed block size
    for (size_t i = 0; i < ciphertext.size(); i += blockSize) {
        // 1. Extract a single block chunk
        std::vector<uint8_t> chunk(ciphertext.begin() + i, ciphertext.begin() + i + blockSize);

        // 2. Construct representation from the extracted block chunk (using 64-bit limbs)
        const operations::Base256 c_num(bytesToByteArray(chunk));

        // 3. Perform RSA mathematical operation: M = C^d mod n
        operations::Base256 m_num =
            modPow(c_num, keyPair.getPrivateKey().d, keyPair.getPrivateKey().n);

        // 4. Retrieve the decrypted byte value and convert it back to a character
        std::vector<uint8_t> m_bytes = byteArrayToBytes(m_num.getBytes());
        if (!m_bytes.empty()) {
            plaintext.push_back(static_cast<char>(m_bytes[0]));
        } else {
            plaintext.push_back('\0');  // Fallback for a zero-value block
        }
    }

    return plaintext;
}
}  // namespace core::decryptor