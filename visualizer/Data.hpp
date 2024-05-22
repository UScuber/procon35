# include <Siv3D.hpp>
# include "Slide.hpp"

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
	json[U"ops"] = Array<bool>(0);
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
