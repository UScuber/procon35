# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Data.hpp"
# include "Board.hpp"
# include "Connect.hpp"

class ManualScene : public App::Scene {
private:
	BoardExample board_example;
	BoardOperate board_operator;
public:
	ManualScene(const InitData& init);
	~ManualScene(void);
	void update(void) override;
	void draw(void) const override;
};
ManualScene::ManualScene(const InitData& init) : IScene{ init } {
	Optional<FilePath> path = Dialog::OpenFile({ FileFilter::Text() });
	if (not path) return;
	this->board_example.initialize(path.value());
	this->board_operator.initialize(this->board_example.get_board());
}
ManualScene::~ManualScene(void) {
	this->board_operator.save_json(U"./answer.json");
}
void ManualScene::update(void) {
	if (KeyP.down()) changeScene(U"SelectScene");
	this->board_example.update(this->board_operator);
	this->board_operator.update();
}
void ManualScene::draw(void) const {
	this->board_example.draw();
	this->board_operator.draw(this->board_example);
}

class AutoScene : public App::Scene {
private:
	BoardExample board_example;
	BoardAuto board_auto;
public:
	AutoScene(const InitData& init);
	void update(void) override;
	void draw(void) const override;
};
AutoScene::AutoScene(const InitData& init) : IScene{ init } {
	Optional<FilePath> path = Dialog::OpenFile({ FileFilter::Text() });
	if (not path) return;
	this->board_example.initialize(path.value());
	this->board_auto.initialize(this->board_example.get_board());
}
void AutoScene::update(void) {
	if (KeyP.down()) changeScene(U"SelectScene");
	this->board_example.update(this->board_auto);
	this->board_auto.update();
}
void AutoScene::draw(void) const {
	this->board_example.draw();
	this->board_auto.draw(this->board_example);
}

class ConnectScene : public App::Scene {
private:
	BoardExample board_example;
	BoardAuto board_auto;
	Connect connect;
public:
	ConnectScene(const InitData& init);
	void update(void) override;
	void draw(void) const override;
};
ConnectScene::ConnectScene(const InitData& init) : IScene{ init } {
	const JSON problem_json = connect.get_problem();
	//Optional<FilePath> path = Dialog::OpenFile({ FileFilter::Text() });
	//if (not path) return;
	//this->board_example.initialize(path.value());
	//this->board_auto.initialize(this->board_example.get_board());
}


class SelectScene : public App::Scene {
public:
	SelectScene(const InitData& init) : IScene{ init } {}
	void update(void) override;
	void draw(void) const override;
};
void SelectScene::update(void) {
	if (SimpleGUI::ButtonAt(U"手動", Scene::CenterF()*0.9)) {
		changeScene(U"ManualScene");
	}
	if (SimpleGUI::ButtonAt(U"通信", Scene::CenterF() * 1.0)) {
		changeScene(U"ConnectScene");
	}
	if (SimpleGUI::ButtonAt(U"自動", Scene::CenterF() * 1.1)) {
		changeScene(U"AutoScene");
	}
}
void SelectScene::draw(void)const {

}

void Main() {

	Window::Resize(1280, 720);
	//Window::SetFullscreen(true);
	Scene::SetBackground(Palette::Gray);

	Connect connect;
	connect.get_problem();

	App manager;
	manager.add<SelectScene>(U"SelectScene");
	manager.add<ManualScene>(U"ManualScene");
	manager.add<ConnectScene>(U"ConnectScene");
	manager.add<AutoScene>(U"AutoScene");

	while (System::Update()) {
		if (not manager.update()) {
			break;
		}
	}
}



