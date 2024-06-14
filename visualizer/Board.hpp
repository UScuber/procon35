# pragma once
# include <Siv3D.hpp>
# include "Piece.hpp"
# include "Patterns.hpp"
# include "Slide.hpp"
# include "Data.hpp"

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
	const Font font_arrows{ 100, Typeface::Bold };  // 詳細表示用フォント
	DataWriter datawriter;  // 行動ログ保存用
	bool is_wasd_piece = true;  // wasdキーがピース選択になっているか
	Point selected_pos{ 0,0 };  // 選択ピースの座標
	int anchor_icon_size = Scene::Size().y / 20;
	Texture anchor_icon{0xf13d_icon, anchor_icon_size};
	//////////////////////////////////////////////////////////////
	// method
	bool is_in_board(const Point& pos) const;  // ボード上に収まっているか
	bool is_in_board(const int y, const int x) const;
	virtual Point calc_piece_pos(int row, int col) const = 0;  // ピースの座標計算
	Point calc_lefttop(void) const;  // 中心軸を考慮した左上座標
	void set_selected_pos(const Point& pos);  // 選択ピースの座標取得
	Point get_selected_pos(void) const;  // 選択ピースの座標取得
	Optional<Point> which_hover(void) const;  // どこのピースがホバーされているかを返す
	Array<Array<Piece>> get_board(void) const;  // ボードを取得する
	Piece get_piece(const int y, const int x) const; // 座標を指定してピースを取得する
	void set_piece_size(const int size);  // ピースサイズを設定
	void save_json(const FilePath& path) const;  // 行動ログのjsonを保存
	//////////////////////////////////////////////////////////////
	// update
	void switch_wasd(void);  // wasdキーの役割変更
	void change_anchor(void);   // 中心軸を変更
	void select_piece(void);  // ホバーで選択状態する
	void move(void);  // 型抜きの移動
	//////////////////////////////////////////////////////////////
	// draw
	void draw_board(void) const;  // ピース群の描画
	void draw_selected(void) const;  // 選択されているピースを黒く表示
	void draw_details(const Board &board) const;  // 詳細表示
};


bool Board::is_in_board(const Point& pos) const {
	return is_in_board(pos.y, pos.x);
}
bool Board::is_in_board(const int y, const int x) const {
	return (0 <= y and y < this->height and 0 <= x and x < this->width);
}
Point Board::calc_lefttop(void) const{
	if (this->anchor == Anchor::Lefttop) return this->selected_pos;
	else if (this->anchor == Anchor::Leftbottom) return  this->selected_pos - Point{ 0, this->patterns.get_pattern().size() - 1 };
	else if (this->anchor == Anchor::Rightbottom) return  this->selected_pos - Point{ this->patterns.get_pattern().front().size() - 1, this->patterns.get_pattern().size() - 1 };
	else if (this->anchor == Anchor::Righttop) return this->selected_pos - Point{ this->patterns.get_pattern().front().size() - 1, 0 };
}
void Board::set_selected_pos(const Point& pos) {
	this->selected_pos = pos;
}
Point Board::get_selected_pos(void) const {
	return this->selected_pos;
}
Optional<Point> Board::which_hover(void) const {
	const Point mouse_pos = Cursor::Pos();
	Point res{ 0,0 };
	res.x = mouse_pos.x / this->piece_size;
	res.y = mouse_pos.y / this->piece_size;
	if (is_in_board(res)) return res;
	else return none;
}
Array<Array<Piece>> Board::get_board(void) const {
	return this->board;
}
Piece Board::get_piece(const int y, const int x) const {
	return this->board[y][x];
}
void Board::set_piece_size(const int size) {
	this->piece_size = size;
}
void Board::save_json(const FilePath& path) const {
	this->datawriter.get_json().save(path);
}

void Board::switch_wasd(void) {
	if (KeyShift.down()) {
		this->is_wasd_piece ^= true;
	}
}
void Board::change_anchor(void) {
	if (KeyQ.down()) {
		this->anchor = static_cast<Anchor>((static_cast<int>(this->anchor) - 1 + 4) % 4);
	}
	else if (KeyE.down()) {
		this->anchor = static_cast<Anchor>((static_cast<int>(this->anchor) + 1 + 4) % 4);
	}
}
void Board::select_piece(void) {
	if (is_wasd_piece) {
		// キー入力によって上下左右に選択座標を動かす
		Point pos = get_selected_pos();
		if (KeyW.down()) {
			if (pos.y == 0) pos.y = this->height - 1;
			else pos.y--;
		}else if (KeyS.down()) {
			if (pos.y == this->height - 1) pos.y = 0;
			else pos.y++;
		}else if (KeyA.down()) {
			if (pos.x == 0) pos.x = this->width - 1;
			else pos.x--;
		}else if (KeyD.down()) {
			if (pos.x == this->width - 1) pos.x = 0;
			else pos.x++;
		}
		set_selected_pos(pos);
	}else {
		// マウスオーバーされているピースを特定する
		Optional<Point> piece_mouseover = which_hover();
		// マウスオーバーされていたら選択座標を変更
		if (piece_mouseover.has_value()) {
			set_selected_pos(piece_mouseover.value());
		}
	}
	// 左上座標を取得
	Point pos = calc_lefttop();
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
void Board::move(void) {
	// キーボードが押された方向を特定、なければ終了
	Dir dir;
	if (KeyUp.down()) dir = Dir::U;
	else if (KeyLeft.down()) dir = Dir::L;
	else if (KeyDown.down()) dir = Dir::D;
	else if (KeyRight.down()) dir = Dir::R;
	else return;
	// 左上座標
	Point pos = calc_lefttop();
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
	// 行動ログを追加
	this->datawriter.add_op(patterns.get_pattern_idx(), pos, dir);
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
void Board::draw_details(const Board &board) const {
	int cnt_lack = 0;
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			if (this->board[row][col].get_number() != board.get_piece(row, col).get_number()) {
				cnt_lack++;
			}
		}
	}
	int piece_sum = this->height * this->width;
	assert(this->height == board.height and this->width == board.width);
	font(U"手数:{}"_fmt(cnt_move)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y / 10 }, Palette::Black);
	font(U"不一致数:{}"_fmt(cnt_lack)).drawAt(Vec2{ Scene::Center().x,  Scene::Size().y / 10 * 2 }, Palette::Black);
	font(U"一致率:{:.0f}%"_fmt((double)(piece_sum - cnt_lack) / (double)piece_sum * 100.0)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y / 10 * 3 }, Palette::Black);
	Vec2 anchor_rects{ Scene::Center().x, Scene::Size().y * 2 / 5 };
	RectF{ Arg::bottomRight(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	RectF{ Arg::topRight(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	RectF{ Arg::topLeft(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	RectF{ Arg::bottomLeft(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	Vec2 anchor_pos = anchor_rects;
	if (this->anchor == Anchor::Lefttop) anchor_pos += Vec2{ -anchor_icon_size / 2, -anchor_icon_size / 2 };
	else if (this->anchor == Anchor::Leftbottom) anchor_pos += Vec2{ -anchor_icon_size / 2, anchor_icon_size / 2 };
	else if(this->anchor == Anchor::Rightbottom) anchor_pos += Vec2{ anchor_icon_size / 2, anchor_icon_size / 2 };
	else if (this->anchor == Anchor::Righttop) anchor_pos += Vec2{ anchor_icon_size / 2, -anchor_icon_size / 2 };
	anchor_icon.scaled(0.8).drawAt(anchor_pos, Palette::Black);
	String arrows = (is_wasd_piece) ? U"↑↑↑↑↑" : U"↓↓↓↓↓";
	font_arrows(arrows).drawBaseAt(Vec2{ Scene::Center().x, Scene::Size().y * 3.75 / 5 }, (is_wasd_piece) ? Palette::Blue : Palette::Red);
}



class BoardOperate : public Board {
private:
	Point calc_piece_pos(int row, int col) const override;
public:
	void initialize(const Array<Array<Piece>>& board);
	BoardOperate(void) {};
	void update(void);
	void draw(const Board& board) const;
};
Point BoardOperate::calc_piece_pos(int row, int col) const {
	return Point{ col * this->piece_size, row * this->piece_size };
}
void BoardOperate::initialize(const Array<Array<Piece>>& board) {
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
	switch_wasd();
	change_anchor();
	patterns.update(not this->is_wasd_piece);
	select_piece();
	move();
}
void BoardOperate::draw(const Board& board) const {
	patterns.draw();
	draw_board();
	draw_selected();
	draw_details(board);
}


class BoardExample : public Board {
private:
	Point calc_piece_pos(int row, int col) const override;
public:
	void initialize(const FilePath& path);
	BoardExample(void) {};
	void update(Board & board);
	void draw(void) const;
};
Point BoardExample::calc_piece_pos(int row, int col) const {
	return Point{ Scene::Size().x - this->width * piece_size + col * this->piece_size, row * this->piece_size };
}
void BoardExample::initialize(const FilePath& path) {
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
void BoardExample::draw(void) const {
	draw_board();
	draw_selected();
}

class BoardAuto : public Board {
private:
	Point calc_piece_pos(int row, int col) const override;
	ChildProcess child;
	
public:
	void initialize(const Array<Array<Piece>>& board);
	BoardAuto(void) {};
	void update(void);
	void draw(const Board& board) const;
};
Point BoardAuto::calc_piece_pos(int row, int col) const {
	return Point{ col * this->piece_size, row * this->piece_size };
}
void BoardAuto::initialize(const Array<Array<Piece>>& board) {
	this->child = ChildProcess{ U"./solver.exe", Pipe::StdInOut };
	if (not child) throw Error{ U"Failed to create a process" };
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
	// プロセスに入力を与える
	this->child.ostream() << this->height << std::endl << this->width << std::endl;
	for (int row = 0; row < this->height; row++) {
		String input = U"";
		for (int col = 0; col < this->width; col++) {
			input += Format(this->get_piece(row, col).get_number());
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	for (int row = 0; row < this->height; row++) {
		String input = U"";
		for (int col = 0; col < this->width; col++) {
			input += Format(board[row][col].get_number());
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	int p, x, y, s;
	this->child.istream() >> p >> x >> y >> s;
	this->datawriter.add_op(p, Point{ x,y }, (Dir)s);
}

void BoardAuto::update(void) {
	move();
}
void BoardAuto::draw(const Board& board) const {
	draw_board();
	draw_details(board);
}



