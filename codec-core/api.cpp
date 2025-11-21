#include "api.h"
#include "enc.h"
#include "dec.h"

namespace DilithiumCodec {

    std::vector<uint8_t> stringToBytes(const std::string& str) {
        return std::vector<uint8_t>(str.begin(), str.end());
    }

    std::string bytesToString(const std::vector<uint8_t>& bytes) {
        return std::string(bytes.begin(), bytes.end());
    }

    KeyPair CryptoService::generateKeyPair() {
        return Encoder::keygen();
    }

    std::vector<uint8_t> CryptoService::encrypt(const std::string& data, const std::vector<uint8_t>& secretKey) {
        return Encoder::signData(stringToBytes(data), secretKey);
    }

    std::string CryptoService::decrypt(const std::vector<uint8_t>& signedData, const std::vector<uint8_t>& publicKey) {
        std::vector<uint8_t> output;
        if (Decoder::verifyData(signedData, publicKey, output)) {
            return bytesToString(output);
        } else {
            throw std::runtime_error("Verification failed: Invalid signature or corrupted data");
        }
    }
}
