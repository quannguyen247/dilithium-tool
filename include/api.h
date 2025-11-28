#ifndef CODEC_API_H
#define CODEC_API_H

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <params.h>

namespace DilithiumCodec {

    // Cấu trúc chứa cặp khóa
    struct KeyPair {
        std::vector<uint8_t> publicKey;
        std::vector<uint8_t> secretKey;
    };

    // Hàm tiện ích để chuyển đổi giữa string và vector byte
    std::vector<uint8_t> stringToBytes(const std::string& str);
    std::string bytesToString(const std::vector<uint8_t>& bytes);

    // General API
    class CryptoService {
    public:
        // KeyGen stage
        static KeyPair generateKeyPair();

        // Sign state (encode) Input data + Secret Key -> Signed Message
        static std::vector<uint8_t> encrypt(const std::string& data, const std::vector<uint8_t>& secretKey);

        // Verify (decode) Signed Message + Public Key -> Original Data
        static std::string decrypt(const std::vector<uint8_t>& signedData, const std::vector<uint8_t>& publicKey);
    };
}

#endif // CODEC_API_H

#define API_MODE 2