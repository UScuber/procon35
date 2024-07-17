#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <tuple>

using namespace std;

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
