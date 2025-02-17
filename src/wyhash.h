#pragma once

#include <cstdint>
#include <cstring>

using u8 = unsigned char;
using u32 = unsigned int;
using u64 = unsigned long long;

namespace wyhash {

inline void mum(u64* a, u64* b) {
#    if defined(__SIZEOF_INT128__)
    __uint128_t r = *a;
    r *= *b;
    *a = static_cast<u64>(r);
    *b = static_cast<u64>(r >> 64U);
#    elif defined(_MSC_VER) && defined(_M_X64)
    *a = _umul128(*a, *b, b);
#    else
    u64 ha = *a >> 32U;
    u64 hb = *b >> 32U;
    u64 la = static_cast<uint32_t>(*a);
    u64 lb = static_cast<uint32_t>(*b);
    u64 hi{};
    u64 lo{};
    u64 rh = ha * hb;
    u64 rm0 = ha * lb;
    u64 rm1 = hb * la;
    u64 rl = la * lb;
    u64 t = rl + (rm0 << 32U);
    auto c = static_cast<u64>(t < rl);
    lo = t + (rm1 << 32U);
    c += static_cast<u64>(lo < t);
    hi = rh + (rm0 >> 32U) + (rm1 >> 32U) + c;
    *a = lo;
    *b = hi;
#    endif
}

// multiply and xor mix function, aka MUM
inline u64 mix(u64 a, u64 b) {
    mum(&a, &b);
    return a ^ b;
}

// read functions. WARNING: we don't care about endianness, so results are different on big endian!
inline u64 r8(const u8* p) {
    u64 v{};
    std::memcpy(&v, p, 8U);
    return v;
}

inline u64 r4(const u8* p) {
    uint32_t v{};
    std::memcpy(&v, p, 4);
    return v;
}

// reads 1, 2, or 3 bytes
inline u64 r3(const u8* p, size_t k) {
    return (static_cast<u64>(p[0]) << 16U) | (static_cast<u64>(p[k >> 1U]) << 8U) | p[k - 1];
}

inline u64 hash(void const* key, size_t len) 
{
  static constexpr u64 secret[4]{
    0xa0761d6478bd642fULL, 
    0xe7037ed1a0b428dbULL,
    0x8ebc6af09c88c6e3ULL, 
    0x589965cc75374cc3ULL
    };

  const u8 *p = static_cast<const u8 *>(key);
  u64 seed = secret[0];
  u64 a{};
  u64 b{};
  if ((len <= 16)) {
    if ((len >= 4)) {
      a = (r4(p) << 32U) | r4(p + ((len >> 3U) << 2U));
      b = (r4(p + len - 4) << 32U) | r4(p + len - 4 - ((len >> 3U) << 2U));
    } else if ((len > 0)) {
      a = r3(p, len);
      b = 0;
    } else {
      a = 0;
      b = 0;
    }
    } else {
        size_t i = len;
        if ((i > 48)) {
            u64 see1 = seed;
            u64 see2 = seed;
            do {
                seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
                see1 = mix(r8(p + 16) ^ secret[2], r8(p + 24) ^ see1);
                see2 = mix(r8(p + 32) ^ secret[3], r8(p + 40) ^ see2);
                p += 48;
                i -= 48;
            } while ((i > 48));
            seed ^= see1 ^ see2;
        }
        while ((i > 16)) {
            seed = mix(r8(p) ^ secret[1], r8(p + 8) ^ seed);
            i -= 16;
            p += 16;
        }
        a = r8(p + i - 16);
        b = r8(p + i - 8);
    }

    return mix(secret[1] ^ len, mix(a ^ secret[1], b ^ seed));
}

inline u64 hash(u64 x){
    return wyhash::mix(x, 0x9E3779B97F4A7C15ULL);
}

} // namespace wyhash