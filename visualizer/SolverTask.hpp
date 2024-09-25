# pragma once
# include <Siv3D.hpp>
# include "Patterns.hpp"
# include "BitBoard.hpp"
# include "Data.hpp"

class SolverTask {
private:
	bool update();
	static void Update(SolverTask* solver_task);
	AsyncTask<void> task;
	std::atomic<bool> m_abort{ false };
	std::atomic<size_t>  op_num = 0;
	bool is_outputed = false;
	bool is_finished_flg = false;
	ChildProcess child;
	//////////////////////////////
	// mutexで保護する
	std::mutex m_mutex;
	std::queue<Array<int>> que_ops;
public:
	SolverTask() {};
	~SolverTask();
	void initialize(const BitBoard& board_start, const BitBoard& board_goal);
	size_t get_op_num(void) const;
	bool is_finished(void) const;
	Array<int> get_op(void);
};

void SolverTask::initialize(const BitBoard& board_start, const BitBoard& board_goal) {
	// wslのユーザ名読み込み
	TextReader reader_wsl_user{ U"./wsl_user.env" };
	if(not reader_wsl_user)  throw Error{ U"Failed to open 'token.env'" };
	String wsl_user;
	reader_wsl_user.readLine(wsl_user);
	// プロセス生成
	this->child = ChildProcess{ U"C:\\Windows\\System32\\wsl.exe", U"/home/{}/solver"_fmt(wsl_user), Pipe::StdInOut};
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

bool SolverTask::update() {
	int p, x, y, s;
	this->child.istream() >> p >> y >> x >> s;
	if (p == -1 and y == -1 and x == -1 and s == -1) {
		this->is_finished_flg = true;
		return true;
	}
	std::lock_guard lock{ this->m_mutex };
	this->que_ops.push(Array<int>({ p, y, x, s }));
	this->is_outputed = false;
	this->op_num++;
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
bool SolverTask::is_finished(void) const {
	return not this->child.isValid();
}
Array<int> SolverTask::get_op(void) {
	std::lock_guard lock{ this->m_mutex };
	Array<int> res = this->que_ops.front();
	this->que_ops.pop();
	return res;
}

