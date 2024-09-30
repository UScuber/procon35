# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Data.hpp"
# include "Board.hpp"
# include "Connect.hpp"

class AutoScene : public App::Scene {
private:
	BoardExample board_example;
	BoardConnect board_auto;
public:
	AutoScene(const InitData& init);
	void update(void) override;
	void draw(void) const override;
};
AutoScene::AutoScene(const InitData& init) : IScene{ init } {
	Optional<FilePath> path = Dialog::OpenFile({ FileFilter::Text() });
	if (not path) return;
	this->board_example.initialize(path.value());
	this->board_auto.initialize(this->board_example.get_board(), false);
}
void AutoScene::update(void) {
	if (KeyP.down()) changeScene(U"SelectScene");
	this->board_auto.update();
}
void AutoScene::draw(void) const {
	this->board_example.draw();
	this->board_auto.draw();
}

class ConnectScene : public App::Scene {
private:
	BoardExample board_example;
	BoardConnect board_connect;
	Connect connect;
public:
	ConnectScene(const InitData& init);
	void update(void) override;
	void draw(void) const override;
};
ConnectScene::ConnectScene(const InitData& init) : IScene{ init } {
	this->connect.get_problem();
	this->board_example.initialize(this->connect.get_problem_board_goal(), false);
	this->board_connect.initialize(this->connect.get_problem_board_start(), this->connect.get_problem_board_goal(), true);
}
void ConnectScene::update(void) {
	if (KeyP.down()) changeScene(U"SelectScene");
	this->board_connect.update();
}
void ConnectScene::draw(void) const {
	this->board_example.draw();
	this->board_connect.draw();
}



class SelectScene : public App::Scene {
public:
	SelectScene(const InitData& init) : IScene{ init } {}
	void update(void) override;
	void draw(void) const override;
};
void SelectScene::update(void) {
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

	App manager;
	manager.add<SelectScene>(U"SelectScene");
	manager.add<ConnectScene>(U"ConnectScene");
	manager.add<AutoScene>(U"AutoScene");


	while (System::Update()) {
		const ScopedRenderStates2D sampler{ SamplerState::ClampNearest };
		if (not manager.update()) {
			break;
		}
	}
}



