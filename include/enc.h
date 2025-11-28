#ifndef CODEC_ENC_H
#define CODEC_ENC_H

#include "api.h"

namespace DilithiumCodec {
    class Encoder {
    public:
        // Encoding (signing) function (wrapper for core C)
        static std::vector<uint8_t> signData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& secretKey);
        
        // Key generation function (wrapper for core C)
        static KeyPair keygen();
    };
}

#endif // CODEC_ENC_H