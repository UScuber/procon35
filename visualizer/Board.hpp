# pragma once
# include <Siv3D.hpp>
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
	double piece_size = 15;	 // 一つのピースの描画サイズ
	Array<Array<int>> board;  // 2次元配列のボードz
	Patterns patterns;  // 型抜きの配列
	Array<Color> piece_colors = {Palette::Red, Palette::Green, Palette::Blue, Palette::Black};
	Array<Array<bool>> is_selected;  // ボード上のピースが選択されてるか
	int cnt_move = 0;  // 手数カウント
	Anchor anchor = Anchor::Lefttop;  // 中心軸
	const Font font{ 50, Typeface::Bold };  // 詳細表示用フォント
	const Font font_arrows{ 100, Typeface::Bold };  // 詳細表示用フォント
	DataWriter datawriter;  // 行動ログ保存用
	bool is_wasd_piece = true;  // wasdキーがピース選択になっているか
	Point selected_pos{ 0,0 };  // 選択ピースの座標
	int anchor_icon_size = Scene::Size().y / 20;  // 錨アイコンの大きさ
	Texture anchor_icon{0xf13d_icon, anchor_icon_size};  // 錨アイコン
	//////////////////////////////////////////////////////////////
	// method
	void initialize(const Array<Array<int>>& board); // 初期化
	void initialize_noshuffle(const Array<Array<int>>& board); // 初期化
	void initialize(const FilePath& path);
	bool is_in_board(const Point& pos) const;  // ボード上に収まっているか
	bool is_in_board(const int y, const int x) const;
	virtual Vec2 calc_piece_pos(int row, int col) const = 0;  // ピースの座標計算
	Point calc_lefttop(void) const;  // 中心軸を考慮した左上座標
	void set_selected_pos(const Point& pos);  // 選択ピースの座標取得
	Point get_selected_pos(void) const;  // 選択ピースの座標取得
	Optional<Point> which_hover(void) const;  // どこのピースがホバーされているかを返す
	Array<Array<int>> get_board(void) const;  // ボードを取得する
	int get_piece(const int y, const int x) const; // 座標を指定してピースを取得する
	void set_piece(const int num, const int y, const int x);  // 座標を指定してピースをセット
	Color get_piece_color(const int num) const;  // ピースの番号から色を取得する
	void set_piece_size(const int size);  // ピースサイズを設定
	void save_json(const FilePath& path) const;  // 行動ログのjsonを保存
	void set_piece_colors(void);
	//////////////////////////////////////////////////////////////
	// update
	void switch_wasd(void);  // wasdキーの役割変更
	void change_anchor(void);   // 中心軸を変更
	void select_piece(void);  // ホバーで選択状態する
	void move(void);  // 型抜きの移動
	void move(int p, int x, int y, Dir dir, bool enable_json);  // 指定して移動
	void move(int p, const Point& pos, Dir dir, bool enable_json);
	//////////////////////////////////////////////////////////////
	// draw
	void draw_board(void) const;  // ピース群の描画
	void draw_selected(void) const;  // 選択されているピースを黒く表示
	void draw_details(const Board &board) const;  // 詳細表示
};

void Board::initialize(const Array<Array<int>>& board) {
	set_piece_colors();
	this->height = board.size();
	this->width = board.front().size();
	this->piece_size = 480.0 / Max(this->height, this->width);
	this->board.resize(height, Array<int>(width));
	this->is_selected.resize(height, Array<bool>(width, false));
	Array<int> tmp;
	for (int i = 0; i < height * width; i++) {
		tmp << board[i / width][i % width];
	}
	tmp.shuffle();
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			int num = tmp[row * this->height + col];
			set_piece(num, row, col);
		}
	}
}
void Board::initialize_noshuffle(const Array<Array<int>>& board){
	set_piece_colors();
	this->height = board.size();
	this->width = board.front().size();
	this->piece_size = 480.0 / Max(this->height, this->width);
	this->board.resize(height, Array<int>(width));
	this->is_selected.resize(height, Array<bool>(width, false));
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			set_piece(board[row][col], row, col);
		}
	}
}
void Board::initialize(const FilePath& path) {
	set_piece_colors();
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
	this->piece_size = 480.0 / Max(this->height, this->width);
	this->board.resize(height, Array<int>(width));
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			set_piece(tmp[row][col], row, col);
		}
	}
	this->is_selected.resize(height, Array<bool>(width, false));
}


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
Array<Array<int>> Board::get_board(void) const {
	return this->board;
}
int Board::get_piece(const int y, const int x) const {
	return this->board[y][x];
}
void Board::set_piece(const int num, const int y, const int x) {
	this->board[y][x] = num;
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
	this->move(this->patterns.get_pattern_idx(), pos.x, pos.y, dir, true);
}
void Board::move(int p, int x, int y, Dir dir, bool enable_json){
	this->move(p, Point{ x,y }, dir, enable_json);
}
void Board::move(int p, const Point& pos, Dir dir, bool enable_json) {
	// スライドさせる
	Array<Array<int>> slided = slide(get_board(), this->patterns.get_pattern(p), pos, dir);
	// スライド後の番号をピースに適用
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			this->board[row][col] = slided[row][col];
		}
	}
	this->cnt_move++;
	// 行動ログを追加
	if(enable_json) this->datawriter.add_op(patterns.get_pattern_idx(), pos, dir);
}

void Board::draw_board(void) const {
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			RectF{ calc_piece_pos(row, col), this->piece_size }.draw(get_piece_color(get_piece(row, col)));
		}
	}
}
void Board::draw_selected(void) const {
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			if (is_selected[row][col]) {
				RectF{ calc_piece_pos(row, col), this->piece_size }.draw(ColorF{ 0.0, 0.5 });
			}
		}
	}
}
void Board::draw_details(const Board &board) const {
	int cnt_lack = 0;
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			if (this->board[row][col] != board.get_piece(row, col)) {
				cnt_lack++;
			}
		}
	}
	int piece_sum = this->height * this->width;
	assert(this->height == board.height and this->width == board.width);
	font(U"手数:{}"_fmt(cnt_move)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y / 10 }, Palette::Black);
	font(U"不一致数:{}"_fmt(cnt_lack)).drawAt(Vec2{ Scene::Center().x,  Scene::Size().y / 10 * 2 }, Palette::Black);
	font(U"一致率:{:.0f}%"_fmt((double)(piece_sum - cnt_lack) / (double)piece_sum * 100.0)).drawAt(Vec2{ Scene::Center().x, Scene::Size().y / 10 * 3 }, Palette::Black);
}



class BoardOperate : public Board {
private:
	Vec2 calc_piece_pos(int row, int col) const override;
public:
	BoardOperate(void) {};
	void update(void);
	void draw(const Board& board) const;
};
Vec2 BoardOperate::calc_piece_pos(int row, int col) const {
	return Vec2{ col * this->piece_size, row * this->piece_size };
}
void BoardOperate::update(void) {
	switch_wasd();
	change_anchor();
	patterns.update(not this->is_wasd_piece);
	select_piece();
	move();
}
void BoardOperate::draw(const Board& board) const {
	Vec2 anchor_rects{ Scene::Center().x, Scene::Size().y * 2 / 5 };
	RectF{ Arg::bottomRight(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	RectF{ Arg::topRight(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	RectF{ Arg::topLeft(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	RectF{ Arg::bottomLeft(anchor_rects), (double)anchor_icon_size }.drawFrame(5, Palette::Black);
	Vec2 anchor_pos = anchor_rects;
	if (this->anchor == Anchor::Lefttop) anchor_pos += Vec2{ -anchor_icon_size / 2, -anchor_icon_size / 2 };
	else if (this->anchor == Anchor::Leftbottom) anchor_pos += Vec2{ -anchor_icon_size / 2, anchor_icon_size / 2 };
	else if (this->anchor == Anchor::Rightbottom) anchor_pos += Vec2{ anchor_icon_size / 2, anchor_icon_size / 2 };
	else if (this->anchor == Anchor::Righttop) anchor_pos += Vec2{ anchor_icon_size / 2, -anchor_icon_size / 2 };
	anchor_icon.scaled(0.8).drawAt(anchor_pos, Palette::Black);
	String arrows = (is_wasd_piece) ? U"↑↑↑↑↑" : U"↓↓↓↓↓";
	font_arrows(arrows).drawBaseAt(Vec2{ Scene::Center().x, Scene::Size().y * 3.75 / 5 }, (is_wasd_piece) ? Palette::Blue : Palette::Red);
	patterns.draw();
	draw_board();
	draw_selected();
	draw_details(board);
}


class BoardExample : public Board {
private:
	Vec2 calc_piece_pos(int row, int col) const override;
public:
	//void initialize(const FilePath& path);
	BoardExample(void) {};
	void update(Board & board);
	void draw(void) const;
};
Vec2 BoardExample::calc_piece_pos(int row, int col) const {
	return Vec2{ Scene::Size().x - this->width * piece_size + col * this->piece_size, row * this->piece_size };
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
	Vec2 calc_piece_pos(int row, int col) const override;
	ChildProcess child;
	Array<Array<int>> board_origin;
	int ops_idx = 0;
	int turn_ms;
	Stopwatch stopwatch;
	void update_board(void);
	void update_gui(void);
public:
	void initialize(const Array<Array<int>>& board);
	void initialize(const Array<Array<int>>& board_start, const Array<Array<int>>& board_goal);
	JSON get_json(void) const;
	BoardAuto(void) {};
	void update(void);
	void draw(const Board& board) const;
};
Vec2 BoardAuto::calc_piece_pos(int row, int col) const {
	return Vec2{ col * this->piece_size, row * this->piece_size };
}
void BoardAuto::initialize(const Array<Array<int>>& board) {
	set_piece_colors();
	this->child = ChildProcess{ U"./solver.exe", Pipe::StdInOut };
	if (not child) throw Error{ U"Failed to create a process" };
	this->height = board.size();
	this->width = board.front().size();
	this->piece_size = 480.0 / Max(this->height, this->width);
	this->board.resize(height, Array<int>(width));
	this->board_origin.resize(height, Array<int>(width));
	this->is_selected.resize(height, Array<bool>(width, false));
	Array<int> tmp;
	for (int i = 0; i < height * width; i++) {
		tmp << board[i / width][i % width];
	}
	tmp.shuffle();
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			int num = tmp[row * this->height + col];
			set_piece(num, row, col);
		}
	}
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			this->board_origin[row][col] =  get_piece(row, col);
		}
	}
	// プロセスに入力を与える
	this->child.ostream() << this->height << std::endl << this->width << std::endl;
	for (int row = 0; row < this->height; row++) {
		String input = U"";
		for (int col = 0; col < this->width; col++) {
			input += Format(this->get_piece(row, col));
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	for (int row = 0; row < this->height; row++) {
		String input = U"";
		for (int col = 0; col < this->width; col++) {
			input += Format(board[row][col]);
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	int n,  p, x, y, s;
	this->child.istream() >> n;
	for (int i = 0; i < n; i++) {
		this->child.istream() >> p >> y >> x >> s;
		this->datawriter.add_op(p, Point{ x,y }, (Dir)s);
	}
	this->datawriter.get_json().save(U"./tmp.json");
}

void BoardAuto::initialize(const Array<Array<int>>& board_start, const Array<Array<int>>& board_goal) {
	Console << board_start;
	Console << board_goal;
	set_piece_colors();
	this->child = ChildProcess{ U"./solver.exe", Pipe::StdInOut };
	if (not child) throw Error{ U"Failed to create a process" };
	this->height = board_start.size();
	this->width = board_start.front().size();
	this->piece_size = 480.0 / Max(this->height, this->width);
	this->board.resize(height, Array<int>(width));
	this->board_origin.resize(height, Array<int>(width));
	this->is_selected.resize(height, Array<bool>(width, false));
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			set_piece(board_start[row][col], row, col);
		}
	}
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			this->board_origin[row][col] = get_piece(row, col);
		}
	}
	// プロセスに入力を与える
	this->child.ostream() << this->height << std::endl << this->width << std::endl;
	for (int row = 0; row < this->height; row++) {
		String input = U"";
		for (int col = 0; col < this->width; col++) {
			input += Format(this->get_piece(row, col));
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	for (int row = 0; row < this->height; row++) {
		String input = U"";
		for (int col = 0; col < this->width; col++) {
			input += Format(board_goal[row][col]);
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	int n, p, x, y, s;
	this->child.istream() >> n;
	for (int i = 0; i < n; i++) {
		this->child.istream() >> p >> y >> x >> s;
		this->datawriter.add_op(p, Point{ x,y }, (Dir)s);
	}
	this->datawriter.get_json().save(U"./answer.json");
}

JSON BoardAuto::get_json(void) const {
	return this->datawriter.get_json();
}

void BoardAuto::update_board(void) {
	if (ops_idx >= this->datawriter.get_json()[U"n"].get<int>()) return;
	if (not this->stopwatch.isRunning()) {
		stopwatch.start();
	}
	else {
		if (stopwatch.ms() >= this->turn_ms) {
			int p, x, y, s;
			p = this->datawriter.get_json()[U"ops"][ops_idx][U"p"].get<int>();
			x = this->datawriter.get_json()[U"ops"][ops_idx][U"x"].get<int>();
			y = this->datawriter.get_json()[U"ops"][ops_idx][U"y"].get<int>();
			s = this->datawriter.get_json()[U"ops"][ops_idx][U"s"].get<int>();
			ops_idx++;
			move(p, x, y, (Dir)s, false);
			//Console << U"p:{}, x:{}, y:{}, s:{}"_fmt(p, x, y, s);
			stopwatch.restart();
		}
	}
}

void BoardAuto::update_gui(void) {
	static double slider_value_tmp = 1.0;
	SimpleGUI::SliderAt(U"{}ms"_fmt(this->turn_ms), slider_value_tmp, 0.001, 1.000, Vec2{ Scene::Center().x, Scene::Size().y * 0.85 }, 80, Scene::Size().x * 0.8);
	this->turn_ms = slider_value_tmp * 1000.0;
	if (SimpleGUI::Button(U"restart", Scene::Size() * 0.9)) {
		for (int row = 0; row < this->height; row++) {
			for (int col = 0; col < this->width; col++) {
				int num = this->board_origin[row][col];
				set_piece(num, row, col);
			}
		}
		this->ops_idx = 0;
		this->cnt_move = 0;
	}
}

void BoardAuto::update(void) {
	update_gui();
	update_board();
}
void BoardAuto::draw(const Board& board) const {
	draw_board();
	draw_details(board);
}



