#pragma once

#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>

#include "board.hpp"


using ull = unsigned long long;
using uchar = unsigned char;

enum class Dir {
  U,D,L,R,
};


struct Board;
std::vector<Board> cutting_dies; // 抜き型
