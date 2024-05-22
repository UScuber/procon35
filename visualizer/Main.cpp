# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Piece.hpp"
# include "Board.hpp"
# include "Patterns.hpp"

void Main(){

	//Window::Resize(1280, 720);
	Window::SetFullscreen(true);
	Scene::SetBackground(Palette::White);

	Optional<FilePath> path = Dialog::OpenFile({ FileFilter::Text() });
	if (not path) return;
	BoardExample board_example(path.value());
	BoardOperate board_operator(board_example.get_board());
	while (System::Update()) {
		board_example.update(board_operator);
		board_operator.update();
		board_example.draw();
		board_operator.draw(board_example);
	}
	board_operator.save_json(U"./answer.json");
}






