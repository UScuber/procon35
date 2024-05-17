# include <Siv3D.hpp> // Siv3D v0.6.14
# include "Piece.hpp"
# include "Board.hpp"
# include "Patterns.hpp"

void Main(){

	Window::SetFullscreen(false);
	Scene::SetBackground(Palette::White);
	Board board;
	while (System::Update()) {
		board.update();
		board.draw();
	}
}






