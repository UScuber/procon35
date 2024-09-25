#pragma once

#include "common.hpp"


struct Bitset {
  static constexpr int SIZE = 8;

  constexpr Bitset();

  inline Bitset &operator<<=(int len);

  inline Bitset operator<<(const int len) const;

  inline Bitset &operator>>=(int len);

  inline Bitset operator>>(const int len) const;

  inline constexpr Bitset &operator&=(const Bitset& bit);

  inline constexpr Bitset &operator|=(const Bitset& bit);

  inline constexpr Bitset operator|(const Bitset& bit) const;

  // &= (1 << len) - 1
  inline Bitset& mask(const int len);

  // &= ~(((1 << (r-l))-1) << l)
  inline Bitset& remove(const int l, const int r);

  inline Bitset& rotate(int l, int m, int r);

  struct reference {
    ull *ptr;
    const int idx;

    inline constexpr reference(ull *p, const int id);

    inline reference &operator=(const uchar v);

    inline reference &operator=(const reference& ref);

    inline constexpr operator uchar() const;
  };

  inline reference operator[](const int idx);

  inline constexpr uchar operator[](const int idx) const;

private:
  ull a[SIZE]; // 512bit

};



constexpr Bitset::Bitset() : a(){}

inline Bitset &Bitset::operator<<=(int len){
  const int div = len / 64;
  const int rem = len % 64;

  if(!rem){
    for(int i = SIZE - 1; i >= div; i--) a[i] = a[i - div];
  }else{
    for(int i = SIZE - 1; i > div; i--){
      a[i] = ((a[i - div] << rem) | (a[i - div - 1] >> (64 - rem)));
    }
    a[div] = a[0] << rem;
  }
  std::fill(a, a + div, 0);
  return *this;
}

inline Bitset Bitset::operator<<(const int len) const{
  return Bitset(*this) <<= len;
}

inline Bitset &Bitset::operator>>=(int len){
  const int div = len / 64;
  const int rem = len % 64;
  const int lim = 8 - div - 1;

  if(!rem){
    for(int i = 0; i <= lim; i++){
      a[i] = a[i + div];
    }
  }else{
    for(int i = 0; i < lim; i++){
      a[i] = ((a[i + div] >> rem) | (a[i + div + 1] << (64 - rem)));
    }
    a[lim] = a[SIZE-1] >> rem;
  }
  std::fill(a + lim + 1, a + SIZE, 0);
  return *this;
}

inline Bitset Bitset::operator>>(const int len) const{
  return Bitset(*this) >>= len;
}

inline constexpr Bitset &Bitset::operator&=(const Bitset& bit){
  for(int i = 0; i < SIZE; i++){
    a[i] &= bit.a[i];
  }
  return *this;
}

inline constexpr Bitset &Bitset::operator|=(const Bitset& bit){
  for(int i = 0; i < SIZE; i++){
    a[i] |= bit.a[i];
  }
  return *this;
}

inline constexpr Bitset Bitset::operator|(const Bitset& bit) const{
  return Bitset(*this) |= bit;
}

// &= (1 << len) - 1
inline Bitset &Bitset::mask(const int len){
  if(len == 512) return *this;
  const int div = len / 64;
  const int rem = len % 64;
  a[div] &= (1ULL << rem) - 1;
  std::fill(a + div+1, a + SIZE, 0);
  return *this;
}

// &= ~(((1 << (r-l))-1) << l)
inline Bitset &Bitset::remove(const int l, const int r){
  if(l/64 == r/64){
    a[l/64] &= ~(((1ULL << (r - l)) - 1) << (l % 64));
  }else{
    a[l/64] &= (1ULL << (l % 64)) - 1;
    if(l/64+1 < r/64) std::fill(a + l/64+1, a + r/64, 0);
    if(r % 64) a[r/64] &= ~((1ULL << (r % 64)) - 1);
  }
  return *this;
}

inline Bitset &Bitset::rotate(int l, int m, int r){
  assert(0 <= l && l <= m && m < r && r <= 256);
  l *= 2; m *= 2; r *= 2;

  Bitset buf = *this;
  buf >>= l;
  buf.mask(r - l);
  remove(l, r);
  buf = (buf >> (m - l)) | (buf << (r - m));
  buf.mask(r - l);
  *this |= (buf << l);
  return *this;
}


inline constexpr Bitset::reference::reference(ull *p, const int id) : ptr(p), idx(id){}

inline Bitset::reference &Bitset::reference::operator=(const uchar v){
  ptr[idx / 32] = (ptr[idx / 32] & ~(3ULL << ((idx & 31) * 2))) | ((ull)v << ((idx & 31) * 2));
  return *this;
}

inline Bitset::reference &Bitset::reference::operator=(const reference &ref){
  return *this = (uchar)ref;
}

inline constexpr Bitset::reference::operator uchar() const{
  return ptr[idx / 32] >> ((idx & 31) * 2) & 3;
}

inline Bitset::reference Bitset::operator[](const int idx){
  return reference(a, idx);
}

inline constexpr uchar Bitset::operator[](const int idx) const{
  return a[idx / 32] >> ((idx & 31) * 2) & 3;
}
