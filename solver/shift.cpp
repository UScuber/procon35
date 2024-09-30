#include <cmath>
#include <queue>
#include <omp.h>
#include <bitset>
#include "board.hpp"

using namespace std;


#define KATA_MM 22 // 256x256
#define KATA_11 0 // 1x1
#define KATA_12 2 // 1x2 (縦 1, 横 2)



// スライドと更新を一度にする
void slide_and_output(const Operation& op, Operations& ops, Board& state_now){
  ops.push_back(op);
  state_now.slide(op);
  cout << op << endl;
}


// 同じ列から探索
bool solve_pos_a(vector<Operation>& ops, int row, int col, Board& state_now, const Board& state_goal){
  const int h = state_now.height();
  int need_val = state_goal[h - 1 - row][col];
  for(int i = row; i < h; i++){
    if(state_now[i][col] != need_val) continue;
    // (i, col) を1x1の抜き型で下寄せ
    slide_and_output({KATA_11, i, col, Dir::D}, ops, state_now);
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
    slide_and_output({KATA_12, i, col, Dir::D}, ops, state_now);

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
        slide_and_output({KATA_MM, i, j - col - 256, Dir::L}, ops, state_now);
        
        // (i, col) を1x1の抜き型で下寄せ
        slide_and_output({KATA_11, i, col, Dir::D}, ops, state_now);
        return true;
      }else if(j < col){ // 左にある
        // 右から col - j 列を左シフト、列を合わせる
        // -> (i, w - (col - j)) を左上として256x256の抜き型で右寄せ
        slide_and_output({KATA_MM, i, w - (col - j), Dir::R}, ops, state_now);        
        
        // (i, col) を1x1の抜き型で下寄せ
        slide_and_output({KATA_11, i, col, Dir::D}, ops, state_now);
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
    slide_and_output({KATA_11, row, j, Dir::U}, ops, state_now);

    if(j < col){
      slide_and_output({KATA_MM, h - 1, w - dist, Dir::R}, ops, state_now);
    }else{
      slide_and_output({KATA_MM, h - 1, dist - 256, Dir::L}, ops, state_now);
    }

    // (h - 1, col) を1x1の抜き型で上シフト
    slide_and_output({KATA_11, h - 1, col, Dir::D}, ops, state_now);
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


// 左右寄せの操作のうち、最もピースが長くつながるような操作を求める
pair<Operation, int> search_best_connection(const int row, const Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();
  constexpr int L = 5;

  int st_j = 0;
  while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

  const int lr_max_size = h - 1 - row; // 左右移動できる最大の幅
  int lr_kata_size = 1;
  while(lr_kata_size*2 <= lr_max_size) lr_kata_size *= 2;

  vector<uchar> s(257*257), tmp(257*257);
  vector<uchar> Zr(257*257), Zbr(257*257); // Zbr: 1個飛ばし
  const int max_ulen = w - st_j; // 右下の未確定の場所の長さ

  int best_len = 0, best_score = -1000000000;
  Operation best_op(1U << 31);

  for(int l = 0; l <= L; l++){
    const int ulen = max_ulen - l;
    if(ulen <= 0) break;

    unsigned int l_pieces = 0; // 右下の[0,l)のピース(2bit*l) 上位bitが後のindex
    for(int j = 0; j < l; j++){
      l_pieces |= (unsigned int)state_goal[h-row-1][st_j + j] << (j * 2);
    }

    // Zalgo
    for(int j = 0; j < ulen; j++){
      s[j] = state_goal[h-row-1][st_j + l + j];
    }
    s[ulen] = 5; // split

    // 右下にあるピースは見ていない
    for(int i = row; i < h-1; i++){
      for(int j = 0; j < w; j++){
        s[(i-row)*(w+1) + ulen+1 + j] = state_now[i][j];
      }
      s[(i-row)*(w+1) + ulen+1 + w] = 4; // split
    }
    z_algo(s.data(), (h-1-row)*(w+1) + ulen+1, Zr.data());
    // cerr << st_j << "\n";
    // for(int j = 0; j < ulen+1; j++) cerr << min((int)Zr[j], 9);
    // cerr << "\n";
    // for(int i = row; i < h-1; i++){
    //   for(int j = 0; j <= w; j++) cerr << min((int)Zr[(i-row)*(w+1) + j + ulen+1], 9);
    //   cerr << "\n";
    // }
    // cerr << "\n";
    // for(int j = 0; j < ulen+1; j++) cerr << min((int)s[j], 9);
    // cerr << "\n";
    // for(int i = row; i < h-1; i++){
    //   for(int j = 0; j <= w; j++) cerr << min((int)s[(i-row)*(w+1) + j + ulen+1], 9);
    //   cerr << "\n";
    // }
    // cerr << "\n";
    // 右下のピースの値の部分を取り除く
    for(int i = 0; i < (h-1-row)*(w+1); i++) Zr[i] = Zr[i + ulen+1];

    // 1個飛ばし(偶数) 0-indexed
    const int even_width = (w + 1) / 2;
    for(int i = row; i < h-1; i++){
      for(int j = 0; j < w; j += 2){
        s[(i-row)*(even_width+1) + ulen+1 + j/2] = state_now[i][j];
      }
      s[(i-row)*(even_width+1) + ulen+1 + even_width] = 4; // split
    }
    z_algo(s.data(), (h-1-row)*(even_width+1) + ulen+1, tmp.data());
    // Zbrにセット
    for(int i = row; i < h-1; i++){
      for(int j = 0; j < w; j += 2){
        Zbr[(i-row)*(w+1) + j] = tmp[(i-row)*(even_width+1) + ulen+1 + j/2];
      }
    }

    // 1個飛ばし(奇数)
    const int odd_width = w / 2;
    for(int i = row; i < h-1; i++){
      for(int j = 1; j < w; j += 2){
        s[(i-row)*(odd_width+1) + ulen+1 + j/2] = state_now[i][j];
      }
      s[(i-row)*(odd_width+1) + ulen+1 + odd_width] = 4; // split
    }
    z_algo(s.data(), (h-1-row)*(odd_width+1) + ulen+1, tmp.data());
    // Zbrにセット
    for(int i = row; i < h-1; i++){
      for(int j = 1; j < w; j += 2){
        Zbr[(i-row)*(w+1) + j] = tmp[(i-row)*(odd_width+1) + ulen+1 + j/2];
      }
    }


    // search
    vector<Operation> good_ops(h-1-row, Operation(1U << 31));
    vector<int> good_lens(h-1-row), good_scores(h-1-row, -1000000000);

    // #pragma omp parallel for
    for(int i = row; i < h-1; i++){
      auto update_fn = [&](int len, const int left_pos, const int y, const int x, const int n, const bool isb, const Dir dir){
        len += l;
        // if(left_pos == st_j) len += len / 2;
        if(good_lens[i-row] >= len) return;
        const int dist = abs(st_j - left_pos);
        int score = len*10;
        if(lr_kata_size < dist) score -= dist - lr_kata_size;
        if(good_scores[i-row] >= score) return;
        good_lens[i-row] = len;
        good_scores[i-row] = score;
        const int kata = (n >= 1 ? 1 + (n-1)*3 : 0) + isb*2;
        good_ops[i-row] = Operation(kata, y, x, dir);
      };

      for(int n = 0; n <= 7; n++){
        const int size = 1 << n;
        unsigned int pieces_j = 0, pieces_w = 0, pieces_jn = 0;

        for(int j = 0; j < l; j++){
          pieces_w |= (unsigned int)state_now[i][w-(l-j)] << (j * 2);
        }
        if(l <= size) for(int j = 0; j < l; j++){
          pieces_jn |= (unsigned int)state_now[i][size-(l-j)] << (j * 2);
        }

        for(int j = 0; j <= w - size; j++){
          // row[j-l, j)が一致
          // 右寄せ 13
          if(pieces_j == l_pieces && l <= j){
            const int len = min((int)Zr[(i-row)*(w+1) + j + size], w - (j + size));
            update_fn(len, j+size-l, i, j, n, false, Dir::R);
          }
          // row[j+size-l, j+size)が一致
          // 右寄せ 21
          if(pieces_jn == l_pieces && l <= size){
            const int len = min((int)Zr[(i-row)*(w+1) + 0], j);
            update_fn(len, size-l, i, j, n, false, Dir::R);
          }
          // row[j-l, j)が一致
          // 左寄せ 13
          if(pieces_j == l_pieces && l <= j){
            const int len = min((int)Zr[(i-row)*(w+1) + j + size], w - (j + size));
            update_fn(len, j-l, i, j, n, false, Dir::L);
          }
          // row[w-l, w)が一致
          // 左寄せ 32
          if(pieces_w == l_pieces && l <= w-size-j){
            const int len = min((int)Zr[(i-row)*(w+1) + j], size);
            update_fn(len, w-size-l, i, j, n, false, Dir::L);
          }

          pieces_j >>= 2;
          pieces_j |= (unsigned int)state_now[i][j] << ((l-1) * 2);
          if(j + size < w){
            pieces_jn >>= 2;
            pieces_jn |= (unsigned int)state_now[i][j + size] << ((l-1) * 2);
          }
        }
      }
    }
    for(int i = 0; i < h-1-row; i++){
      if(best_score < good_scores[i]){
        best_score = good_scores[i];
        best_len = good_lens[i];
        best_op = good_ops[i];
      }
    }
  }
  
  static int count = 0;
  cerr << "len: " << best_len << "\n";
  // count++;
  // if(best_len >= 0 && count <= 3){
  //   best_op.debug();
  //   cerr << "\n";
  //   cerr << "row: " << row << ", stj: " << st_j << "\n";
  //   // state_now.debug();
  //   // cerr << "\n";
  //   for(int j = 0; j < best_op.x(); j++) cerr << (int)state_now[best_op.y()][j];
  //   cerr << "|";
  //   for(int j = best_op.x(); j < w; j++) cerr << (int)state_now[best_op.y()][j];
  //   cerr << "\n";
  //   cerr << "\n";
  //   for(int j = 0; j < st_j; j++) cerr << (int)state_goal[h-row-1][j];
  //   cerr << "|";
  //   for(int j = st_j; j < w; j++) cerr << (int)state_goal[h-row-1][j];
  //   cerr << "\n";
  //   cerr << "\n";
  //   for(int j = 0; j < w; j++) cerr << (int)Zr[(best_op.y()-row)*(w+1) + j];
  //   cerr << "\n";
  //   // if(count >= 3) exit(0);
  // }

  // assert(best_op != Operation(1U << 31));

  return { best_op, best_len };
}


// 揃えたいピースが右下にしかない場合、適当に上に持ってくる
void check_unique_piece_move(Operations& ops, const int row, Board& state_now, const Board& state_goal){
  const int h = state_now.height(), w = state_now.width();

  int st_j = 0;
  while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

  if(st_j >= w) return;

  bool exist[4] = {};
  for(int i = row; i < h-1; i++){
    for(int j = 0; j < w; j++){
      exist[state_now[i][j]] = true;
    }
  }
  bool must_move = false;
  for(int j = st_j; j < w; j++){
    if(!exist[state_goal[h-row-1][j]]){
      must_move = true;
      break;
    }
  }
  if(!must_move) return;

  // 1行上に上げる(1行飛びの抜き型)
  slide_and_output({23, h-2, st_j, Dir::U}, ops, state_now);

  cerr << "RB piece moved(" << ops.size() << ")\n";
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

    z_algo(s, (h-1-row)*(w+1) + ulen+1, z);
    
    bool moved = false;
    // 今ある中で一番長いものを探す
    int max_len = 0; // 盤面を考慮した最大のピース群
    // int pure_max_len = 0, pure_pos_y = -1; // 存在する最大のピース群(2手で揃えられない可能性)
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
        step = 1;
        int len = min((int)z[(i-row)*(w+1) + ulen+1 + j], u_kata_size); // 長さ
        if(len / step >= limit || true){ // 十分大きなピース群を発見
          if(abs(j - st_j) > lr_kata_size && (!((h-1 - i) & 1) || j == st_j)){
            continue;
          }
          // if(len / step > max_len) max_len = len / step;
          if(len*2 + (j == st_j) > max_len) max_len = len*2 + (j == st_j);
        }
      }
    }

    cerr << "maxlen: " << max_len/2 << ", limit: " << limit << ", st_j: " << st_j << "\n";
    // static int cnt = 0;
    // if(max_len/2 == 6 && limit == 1 && st_j == 10){
    //   cnt++;
    //   cerr << "row: " << row << ", stj: " << st_j << "\n";
    //   state_now.debug();
    //   cerr << "\n";
    //   for(int j = 0; j < st_j; j++) cerr << (int)state_now[h-1][j];
    //   cerr << "|";
    //   for(int j = st_j; j < w; j++) cerr << (int)state_now[h-1][j];
    //   cerr << "\n";
    //   for(int j = 0; j < st_j; j++) cerr << (int)state_goal[h-row-1][j];
    //   cerr << "|";
    //   for(int j = st_j; j < w; j++) cerr << (int)state_goal[h-row-1][j];
    //   cerr << "\n";
    //   if(cnt >= 3) exit(0);
    // }

    // 2手以内で揃えられるピース群がない
    if(max_len/2 <= 0){
      // type2の抜き型で未完成部分の行の偶奇を変える
      slide_and_output({23, row + ((h-1 - row) & 1), 0, Dir::U}, ops, state_now);
      return;
    }


    // max_lenのものを動かす
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
        step = 1;
        int len = min((int)z[(i-row)*(w+1) + ulen+1 + j], u_kata_size); // 長さ
        // if(len / step > limit){ // 十分大きなピース群を発見
        // if(len / step >= max_len){
        if(len*2 + (j == st_j) >= max_len){
          int nuki_row = (i + lr_kata_size <= h - 1 ? i : h-1 - lr_kata_size); // 左右の抜き型を使う行
          if(abs(j - st_j) > lr_kata_size && (!((h-1 - i) & 1) || j == st_j)){
            continue;
          }
          // type2の抜き型を使ってst_jに持ってく
          if(abs(j - st_j) > lr_kata_size){
            if(j < st_j){
              slide_and_output({23, i, w - abs(st_j - j), Dir::R}, ops, state_now);
            }else if(st_j < j){
              slide_and_output({23, i, -256 + abs(st_j - j), Dir::L}, ops, state_now);
            }
          }else{
            if(j < st_j){
              // 右から st_j - j 行を左へもっていく
              slide_and_output({lr_kata_id, nuki_row, w - abs(st_j - j), Dir::R}, ops, state_now);
            }else if(st_j < j){
              // 左から j - st_j 行を右へもっていく
              slide_and_output({lr_kata_id, nuki_row, abs(st_j - j) - lr_kata_size, Dir::L}, ops, state_now);
            }
            if(max_len/2 == 6 && limit == 1 && st_j == 10){
              cerr << "\n";
              state_now.debug();
              cerr << "\n";
            }
            slide_and_output({u_kata_id, i + 1 - u_kata_size, st_j, Dir::U}, ops, state_now);
          }
          moved = true;
          cerr << max_len/2 << " clearly(" << ops.size() << ")\n";
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
  int prev_maxlen = 1;
  while(true){
    check_unique_piece_move(ops, row, state_now, state_goal);

    int st_j = 0;
    while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

    // solve_row_clearly(ops, row, state_now, state_goal, 3 * (w - st_j) / w);
    solve_row_clearly(ops, row, state_now, state_goal, prev_maxlen);
    // solve_row_clearly(ops, row, state_now, state_goal, 4 * (w - st_j) / w);

    while(st_j < w && state_now[h-1][st_j] == state_goal[h-row-1][st_j]) st_j++;

    if(st_j >= w) break;

    int best_score = 1000000000;
    Operation best(1U << 31);

    // move none
    // best_score = evaluate_board(row, state_now, state_goal);

    tie(best, prev_maxlen) = search_best_connection(row, state_now, state_goal);
    best.debug();

    best_score = evaluate_board(row, state_now, state_goal);

    if(best == Operation(1U << 31)){
      prev_maxlen = 1;
      cerr << "search all\n";
      Operation best_op[256*2+1];
      int best_scores[256*2+1];
      fill(best_scores, best_scores + 256*2+1, 1000000000);

      #pragma omp parallel for
      for(int j = 0; j < w*2-1; j++){
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
    }

    // 前回と同じスコアの場合、改善していないのでエラー
    if(best != Operation(1U << 31)){
      slide_and_output(best, ops, state_now);
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
    cerr << best_score << "(" << ops.size() << ")\n";
  }

  for(int j = 0; j < w; j++){
    assert(state_now[h-1][j] == state_goal[h-row-1][j]);
  }
  slide_and_output({KATA_MM, h - 1, 0, Dir::D}, ops, state_now);
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
          slide_and_output({KATA_11, row, j, Dir::R}, ops, state_now);
          break;
        }
      }
    }
    slide_and_output({KATA_MM, h - 1, 0, Dir::D}, ops, state_now);
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

  Board state_now = state_start;
  for(const Operation op : ops){
    state_now.slide(op);
  }

  // チェック
  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      assert(state_now[i][j] == state_goal[i][j]);
    }
  }

  // 操作終了
  cout << "-1 -1 -1 -1" << endl;

  return 0;
}
