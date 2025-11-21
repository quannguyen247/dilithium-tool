#ifndef CODEC_ENC_H
#define CODEC_ENC_H

#include "api.h"

namespace DilithiumCodec {
    class Encoder {
    public:
        // Hàm thực hiện việc ký (đóng gói) dữ liệu
        static std::vector<uint8_t> signData(const std::vector<uint8_t>& data, const std::vector<uint8_t>& secretKey);
        
        // Hàm sinh khóa (wrapper cho core C)
        static KeyPair keygen();
    };
}

#endif // CODEC_ENC_H