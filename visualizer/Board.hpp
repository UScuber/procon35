# pragma once
# include <Siv3D.hpp>
# include "Piece.hpp"
# include "Patterns.hpp"
# include "Slide.hpp"

enum class Anchor {
	Lefttop, Leftbottom, Rightbottom, Righttop,
};

class Board {
public:
	//////////////////////////////////////////////////////////////
	// field
	int height, width;	  // ボードの縦横
	int piece_size = 15;	 // 一つのピースの描画サイズ
	Array<Array<Piece>> board;  // 2次元配列のボードz
	Patterns patterns;  // 型抜きの配列
	Array<Array<bool>> is_selected;  // ボード上のピースが選択されてるか
	int cnt_move = 0;  // 手数カウント
	Anchor anchor = Anchor::Lefttop;  // 中心軸
	const Font font{ 50, Typeface::Bold };  // 詳細表示用フォント
	//////////////////////////////////////////////////////////////
	// method
	virtual Point calc_piece_pos(int row, int col) const = 0;  // ピースの座標計算
	Point calc_lefttop(const Point& pos) const;  // 中心軸を考慮した左上座標
	bool is_in_board(const Point& pos) const;  // ボード上に収まっているか
	bool is_in_board(const int y, const int x) const;
	Optional<Point> which_hover(void) const;  // どこのピースがホバーされているかを返す
	Array<Array<Piece>> get_board(void) const;  // ボードを取得する
	void set_piece_size(const int size);  // ピースサイズを設定
	//////////////////////////////////////////////////////////////
	// update
	virtual void update(void) = 0;  // ボードの処理
	void change_anchor(void);   // 中心軸を変更
	void select_piece(void);  // ホバーで選択状態する
	void move(void);  // 型抜きの移動
	//////////////////////////////////////////////////////////////
	// draw
	virtual void draw(void) const = 0;  // ボードの描画
	void draw_board(void) const;  // ピース群の描画
	void draw_selected(void) const;  // 選択されているピースを黒く表示
	void draw_details(void) const;  // 詳細表示
};


bool Board::is_in_board(const Point& pos) const{
	return is_in_board(pos.y, pos.x);
}
bool Board::is_in_board(const int y, const int x) const {
	return (0 <= y and y < this->height and 0 <= x and x < this->width);
}
Point Board::calc_lefttop(const Point& pos) const{
	if (this->anchor == Anchor::Lefttop) return pos;
	else if (this->anchor == Anchor::Leftbottom) return pos - Point{ 0, this->patterns.get_pattern().size()-1 };
	else if (this->anchor == Anchor::Rightbottom) return pos - Point{ this->patterns.get_pattern().front().size()-1, this->patterns.get_pattern().size()-1};
	else if (this->anchor == Anchor::Righttop) return pos - Point{ this->patterns.get_pattern().front().size()-1, 0 };
}

Optional<Point> Board::which_hover(void) const {
	const Point mouse_pos = Cursor::Pos();
	Point res{ 0,0 };
	res.x = mouse_pos.x / this->piece_size;
	res.y = mouse_pos.y / this->piece_size;
	if (is_in_board(res)) return res;
	else return none;
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

void Board::change_anchor(void) {
	if (KeyQ.down()) {
		this->anchor = static_cast<Anchor>((static_cast<int>(this->anchor) - 1 + 4) % 4);
	}else if (KeyE.down()) {
		this->anchor = static_cast<Anchor>((static_cast<int>(this->anchor) + 1 + 4) % 4);
	}
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

void Board::draw_board(void) const {
	for (const Array<Piece>& ary : this->board) {
		for (const Piece& piece : ary) {
			piece.draw();
		}
	}
}
void Board::draw_selected(void) const {
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			if (is_selected[row][col]) {
				board[row][col].get_square().draw(ColorF{ 0.0, 0.5 });
			}
		}
	}
}
void Board::draw_details(void) const {
	font(U"手数:{}"_fmt(cnt_move)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y / 3 }, Palette::Black);
}

Array<Array<Piece>> Board::get_board(void) const{
	return this->board;
}
void Board::set_piece_size(const int size) {
	this->piece_size = size;
}


class BoardOperate : public Board {
private:
	Point calc_piece_pos(int row, int col) const override;
public:
	BoardOperate(const Array<Array<Piece>>& board);
	void update(void) override;
	void draw(void) const override;
};
Point BoardOperate::calc_piece_pos(int row, int col) const {
	return Point{ col * this->piece_size, row * this->piece_size };
}
BoardOperate::BoardOperate(const Array<Array<Piece>>& board) {
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
void BoardOperate::update(void) {
	change_anchor();
	patterns.update();
	select_piece();
	move();
}
void BoardOperate::draw(void) const {
	patterns.draw();
	draw_board();
	draw_selected();
	draw_details();
}


class BoardExample : public Board {
private:
	Point calc_piece_pos(int row, int col) const override;
public:
	BoardExample(FilePath& path);
	void update(void) override;
	void update(Board& board);
	void draw(void) const override;
};
Point BoardExample::calc_piece_pos(int row, int col) const {
	return Point{ Scene::Size().x - this->width * piece_size + col * this->piece_size, row * this->piece_size };
}
BoardExample::BoardExample(FilePath& path) {
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
void BoardExample::update(Board& board) {
	this->is_selected = board.is_selected;
	return;
}
void BoardExample::update(void) {}
void BoardExample::draw(void) const {
	draw_board();
	draw_selected();
}

