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
public:
	Connect(void);
	Optional<JSON> get_problem(void) const;
	Optional<JSON> post_answer(const JSON& json) const;
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

Optional<JSON> Connect::get_problem(void) const {
	const URL url = url_base + U"/problem?token=" + this->token;
	const FilePath save_file_path = U"./problem.json";
	if (const auto response = SimpleHTTP::Get(url, this->headers, save_file_path)) {
		if (response.isOK()) {
			const JSON response_json = JSON::Load(save_file_path);
			Console << response_json;
			return response_json;
		}
		output_console_fail(U"status code:{} \t get problem"_fmt((int)response.getStatusCode()));
	}
	output_console_fail(U"get problem");
	return none;
}

Optional<JSON> Connect::post_answer(const JSON& json) const {
	const URL url = url_base + U"/problem?token=" + this->token;
	const FilePath save_file_path = U"./answer.json";
	const std::string data = json.formatUTF8();
	if (const auto response = SimpleHTTP::Post(url, this->headers, data.data(), data.size(), save_file_path)) {
		if (response.isOK()) {
			const JSON response_json = JSON::Load(save_file_path);
			Console << response_json;
			return response_json;
		}
		output_console_fail(U"status code:{} \t post answer"_fmt((int)response.getStatusCode()));
	}
	output_console_fail(U"post answer");
	return none;
}

