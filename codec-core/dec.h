#ifndef CODEC_DEC_H
#define CODEC_DEC_H

#include "api.h"

namespace DilithiumCodec {
    class Decoder {
    public:
        // Hàm thực hiện việc mở (xác thực) dữ liệu
        // Trả về true nếu thành công, false nếu chữ ký không hợp lệ
        static bool verifyData(const std::vector<uint8_t>& signedData, 
                             const std::vector<uint8_t>& publicKey, 
                             std::vector<uint8_t>& outputData);
    };
}

#endif // CODEC_DEC_H