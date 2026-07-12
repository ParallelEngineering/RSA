#include "signature.h"

#include "helper.h"
#include "math_utils.h"

namespace core::signature {
namespace {

[[nodiscard]] operations::BigInt digestToInteger(
    const std::vector<std::uint8_t>& digest) {
    return operations::BigInt(bytesToByteArray(digest));
}

[[nodiscard]] std::size_t modulusSize(const operations::BigInt& modulus) {
    return modulus.getBytes().size() * sizeof(std::uint64_t);
}

}  // namespace

std::vector<std::uint8_t> signDigest(
    const PrivateKey& privateKey,
    const std::vector<std::uint8_t>& digest) {
    const operations::BigInt one(1);
    if (digest.empty() || privateKey.n <= one || privateKey.d <= one) {
        return {};
    }

    const auto digestInteger = digestToInteger(digest);
    if (digestInteger >= privateKey.n) {
        return {};
    }

    const auto signature = operations::math::modPow(
        digestInteger, privateKey.d, privateKey.n);
    return byteArrayToBytes(signature.getBytes(), modulusSize(privateKey.n));
}

bool verifyDigest(
    const PublicKey& publicKey,
    const std::vector<std::uint8_t>& digest,
    const std::vector<std::uint8_t>& signature) {
    const operations::BigInt one(1);
    if (digest.empty() || publicKey.n <= one || publicKey.e <= one
        || signature.size() != modulusSize(publicKey.n)) {
        return false;
    }

    const auto digestInteger = digestToInteger(digest);
    const operations::BigInt signatureInteger(bytesToByteArray(signature));
    if (digestInteger >= publicKey.n || signatureInteger >= publicKey.n) {
        return false;
    }

    const auto verifiedDigest = operations::math::modPow(
        signatureInteger, publicKey.e, publicKey.n);
    return verifiedDigest == digestInteger;
}

}  // namespace core::signature
