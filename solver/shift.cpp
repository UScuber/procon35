#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <tuple>
#include "board.hpp"

using namespace std;


vector<Board> cutting_dies; // 抜き型



void one_slide(vector<vector<int>> &a, const int posy, const int posx, const Dir dir){
  const int h = a.size();

  const int piece = a[posy][posx];

  if(dir == Dir::U){
    for(int i = posy; i < h-1; i++){
      a[i][posx] = a[i+1][posx];
    }
    a[h-1][posx] = piece;
  }
  if(dir == Dir::L){
    rotate(
      a[posy].begin() + posx,
      a[posy].begin() + posx+1,
      a[posy].end()
    );
  }
  if(dir == Dir::D){
    for(int i = posy-1; i >= 0; i--){
      a[i+1][posx] = a[i][posx];
    }
    a[0][posx] = piece;
  }
  if(dir == Dir::R){
    rotate(
      a[posy].begin(),
      a[posy].begin() + posx,
      a[posy].begin() + posx+1
    );
  }
}

void teikei_left_slide(vector<vector<int>> &a, const int size, const int type, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min(size, h-posy), kata_tx = min(size, w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  if(type == 0){
    for(int i = kata_sy; i < kata_ty; i++){
      rotate(
        a[i+board_y].begin() + kata_sx+board_x,
        a[i+board_y].begin() + kata_tx+board_x,
        a[i+board_y].end()
      );
    }
  }

  // 0-indexedで偶数が抜く行
  if(type == 1){
    for(int i = (kata_sy+1)/2*2; i < kata_ty; i += 2){
      rotate(
        a[i+board_y].begin() + kata_sx+board_x,
        a[i+board_y].begin() + kata_tx+board_x,
        a[i+board_y].end()
      );
    }
  }

  // 0-indexedで偶数が抜く列
  if(type == 2){
    for(int i = kata_sy; i < kata_ty; i++){
      auto &row = a[i+board_y];
      vector<int> picked_pieces;

      // pick
      for(int j = (kata_sx+1)/2*2; j < kata_tx; j += 2){
        picked_pieces.push_back(row[j+board_x]);
        row[j+board_x] = -1;
      }

      // shift
      int picked_count = 0;
      for(int j = kata_sx+board_x; j < w; j++){
        if(row[j] < 0){
          picked_count++;
        }else if(picked_count){
          row[j - picked_count] = row[j];
          row[j] = -1;
        }
      }

      // put
      copy(picked_pieces.begin(), picked_pieces.end(), row.end() - picked_pieces.size());
      // int put_count = 0;
      // for(int j = 0; j < w; j++){
      //   if(row[j] < 0) row[j] = picked_pieces[put_count++];
      // }
    }
  }
}

void teikei_right_slide(vector<vector<int>> &a, const int size, const int type, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min(size, h-posy), kata_tx = min(size, w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  if(type == 0){
    for(int i = kata_sy; i < kata_ty; i++){
      rotate(
        a[i+board_y].begin(),
        a[i+board_y].begin() + kata_sx+board_x,
        a[i+board_y].begin() + kata_tx+board_x
      );
    }
  }

  // 0-indexedで偶数が抜く行
  if(type == 1){
    for(int i = (kata_sy+1)/2*2; i < kata_ty; i += 2){
      rotate(
        a[i+board_y].begin(),
        a[i+board_y].begin() + kata_sx+board_x,
        a[i+board_y].begin() + kata_tx+board_x
      );
    }
  }

  // 0-indexedで偶数が抜く列
  if(type == 2){
    for(int i = kata_sy; i < kata_ty; i++){
      auto &row = a[i+board_y];
      vector<int> picked_pieces;

      // pick
      for(int j = (kata_tx-1)/2*2; j >= kata_sx; j -= 2){
        picked_pieces.push_back(row[j+board_x]);
        row[j+board_x] = -1;
      }

      // shift
      int picked_count = 0;
      for(int j = kata_tx+board_x-1; j >= 0; j--){
        if(row[j] < 0){
          picked_count++;
        }else if(picked_count){
          row[j + picked_count] = row[j];
          row[j] = -1;
        }
      }

      // put
      // copy(picked_pieces.begin(), picked_pieces.end(), row.begin());
      int put_count = 0;
      for(int j = (int)picked_pieces.size()-1; j >= 0; j--){
        if(row[j] < 0) row[j] = picked_pieces[put_count++];
      }
    }
  }
}


void reverse_row(vector<vector<int>> &a, const int sy, const int ty, const int sx, const int tx){
  vector<int> tmp(tx - sx);
  for(int i = sy; i < (sy+ty)/2; i++){
    copy(a[i].begin() + sx, a[i].begin() + tx, tmp.begin());
    copy(a[ty+sy-i-1].begin() + sx, a[ty+sy-i-1].begin() + tx, a[i].begin() + sx);
    copy(tmp.begin(), tmp.end(), a[ty+sy-i-1].begin() + sx);
  }
}

void teikei_up_slide(vector<vector<int>> &a, const int size, const int type, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min(size, h-posy), kata_tx = min(size, w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  if(type == 0){
    // rotate(kata_sy+board_y, kata_ty+board_y, h)
    reverse_row(a, kata_sy+board_y, kata_ty+board_y, kata_sx+board_x, kata_tx+board_x);
    reverse_row(a, kata_ty+board_y, h, kata_sx+board_x, kata_tx+board_x);
    reverse_row(a, kata_sy+board_y, h, kata_sx+board_x, kata_tx+board_x);
  }

  // 0-indexedで偶数が抜く行
  if(type == 1){
    for(int j = kata_sx; j < kata_tx; j++){
      vector<int> picked_pieces;

      // pick
      for(int i = (kata_sy+1)/2*2; i < kata_ty; i += 2){
        picked_pieces.push_back(a[i+board_y][j+board_x]);
        a[i+board_y][j+board_x] = -1;
      }

      // shift
      int picked_count = 0;
      for(int i = kata_sy+board_y; i < h; i++){
        if(a[i][j+board_x] < 0){
          picked_count++;
        }else if(picked_count){
          a[i - picked_count][j+board_x] = a[i][j+board_x];
          a[i][j+board_x] = -1;
        }
      }

      // put
      int put_count = 0;
      for(int i = h-(int)picked_pieces.size(); i < h; i++){
        if(a[i][j+board_x] < 0) a[i][j+board_x] = picked_pieces[put_count++];
      }
    }
  }

  // 0-indexedで偶数が抜く列
  if(type == 2){
    for(int j = (kata_sx+1)/2*2; j < kata_tx; j += 2){
      vector<int> picked_pieces;

      // pick
      for(int i = kata_sy; i < kata_ty; i++){
        picked_pieces.push_back(a[i+board_y][j+board_x]);
        a[i+board_y][j+board_x] = -1;
      }

      // shift
      int picked_count = 0;
      for(int i = kata_sy+board_y; i < h; i++){
        if(a[i][j+board_x] < 0){
          picked_count++;
        }else if(picked_count){
          a[i - picked_count][j+board_x] = a[i][j+board_x];
          a[i][j+board_x] = -1;
        }
      }

      // put
      int put_count = 0;
      for(int i = h-(int)picked_pieces.size(); i < h; i++){
        if(a[i][j+board_x] < 0) a[i][j+board_x] = picked_pieces[put_count++];
      }
    }
  }
}

void teikei_down_slide(vector<vector<int>> &a, const int size, const int type, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min(size, h-posy), kata_tx = min(size, w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  if(type == 0){
    // rotate(0, kata_sy+board_y, kata_ty+board_y)
    reverse_row(a, 0, kata_sy+board_y, kata_sx+board_x, kata_tx+board_x);
    reverse_row(a, kata_sy+board_y, kata_ty+board_y, kata_sx+board_x, kata_tx+board_x);
    reverse_row(a, 0, kata_ty+board_y, kata_sx+board_x, kata_tx+board_x);
  }

  // 0-indexedで偶数が抜く行
  if(type == 1){
    for(int j = kata_sx; j < kata_tx; j++){
      vector<int> picked_pieces;

      // pick
      for(int i = (kata_ty-1)/2*2; i >= kata_sy; i -= 2){
        picked_pieces.push_back(a[i+board_y][j+board_x]);
        a[i+board_y][j+board_x] = -1;
      }

      // shift
      int picked_count = 0;
      for(int i = kata_ty+board_y-1; i >= 0; i--){
        if(a[i][j+board_x] < 0){
          picked_count++;
        }else if(picked_count){
          a[i + picked_count][j+board_x] = a[i][j+board_x];
          a[i][j+board_x] = -1;
        }
      }

      // put
      int put_count = 0;
      for(int i = (int)picked_pieces.size()-1; i >= 0; i--){
        if(a[i][j+board_x] < 0) a[i][j+board_x] = picked_pieces[put_count++];
      }
    }
  }

  // 0-indexedで偶数が抜く列
  if(type == 2){
    for(int j = (kata_sx+1)/2*2; j < kata_tx; j += 2){
      vector<int> picked_pieces;

      // pick
      for(int i = kata_ty-1; i >= kata_sy; i--){
        picked_pieces.push_back(a[i+board_y][j+board_x]);
        a[i+board_y][j+board_x] = -1;
      }

      // shift
      int picked_count = 0;
      for(int i = kata_ty+board_y-1; i >= 0; i--){
        if(a[i][j+board_x] < 0){
          picked_count++;
        }else if(picked_count){
          a[i + picked_count][j+board_x] = a[i][j+board_x];
          a[i][j+board_x] = -1;
        }
      }

      // put
      int put_count = 0;
      for(int i = (int)picked_pieces.size()-1; i >= 0; i--){
        if(a[i][j+board_x] < 0) a[i][j+board_x] = picked_pieces[put_count++];
      }
    }
  }
}

// 定型抜き型専用のスライド
vector<vector<int>> teikei_slide(const vector<vector<int>> &a, int kata_index, const int posy, const int posx, const Dir dir){
  const int h = a.size();
  const int w = a[0].size();
  assert(0 <= kata_index && kata_index < 25);

  vector<vector<int>> result = a;

  // 1x1
  if(kata_index == 0){
    assert(0 <= posy && posy < h && 0 <= posx && posx < w);
    one_slide(result, posy, posx, dir);
    return result;
  }

  kata_index--;
  const int size = 1 << (kata_index / 3 + 1);
  const int type = kata_index % 3;

  assert(-size < posy && posy < h && -size < posx && posx < w);
  
  if(dir == Dir::U) teikei_up_slide(result, size, type, posy, posx);
  if(dir == Dir::L) teikei_left_slide(result, size, type, posy, posx);
  if(dir == Dir::D) teikei_down_slide(result, size, type, posy, posx);
  if(dir == Dir::R) teikei_right_slide(result, size, type, posy, posx);

  return result;
}



void print_state(const Board& a){
  const int h = a.height(), w = a.width();
  for(int i = 0;i < h;i++){
    for(int j = 0;j < w;j++){
      cout << (int)a[i][j];
    }
    cout << "\n";
  }
  cout << endl;
}


#define KATA_MM 22 // 256x256
#define KATA_11 0 // 1x1
#define KATA_12 2 // 1x2 (縦 1, 横 2)

// 同じ列から探索
bool solve_pos_a(vector<Operation>& ops, int row, int col, Board& state_now, const Board& state_goal){
  const int h = state_now.height();
  int need_val = state_goal[h - 1 - row][col];
  for(int i = row; i < h; i++){
    if(state_now[i][col] != need_val) continue;
    // (i, col) を1x1の抜き型で下寄せ
    ops.push_back({KATA_11, i, col, Dir::D});
    state_now.slide(cutting_dies[KATA_11], i, col, Dir::D);
    return true;
  }
  return false;
}
// 同じ列から探索 1x2
bool solve_pos_a2(vector<Operation>& ops, int row, int col, Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();
  if(col + 1 >= w) return false;
  int need_val[2] = {state_goal[h - 1 - row][col], state_goal[h - 1 - row][col + 1]};
  for(int i = row; i < h; i++){
    if(state_now[i][col + 0] != need_val[0]) continue;
    if(state_now[i][col + 1] != need_val[1]) continue;
    // (i, col) を1x2の抜き型で下寄せ
    ops.push_back({KATA_12, i, col, Dir::D});
    state_now.slide(cutting_dies[KATA_12], i, col, Dir::D);
    return true;
  }
  return false;
}
// 未確定の場所から探索
bool solve_pos_b(vector<Operation>& ops, int row, int col, Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();
  int need_val = state_goal[h - 1 - row][col];
  for(int i = row + 1; i < h; i++){
    for(int j = 0; j < w; j++){
      if(state_now[i][j] != need_val) continue;
      if(col < j){ // 右にある
        // 左から j - col 列を右シフト、列を合わせる
        // -> (i, j - col - 1)   を右上として256x256の抜き型で左寄せ
        // -> (i, j - col - 256) を左上として256x256の抜き型で左寄せ
        ops.push_back({KATA_MM, i, j - col - 256, Dir::L});
        state_now.slide(cutting_dies[KATA_MM], i, j - col - 256, Dir::L);
        
        // (i, col) を1x1の抜き型で下寄せ
        ops.push_back({KATA_11, i, col, Dir::D});
        state_now.slide(cutting_dies[KATA_11], i, col, Dir::D);
        return true;
      }else if(j < col){ // 左にある
        // 右から col - j 列を左シフト、列を合わせる
        // -> (i, w - (col - j)) を左上として256x256の抜き型で右寄せ
        ops.push_back({KATA_MM, i, w - (col - j), Dir::R});
        state_now.slide(cutting_dies[KATA_MM], i, w - (col - j), Dir::R);
        
        
        // (i, col) を1x1の抜き型で下寄せ
        ops.push_back({KATA_11, i, col, Dir::D});
        state_now.slide(cutting_dies[KATA_11], i, col, Dir::D);
        return true;
      }
    }
  }
  return false;
}
// 未確定の (row, j) を探索
bool solve_pos_c(vector<Operation>& ops, int row, int col, Board& state_now, const Board& state_goal, const vector<bool>& used){
  const int h = state_now.height(), w = state_now.width();
  int need_val = state_goal[h - 1 - row][col];
  // (row, col) の左側
  for(int j = 0;j < w;j++){
    if(state_now[row][j] != need_val) continue;
    if(used[j])continue;
    // j と col の距離
    int dist = abs(j - col);

    // (row, j) を下シフト
    ops.push_back({KATA_11, row, j, Dir::U});
    state_now.slide(cutting_dies[KATA_11], row, j, Dir::U);

    if(j < col){
      ops.push_back({KATA_MM, h - 1, w - dist, Dir::R});
      state_now.slide(cutting_dies[KATA_MM], h - 1, w - dist, Dir::R);
    }else{
      ops.push_back({KATA_MM, h - 1, dist - 256, Dir::L});
      state_now.slide(cutting_dies[KATA_MM], h - 1, dist - 256, Dir::R);
    }

    // (h - 1, col) を1x1の抜き型で上シフト
    ops.push_back({KATA_11, h - 1, col, Dir::D});
    state_now.slide(cutting_dies[KATA_11], h - 1, col, Dir::D);
    return true;
  }
  return false;
}
void solve_row(vector<Operation>& ops, int row, Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();
  vector<bool> used(w);
  if(row == h - 1){
    for(int i = 0;i < w;i++){
      for(int j = i;j < w;j++){
        if(state_goal[0][w - 1 - i] == state_now[row][j]){
          // (row, j) を左シフト
          ops.push_back({KATA_11, row, j, Dir::R});
          state_now.slide(cutting_dies[KATA_11], row, j, Dir::R);
          break;
        }
      }
    }
    ops.push_back({KATA_MM, h - 1, 0, Dir::D});
    state_now.slide(cutting_dies[KATA_MM], h - 1, 0, Dir::D);
    return;
  }
  int cnt = 0;
  while(cnt < w){
    for(int i = 0;i + 1 < w;i++){
      if(used[i] || used[i + 1])continue;
      if(solve_pos_a2(ops, row, i, state_now, state_goal)){
        used[i] = used[i + 1] = true;
        cnt += 2;
      }
    }
    for(int i = 0;i < w;i++){
      if(used[i])continue;
      if(solve_pos_a(ops, row, i, state_now, state_goal)){
        used[i] = true;
        cnt++;
      }
    }
    // 下側を回転させたかどうか
    bool isrot = false;
    for(int i = 0;i < w;i++){
      if(used[i])continue;
      if(solve_pos_b(ops, row, i, state_now, state_goal)){
        used[i] = true;
        isrot = true;
        cnt++;
        break;
      }
    }
    if(isrot)continue;
    for(int i = 0;i < w;i++){
      if(used[i])continue;
      if(solve_pos_c(ops, row, i, state_now, state_goal, used)){
        used[i] = true;
        cnt++;
      }
    }
  }
}
Operations solve(const Board &state_start, const Board &state_goal){
  const int h = state_start.height();
  // 操作 (p, x, y, s)
  Operations ops;
  // 現在の盤面
  Board state_now = state_start;
  for(int i = 0;i < h; i++){
    solve_row(ops, i, state_now, state_goal);
  }
  return ops;
}
int main(){
  // 定型抜き型作成
  cutting_dies.push_back(vector<vector<int>>({{ 1 }})); // 1x1
  for(int i = 1; i <= 8; i++){
    const int n = 1 << i;
    vector<vector<int>> kata(n, vector<int>(n, 1));
    cutting_dies.emplace_back(kata);
    for(int i = 1; i < n; i += 2){
      kata[i] = vector<int>(n, 0);
    }
    cutting_dies.emplace_back(kata);
    for(int i = 0; i < n; i++){
      for(int j = 0; j < n; j++){
        kata[i][j] ^= (i + j) & 1;
      }
    }
    cutting_dies.emplace_back(kata);
  }


  // 盤面入力
  int h,w;
  cin >> h >> w;
  Board state_start(h, w);
  for(int i = 0; i < h; i++){
    string s; cin >> s;
    for(int j = 0; j < w; j++){
      state_start[i][j] = s[j] - '0';
    }
  }
  Board state_goal(h, w);
  for(int i = 0; i < h; i++){
    string s; cin >> s;
    for(int j = 0; j < w; j++){
      state_goal[i][j] = s[j] - '0';
    }
  }
  
  // 手順を計算
  auto ops = solve(state_start, state_goal);

  // 出力
  // cout << "start : \n";
  // print_state(state_start);
  Board state_now = state_start;
  for(const Operation op : ops){
    const int s = op.kata_idx();
    const int y = op.y();
    const int x = op.x();
    const Dir d = op.dir();
    cout << op << endl;
    state_now.slide(cutting_dies[s], y, x, d);
    // print_state(state_now);
  }
  // cout << "expect : \n";
  // print_state(state_goal);

  // チェック
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      assert(state_now[i][j] == state_goal[i][j]);
    }
  }
  return 0;
}
