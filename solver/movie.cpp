#include <cmath>
#include <queue>
#include <omp.h>
#include <bitset>
#include <random>
#include "board.hpp"

using namespace std;
using ll = long long;

using namespace std;
using std::cin;


#define KATA_MM 22 // 256x256
#define KATA_11 0 // 1x1
#define KATA_12 2 // 1x2 (縦 1, 横 2)


random_device seed;
mt19937_64 mt(seed());
ll rnd(ll l, ll r){
    return mt() % (r - l) + l;
}

// スライドと更新を一度にする
void slide_and_output(const Operation& op, Operations& ops, Board& state_now){
  ops.push_back(op);
  state_now.slide(op);
  cout << op << endl;
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
  Board state_goal(h, w);
  for(int i = 0; i < h; i++){
    string s; cin >> s;
    for(int j = 0; j < w; j++){
      state_goal[i][j] = s[j] - '0';
    }
  }

	Operations ops;
	for(int r = 0; r < 100; r++){
		int id = rnd(9, 25);
		int y = rnd(0, h);
		int x = rnd(0, w);
		Dir d = (Dir)rnd(0, 4);
		ops.push_back({id, y, x, d});
		state_goal.slide_reverse(ops.back());
	}
	reverse(ops.begin(), ops.end());

	state_goal.debug();
	for(auto op : ops){
		cout << op << endl;
	}

  // 操作終了
  cout << "-1 -1 -1 -1" << endl;
  cerr << (double)ops.size() / h / w << "\n";

  return 0;
}
