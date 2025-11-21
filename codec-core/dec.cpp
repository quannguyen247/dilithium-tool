#include "dec.h"
#include <cstring>
#include <stdexcept>

// Include thư viện C của Dilithium
extern "C" {
    #include "../dilithium-core/ref/api.h"
    #include "../dilithium-core/ref/params.h"
}

// Mapping function names based on mode
#if DILITHIUM_MODE == 2
    #define crypto_sign_open pqcrystals_dilithium2_ref_open
#elif DILITHIUM_MODE == 3
    #define crypto_sign_open pqcrystals_dilithium3_ref_open
#elif DILITHIUM_MODE == 5
    #define crypto_sign_open pqcrystals_dilithium5_ref_open
#endif

namespace DilithiumCodec {

    bool Decoder::verifyData(const std::vector<uint8_t>& signedData, 
                           const std::vector<uint8_t>& publicKey, 
                           std::vector<uint8_t>& outputData) {
        
        if (publicKey.size() != CRYPTO_PUBLICKEYBYTES) {
            throw std::invalid_argument("Invalid public key size");
        }

        // Buffer cho tin nhắn đã giải mã (kích thước tối đa bằng kích thước tin nhắn đã ký)
        outputData.resize(signedData.size());
        size_t mlen = 0;

        int ret = crypto_sign_open(outputData.data(), &mlen,
                                 signedData.data(), signedData.size(),
                                 NULL, 0, // Không dùng context
                                 publicKey.data());

        if (ret != 0) {
            return false; // Chữ ký không hợp lệ
        }

        // Resize lại vector theo kích thước thực tế của tin nhắn gốc
        outputData.resize(mlen);
        return true;
    }
}