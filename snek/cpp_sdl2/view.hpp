#pragma once

#include <memory>

namespace Snek
{

class App;
class Game;
class NumberDraw;
class SpriteDraw;
class Texture;

class View
{
public:
	View(App *app, Game *game);

	void render();

private:
	App *app;
	Game *game;
	std::shared_ptr<NumberDraw> numbers;
	std::shared_ptr<SpriteDraw> sprites;
	std::shared_ptr<Texture> scoreLabel;

	void renderGamefield();
	void renderScore();
};

}
