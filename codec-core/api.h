#ifndef CODEC_API_H
#define CODEC_API_H

#include <vector>
#include <string>
#include <cstdint>
#include <stdexcept>

// Chọn mode mặc định là 2 (An toàn chuẩn NIST)
#ifndef DILITHIUM_MODE
#define DILITHIUM_MODE 2
#endif

namespace DilithiumCodec {

    // Cấu trúc chứa cặp khóa
    struct KeyPair {
        std::vector<uint8_t> publicKey;
        std::vector<uint8_t> secretKey;
    };

    // Hàm tiện ích để chuyển đổi giữa string và vector byte
    std::vector<uint8_t> stringToBytes(const std::string& str);
    std::string bytesToString(const std::vector<uint8_t>& bytes);

    // Interface chung
    class CryptoService {
    public:
        // Sinh cặp khóa mới
        static KeyPair generateKeyPair();

        // "Mã hóa" (Thực chất là Ký): Input data + Secret Key -> Signed Message
        static std::vector<uint8_t> encrypt(const std::string& data, const std::vector<uint8_t>& secretKey);

        // "Giải mã" (Thực chất là Mở/Xác thực): Signed Message + Public Key -> Original Data
        static std::string decrypt(const std::vector<uint8_t>& signedData, const std::vector<uint8_t>& publicKey);
    };
}

#endif // CODEC_API_H