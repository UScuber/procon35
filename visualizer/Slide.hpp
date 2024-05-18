# pragma once
# include <Siv3D.hpp>
# include "Piece.hpp"
# include "Patterns.hpp"

enum class Dir {
	U, L, D, R,
};

void left_slide(Array<Array<int>>& a, const Pattern& kata, const int posy, const int posx) {
	const int h = a.size();
	const int w = a[0].size();
	const int kata_sy = Max(0, -posy), kata_sx = Max(0, -posx);
	const int kata_ty = Min((int)kata.size(), h - posy), kata_tx = Min((int)kata[0].size(), w - posx);
	const int board_y = Max(0, posy) - kata_sy, board_x = Max(0, posx) - kata_sx;

	for (int i = kata_sy; i < kata_ty; i++) {
		auto& row = a[i + board_y];
		Array<int> picked_pieces;

		// pick
		for (int j = kata_sx; j < kata_tx; j++) {
			if (kata[i][j]) {
				picked_pieces.push_back(row[j + board_x]);
				row[j + board_x] = -1;
			}
		}

		// shift
		int picked_count = 0;
		for (int j = 0; j < w; j++) {
			if (row[j] < 0) {
				picked_count++;
			}
			else if (picked_count) {
				row[j - picked_count] = row[j];
				row[j] = -1;
			}
		}

		// put
		int put_count = 0;
		for (int j = 0; j < w; j++) {
			if (row[j] < 0) row[j] = picked_pieces[put_count++];
		}
	}
}

void right_slide(Array<Array<int>>& a, const Pattern& kata, const int posy, const int posx) {
	const int h = a.size();
	const int w = a[0].size();
	const int kata_sy = Max(0, -posy), kata_sx = Max(0, -posx);
	const int kata_ty = Min((int)kata.size(), h - posy), kata_tx = Min((int)kata[0].size(), w - posx);
	const int board_y = Max(0, posy) - kata_sy, board_x = Max(0, posx) - kata_sx;

	for (int i = kata_sy; i < kata_ty; i++) {
		auto& row = a[i + board_y];
		Array<int> picked_pieces;

		// pick
		for (int j = kata_tx - 1; j >= kata_sx; j--) {
			if (kata[i][j]) {
				picked_pieces.push_back(row[j + board_x]);
				row[j + board_x] = -1;
			}
		}

		// shift
		int picked_count = 0;
		for (int j = w - 1; j >= 0; j--) {
			if (row[j] < 0) {
				picked_count++;
			}
			else if (picked_count) {
				row[j + picked_count] = row[j];
				row[j] = -1;
			}
		}

		// put
		int put_count = 0;
		for (int j = w - 1; j >= 0; j--) {
			if (row[j] < 0) row[j] = picked_pieces[put_count++];
		}
	}
}

void up_slide(Array<Array<int>>& a, const Pattern& kata, const int posy, const int posx) {
	const int h = a.size();
	const int w = a[0].size();
	const int kata_sy = Max(0, -posy), kata_sx = Max(0, -posx);
	const int kata_ty = Min((int)kata.size(), h - posy), kata_tx = Min((int)kata[0].size(), w - posx);
	const int board_y = Max(0, posy) - kata_sy, board_x = Max(0, posx) - kata_sx;

	for (int j = kata_sx; j < kata_tx; j++) {
		Array<int> picked_pieces;

		// pick
		for (int i = kata_sy; i < kata_ty; i++) {
			if (kata[i][j]) {
				picked_pieces.push_back(a[i + board_y][j + board_x]);
				a[i + board_y][j + board_x] = -1;
			}
		}

		// shift
		int picked_count = 0;
		for (int i = 0; i < h; i++) {
			if (a[i][j + board_x] < 0) {
				picked_count++;
			}
			else if (picked_count) {
				a[i - picked_count][j + board_x] = a[i][j + board_x];
				a[i][j + board_x] = -1;
			}
		}

		// put
		int put_count = 0;
		for (int i = 0; i < h; i++) {
			if (a[i][j + board_x] < 0) a[i][j + board_x] = picked_pieces[put_count++];
		}
	}
}

void down_slide(Array<Array<int>>& a, const Pattern& kata, const int posy, const int posx) {
	const int h = a.size();
	const int w = a[0].size();
	const int kata_sy = Max(0, -posy), kata_sx = Max(0, -posx);
	const int kata_ty = Min((int)kata.size(), h - posy), kata_tx = Min((int)kata[0].size(), w - posx);
	const int board_y = Max(0, posy) - kata_sy, board_x = Max(0, posx) - kata_sx;

	for (int j = kata_sx; j < kata_tx; j++) {
		Array<int> picked_pieces;

		// pick
		for (int i = kata_ty - 1; i >= kata_sy; i--) {
			if (kata[i][j]) {
				picked_pieces.push_back(a[i + board_y][j + board_x]);
				a[i + board_y][j + board_x] = -1;
			}
		}

		// shift
		int picked_count = 0;
		for (int i = h - 1; i >= 0; i--) {
			if (a[i][j + board_x] < 0) {
				picked_count++;
			}
			else if (picked_count) {
				a[i + picked_count][j + board_x] = a[i][j + board_x];
				a[i][j + board_x] = -1;
			}
		}

		// put
		int put_count = 0;
		for (int i = h - 1; i >= 0; i--) {
			if (a[i][j + board_x] < 0) a[i][j + board_x] = picked_pieces[put_count++];
		}
	}
}



Array<Array<int>> slide(const Array<Array<int>>& a, const Pattern& kata, const Point& pos, const Dir dir) {
	const int h = a.size();
	const int w = a[0].size();

	assert(-(int)kata.size() < pos.y && pos.y < h && -(int)kata[0].size() < pos.x && pos.x < w);

	Array<Array<int>> result = a;

	if (dir == Dir::U) up_slide(result, kata, pos.y, pos.x);
	if (dir == Dir::L) left_slide(result, kata, pos.y, pos.x);
	if (dir == Dir::D) down_slide(result, kata, pos.y, pos.x);
	if (dir == Dir::R) right_slide(result, kata, pos.y, pos.x);

	return result;
}
