#include <cmath>
#include <queue>
#include <omp.h>
#include <bitset>
#include "board.hpp"

using namespace std;



void one_slide(vector<vector<int>>& a, const int posy, const int posx, const Dir dir){
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

void teikei_left_slide(vector<vector<int>>& a, const int size, const int type, const int posy, const int posx){
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
      auto& row = a[i+board_y];
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

void teikei_right_slide(vector<vector<int>>& a, const int size, const int type, const int posy, const int posx){
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
      auto& row = a[i+board_y];
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


void reverse_row(vector<vector<int>>& a, const int sy, const int ty, const int sx, const int tx){
  vector<int> tmp(tx - sx);
  for(int i = sy; i < (sy+ty)/2; i++){
    copy(a[i].begin() + sx, a[i].begin() + tx, tmp.begin());
    copy(a[ty+sy-i-1].begin() + sx, a[ty+sy-i-1].begin() + tx, a[i].begin() + sx);
    copy(tmp.begin(), tmp.end(), a[ty+sy-i-1].begin() + sx);
  }
}

void teikei_up_slide(vector<vector<int>>& a, const int size, const int type, const int posy, const int posx){
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

void teikei_down_slide(vector<vector<int>>& a, const int size, const int type, const int posy, const int posx){
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
vector<vector<int>> teikei_slide(const vector<vector<int>>& a, int kata_index, const int posy, const int posx, const Dir dir){
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
    state_now.slide(ops.back());
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
    state_now.slide(ops.back());
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
        state_now.slide(ops.back());
        
        // (i, col) を1x1の抜き型で下寄せ
        ops.push_back({KATA_11, i, col, Dir::D});
        state_now.slide(ops.back());
        return true;
      }else if(j < col){ // 左にある
        // 右から col - j 列を左シフト、列を合わせる
        // -> (i, w - (col - j)) を左上として256x256の抜き型で右寄せ
        ops.push_back({KATA_MM, i, w - (col - j), Dir::R});
        state_now.slide(ops.back());
        
        
        // (i, col) を1x1の抜き型で下寄せ
        ops.push_back({KATA_11, i, col, Dir::D});
        state_now.slide(ops.back());
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
  for(int j = 0; j < w; j++){
    if(state_now[row][j] != need_val) continue;
    if(used[j])continue;
    // j と col の距離
    int dist = abs(j - col);

    // (row, j) を下シフト
    ops.push_back({KATA_11, row, j, Dir::U});
    state_now.slide(ops.back());

    if(j < col){
      ops.push_back({KATA_MM, h - 1, w - dist, Dir::R});
      state_now.slide(ops.back());
    }else{
      ops.push_back({KATA_MM, h - 1, dist - 256, Dir::L});
      state_now.slide(ops.back());
    }

    // (h - 1, col) を1x1の抜き型で上シフト
    ops.push_back({KATA_11, h - 1, col, Dir::D});
    state_now.slide(ops.back());
    return true;
  }
  return false;
}


bool check_all_bottom(const int row, const Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();
  for(int j = 0; j < w; j++){
    bool ok = false;
    for(int i = row; i < h; i++){
      if(state_now[i][j] == state_goal[h-row-1][j]){
        ok = true;
        break;
      }
    }
    if(!ok) return false;
  }
  return true;
}


inline int calc_dp(const int row, int st_j, const Board& state_now, const Board& state_goal, const int ahead_num){
  const int h = state_now.height(), w = state_now.width();

  bitset<256> used[256];
  uchar que[256][256];
  uchar *que_s[256], *que_t[256];

  int dp[256+1], prev[256+1], prev2[256+1];

  int result = 0;

  for(int itr = 0; itr < ahead_num; itr++){
    for(int i = row; i < h; i++){
      que_s[i] = que_t[i] = que[i];
      for(int j = st_j; j < w; j++){
        if(used[i][j] || state_now[i][j] != state_goal[h-row-1-itr][j]) continue;
        if(j == w-1 || used[i][j+1] || state_now[i][j+1] != state_goal[h-row-1-itr][j+1]) *que_t[i]++ = j;
      }
    }

    std::fill(dp + st_j, dp + w+1, 1000000000);
    dp[st_j] = 0;
    prev[st_j] = prev2[st_j] = -1;
    for(int j = st_j; j < w; j++){
      int len = 0;
      for(int i = h-1; i >= row; i--){
        if(used[i][j] || state_now[i][j] != state_goal[h-row-1-itr][j]) continue;
        // const int v = que[i].front() - j + 1;
        const int v = *que_s[i] - j + 1;
        while(len < v){
          len++;
          const int b = 1 << __lg(i-row+1);
          const int val = (i == h-1) ? 0 : (len + b-1) / b;
          if(dp[j + len] > dp[j] + val){
            dp[j + len] = dp[j] + val;
            prev[j + len] = j;
            prev2[j + len] = i;
          }
        }
        while(que_s[i] != que_t[i] && *que_s[i] <= j) que_s[i]++;
      }
      dp[j + 1] = min(dp[j + 1], dp[j] + 100); // 何もない場合
    }
    assert(dp[w] <= 100000000);
    // int j = w;
    // while(j > st_j){
    //   for(int k = prev[j]; k < j; k++){
    //     assert(state_now[prev2[j]][k] == state_goal[h-row-1-itr][k]);
    //     used[prev2[j]][k] = 1;
    //   }
    //   j = prev[j];
    // }
    if(!itr) result += dp[w];
    // else result += dp[w] / 10;
    // result += dp[w] * (4 - itr) * (4 - itr);
    break;
  }
  return result;
  // return dp[w];
}


// z[i]=256となる場合、overflowします
void z_algo(const uchar s[257*257], const int n, uchar z[257*257]){
  z[0] = n < 256 ? n : 255;
  int i = 1, j = 0;
  while(i < n){
    while(i + j < n && s[j] == s[i + j]) j++;
    z[i] = j;
    if(!j){
      i++;
      continue;
    }
    int k = 1;
    while(k < j && z[k] + k < j) z[i + k++] = z[k];
    i += k;
    j -= k;
  }
}

// 盤面の評価関数
double evaluate_board(const int row, const Board& state_now, const Board& state_goal){
  const int h = state_now.height();
  const int w = state_now.width();

  int st_j = 0;
  while(state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

  if(st_j >= w) return -1000000000;

  vector<bitset<256>> used(256);
  vector<uchar> s(257*257), z(257*257);
  int result = -st_j * 500000.0 / w; // 初期でそろっているやつ

  for(int iter = 0; ; iter++){
    const int ulen = w - st_j; // 右下の未確定の場所の長さ
    for(int j = 0; j < ulen; j++){
      s[j] = state_goal[h-row-1][st_j + j];
    }
    s[ulen] = 5; // split
    for(int i = row; i < h-1; i++){
      for(int j = 0; j < w; j++){
        s[(i-row)*(w+1) + ulen+1 + j] = !used[i][j] ? state_now[i][j] : 4;
      }
      s[(i-row)*(w+1) + ulen+1 + w] = 4; // split
    }
    // 右下に存在するピース
    for(int j = 0; j < ulen; j++){
      s[(h-1-row)*(w+1) + ulen+1 + j] = state_now[h-1][st_j + j];
    }

    z_algo(s.data(), (h-1-row)*(w+1) + ulen*2+1, z.data());

    double best_score = 1000000000, second_score = 1000000000;
    int best_y = -1, best_x = -1;

    for(int i = h-2; i >= row; i--){
      for(int j = 0; j < w; j++){
        const int l = z[(i-row)*(w+1) + ulen+1 + j];
        const int num = 1 + (st_j != j);
        if(!l) continue;
        // const int score = -l*l / num;
        const double score = -l*l * 100.0 / num + abs(j - st_j);
        if(best_score > score){
          second_score = best_score;
          best_score = score;
          best_y = i;
          best_x = j;
        }else if(second_score > score) second_score = score;
      }
    }
    // 右下
    for(int j = 0; j < ulen; j++){
      const int l = z[(h-1-row)*(w+1) + ulen+1 + j];
      const int num = 3;
      if(!l) continue;
      // const int score = -l*l / num;
      const double score = -l*l * 100 * pow(0.9, abs(j - st_j));
      if(best_score > score){
        best_score = score;
        best_y = h-1;
        best_x = j; // st_jからの相対位置
      }else if(second_score > score) second_score = score;
    }

    if(best_y == -1) return 1000000000;
    if(best_y == -1) cerr << "\n" << row << " " << iter << "\n";
    assert(best_y != -1);

    const int len = z[(best_y-row)*(w+1) + ulen+1 + best_x];
    for(int j = 0; j < len; j++){
      used[best_y][best_x + j] = 1;
    }
    const bool above = st_j == best_x;
    st_j += len;
    int new_len = len;
    while(state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++, new_len++;

    // iterが大きいほどスコアが反映されにくくなる
    const double pr = pow(0.95, iter) * 100;
    result += (-new_len / (2.0 - above*0.5)) * pr;
    // result += (2 - above) * pr / new_len;
    result += abs(st_j - best_x) * pr / 50;
    // result += second_score * pr / 100 / 100;
    // cerr << best_y << "," << len << "  ";

    // best_y == h-1の時は盤面がずれて評価できない可能性があるので初めで打ち切る
    if(st_j >= w || best_y == h-1) break;
  }

  return result;
}

// 一定以上の長さのピースがあるうちは即採用する
void solve_row_clearly(Operations& ops, const int row, Board& state_now, const Board& state_goal, int limit){
  const int h = state_now.height(), w = state_now.width();

  while(true){
    int st_j = 0;
    while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;
    
    if(st_j == w) return;
    
    uchar s[257*257], z[257*257];
    const int ulen = w - st_j; // 右下の未確定の場所の長さ
    for(int j = 0; j < ulen; j++){
      s[j] = state_goal[h-row-1][st_j + j];
    }
    s[ulen] = 5; // split
    for(int i = row; i < h-1; i++){
      for(int j = 0; j < w; j++){
        s[(i-row)*(w+1) + ulen+1 + j] = state_now[i][j];
      }
      s[(i-row)*(w+1) + ulen+1 + w] = 4; // split
    }

    z_algo(s, (h-1-row)*(w+1) + ulen*2+1, z);
    
    bool moved = false;
    for(int i = row; i < h-1; i++){
      int lr_max_size = h - 1 - row; // 左右移動できる最大の幅
      int lr_kata_id = 0; // lr_max_size 以下最大の抜き型
      if(lr_max_size > 1){
        lr_kata_id = 1;
        lr_max_size /= 2;
        while(lr_max_size > 1){
          lr_kata_id += 3;
          lr_max_size /= 2;
        }
      }
      int lr_kata_size = cutting_dies[lr_kata_id].height();

      int u_max_size = i - row + 1; // 上下移動できる最大の幅
      int u_kata_id = 0; // u_max_size 以下最大の抜き型
      if(u_max_size > 1){
        u_kata_id = 1;
        u_max_size /= 2;
        while(u_max_size > 1){
          u_kata_id += 3;
          u_max_size /= 2;
        }
      }
      int u_kata_size = cutting_dies[u_kata_id].height();

      for(int j = 0; j < w; j++){
        int step = 1 + (j != st_j); // かかる手数
        int len  = min((int)z[(i-row)*(w+1) + ulen+1 + j], u_kata_size); // 長さ
        if(len / step > limit){ // 十分大きなピース群を発見
          int nuki_row = (i + lr_kata_size <= h - 1 ? i : row); // 左右の抜き型を使う行
          if(abs(j - st_j) > lr_kata_size){
            continue;
          }
          if(j < st_j){
            // 右から st_j - j 行を左へもっていく
            ops.push_back({lr_kata_id, nuki_row, w - abs(st_j - j), Dir::R});
            state_now.slide(ops.back());
          }else if(st_j < j){
            // 左から j - st_j 行を右へもっていく
            ops.push_back({lr_kata_id, nuki_row, abs(st_j - j) - lr_kata_size, Dir::L});
            state_now.slide(ops.back());
          }
          ops.push_back({u_kata_id, i + 1 - u_kata_size, st_j, Dir::U});
          state_now.slide(ops.back());
          moved = true;
          break;
        }
      }
      if(moved) break;
    }
    if(!moved) return; // 動かせなかったとき
  }
}

void solve_row_effi_roughly(Operations& ops, const int row, Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();

  int prev_score = -1;
  while(true){
    int st_j = 0;
    while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

    solve_row_clearly(ops, row, state_now, state_goal, 3 * (w - st_j) / w);

    while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

    if(st_j >= w) break;

    int best_score = 1000000000;
    Operation best(1U << 31);

    // move none
    best_score = evaluate_board(row, state_now, state_goal);

    Operation best_op[256*2+1];
    int best_scores[256*2+1];
    fill(best_scores, best_scores + 256*2+1, 1000000000);

    #pragma omp parallel for
    for(int j = 0; j < w*2+1; j++){
      Board *tmp_state = new Board(0, 0); // 盤面を愚直に持つとstackoverflowした

      for(int i = row; i < h; i++){
        for(int k = 0; k < (int)cutting_dies.size(); k++){
          const int sy = i;
          const int sx = j - (w-1);
          if(sx + cutting_dies[k].width() <= 0) continue; // 左に行きすぎ
          // if(sx < st_j && sy + cutting_dies[k].height() - ((k > 0 && (k-1) % 3 == 1) ? 1 : 0) >= h-1) continue;
          for(const Dir dir : { Dir::L, Dir::R, Dir::U }){
            // auto tmp_state = state_now;
            // tmp_state.slide(cutting_dies[k], sy, sx, dir);
            // const int score = evaluate_board(row, tmp_state, state_goal);
            *tmp_state = state_now;
            tmp_state->slide(cutting_dies[k], sy, sx, dir);
            const int score = evaluate_board(row, *tmp_state, state_goal);
            if(score < best_scores[j]){
              best_scores[j] = score;
              best_op[j] = Operation(k, sy, sx, dir);
            }
          }
        }
      }
      delete tmp_state;
    }

    for(int j = 0; j < w*2+1; j++){
      if(best_scores[j] < best_score){
        best_score = best_scores[j];
        best = best_op[j];
      }
    }

    assert(best_score < 1000000000);

    // 前回と同じスコアの場合、改善していないのでエラー
    if(best != Operation(1U << 31) && prev_score != best_score){
      ops.push_back(best);
      state_now.slide(best);
      prev_score = best_score;
    }else{
      cerr << "\n";
      cerr << "row: " << row << "\n";
      cerr << "current\n";
      state_now.debug();
      cerr << "\n";
      cerr << "best slided\n";
      state_now.slide(best);
      state_now.debug();
      cerr << "\n";
      cerr << "goal row\n";
      for(int j = 0; j < w; j++) cout << (int)state_goal[h-row-1][j];
      cerr << "\n";
      assert(false);
    }
    cerr << best_score << "(" << ops.size() << ") ";
  }

  for(int j = 0; j < w; j++){
    assert(state_now[h-1][j] == state_goal[h-row-1][j]);
  }
  ops.push_back({KATA_MM, h - 1, 0, Dir::D});
  state_now.slide(ops.back());
  cerr << row << "finished, " << ops.size() << "\n";
}



void solve_row(vector<Operation>& ops, int row, Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();
  vector<bool> used(w);
  if(row == h - 1){
    for(int i = 0; i < w;i++){
      for(int j = i; j < w;j++){
        if(state_goal[0][w - 1 - i] == state_now[row][j]){
          // (row, j) を左シフト
          ops.push_back({KATA_11, row, j, Dir::R});
          state_now.slide(ops.back());
          break;
        }
      }
    }
    ops.push_back({KATA_MM, h - 1, 0, Dir::D});
    state_now.slide(ops.back());
    return;
  }

  solve_row_effi_roughly(ops, row, state_now, state_goal);
  return;

  int cnt = 0;
  while(cnt < w){
    for(int i = 0; i + 1 < w;i++){
      if(used[i] || used[i + 1])continue;
      if(solve_pos_a2(ops, row, i, state_now, state_goal)){
        used[i] = used[i + 1] = true;
        cnt += 2;
      }
    }
    for(int i = 0; i < w;i++){
      if(used[i])continue;
      if(solve_pos_a(ops, row, i, state_now, state_goal)){
        used[i] = true;
        cnt++;
      }
    }
    // 下側を回転させたかどうか
    bool isrot = false;
    for(int i = 0; i < w;i++){
      if(used[i])continue;
      if(solve_pos_b(ops, row, i, state_now, state_goal)){
        used[i] = true;
        isrot = true;
        cnt++;
        break;
      }
    }
    if(isrot)continue;
    for(int i = 0; i < w;i++){
      if(used[i])continue;
      if(solve_pos_c(ops, row, i, state_now, state_goal, used)){
        used[i] = true;
        cnt++;
      }
    }
  }
}
Operations solve(const Board& state_start, const Board& state_goal){
  const int h = state_start.height();
  // 操作 (p, x, y, s)
  Operations ops;
  // 現在の盤面
  Board state_now = state_start;
  for(int i = 0; i < h; i++){
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
        kata[i][j] ^= (i + j)&  1;
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
  cout << ops.size() << "\n";
  Board state_now = state_start;
  for(const Operation op : ops){
    cout << op << "\n";
    state_now.slide(op);
  }
  cout << endl;

  // チェック
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      assert(state_now[i][j] == state_goal[i][j]);
    }
  }
  return 0;
}
