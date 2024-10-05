# pragma once
# include <Siv3D.hpp>
# include "Patterns.hpp"
# include "BitBoard.hpp"
# include "Data.hpp"

class SolverTask {
private:
	bool update(void);
	static void Update(SolverTask* solver_task);
	AsyncTask<void> task;
	std::atomic<bool> m_abort{ false };
	std::atomic<size_t>  op_num = 0;
	ChildProcess child;
	//////////////////////////////
	// mutexで保護する
	std::mutex m_mutex;
	std::queue<Array<int>> que_ops;
public:
	SolverTask() {};
	~SolverTask();
	void initialize(const BitBoard& board_start, const BitBoard& board_goal, const int option_number);
	size_t get_op_num(void) const;
	Array<Array<int>> get_op(void);
	ChildProcess& get_child(void);
};

void SolverTask::initialize(const BitBoard& board_start, const BitBoard& board_goal, const	int option_number) {
	// wslのユーザ名読み込み
	TextReader reader_wsl_path{ U"./wsl_path.env" };
	if(not reader_wsl_path)  throw Error{ U"Failed to open 'wsl_path.env'" };
	String wsl_path;
	reader_wsl_path.readLine(wsl_path);
	// プロセス生成
	this->child = ChildProcess{ U"C:\\Windows\\System32\\wsl.exe", wsl_path, Pipe::StdInOut};
	if (not this->child) throw Error{ U"Failed to create a process" };

	// プロセスに入力を与える
	this->child.ostream() << board_start.height() << std::endl << board_start.width() << std::endl;
	for (int row = 0; row < board_start.height(); row++) {
		String input = U"";
		for (int col = 0; col < board_start.width(); col++) {
			input += Format(board_start[row][col]);
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	for (int row = 0; row < board_goal.height(); row++) {
		String input = U"";
		for (int col = 0; col < board_goal.width(); col++) {
			input += Format(board_goal[row][col]);
		}
		this->child.ostream() << input.narrow() << std::endl;
	}
	
	// 非同期処理
	this->task = Async(Update, this);

}

SolverTask::~SolverTask() {
	if (task.isValid()) {
		this->m_abort = true;
		this->task.wait();
	}
}

bool SolverTask::update(void) {
	int p, x, y, s;
	this->child.istream() >> p >> y >> x >> s;
	std::lock_guard lock{ this->m_mutex };
	//Console << U"{}\t{}\t{}\t{}"_fmt(p, y, x, s);
	this->que_ops.push(Array<int>({ p, y, x, s }));
	this->op_num++;
	if ((p == -1 and y == -1 and x == -1 and s == -1) or
		(p == -2 and y == -2 and x == -2 and s == -2)) {
		return true;
	}
	return false;
}

void SolverTask::Update(SolverTask* solver_task) {
	while (not solver_task->m_abort) {
		if (solver_task->update()) {
			break;
		}
	}
}

size_t SolverTask::get_op_num(void) const {
	return this->op_num;
}
Array<Array<int>> SolverTask::get_op(void) {
	Array<Array<int>> res;
	std::lock_guard lock{ this->m_mutex };
	while (not que_ops.empty()){
		res.push_back(this->que_ops.front());
		this->que_ops.pop();
	}
	return res;
}

ChildProcess& SolverTask::get_child(void) {
	return this->child;
}

