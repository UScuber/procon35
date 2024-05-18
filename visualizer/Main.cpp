# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Piece.hpp"
# include "Board.hpp"
# include "Patterns.hpp"

void Main(){

	Window::Resize(1280, 720);
	Scene::SetBackground(Palette::White);
	Board board_example(false, U"./example.txt");
	Board board_operator(true, board_example.get_board());
	while (System::Update()) {
		board_operator.update();
		board_example.draw();
		board_operator.draw();
	}
}






