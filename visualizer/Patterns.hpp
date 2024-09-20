# pragma once

# include <Siv3D.hpp>
# include "BitBoard.hpp"


// 一つの型抜き
using Pattern = BitBoard;
class Patterns {
private:
	const Array<int> pattern_sizes = { 1,2,4,8,16,32,64,128,256 };
	Array<Pattern> patterns;
	Array<std::pair<int, Rect>> rect_pairs;
	int selected_idx = 0;
	const Font font{ 20, Typeface::Bold };
	// マウスによる選択
	void select_mouse(void);
	// キーボードによる選択
	void select_key(void);
public:
	Patterns(void);
	// 型抜きの一覧描画
	void draw(void) const;
	// 型抜きの処理
	void update(bool is_wasd_patterns);
	// 指定されたindexの型抜きを取得
	Pattern get_pattern(const int idx) const;
	// 引数なしで現在選択されている型抜きを選択
	Pattern get_pattern(void) const;
	// 現在選択されている型抜きのインデックスを取得
	int get_pattern_idx(void) const;
};

Patterns::Patterns(void) {
	// 定型抜き型の生成
	for (int size : pattern_sizes) {
		// 全要素1
		patterns << Array<Array<int>>(size, Array<int>(size, 1));
		if (size == 1) continue;
		// 偶数行1
		Array<Array<int>> tmp;
		for (int row = 0; row < size; row++) {
			tmp << Array<int>(size, (row + 1) % 2);
		}
		patterns << tmp;
		// 奇数行1
		Array<int> ttmp;
		for (int col = 0; col < size; col++) {
			ttmp << (col + 1) % 2;
		}
		patterns << Array<Array<int>>(size, ttmp);
	}
	// 抜き型選択ボタンの四角形生成
	Size size(Scene::Size().x / (int)pattern_sizes.size(), Scene::Size().y / 15);
	for (int col = 0; col < pattern_sizes.size(); col++) {
		for (int row = 0; row < 3; row++) {
			Rect rect{ Point(size.x * col, Scene::Size().y * 4 / 5 + size.y * row),  size };
			int idx = col * 3 + row - ((col == 0 and row == 0) ? 0 : 2);
			this->rect_pairs << std::make_pair(idx, rect);
			if (col == 0) break;
		}
	}
}

void Patterns::draw(void) const {
	for (const std::pair<int, Rect>& p : rect_pairs) {
		const int idx = p.first;
		const Rect rect = p.second;
		rect.draw(Palette::Skyblue).drawFrame(1.0, Palette::Black);
		font(U"{}"_fmt(idx)).draw(rect.stretched(-5));
		if (idx == selected_idx) {
			rect.draw(ColorF{ 0.0, 0.5 });
		}
	}
}

void Patterns::select_mouse(void) {
	for (const std::pair<int, Rect>& p : rect_pairs) {
		const int idx = p.first;
		const Rect rect = p.second;
		if (rect.leftClicked()) {
			this->selected_idx = idx;
		}
	}
}
void Patterns::select_key(void) {
	if (this->selected_idx == 0) {
		if (KeyA.down()) this->selected_idx = 22;
		else if (KeyD.down()) this->selected_idx = 1;
		return;
	}
	if (KeyW.down()) {
		if ((this->selected_idx - 1) % 3 == 0) this->selected_idx += 2;
		else this->selected_idx--;
	}else if (KeyS.down()) {
		if (this->selected_idx % 3 == 0) this->selected_idx -= 2;
		else this->selected_idx++;
	}else if (KeyA.down()) {
		if (get_pattern().height() == 2) this->selected_idx = 0;
		else this->selected_idx -= 3;
	}else if (KeyD.down()) {
		if (get_pattern().height() == 256) this->selected_idx = 0;
		else this->selected_idx += 3;
	}
}
void Patterns::update(bool is_wasd_patterns) {
	select_mouse();
	if(is_wasd_patterns) select_key();
}

Pattern Patterns::get_pattern(const int idx) const {
	return this->patterns[idx];
}
Pattern Patterns::get_pattern(void) const {
	return this->patterns[selected_idx];
}
int Patterns::get_pattern_idx(void) const {
	return this->selected_idx;
}





