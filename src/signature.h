#ifndef RSA_SIGNATURE_H
#define RSA_SIGNATURE_H

#include <cstdint>
#include <vector>

#include "keyPair.h"

namespace core::signature {

[[nodiscard]] std::vector<std::uint8_t> signDigest(
    const PrivateKey& privateKey,
    const std::vector<std::uint8_t>& digest);

[[nodiscard]] bool verifyDigest(
    const PublicKey& publicKey,
    const std::vector<std::uint8_t>& digest,
    const std::vector<std::uint8_t>& signature);

}  // namespace core::signature

#endif  // RSA_SIGNATURE_H
