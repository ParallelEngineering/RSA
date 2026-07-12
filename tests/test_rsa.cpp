#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

#include "decrypt.h"
#include "encrypt.h"
#include "keyPair.h"
#include "signature.h"

using core::decryptor::decrypt;
using core::encryptor::encrypt;

TEST_CASE("RSA Core: Basic Encryption and Decryption Roundtrip") {
    static keyPair pair;

    SECTION("Decrypting encrypted normal string recovers the original text") {
        std::string plaintext = "Hello, C++ World!";

        std::vector<uint8_t> ciphertext = encrypt(pair, plaintext);

        REQUIRE_FALSE(ciphertext.empty());

        std::string recovered = decrypt(pair, ciphertext);

        REQUIRE(recovered == plaintext);
    }

    SECTION("Decrypting encrypted empty string recovers empty string") {
        std::string plaintext = "";

        std::vector<uint8_t> ciphertext = encrypt(pair, plaintext);
        std::string recovered = decrypt(pair, ciphertext);

        REQUIRE(recovered == plaintext);
    }

    SECTION("Correct handling of spaces, numbers, and special symbols") {
        std::string plaintext = "RSA_4096_Test! @#$%^&*()_+ 12345";

        std::vector<uint8_t> ciphertext = encrypt(pair, plaintext);
        std::string recovered = decrypt(pair, ciphertext);

        REQUIRE(recovered == plaintext);
    }
}

TEST_CASE("RSA Core: Security and Key Isolation") {
    static keyPair pairA;
    static keyPair pairB;

    std::string plaintext = "Highly Confidential Cryptographic Data";

    std::vector<uint8_t> ciphertext = encrypt(pairA, plaintext);

    SECTION("Decrypting with the incorrect keypair must not recover the plaintext") {
        std::string recovered = decrypt(pairB, ciphertext);

        REQUIRE(recovered != plaintext);
    }
}

TEST_CASE("RSA Core: Key Serialization and Base64 Import/Export") {
    static keyPair originalPair;

    std::string pubBase64 = keyPair::base64Encode(originalPair.getPublicKey().serialize());
    std::string privBase64 = keyPair::base64Encode(originalPair.getPrivateKey().serialize());

    REQUIRE_FALSE(pubBase64.empty());
    REQUIRE_FALSE(privBase64.empty());

    keyPair importedPair(pubBase64, privBase64);

    std::string plaintext = "Verification of Imported Keys";
    std::vector<uint8_t> ciphertext = encrypt(importedPair, plaintext);
    std::string recovered = decrypt(importedPair, ciphertext);

    REQUIRE(recovered == plaintext);
}

TEST_CASE("RSA Core: Key Creation and Struct-based Initialization") {
    static keyPair originalPair;

    SECTION("Creating keyPair from deserialized PublicKey and PrivateKey structs") {
        PublicKey pub = originalPair.getPublicKey();
        PrivateKey priv = originalPair.getPrivateKey();

        // 1. Verify the new struct-based constructor
        keyPair constructedPair(pub, priv);

        std::string plaintext = "Struct Constructor Roundtrip Verification";
        std::vector<uint8_t> ciphertext = encrypt(constructedPair, plaintext);
        std::string recovered = decrypt(originalPair, ciphertext);

        REQUIRE(recovered == plaintext);

        // 2. Verify the static factory method 'create' with structs
        keyPair factoryStructPair = keyPair::create(pub, priv);

        std::vector<uint8_t> ciphertext2 = encrypt(originalPair, plaintext);
        std::string recovered2 = decrypt(factoryStructPair, ciphertext2);

        REQUIRE(recovered2 == plaintext);
    }

    SECTION("Creating keyPair from raw serialized byte vectors") {
        std::vector<uint8_t> pubBytes = originalPair.getPublicKey().serialize();
        std::vector<uint8_t> privBytes = originalPair.getPrivateKey().serialize();

        REQUIRE_FALSE(pubBytes.empty());
        REQUIRE_FALSE(privBytes.empty());

        // Verify the static factory method 'create' with raw byte vectors (bypassing Base64)
        keyPair factoryBytesPair = keyPair::create(pubBytes, privBytes);

        std::string plaintext = "Binary Serialization Factory Verification";
        std::vector<uint8_t> ciphertext = encrypt(factoryBytesPair, plaintext);
        std::string recovered = decrypt(originalPair, ciphertext);

        REQUIRE(recovered == plaintext);
    }

    SECTION("Creating keyPair from invalid/corrupt serialized bytes throws exception") {
        std::vector<uint8_t> invalidPub = {1, 2, 3, 4};  // Invalid size and payload
        std::vector<uint8_t> invalidPriv = {5, 6, 7, 8};

        // Expect std::runtime_error as specified in keyPair::create
        REQUIRE_THROWS_AS(keyPair::create(invalidPub, invalidPriv), std::runtime_error);
    }
}

TEST_CASE("RSA Core: Digest signatures prove private-key possession") {
    keyPair pairA(PublicKey{operations::BigInt(3233), operations::BigInt(17)},
                  PrivateKey{operations::BigInt(3233), operations::BigInt(2753)});
    keyPair pairB(PublicKey{operations::BigInt(2773), operations::BigInt(17)},
                  PrivateKey{operations::BigInt(2773), operations::BigInt(157)});
    const std::vector<std::uint8_t> digest = {0x2A};

    const auto signature = core::signature::signDigest(pairA.getPrivateKey(), digest);
    REQUIRE_FALSE(signature.empty());
    REQUIRE(core::signature::verifyDigest(pairA.getPublicKey(), digest, signature));

    SECTION("A different public key does not verify the signature") {
        REQUIRE_FALSE(core::signature::verifyDigest(pairB.getPublicKey(), digest, signature));
    }

    SECTION("A modified digest does not verify the signature") {
        auto modifiedDigest = digest;
        modifiedDigest.front() ^= 0x01;
        REQUIRE_FALSE(
            core::signature::verifyDigest(pairA.getPublicKey(), modifiedDigest, signature));
    }

    SECTION("A modified signature is rejected") {
        auto modifiedSignature = signature;
        modifiedSignature.back() ^= 0x01;
        REQUIRE_FALSE(
            core::signature::verifyDigest(pairA.getPublicKey(), digest, modifiedSignature));
    }

    SECTION("Malformed inputs are rejected") {
        REQUIRE(core::signature::signDigest(pairA.getPrivateKey(), {}).empty());
        REQUIRE_FALSE(core::signature::verifyDigest(pairA.getPublicKey(), {}, signature));
        REQUIRE_FALSE(core::signature::verifyDigest(pairA.getPublicKey(), digest, {}));
    }
}
