#include "HashUtils.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace {
constexpr std::array<std::uint32_t, 64> K = {
    0x428a2f98u,0x71374491u,0xb5c0fbcfu,0xe9b5dba5u,0x3956c25bu,0x59f111f1u,0x923f82a4u,0xab1c5ed5u,
    0xd807aa98u,0x12835b01u,0x243185beu,0x550c7dc3u,0x72be5d74u,0x80deb1feu,0x9bdc06a7u,0xc19bf174u,
    0xe49b69c1u,0xefbe4786u,0x0fc19dc6u,0x240ca1ccu,0x2de92c6fu,0x4a7484aau,0x5cb0a9dcu,0x76f988dau,
    0x983e5152u,0xa831c66du,0xb00327c8u,0xbf597fc7u,0xc6e00bf3u,0xd5a79147u,0x06ca6351u,0x14292967u,
    0x27b70a85u,0x2e1b2138u,0x4d2c6dfcu,0x53380d13u,0x650a7354u,0x766a0abbu,0x81c2c92eu,0x92722c85u,
    0xa2bfe8a1u,0xa81a664bu,0xc24b8b70u,0xc76c51a3u,0xd192e819u,0xd6990624u,0xf40e3585u,0x106aa070u,
    0x19a4c116u,0x1e376c08u,0x2748774cu,0x34b0bcb5u,0x391c0cb3u,0x4ed8aa4au,0x5b9cca4fu,0x682e6ff3u,
    0x748f82eeu,0x78a5636fu,0x84c87814u,0x8cc70208u,0x90befffau,0xa4506cebu,0xbef9a3f7u,0xc67178f2u
};

inline std::uint32_t rotr(std::uint32_t x, unsigned n) {
    return (x >> n) | (x << (32 - n));
}
inline std::uint32_t ch(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return (x & y) ^ (~x & z);
}
inline std::uint32_t maj(std::uint32_t x, std::uint32_t y, std::uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}
inline std::uint32_t bsig0(std::uint32_t x) { return rotr(x,2) ^ rotr(x,13) ^ rotr(x,22); }
inline std::uint32_t bsig1(std::uint32_t x) { return rotr(x,6) ^ rotr(x,11) ^ rotr(x,25); }
inline std::uint32_t ssig0(std::uint32_t x) { return rotr(x,7) ^ rotr(x,18) ^ (x >> 3); }
inline std::uint32_t ssig1(std::uint32_t x) { return rotr(x,17) ^ rotr(x,19) ^ (x >> 10); }

std::array<std::uint8_t, 32> sha256(const std::vector<std::uint8_t>& input) {
    std::vector<std::uint8_t> data = input;
    const std::uint64_t bit_len = static_cast<std::uint64_t>(data.size()) * 8;
    data.push_back(0x80);
    while ((data.size() % 64) != 56) data.push_back(0);
    for (int i = 7; i >= 0; --i)
        data.push_back(static_cast<std::uint8_t>((bit_len >> (i * 8)) & 0xff));

    std::uint32_t h0=0x6a09e667u,h1=0xbb67ae85u,h2=0x3c6ef372u,h3=0xa54ff53au;
    std::uint32_t h4=0x510e527fu,h5=0x9b05688cu,h6=0x1f83d9abu,h7=0x5be0cd19u;

    for (std::size_t off=0; off<data.size(); off+=64) {
        std::array<std::uint32_t,64> w{};
        for (int i=0;i<16;++i)
            w[i]=(std::uint32_t(data[off+i*4])<<24)|(std::uint32_t(data[off+i*4+1])<<16)|
                 (std::uint32_t(data[off+i*4+2])<<8)|std::uint32_t(data[off+i*4+3]);
        for (int i=16;i<64;++i)
            w[i]=ssig1(w[i-2])+w[i-7]+ssig0(w[i-15])+w[i-16];

        auto a=h0,b=h1,c=h2,d=h3,e=h4,f=h5,g=h6,h=h7;
        for (int i=0;i<64;++i) {
            const auto t1=h+bsig1(e)+ch(e,f,g)+K[i]+w[i];
            const auto t2=bsig0(a)+maj(a,b,c);
            h=g; g=f; f=e; e=d+t1; d=c; c=b; b=a; a=t1+t2;
        }
        h0+=a;h1+=b;h2+=c;h3+=d;h4+=e;h5+=f;h6+=g;h7+=h;
    }

    std::array<std::uint32_t,8> hs={h0,h1,h2,h3,h4,h5,h6,h7};
    std::array<std::uint8_t,32> out{};
    for (std::size_t i=0;i<hs.size();++i)
        for (int j=0;j<4;++j)
            out[i*4+j]=static_cast<std::uint8_t>((hs[i]>>(24-j*8))&0xff);
    return out;
}
}

std::string sha256File(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) throw std::runtime_error("Cannot open file: " + path);

    std::vector<std::uint8_t> data((std::istreambuf_iterator<char>(file)),
                                    std::istreambuf_iterator<char>());
    const auto digest = sha256(data);

    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (auto byte : digest) out << std::setw(2) << static_cast<unsigned int>(byte);
    return out.str();
}
