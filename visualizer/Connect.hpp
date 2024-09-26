# pragma once
# include <Siv3D.hpp>

void output_console_fail(const String& str) {
	Console << U"------";
	Console << str << U" failed!!";
	Console << U"------";
}

class Connect {
private:
	URL url_base;
	String token;
	const HashTable<String, String> headers{ { U"Content-Type", U"application/json" } };
	JSON get_problem_response_json;
	JSON post_answer_response_json;
public:
	Connect(void);
	bool get_problem(void);
	bool post_answer(const JSON& json);
	Array<Array<int>> get_problem_board_start(void) const;
	Array<Array<int>> get_problem_board_goal(void) const;
};

Connect::Connect(void) {
	// 接続先のURLをローカルファイルから取得
	TextReader reader_url{ U"./url.env" };
	if (not reader_url) {
		throw Error{ U"Failed to open 'url.env'" };
	}
	reader_url.readLine(url_base);
	// solver.exeのチームトークンをローカルファイルから取得
	TextReader reader_token{ U"./token.env" };
	if (not reader_token) {
		throw Error{ U"Failed to open 'token.env'" };
	}
	reader_token.readLine(token);
}

bool Connect::get_problem(void) {
	const URL url = url_base + U"/problem?token=" + this->token;
	const FilePath save_file_path = U"./problem.json";
	if (const auto response = SimpleHTTP::Get(url, this->headers, save_file_path)) {
		if (response.isOK()) {
			this->get_problem_response_json = JSON::Load(save_file_path);
			return true;
		}
		else {
			output_console_fail(U"status code:{} \t get problem"_fmt((int)response.getStatusCode()));
		}
	}
	output_console_fail(U"get problem");
	return false;
}

bool Connect::post_answer(const JSON& json) {
	const URL url = url_base + U"/answer?token=" + this->token;
	const FilePath save_file_path = U"./answer.json";
	const std::string data = json.formatUTF8();
	if (const auto response = SimpleHTTP::Post(url, this->headers, data.data(), data.size(), save_file_path)) {
		if (response.isOK()) {
			this->post_answer_response_json = JSON::Load(save_file_path);
			return true;
		}
		else {
			output_console_fail(U"status code:{} \t post answer"_fmt((int)response.getStatusCode()));
		}
	}
	output_console_fail(U"post answer");
	return false;
}

Array<Array<int>> Connect::get_problem_board_start(void) const {
	const int width = this->get_problem_response_json[U"board"][U"width"].get<int>();
	const int height = this->get_problem_response_json[U"board"][U"height"].get<int>();
	Array<Array<int>> res(height, Array<int>(width));
	for (auto&& [key, value] : this->get_problem_response_json[U"board"][U"start"]) {
		int i = Parse<int>(key);
		std::string str = value.format().narrow();
		// 取得した数値の文字列にはダブルクォーテーションが含まれている
		for (int j = 0; j < width; j++) {
			res[i][j] = (int)str[j+1] - (int)U'0';
		}
	}
	return res;
}

Array<Array<int>> Connect::get_problem_board_goal(void) const {
	const int width = this->get_problem_response_json[U"board"][U"width"].get<int>();
	const int height = this->get_problem_response_json[U"board"][U"height"].get<int>();
	Array<Array<int>> res(height, Array<int>(width));
	for (auto&& [key, value] : this->get_problem_response_json[U"board"][U"goal"]) {
		int i = Parse<int>(key);
		std::string str = value.format().narrow();
		// 取得した数値の文字列にはダブルクォーテーションが含まれている
		for (int j = 0; j < width; j++) {
			res[i][j] = (int)str[j + 1] - (int)U'0';
		}
	}
	return res;
}
