# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Data.hpp"
# include "Board.hpp"

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
	this->board_example.update(this->board_operator);
	this->board_operator.update();
}
void ManualScene::draw(void) const {
	this->board_example.draw();
	this->board_operator.draw(this->board_example);
}



class SelectScene : public App::Scene {
public:
	SelectScene(const InitData& init) : IScene{ init } {}
	void update(void) override;
	void draw(void) const override;
};
void SelectScene::update(void) {
	if (SimpleGUI::ButtonAt(U"手動", Scene::CenterF())) {
		changeScene(U"ManualScene");
	}
}
void SelectScene::draw(void)const {

}

void Main() {

	//Window::Resize(1280, 720);
	Window::SetFullscreen(true);
	Scene::SetBackground(Palette::White);

	App manager;
	manager.add<SelectScene>(U"SelectScene");
	manager.add<ManualScene>(U"ManualScene");

	while (System::Update()) {
		if (not manager.update()) {
			break;
		}
	}
}



