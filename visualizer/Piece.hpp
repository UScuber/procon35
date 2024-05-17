# pragma once
# include <Siv3D.hpp>


class Piece {
private:
	// ピースの数値
	int number;
	// ピースの正方形
	Rect square;
	// 数値に対応する色
	const Array<Color> colors = { HSV{22,0.86,0.90}, HSV{100,0.40,0.90}, HSV{221,0.49,0.90}, HSV{45,1.00, 0.90} };
public:
	Piece(void);
	Piece(int num, const Point& pos, const int size);
	/// @brief ピース単体を描画する
	void draw(void) const;
	/// @brief ピースがマウスホバーされているかを判定する
	bool is_mouse_hover(void) const;
	/// @brief ピースがクリックされたかを判定する
	bool is_left_clicked(void) const;

	// 正方形を設定
	void set_square(const Point& pos, const int size);
	// 正方形を取得
	Rect get_square(void) const;
	// 数値を取得
	int get_number(void);
	// 数値を設定
	void set_number(const int num);

	// 演算子定義
	Piece& operator=(const Piece&) {
		return *this;
	}
};

Piece::Piece(void) {}
Piece::Piece(const  int num, const Point& pos, const int size) {
	assert(0 <= num and num <= 3);
	this->number = num;
	this->square = Rect{ pos, size };
}
void Piece::draw(void) const {
	square.draw(this->colors[this->number]);
 }
bool Piece::is_mouse_hover(void) const {
	return this->square.mouseOver();
}
bool Piece::is_left_clicked(void) const {
	return this->square.leftClicked();
}

void Piece::set_square(const Point& pos, const int size) {
	this->square = Rect{ pos, size };
}
Rect Piece::get_square(void) const {
	return this->square;
}
void Piece::set_number(int num) {
	this->number = num;
}
int Piece::get_number(void) {
	return  this->number;
}
