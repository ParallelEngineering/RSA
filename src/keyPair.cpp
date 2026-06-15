#include "keyPair.h"

#include <filesystem>
#include <fstream>
#include <string>

// Serializes two 4 bytes Byte Arrays with Big endian
std::vector<uint8_t> keyPair::s_serialize(const operations::Base256 &first,
                                          const operations::Base256 &second) {
    std::vector<uint8_t> serialized;
    const auto &firstBytes = first.getBytes();
    const auto &secondBytes = second.getBytes();

    uint32_t firstSize = firstBytes.size();
    uint32_t secondSize = secondBytes.size();

    serialized.push_back((firstSize >> 24) & 0xFF);
    serialized.push_back((firstSize >> 16) & 0xFF);
    serialized.push_back((firstSize >> 8) & 0xFF);
    serialized.push_back(firstSize & 0xFF);

    serialized.insert(serialized.end(), firstBytes.begin(), firstBytes.end());

    serialized.push_back((secondSize >> 24) & 0xFF);
    serialized.push_back((secondSize >> 16) & 0xFF);
    serialized.push_back((secondSize >> 8) & 0xFF);
    serialized.push_back(secondSize & 0xFF);

    serialized.insert(serialized.end(), secondBytes.begin(), secondBytes.end());

    return serialized;
}

// Deserializes two 4 bytes Byte Arrays with Big endian
bool keyPair::s_deserialize(const std::vector<uint8_t> &data, operations::Base256 &outFirst,
                            operations::Base256 &outSecond) {
    if (data.size() < 8) return false;

    size_t index = 0;

    uint32_t firstSize =
        (data[index] << 24) | (data[index + 1] << 16) | (data[index + 2] << 8) | data[index + 3];
    index += 4;

    if (index + firstSize > data.size()) return false;
    const auto firstBegin =
        data.begin() + static_cast<std::vector<uint8_t>::difference_type>(index);
    const auto firstEnd =
        firstBegin + static_cast<std::vector<uint8_t>::difference_type>(firstSize);
    std::vector<uint8_t> firstBytes(firstBegin, firstEnd);
    index += firstSize;

    if (index + 4 > data.size()) return false;

    const uint32_t secondSize =
        (data[index] << 24) | (data[index + 1] << 16) | (data[index + 2] << 8) | data[index + 3];
    index += 4;

    if (index + secondSize > data.size()) return false;
    const auto secondBegin =
        data.begin() + static_cast<std::vector<uint8_t>::difference_type>(index);
    const auto secondEnd =
        secondBegin + static_cast<std::vector<uint8_t>::difference_type>(secondSize);
    std::vector<uint8_t> secondBytes(secondBegin, secondEnd);

    outFirst = operations::Base256(firstBytes);
    outSecond = operations::Base256(secondBytes);

    return true;
}

std::string keyPair::base64Encode(const std::vector<uint8_t> &data) {
    std::vector<uint8_t> result;
    size_t index = 0;

    while (index < data.size()) {
        uint32_t dataSegment = 0;
        for (int i = 2; i >= 0; i--) {
            if (index >= data.size()) break;
            const uint8_t number = data.at(index++);
            dataSegment += number << (i * 8);
        }

        for (int j = 0; j < 4; j++) {
            dataSegment <<= 6;
            result.push_back(static_cast<uint8_t>((dataSegment & (0b00111111 << 24)) >> 24));
        }
    }

    std::string outString;
    for (auto c : result) {
        outString += base64Chars[c];
    }
    return outString;
}

std::vector<uint8_t> keyPair::base64Decode(std::string data) {
    std::vector<uint8_t> result;

    while (!data.empty()) {
        if (data.length() < 4) break;
        std::string letterSegment = data.substr(0, 4);
        data.erase(0, 4);

        uint32_t dataSegment = 0;
        for (int i = 3; i >= 0; i--) {
            if (static_cast<size_t>(3 - i) >= letterSegment.length()) continue;
            const uint8_t number = getBase64Index(letterSegment.at(3 - i));
            if (number > 0b111111) {
                std::cerr << "Invalid char: " << letterSegment.at(3 - i) << std::endl;
                continue;
            }
            dataSegment += number << (i * 6);
        }

        for (int j = 0; j < 3; j++) {
            dataSegment <<= 8;
            result.push_back((dataSegment & (0xFF << 24)) >> 24);
        }
    }
    return result;
}

uint8_t keyPair::getBase64Index(char letter) {
    for (int i = 0; base64Chars[i] != '\0'; i++) {
        if (base64Chars[i] == letter) {
            return i;
        }
    }
    return 0;
}
