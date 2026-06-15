#include "encrypt.h"

namespace core {
Encryptor::Encryptor(PublicKey pubKey) : key(std::move(pubKey)) {}

std::vector<uint8_t> Encryptor::encrypt(const std::string& plaintext) const {
    std::vector<uint8_t> ciphertext;

    // The ciphertext block size is determined by the byte-length of the modulus n
    const size_t blockSize = key.n.getBytes().size();
    if (blockSize == 0) return ciphertext;

    for (const char c : plaintext) {
        // 1. Convert the character byte to an arbitrary-precision Base256 representation
        const operations::Base256 m(static_cast<uint8_t>(c));

        // 2. Perform RSA mathematical operation: C = M^e mod n
        operations::Base256 c_num = operations::Base256::modPow(m, key.e, key.n);

        // 3. Extract the raw bytes from the computed ciphertext number
        std::vector<uint8_t> c_bytes = c_num.getBytes();

        // 4. Padding: Pad the byte vector with trailing zeros up to the required block size.
        //    (Assuming little-endian layout, where the most significant bytes are placed at the end)
        while (c_bytes.size() < blockSize) {
            c_bytes.push_back(0);
        }

        // 5. Append the padded block to the final ciphertext vector
        ciphertext.insert(ciphertext.end(), c_bytes.begin(), c_bytes.end());
    }

    return ciphertext;
}
}