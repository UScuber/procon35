#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "board.hpp"



#ifndef NDEBUG
  #define assert(expr) \
    (!!(expr) || \
      (std::cout << "-2 -2 -2 -2 " << __FILE__ << ":" << __LINE__ << ": " << __PRETTY_FUNCTION__ << ": Assertion`" << #expr << "` failed." << std::endl, std::abort(), 0))

  #define cerr std::cerr
#else
  #define assert(expr)

  #define cerr if(1); else std::cerr
#endif


using ull = unsigned long long;
using uchar = unsigned char;

enum class Dir {
  U,D,L,R,
};


int rnd(const int l, const int r){
  static std::random_device seed;
  static std::mt19937_64 mt(seed());

  return mt() % (r - l) + l;
}


struct Board;
std::vector<Board> cutting_dies; // 抜き型
