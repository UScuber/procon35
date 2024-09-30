#pragma once

#include <iostream>
#include <algorithm>
#include <cassert>


enum class Dir {
  U,D,L,R,
};


struct Operation {
  inline constexpr Operation();

  inline constexpr Operation(const unsigned int x);

  inline constexpr Operation(const int kata, const int y, const int x, const Dir dir);

  inline constexpr int kata_idx() const;

  inline constexpr int y() const;
  
  inline constexpr int x() const;

  inline constexpr Dir dir() const;

  void debug() const;

private:

  // |--kata---|f---y----|f---x----|di|---|
  unsigned int data;
};

using Operations = std::vector<Operation>;



inline constexpr Operation::Operation() : data(0){}

inline constexpr Operation::Operation(const unsigned int x) : data(x){}

inline constexpr Operation::Operation(const int kata, const int y, const int x, const Dir dir) : data(0){
  assert(0 <= kata && kata < 25 + 256);
  assert(-256 < y && y < 256 && -256 < x && x < 256);
  data =  kata
        | ((y < 0) << 9)
        | (std::abs(y) << 10)
        | ((x < 0) << 18)
        | (std::abs(x) << 19)
        | ((int)dir << 27);
}

inline constexpr int Operation::kata_idx() const{
  return data & ((1 << 9) - 1);
}

inline constexpr int Operation::y() const{
  const int v = (data >> 10) & ((1 << 8) - 1);
  if(data >> 9 & 1) return -v;
  return v;
}

inline constexpr int Operation::x() const{
  const int v = (data >> 19) & ((1 << 8) - 1);
  if(data >> 18 & 1) return -v;
  return v;
}

inline constexpr Dir Operation::dir() const{
  return (Dir)(data >> 27 & 3);
}

void Operation::debug() const{
  std::cerr << "idx: " << kata_idx() << ", y: " << y() << ", x: " << x() << ", dir: ";
  char c = 'U';
  if(dir() == Dir::D) c = 'D';
  else if(dir() == Dir::L) c = 'L';
  else if(dir() == Dir::R) c = 'R';
  std::cerr << c << std::endl;
}

std::istream& operator>>(std::istream& is, Operation& ops){
  int kata,y,x,dir;
  is >> kata >> y >> x >> dir;
  ops = Operation(kata, y, x, (Dir)dir);
  return is;
}

std::ostream& operator<<(std::ostream& os, const Operation& ops){
  return os << ops.kata_idx() << ' ' << ops.y() << ' ' << ops.x() << ' ' << (int)ops.dir();
}