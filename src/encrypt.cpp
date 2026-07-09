#include "encrypt.h"
#include "math_utils.h"
#include "helper.h"

using namespace operations::math;

namespace core::encryptor {
std::vector<uint8_t> encrypt(keyPair& keyPair, const std::string& plaintext) {
    std::vector<uint8_t> ciphertext;

    // The block size in bytes is determined by the size of modulus n multiplied by 8 (64 bits per limb)
    const size_t blockSize = keyPair.getPublicKey().n.getBytes().size() * 8;
    if (blockSize == 0) return ciphertext;

    for (const char c : plaintext) {
        // 1. Convert the character byte to the internal 64-bit limb representation
        const operations::Base256 m(static_cast<uint8_t>(c));

        // 2. Perform RSA mathematical operation: C = M^e mod n
        operations::Base256 c_num = modPow(m, keyPair.getPublicKey().e, keyPair.getPublicKey().n);

        // 3. Extract raw bytes from the computed ciphertext number (padded to target block size)
        std::vector<uint8_t> c_bytes = byteArrayToBytes(c_num.getBytes(), blockSize);

        // 4. Append the padded block to the final ciphertext vector
        ciphertext.insert(ciphertext.end(), c_bytes.begin(), c_bytes.end());
    }

    return ciphertext;
}
}  // namespace core::encryptor