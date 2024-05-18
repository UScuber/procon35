# pragma once
# include <Siv3D.hpp>

// 一つの型抜き
using Pattern = Array<Array<bool>>;
class Patterns {
private:
	const Array<int> pattern_sizes = { 1,2,4,8,16,32,64,128,256 };
	Array<Pattern> patterns;
	Array<std::pair<int, Rect>> rect_pairs;
	int selected_idx = 0;
	const Font font{ 20, Typeface::Bold };
public:
	Patterns(void);
	// 型抜きの一覧描画
	void draw(void) const;
	// 型抜きの処理
	void update(void);
	// 指定されたindexの型抜きを取得
	Pattern get_pattern(const int idx) const;
	// 引数なしで現在選択されている型抜きを選択
	Pattern get_pattern(void) const;
};

Patterns::Patterns(void) {
	// 定型抜き型の生成
	for (int size : pattern_sizes) {
		// 全要素1
		patterns << (Array<Array<bool>>(size, Array<bool>(size, true)));
		if (size == 1) continue;
		// 偶数行1
		Array<Array<bool>> tmp;
		for (int row = 0; row < size; row++) {
			tmp << (Array<bool>(size, (row + 1) % 2));
		}
		patterns << tmp;
		// 奇数行1
		Array<bool> ttmp;
		for (int col = 0; col < size; col++) {
			ttmp << (col + 1) % 2;
		}
		patterns << Array<Array<bool>>(size, ttmp);
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
Pattern Patterns::get_pattern(const int idx) const {
	return this->patterns[idx];
}
Pattern Patterns::get_pattern(void) const {
	return this->patterns[selected_idx];
}
void Patterns::update(void){
	for (const std::pair<int, Rect>& p : rect_pairs) {
		const int idx = p.first;
		const Rect rect = p.second;
		if (rect.leftClicked()) {
			this->selected_idx = idx;
		}
	}
}





