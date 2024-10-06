# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Data.hpp"
# include "Board.hpp"
# include "Connect.hpp"
# include "SushiGUI.hpp"

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
	const Font font{ 100, Typeface::Bold };
};
void SelectScene::update(void) {
	Arg::rightCenter_<Vec2> anchor_connect = Vec2{ Scene::CenterF().x * 0.90, Scene::Center().y };
	Arg::leftCenter_<Vec2> anchor_auto = Vec2{ Scene::CenterF().x * 1.1, Scene::Center().y };
	if (SushiGUI::button4(font, U"通信", anchor_connect, Size{ 200, 100})) {
		changeScene(U"ConnectScene");
	}
	if (SushiGUI::button4(font, U"自動", anchor_auto, Size{ 200, 100})) {
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



