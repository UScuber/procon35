#pragma once

#include <vector>
#include "bitset.hpp"


struct Board {

  Board() = delete;

  Board(const int h, const int w) : a(), h(h), w(w){}

  Board(const std::vector<std::vector<int>> &b) : a(), h((int)b.size()), w((int)b[0].size()){
    for(int i = 0; i < h; i++){
      for(int j = 0; j < w; j++){
        a[j][i] = (uchar)b[i][j];
      }
    }
  }

  inline constexpr uchar get(const int i, const int j) const{
    return a[j][i];
  }

  inline void set(const int i, const int j, const uchar v){
    a[j][i] = v;
  }

  inline constexpr int height() const{
    return h;
  }
  
  inline constexpr int width() const{
    return w;
  }

  struct reference {
    Bitset *ptr;
    const int posy;

    inline constexpr reference(Bitset *p, const int y) : ptr(p), posy(y){}

    inline Bitset::reference operator[](const int posx){
      return ptr[posx][posy];
    }

    inline uchar operator[](const int posx) const{
      return ptr[posx][posy];
    }
  };

  struct reference_const {
    const Bitset *ptr;
    const int posy;

    inline constexpr reference_const(const Bitset *p, const int y) : ptr(p), posy(y){}

    inline constexpr uchar operator[](const int posx) const{
      return ptr[posx][posy];
    }
  };

  inline reference operator[](const int posy){
    return reference(a, posy);
  }

  inline constexpr reference_const operator[](const int posy) const{
    return reference_const(a, posy);
  }

private:

  Bitset a[256];
  int h,w;
};
