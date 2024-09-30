#include <iostream>
#include <vector>
#include <algorithm>
#include <cassert>
#include <tuple>
#include "board.hpp"

using namespace std;


int main(){
  // 定型抜き型作成
  cutting_dies.push_back(vector<vector<int>>{{ 1 }}); // 1x1
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


  // チェック
  Board state_now = state_start;
  while(true){
    int kata,y,x,dir;
    cin >> kata >> y >> x >> dir;
    if(kata < 0) break;
    state_now.slide({kata, y, x, (Dir)dir});
  }

  for(int i = 0; i < h; i++){
    for(int j = 0; j < w; j++){
      if(state_now[i][j] != state_goal[i][j]){
        cout << "NG" << endl;
        return 0;
      }
    }
  }
  cout << "OK" << endl;

  return 0;
}
