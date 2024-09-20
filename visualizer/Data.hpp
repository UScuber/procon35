#pragma once
# include <Siv3D.hpp>
# include "Common.hpp"


using App = SceneManager<String>;

class DataWriter {
private:
	JSON json;
	int n = 0;
public:
	DataWriter(void);
	void add_op(const int pattern_idx, const Point& pos, const Dir dir);
	JSON get_json(void) const;
};
DataWriter::DataWriter(void) {
	json[U"n"] = 0;
	json[U"ops"] = Array<JSON>(0);
}
void DataWriter::add_op(const int pattern_idx, const Point& pos, const Dir dir) {
	JSON tmp;
	tmp[U"p"] = pattern_idx;
	tmp[U"x"] = pos.x;  tmp[U"y"] = pos.y;
	tmp[U"s"] = static_cast<int>(dir);
	json[U"ops"].push_back(tmp);
	n++;
	json[U"n"] = n;
}
JSON DataWriter::get_json(void) const {
	return this->json;
}

class DataLoader {
private:
	Array<Color> piece_colors;
public:
	DataLoader(void);
	Color get_piece_color(int idx) const;
};

DataLoader::DataLoader(void) {
	JSON json = JSON::Load(U"./pieceColors.json");
	this->piece_colors << json[U"pieceColor1"].get<HSV>();
	this->piece_colors << json[U"pieceColor2"].get<HSV>();
	this->piece_colors << json[U"pieceColor3"].get<HSV>();
	this->piece_colors << json[U"pieceColor4"].get<HSV>();
}
Color DataLoader::get_piece_color(int idx) const {
	assert(0 <= idx and idx < 4);
	return this->piece_colors[idx];
}

