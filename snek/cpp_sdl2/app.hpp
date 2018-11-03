#pragma once

#include <SDL.h>
#include <list>
#include <memory>

namespace Snek
{

class Audio;
class Game;

class App
{
public:
	SDL_Window *window;
	SDL_Renderer *renderer;

	App();
	~App();

	void run();

private:
	std::shared_ptr<Audio> audio_;
	std::unique_ptr<Game> game;
	std::list<SDL_Joystick*> joysticks_;
	bool quit;

	void handleEvent(const SDL_Event &event);
	void handleDeadEvent(const SDL_Event &event);
	void handleGameEvent(const SDL_Event &event);
};

}
