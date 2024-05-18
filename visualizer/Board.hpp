# pragma once
# include <Siv3D.hpp>
# include "Piece.hpp"
# include "Patterns.hpp"
# include "Slide.hpp"

enum class Anchor {
	Lefttop, Leftbottom, Rightbottom, Righttop,
};

class Board {
private:
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
	// 操作用のボードか
	bool is_operating;
	// 手数カウント
	int cnt_move = 0;
	// 中心軸
	Anchor anchor = Anchor::Lefttop;
	// 詳細表示用フォント
	const Font font{ 50, Typeface::Bold };
	// ボード上に収まっているか
	bool is_in_board(int y, int x);
	// 中心軸を考慮した左上座標
	Point calc_lefttop(const Point& pos) const;
	// ピースの座標計算
	Point calc_piece_pos(int row, int col) const;
	// どこのピースがクリックされたかを返す  
	Optional<Point> which_clicked(void) const;
	// どこのピースがホバーされているかを返す 
	Optional<Point> which_hover(void) const;
	// ホバーで選択状態する
	void select_piece(void);
public:
	// ボードをランダム生成
	Board(bool is_example);
	// ボードからシャッフルして生成
	Board(bool is_example, const Array<Array<Piece>>& board);
	// テキストファイルからボードをを生成
	Board(bool is_example, const FilePath& path);
	// ボードの処理
	void update(void);
	// 型抜きの移動
	void move(void);
	// ボードの描画
	void draw(void) const;

	Array<Array<Piece>> get_board(void) const;
	void set_piece_size(const int size);

};


bool Board::is_in_board(int y, int x) {
	return (0 <= y and y < this->height and 0 <= x and x < this->width);
}
Point Board::calc_lefttop(const Point& pos) const{
	if (this->anchor == Anchor::Lefttop) return pos;
	else if (this->anchor == Anchor::Leftbottom) return pos - Point{ 0, this->patterns.get_pattern().size()-1 };
	else if (this->anchor == Anchor::Rightbottom) return pos - Point{ this->patterns.get_pattern().front().size()-1, this->patterns.get_pattern().size()-1};
	else if (this->anchor == Anchor::Righttop) return pos - Point{ this->patterns.get_pattern().front().size()-1, 0 };
}
Point Board::calc_piece_pos(const int row, const int col) const {
	if (this->is_operating) {
		return Point{ col * this->piece_size , row * this->piece_size };
	}else {
		return Point{ Scene::Size().x - this->width * piece_size + col * this->piece_size, row * this->piece_size };
	}
}
Optional<Point> Board::which_clicked(void) const {
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (board[row][col].is_left_clicked()) {
				return Point{ col, row };
			}
		}
	}
	return none;
}
Optional<Point> Board::which_hover(void) const {
	for (int row = 0; row < height; row++) {
		for (int col = 0; col < width; col++) {
			if (board[row][col].is_mouse_hover()) {
				return Point{ col, row };
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
	// 座標を左上に合わせる
	Point pos = calc_lefttop(piece_mouseover.value());
	// 抜き型を取得
	Pattern pattern = patterns.get_pattern();
	// 選択状態にする
	this->is_selected.clear();
	this->is_selected.resize(this->height, Array<bool>(this->width, false));
	for (int row = pos.y; row < pos.y + (int)pattern.size(); row++) {
		for (int col = pos.x; col < pos.x + (int)pattern.front().size(); col++) {
			if (not is_in_board(row, col)) continue;
			this->is_selected[row][col] = pattern[row - pos.y][col - pos.x];
		}
	}
}

Board::Board(bool is_operating) {
	this->is_operating = is_operating;
	this->height = 32;
	this->width = 32;
	this->is_selected.resize(height, Array<bool>(width, false));
	for (int row = 0; row < height; row++) {
		Array<Piece> tmp;
		for (int col = 0; col < width; col++) {
			tmp << Piece(Random(0, 3), calc_piece_pos(row, col), piece_size);
		}
		this->board << tmp;
	}
}
Board::Board(bool is_operating, const Array<Array<Piece>>& board) {
	this->is_operating = is_operating;
	this->height = board.size();
	this->width = board.front().size();
	this->board.resize(height, Array<Piece>(width));
	this->is_selected.resize(height, Array<bool>(width, false));
	Array<int> tmp;
	for (int i = 0; i < height * width; i++) {
		tmp << board[i / width][i % width].get_number();
	}
	tmp.shuffle();
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			int num = tmp[row * this->height + col];
			this->board[row][col].set_number(num);
			this->board[row][col].set_square(calc_piece_pos(row, col), this->piece_size);
		}
	}
}
Board::Board(bool is_operating, const FilePath& path) {
	this->is_operating = is_operating;
	TextReader reader{ path };
	if (not reader) throw Error{ U"failed to open {}"_fmt(path) };
	String line;
	Array<Array<int>> tmp;
	while (reader.readLine(line)) {
		Array<int> row;
		for (const char8& num : line) {
			row << num - U'0';
		}
		tmp << row;
	}
	this->height = tmp.size();
	this->width = tmp.front().size();
	this->board.resize(height, Array<Piece>(width));
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			this->board[row][col].set_number(tmp[row][col]);
			this->board[row][col].set_square(calc_piece_pos(row, col), this->piece_size);
		}
	}
	this->is_selected.resize(height, Array<bool>(width, false));
}

void Board::update(void) {
	if (KeyQ.down()) {
		this->anchor = static_cast<Anchor>((static_cast<int>(this->anchor) - 1 + 4) % 4);
	}else if (KeyE.down()) {
		this->anchor = static_cast<Anchor>((static_cast<int>(this->anchor) + 1 + 4) % 4);
	}
	patterns.update();
	select_piece();
	move();
}
void Board::move(void) {
	// キーボードが押された方向を特定、なければ終了
	Dir dir;
	if (KeyW.down()) {
		dir = Dir::U;
	}else if (KeyA.down()) {
		dir = Dir::L;
	}else if (KeyS.down()) {
		dir = Dir::D;
	}else if (KeyD.down()) {
		dir = Dir::R;
	}else {
		return;
	}
	// マウスオーバー位置を特定、なければ終了
	Optional<Point> piece_mouseover = which_hover();
	if (not piece_mouseover.has_value()) {
		return;
	}
	Point pos = calc_lefttop(piece_mouseover.value());
	// ピースの番号だけを抽出
	Array<Array<int>> board_int(this->height, Array<int>(this->width, -1));
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			board_int[row][col] = this->board[row][col].get_number();
		}
	}
	// スライドさせる
	Array<Array<int>> slided = slide(board_int, this->patterns.get_pattern(), pos, dir);
	Array<int> tmp(4, 0);
	// スライド後の番号をピースに適用
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			this->board[row][col].set_number(slided[row][col]);
		}
	}
	this->cnt_move++;
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
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			if (is_selected[row][col]){
				board[row][col].get_square().draw(ColorF{ 0.0, 0.5 });
			}
		}
	}
	// 手数表示
	if (is_operating) {
		font(U"手数:{}"_fmt(cnt_move)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y / 3 }, Palette::Black);
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
//	Point calc_piece_pos(int row, int col) const override;
//public:
//	using Board::Board;
//};
//Point BoardOperate::calc_piece_pos(int row, int col) const {
//	return Point{ row * this->piece_size, col * this->piece_size };
//}

