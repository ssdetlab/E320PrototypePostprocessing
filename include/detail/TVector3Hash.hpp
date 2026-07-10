#pragma once

#include <cstdint>
#include <cstring>

#include "TVector3.h"

inline std::uint64_t doubleBits(double x) {
  std::uint64_t u = 0;
  std::memcpy(&u, &x, sizeof(double));
  return u;
}

struct TVector3Hash {
  std::size_t operator()(const TVector3 &v) const noexcept {
    uint64_t h1 = doubleBits(v.x());
    uint64_t h2 = doubleBits(v.y());
    uint64_t h3 = doubleBits(v.z());

    uint64_t h = h1;
    h ^= h2 + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
    h ^= h3 + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);

    return static_cast<std::size_t>(h);
  }
};

struct TVector3Eq {
  bool operator()(const TVector3 &a, const TVector3 &b) const noexcept {
    return (doubleBits(a.x()) == doubleBits(b.x())) &&
           (doubleBits(a.y()) == doubleBits(b.y())) &&
           (doubleBits(a.z()) == doubleBits(b.z()));
  }
};
