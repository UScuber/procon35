

class BoardOperate : public Board {
private:
	Vec2 calc_piece_pos(int row, int col) const override;
    
	int anchor_icon_size = Scene::Size().y / 20;  // 錨アイコンの大きさ
	Texture anchor_icon{0xf13d_icon, anchor_icon_size};  // 錨アイコン
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


void Board::draw_selected(void) const {
	for (int row = 0; row < this->height; row++) {
		for (int col = 0; col < this->width; col++) {
			if (is_selected[row][col]) {
				RectF{ calc_piece_pos(row, col), this->piece_size }.draw(ColorF{ 0.0, 0.5 });
			}
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
	for (int row = pos.y; row < pos.y + (int)pattern.height(); row++) {
		for (int col = pos.x; col < pos.x + (int)pattern.width(); col++) {
			if (not is_in_board(row, col)) continue;
			this->is_selected[row][col] = pattern[row - pos.y][col - pos.x];
		}
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


Optional<Point> Board::which_hover(void) const {
	const Point mouse_pos = Cursor::Pos();
	Point res{ 0,0 };
	res.x = mouse_pos.x / this->piece_size;
	res.y = mouse_pos.y / this->piece_size;
	if (is_in_board(res)) return res;
	else return none;
}


void Board::set_selected_pos(const Point& pos) {
	this->selected_pos = pos;
}
Point Board::get_selected_pos(void) const {
	return this->selected_pos;
}


Point Board::calc_lefttop(void) const{
	if (this->anchor == Anchor::Lefttop) return this->selected_pos;
	else if (this->anchor == Anchor::Leftbottom) return  this->selected_pos - Point{ 0, this->patterns.get_pattern().height() - 1 };
	else if (this->anchor == Anchor::Rightbottom) return  this->selected_pos - Point{ this->patterns.get_pattern().width() - 1, this->patterns.get_pattern().height() - 1 };
	else if (this->anchor == Anchor::Righttop) return this->selected_pos - Point{ this->patterns.get_pattern().width() - 1, 0 };
}


