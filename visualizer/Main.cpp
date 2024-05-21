# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Piece.hpp"
# include "Board.hpp"
# include "Patterns.hpp"

void Main(){

	Window::Resize(1280, 720);
	Scene::SetBackground(Palette::White);
	FilePath path = U"./example.txt";
	BoardExample board_example(path);
	BoardOperate board_operator(board_example.get_board());
	while (System::Update()) {
		board_example.update(board_operator);
		board_operator.update();
		board_example.draw();
		board_operator.draw();
	}
}






