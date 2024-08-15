#pragma once

#include <vector>
#include "bitset.hpp"
#include "operation.hpp"
using namespace std;

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

  inline uchar get(const int i, const int j) const{
    return a[j][i];
  }

  inline void set(const int i, const int j, const uchar v){
    a[j][i] = v;
  }

  inline int height() const{
    return h;
  }
  
  inline int width() const{
    return w;
  }

  void slide(const Board& kata, const int posy, const int posx, const Dir dir){
    if(dir == Dir::U) up_slide(kata, posy, posx);
    if(dir == Dir::L) left_slide(kata, posy, posx);
    if(dir == Dir::D) down_slide(kata, posy, posx);
    if(dir == Dir::R) right_slide(kata, posy, posx);
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
  void left_slide(const Board &kata, const int posy, const int posx){
    const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
    const int kata_ty = min(kata.height(), h-posy), kata_tx = min(kata.width(), w-posx);
    const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

    for(int i = kata_sy; i < kata_ty; i++){
      auto row = reference(a, i+board_y);
      vector<uchar> picked_pieces;
      vector<bool> ispicked(w);

      // pick
      for(int j = kata_sx; j < kata_tx; j++){
        if(kata[i][j]){
          picked_pieces.push_back(row[j+board_x]);
          ispicked[j+board_x] = true;
        }
      }

      // shift
      int picked_count = 0;
      for(int j = 0; j < w; j++){
        if(ispicked[j]){
          picked_count++;
        }else if(picked_count){
          row[j - picked_count] = row[j];
        }
      }

      // put
      int put_count = 0;
      for(int j = w - picked_count; j < w; j++){
        row[j] = picked_pieces[put_count++];
      }
    }
  }
  void right_slide(const Board &kata, const int posy, const int posx){
    const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
    const int kata_ty = min(kata.height(), h-posy), kata_tx = min(kata.width(), w-posx);
    const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

    for(int i = kata_sy; i < kata_ty; i++){
      auto row = reference(a, i+board_y);
      vector<uchar> picked_pieces;
      vector<bool> ispicked(w);

      // pick
      for(int j = kata_tx-1; j >= kata_sx; j--){
        if(kata[i][j]){
          picked_pieces.push_back(row[j+board_x]);
          ispicked[j+board_x] = true;
        }
      }

      // shift
      int picked_count = 0;
      for(int j = w-1; j >= 0; j--){
        if(ispicked[j]){
          picked_count++;
        }else if(picked_count){
          row[j + picked_count] = row[j];
        }
      }

      // put
      int put_count = 0;
      for(int j = picked_count-1; j >= 0; j--){
        row[j] = picked_pieces[put_count++];
      }
    }
  }
  void up_slide(const Board &kata, const int posy, const int posx){
    const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
    const int kata_ty = min(kata.height(), h-posy), kata_tx = min(kata.width(), w-posx);
    const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

    for(int j = kata_sx; j < kata_tx; j++){
      auto& column = a[j+board_x];
      vector<uchar> picked_pieces;
      vector<int> picked_pos;

      // pick
      for(int i = kata_sy; i < kata_ty; i++){
        if(kata[i][j]){
          picked_pieces.push_back(column[i+board_y]);
          picked_pos.push_back(i+board_y);
        }
      }
      picked_pos.push_back(h);

      // shift
      for(int i = 0; i + 1 < (int)picked_pos.size(); i++){
        if(picked_pos[i] + 1 != picked_pos[i+1]){
          column.rotate(picked_pos[i]-i, picked_pos[i]+1, picked_pos[i+1]);
        }
      }
    }
  }
  void down_slide(const Board &kata, const int posy, const int posx){
    const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
    const int kata_ty = min(kata.height(), h-posy), kata_tx = min(kata.width(), w-posx);
    const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

    for(int j = kata_sx; j < kata_tx; j++){
      auto& column = a[j+board_x];
      vector<uchar> picked_pieces;
      vector<int> picked_pos;

      // pick
      for(int i = kata_ty - 1; i >= kata_sy; i--){
        if(kata[i][j]){
          picked_pieces.push_back(column[i+board_y]);
          picked_pos.push_back(i+board_y);
        }
      }
      picked_pos.push_back(-1);

      // shift
      for(int i = 0; i + 1 < (int)picked_pos.size(); i++){
        column.rotate(picked_pos[i+1]+1, picked_pos[i], picked_pos[i]+i+1);
      }
    }
  }

};
