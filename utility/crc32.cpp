#include "crc32.h"

namespace utility {

//---------------------------------------------------------------------------------
/**
 * @brief 文字列をcrc32ハッシュ化する
 */
uint32_t stringToHash(std::string str) noexcept {
    uint32_t c = 0xFFFFFFFF;

    // 終端文字は含めない
    auto len = str.size() - 1;
    for (auto i = 0; i < len; i++) {
        c = CRC32Table[(c ^ str[i]) & 0xFF] ^ (c >> 8);
    }
    return c ^ 0xFFFFFFFF;
}
}  // namespace utility
