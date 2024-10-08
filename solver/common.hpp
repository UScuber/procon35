#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
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


struct Board;
std::vector<Board> cutting_dies; // 抜き型
