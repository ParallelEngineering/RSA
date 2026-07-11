#ifndef KEY_PAIR_H
#define KEY_PAIR_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "bigint.h"
#include "key_fwd.h"

#define KEY_FOLDER "rsa-keys"

enum { NONE, PUBLIC, PRIVATE, BOTH };

struct PublicKey {
    operations::BigInt n;
    operations::BigInt e;

    [[nodiscard]] std::vector<uint8_t> serialize() const;
};

struct PrivateKey {
    operations::BigInt n;
    operations::BigInt d;

    [[nodiscard]] std::vector<uint8_t> serialize() const;
};

class keyPair {
   private:
    PublicKey public_key;
    PrivateKey private_key;

    static constexpr char base64Chars[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

   public:
    keyPair();

    keyPair(const std::string &publicKey, const std::string &privateKey);

    // Constructor to build a keyPair without triggering expensive prime generation
    keyPair(PublicKey publicKey, PrivateKey privateKey);

    PublicKey getPublicKey() { return public_key; }
    PrivateKey getPrivateKey() { return private_key; }

    // Static factory methods to safely create a keyPair
    static keyPair create(const PublicKey &publicKey, const PrivateKey &privateKey);
    static keyPair create(const std::vector<uint8_t> &pubData,
                          const std::vector<uint8_t> &privData);

    static std::vector<uint8_t> s_serialize(const operations::BigInt &first,
                                            const operations::BigInt &second);
    static bool s_deserialize(const std::vector<uint8_t> &data, operations::BigInt &outFirst,
                              operations::BigInt &outSecond);

    // Base64 helper functions
    static std::string base64Encode(const std::vector<uint8_t> &data);
    static std::vector<uint8_t> base64Decode(std::string data);
    static uint8_t getBase64Index(char letter);
};

#endif
