﻿# pragma once
# include <Siv3D.hpp>
# include "Patterns.hpp"
// # include "Slide.hpp"
# include "BitBoard.hpp"
# include "Data.hpp"
# include "SolverTask.hpp"
# include "Connect.hpp"

class Board {
public:
	//////////////////////////////////////////////////////////////
	// field
	int height, width;	  // ボードの縦横
	double piece_size = 15;	 // 一つのピースの描画サイズ
	BitBoard board = BitBoard(0, 0);  // 2次元配列のボード
	Patterns patterns;  // 型抜きの配列
	Array<Color> piece_colors = {Palette::Red, Palette::Green, Palette::Blue, Palette::Black};
	int cnt_move = 0;  // 手数カウント
	const Font font{ 50, Typeface::Bold };  // 詳細表示用フォント
	DataWriter datawriter;  // 行動ログ保存用
	DynamicTexture board_dynamic_texture; // ボード描画用のダイナミックテクスチャ
	//////////////////////////////////////////////////////////////
	// method
	void initialize(const BitBoard& board, const bool is_shuffle = true); // 初期化
	void initialize(const FilePath& path);
	bool is_in_board(const Point& pos) const;  // ボード上に収まっているか
	bool is_in_board(const int y, const int x) const;
	virtual Vec2 calc_piece_pos(int row, int col) const = 0;  // ピースの座標計算
	double calc_piece_size(void);  // ピースの大きさを計算
	BitBoard get_board(void) const;  // ボードを取得する
	int get_piece(const int y, const int x) const; // 座標を指定してピースを取得する
	void set_piece(const int num, const int y, const int x);  // 座標を指定してピースをセット
	Color get_piece_color(const int num) const;  // ピースの番号から色を取得する
	void set_piece_size(const int size);  // ピースサイズを設定
	void save_json(const FilePath& path) const;  // 行動ログのjsonを保存
	void set_piece_colors(void);
	//////////////////////////////////////////////////////////////
	// update
	void move(int p, int x, int y, Dir dir, bool enable_json);  // 指定して移動
	void move(int p, const Point& pos, Dir dir, bool enable_json);
	void update_board_texture(void); // ボードのテクスチャを更新
	//////////////////////////////////////////////////////////////
	// draw
	void draw_board(void) const;  // ピース群の描画
	void draw_details(void) const;  // 詳細表示
};

void Board::initialize(const BitBoard& board, const bool is_shuffle) {
	set_piece_colors();
	this->height = board.height();  this->width = board.width();
	this->piece_size = calc_piece_size();
	this->board = BitBoard(height, width);
	if (is_shuffle) {
		Array<int> tmp;
		for (int i = 0; i < height * width; i++)  tmp << board[i / width][i % width];
		tmp.shuffle();
		for (int row = 0; row < this->height; row++) {
			for (int col = 0; col < this->width; col++) {
				int num = tmp[row * this->width + col];
				set_piece(num, row, col);
			}
		}
	}else {
		for (int row = 0; row < this->height; row++) {
			for (int col = 0; col < this->width; col++) {
				set_piece(board[row][col], row, col);
			}
		}
	}
	update_board_texture();
}
void Board::initialize(const FilePath& path) {
	set_piece_colors();
	TextReader reader{ path };
	if (not reader) throw Error{ U"failed to open {}"_fmt(path) };
	String line;
	Array<Array<int>> tmp;
	while (reader.readLine(line)) {
		Array<int> row;
		for (const char8& num : line)  row << num - U'0';
		tmp << row;
	}
	initialize(tmp, false);
}


bool Board::is_in_board(const Point& pos) const {
	return is_in_board(pos.y, pos.x);
}
bool Board::is_in_board(const int y, const int x) const {
	return (0 <= y and y < this->height and 0 <= x and x < this->width);
}
BitBoard Board::get_board(void) const {
	return this->board;
}
int Board::get_piece(const int y, const int x) const {
	return this->board[y][x];
}
void Board::set_piece(const int num, const int y, const int x) {
	this->board[y][x] = num;
}
double Board::calc_piece_size(void) {
	return Min(Scene::Size().x / 2.0, Scene::Size().y * 0.8) / Max(this->height, this->width);
}
Color Board::get_piece_color(const int num) const{
	return this->piece_colors[num];
}
void Board::set_piece_size(const int size) {
	this->piece_size = size;
}
void Board::save_json(const FilePath& path) const {
	this->datawriter.get_json().save(path);
}
void Board::set_piece_colors(void) {
	JSON color_json = JSON::Load(U"./pieceColors.json");
	if (not color_json) throw Error{ U"cannot open pieceColors.json" };
	for (int i = 0; i < 4; i++) {
		piece_colors[i] = color_json[U"pieceColors"][i].get<Color>();
	}
}

void Board::move(int p, int x, int y, Dir dir, bool enable_json){
	this->move(p, Point{ x,y }, dir, enable_json);
}
void Board::move(int p, const Point& pos, Dir dir, bool enable_json) {
	// スライドさせる
	//Array<Array<int>> slided = slide(get_board(), this->patterns.get_pattern(p), pos, dir);
	BitBoard slided = get_board();
	slided.slide(this->patterns.get_pattern(p), pos.y, pos.x, dir);
	// スライド後の番号をピースに適用
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			this->board[row][col] = slided[row][col];
		}
	}
	this->cnt_move++;
	update_board_texture();
	// 行動ログを追加
	if(enable_json) this->datawriter.add_op(p, pos, dir);
}
void Board::update_board_texture(void) {
	Image board_image{ (size_t)this->width, (size_t)this->height };
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			board_image[row][col] = get_piece_color(get_piece(row, col));
		}
	}
	this->board_dynamic_texture.fill(board_image);
}

void Board::draw_board(void) const {
	this->board_dynamic_texture
		.resized(this->piece_size * this->width, this->piece_size * this->height)
		.draw(this->calc_piece_pos(0,0));
}
void Board::draw_details(void) const {
	font(U"手数:{}"_fmt(cnt_move)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y * 14.0/ 15.0 }, Palette::Black);
}



class BoardExample : public Board {
private:
	Vec2 calc_piece_pos(int row, int col) const override;
public:
	//void initialize(const FilePath& path);
	BoardExample(void) {};
	void draw(void) const;
};
Vec2 BoardExample::calc_piece_pos(int row, int col) const {
	return Vec2{ Scene::Size().x - this->width * piece_size + col * this->piece_size, row * this->piece_size };
}
void BoardExample::draw(void) const {
	draw_board();
}



class BoardConnect : public Board {
private:
	Vec2 calc_piece_pos(int row, int col) const override;
	SolverTask solver_task;
	bool is_network = false;
	bool is_finished = false;
	bool is_success_post = false;
public:
	BoardConnect() {};
	void initialize(const BitBoard& board_goal, const bool is_network);
	void initialize(const BitBoard& board_start, const BitBoard& board_goal, const bool is_network);
	JSON get_json(void) const;
	void update(void);
	void draw(void) const;
};
Vec2 BoardConnect::calc_piece_pos(int row, int col) const {
	return Vec2{ col * this->piece_size, row * this->piece_size };
}
void BoardConnect::initialize(const BitBoard& board_goal, const bool is_network) {
	this->height = board_goal.height();  this->width = board_goal.width();
	Array<int> goal_1d;
	for (int idx = 0; idx < this->height*this->width; idx++) {
		goal_1d << board_goal[idx / this->width][idx % this->width];
	}
	goal_1d.shuffle();
	BitBoard board_start(this->height, this->width);
	for (int idx = 0; idx < this->height * this->width; idx++) {
		board_start.set(idx/this->width, idx%this->width, goal_1d[idx]);
	}
	initialize(board_start, board_goal, is_network);
}
void BoardConnect::initialize(const BitBoard& board_start, const BitBoard& board_goal, const bool is_network) {
	set_piece_colors();
	this->is_network = is_network;
	this->height = board_goal.height();  this->width = board_goal.width();
	this->piece_size = calc_piece_size();
	this->board = BitBoard(this->height, this->width);
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			set_piece(board_start[row][col], row, col);
		}
	}
	this->solver_task.initialize(board_start, board_goal);
	update_board_texture();
}


JSON BoardConnect::get_json(void) const {
	return this->datawriter.get_json();
}

void BoardConnect::update(void) {
	if (this->cnt_move != this->solver_task.get_op_num() and not this->is_finished) {
		Array<int> op = this->solver_task.get_op();
		if (op[0] == -1 and op[1] == -1 and op[2] == -1 and op[3] == -1) {
			this->is_finished = true;
			if (this->is_network) {
				Connect connect;
				this->is_success_post = connect.post_answer(this->datawriter.get_json());
				this->datawriter.get_json().save(U"./answer.json");
			}
		}else if (op[0] == -2 and op[1] == -2 and op[2] == -2 and op[3] == -2) {
			String error_message;
			this->solver_task.get_child().istream() >> error_message;
			Console << error_message;
		}else {
			this->move(op[0], op[2], op[1], (Dir)op[3], true);
		}
	}
}
void BoardConnect::draw(void) const {
	draw_board();
	draw_details();
	if (this->is_finished and this->is_success_post) {
		this->font( this->is_network ? U"post is successful!" : U"Done!").drawAt(Vec2{Scene::Center().x, Scene::Size().y * 13.0 / 15.0}, Palette::Black);
	}
}


