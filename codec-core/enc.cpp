#include "enc.h"
#include <cstring>
#include <stdexcept>

// Include thư viện C của Dilithium
extern "C" {
    #include "../dilithium-core/ref/api.h"
    #include "../dilithium-core/ref/params.h"
}

// Mapping function names based on mode
#if DILITHIUM_MODE == 2
    #define crypto_sign_keypair pqcrystals_dilithium2_ref_keypair
    #define crypto_sign pqcrystals_dilithium2_ref
#elif DILITHIUM_MODE == 3
    #define crypto_sign_keypair pqcrystals_dilithium3_ref_keypair
    #define crypto_sign pqcrystals_dilithium3_ref
#elif DILITHIUM_MODE == 5
    #define crypto_sign_keypair pqcrystals_dilithium5_ref_keypair
    #define crypto_sign pqcrystals_dilithium5_ref
#endif

namespace DilithiumCodec {

    KeyPair Encoder::keygen() {
        KeyPair kp;
        kp.publicKey.resize(CRYPTO_PUBLICKEYBYTES);
        kp.secretKey.resize(CRYPTO_SECRETKEYBYTES);

        if (crypto_sign_keypair(kp.publicKey.data(), kp.secretKey.data()) != 0) {
            throw std::runtime_error("Key generation failed");
        }
        return kp;
    }

    std::vector<uint8_t> Encoder::signData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& secretKey) {
        if (secretKey.size() != CRYPTO_SECRETKEYBYTES) {
            throw std::invalid_argument("Invalid secret key size");
        }

        // Kích thước tối đa của chữ ký + tin nhắn
        size_t smlen = CRYPTO_BYTES + data.size();
        std::vector<uint8_t> signedMessage(smlen);

        if (crypto_sign(signedMessage.data(), &smlen, 
                       data.data(), data.size(), 
                       NULL, 0, // Không dùng context (optional)
                       secretKey.data()) != 0) {
            throw std::runtime_error("Signing failed");
        }

        // Resize lại vector theo kích thước thực tế trả về
        signedMessage.resize(smlen);
        return signedMessage;
    }
}