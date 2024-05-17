# pragma once
# include <Siv3D.hpp>
# include "Piece.hpp"
# include "Patterns.hpp"

class Board {
protected:
	// ボードの縦横
	int height, width;
	// 一つのピースの描画サイズ
	int piece_size = 15;
	// 2次元配列のボード
	Array<Array<Piece>> board;
	// 型抜きの配列
	Patterns patterns;
	// ボード上のピースが選択されてるか
	Array<Array<bool>> is_selected;

	// ピースの座標計算
	virtual Point calc_piece_pos(int col, int row) const;
	// どこのピースがクリックされたかを返す  
	Optional<Point> which_clicked(void) const;
	// どこのピースがホバーされているかを返す 
	Optional<Point> which_hover(void) const;
	// ホバーで選択状態する
	void select_piece(void);
public:
	// ボードをランダム生成
	Board(void);
	// ボードからシャッフルして生成
	Board(const Array<Array<Piece>>& board);
	// ボードの処理
	void update(void);
	// 型抜きの移動
	void move(void);
	// ボードの描画
	void draw(void) const;

	Array<Array<Piece>> get_board(void) const;
	void set_piece_size(const int size);

};


Point Board::calc_piece_pos(const int col, const int row) const {
	return Point{ col * this->piece_size, row * this->piece_size };
}
Optional<Point> Board::which_clicked(void) const {
	for (int col = 0; col < height; col++) {
		for (int row = 0; row < width; row++) {
			if (board[col][row].is_left_clicked()) {
				return Point{ row, col };
			}
		}
	}
	return none;
}
Optional<Point> Board::which_hover(void) const {
	for (int col = 0; col < height; col++) {
		for (int row = 0; row < width; row++) {
			if (board[col][row].is_mouse_hover()) {
				return Point{ row, col };
			}
		}
	}
	return none;
}

void Board::select_piece(void) {
	// マウスオーバーされているピースを特定する
	Optional<Point> piece_mouseover = which_hover();
	// マウスオーバーされていなかったら終了
	if (not piece_mouseover.has_value()) {
		return;
	}
	Point pos = piece_mouseover.value();
	// 抜き型を取得
	Pattern pattern = patterns.get_pattern();
	// 選択状態にする
	this->is_selected.clear();
	this->is_selected.resize(this->height, Array<bool>(this->width, false));
	for (int col = pos.y; col < height and col < pos.y + pattern.size(); col++) {
		for (int row = pos.x; row < width and row < pos.x + pattern.front().size(); row++) {
			this->is_selected[col][row] = pattern[col - pos.y][row - pos.x];
		}
	}
}

Board::Board(void) {
	this->height = 32;
	this->width = 32;
	this->is_selected.resize(height, Array<bool>(width, false));
	for (int col = 0; col < height; col++) {
		Array<Piece> tmp;
		for (int row = 0; row < width; row++) {
			tmp << Piece(Random(0, 3), calc_piece_pos(col, row), piece_size);
		}
		this->board << tmp;
	}
}
Board::Board(const Array<Array<Piece>>& board) {
	this->height = board.size();
	this->width = board.front().size();
	this->board.resize(height, Array<Piece>(width));
	this->is_selected.resize(height, Array<bool>(width, false));
	Array<Piece> tmp;
	for (int i = 0; i < height * width; i++) {
		tmp << board[i / width][i % width];
	}
	tmp.shuffle();
	for (int i = 0; i < height * width; i++) {
		this->board[i / width][i % width] = tmp[i];
	}
}

void Board::update(void) {
	patterns.update();
	select_piece();
}
void Board::move(void) {

}

void Board::draw(void) const {
	patterns.draw();
	// ピース群の描画
	for (const Array<Piece>&ary : this->board) {
		for (const Piece & piece : ary) {
			piece.draw();
		}
	}
	// 選択されているピースを黒く表示
	for (int col = 0; col < this->height; col++) {
		for (int row = 0; row < this->width; row++) {
			if (is_selected[col][row]) {
				board[col][row].get_square().draw(ColorF{ 0.0, 0.5 });
			}
		}
	}
}


Array<Array<Piece>> Board::get_board(void) const{
	return this->board;
}
void Board::set_piece_size(const int size) {
	this->piece_size = size;
}


//class BoardOperate : public Board {
//protected:
//
//	Point calc_piece_pos(int col, int row) const override;
//public:
//	using Board::Board;
//};
//Point BoardOperate::calc_piece_pos(int col, int row) const {
//	return Point{ col * this->piece_size, row * this->piece_size };
//}

