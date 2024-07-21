#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <tuple>

using namespace std;
using std::cin;

enum class Dir {
  U,D,L,R,
};

vector<vector<vector<int>>> cutting_dies; // 抜き型


void left_slide(vector<vector<int>> &a, const vector<vector<int>> &kata, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min((int)kata.size(), h-posy), kata_tx = min((int)kata[0].size(), w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  for(int i = kata_sy; i < kata_ty; i++){
    auto &row = a[i+board_y];
    vector<int> picked_pieces;

    // pick
    for(int j = kata_sx; j < kata_tx; j++){
      if(kata[i][j]){
        picked_pieces.push_back(row[j+board_x]);
        row[j+board_x] = -1;
      }
    }

    // shift
    int picked_count = 0;
    for(int j = 0; j < w; j++){
      if(row[j] < 0){
        picked_count++;
      }else if(picked_count){
        row[j - picked_count] = row[j];
        row[j] = -1;
      }
    }

    // put
    int put_count = 0;
    for(int j = 0; j < w; j++){
      if(row[j] < 0) row[j] = picked_pieces[put_count++];
    }
  }
}

void right_slide(vector<vector<int>> &a, const vector<vector<int>> &kata, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min((int)kata.size(), h-posy), kata_tx = min((int)kata[0].size(), w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  for(int i = kata_sy; i < kata_ty; i++){
    auto &row = a[i+board_y];
    vector<int> picked_pieces;

    // pick
    for(int j = kata_tx-1; j >= kata_sx; j--){
      if(kata[i][j]){
        picked_pieces.push_back(row[j+board_x]);
        row[j+board_x] = -1;
      }
    }

    // shift
    int picked_count = 0;
    for(int j = w-1; j >= 0; j--){
      if(row[j] < 0){
        picked_count++;
      }else if(picked_count){
        row[j + picked_count] = row[j];
        row[j] = -1;
      }
    }

    // put
    int put_count = 0;
    for(int j = w-1; j >= 0; j--){
      if(row[j] < 0) row[j] = picked_pieces[put_count++];
    }
  }
}

void up_slide(vector<vector<int>> &a, const vector<vector<int>> &kata, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min((int)kata.size(), h-posy), kata_tx = min((int)kata[0].size(), w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  for(int j = kata_sx; j < kata_tx; j++){
    vector<int> picked_pieces;

    // pick
    for(int i = kata_sy; i < kata_ty; i++){
      if(kata[i][j]){
        picked_pieces.push_back(a[i+board_y][j+board_x]);
        a[i+board_y][j+board_x] = -1;
      }
    }

    // shift
    int picked_count = 0;
    for(int i = 0; i < h; i++){
      if(a[i][j+board_x] < 0){
        picked_count++;
      }else if(picked_count){
        a[i - picked_count][j+board_x] = a[i][j+board_x];
        a[i][j+board_x] = -1;
      }
    }

    // put
    int put_count = 0;
    for(int i = 0; i < h; i++){
      if(a[i][j+board_x] < 0) a[i][j+board_x] = picked_pieces[put_count++];
    }
  }
}

void down_slide(vector<vector<int>> &a, const vector<vector<int>> &kata, const int posy, const int posx){
  const int h = a.size();
  const int w = a[0].size();
  const int kata_sy = max(0, -posy), kata_sx = max(0, -posx);
  const int kata_ty = min((int)kata.size(), h-posy), kata_tx = min((int)kata[0].size(), w-posx);
  const int board_y = max(0, posy) - kata_sy, board_x = max(0, posx) - kata_sx;

  for(int j = kata_sx; j < kata_tx; j++){
    vector<int> picked_pieces;

    // pick
    for(int i = kata_ty-1; i >= kata_sy; i--){
      if(kata[i][j]){
        picked_pieces.push_back(a[i+board_y][j+board_x]);
        a[i+board_y][j+board_x] = -1;
      }
    }

    // shift
    int picked_count = 0;
    for(int i = h-1; i >= 0; i--){
      if(a[i][j+board_x] < 0){
        picked_count++;
      }else if(picked_count){
        a[i + picked_count][j+board_x] = a[i][j+board_x];
        a[i][j+board_x] = -1;
      }
    }

    // put
    int put_count = 0;
    for(int i = h-1; i >= 0; i--){
      if(a[i][j+board_x] < 0) a[i][j+board_x] = picked_pieces[put_count++];
    }
  }
}



vector<vector<int>> slide(const vector<vector<int>> &a, const int kata_index, const int posy, const int posx, const Dir dir){
  const int h = a.size();
  const int w = a[0].size();
  assert(0 <= kata_index < (int)cutting_dies.size());

  const auto &kata = cutting_dies[kata_index];

  assert(-(int)kata.size() < posy && posy < h && -(int)kata[0].size() < posx && posx < w);

  vector<vector<int>> result = a;
  
  if(dir == Dir::U) up_slide(result, kata, posy, posx);
  if(dir == Dir::L) left_slide(result, kata, posy, posx);
  if(dir == Dir::D) down_slide(result, kata, posy, posx);
  if(dir == Dir::R) right_slide(result, kata, posy, posx);
	
  return result;
}




void one_slide(vector<vector<int>> &a, const int posy, const int posx, const Dir dir){
  const int h = a.size();
  const int w = a[0].size();

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
    copy(a[ty+tx-i-1].begin() + sx, a[ty+tx-i-1].begin() + tx, a[i].begin() + sx);
    copy(tmp.begin(), tmp.end(), a[ty+tx-i-1].begin() + sx);
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
  assert(0 <= kata_index < 25);

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


int main(){
  // 定型抜き型作成
  cutting_dies.push_back({{ 1 }}); // 1x1
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
  vector<vector<int>> state_start(h, vector<int>(w));
  for(int i = 0; i < h; i++){
    string s; cin >> s;
    for(int j = 0; j < w; j++){
      state_start[i][j] = s[j] - '0';
    }
  }
  vector<vector<int>> state_goal(h, vector<int>(w));
  for(int i = 0; i < h; i++){
    string s; cin >> s;
    for(int j = 0; j < w; j++){
      state_goal[i][j] = s[j] - '0';
    }
  }


  // チェック  
  int ops_num;
  cin >> ops_num;

  vector<vector<int>> state_now = state_start;
  for(int i = 0; i < ops_num; i++){
    int s,y,x,d;
    cin >> s >> y >> x >> d;
    state_now = slide(state_now, s, y, x, (Dir)d);
    // state_now = teikei_slide(state_now, s, y, x, (Dir)d);
  }

  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      if(state_now[i][j] != state_goal[i][j]){
        cout << "NG" << endl;
      }
    }
  }
  cout << "OK" << endl;

  return 0;
}
