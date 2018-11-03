#include "app.hpp"

#include "audio.hpp"
#include "error.hpp"
#include "game.hpp"
#include "random.hpp"
#include "view.hpp"
#include <algorithm>
#include <string>
#include <iostream>

using namespace std::string_literals;

namespace Snek
{
App::App()
	: window(nullptr), renderer(nullptr), quit(false)
{
	initRandom();

	// Init SDL.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_AUDIO) != 0)
		throw Error("cannot init SDL: "s + SDL_GetError());

	// Init video.
	this->window = SDL_CreateWindow("Snek!",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		512, 640,
		SDL_WINDOW_SHOWN);
	if (this->window == nullptr)
		throw Error("cannot create window: "s + SDL_GetError());

	this->renderer = SDL_CreateRenderer(this->window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (this->renderer == nullptr)
		throw Error("cannot create renderer: "s + SDL_GetError());

	// Open joysticks.
	int numJoysticks = SDL_NumJoysticks();
	if (numJoysticks < 0)
		std::cerr << "it was impossible to obtain joysticks: " << SDL_GetError() << std::endl;
	for (int joystickIdx = 0; joystickIdx < numJoysticks; ++joystickIdx)
	{
		SDL_Joystick *joystick = SDL_JoystickOpen(joystickIdx);
		if (joystick != nullptr)
			joysticks_.push_back(joystick);
		else
		{
			std::cerr << "couldn't open joystick " << joystickIdx << ": "
				<< SDL_GetError() << std::endl;
		}
	}

	// Init audio.
	this->audio_ = std::make_shared<Audio>();
}

App::~App()
{
	std::for_each(joysticks_.begin(), joysticks_.end(), SDL_JoystickClose);
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
}

void App::run()
{
	this->game = std::make_unique<Game>(Vec2 {16, 16});
	View view(this, this->game.get());
	const unsigned TICK_FREQUENCY = 10;
	const unsigned TICK_PERIOD = 1000 / TICK_FREQUENCY;
	unsigned nextTick = SDL_GetTicks() + TICK_PERIOD;

	SDL_Event event;
	while (!this->quit)
	{
		while (SDL_PollEvent(&event) != 0)
			handleEvent(event);
		if (SDL_GetTicks() > nextTick)
		{
			nextTick = SDL_GetTicks() + TICK_PERIOD;
			game->tick();
		}
		this->audio_->play(game->popAudioRequests());
		view.render();
	}
}

void App::handleEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_QUIT:
		this->quit = true;
		break;
	case SDL_KEYUP:
	{
		auto keyboardEvent = reinterpret_cast<const SDL_KeyboardEvent&>(event);
		if (keyboardEvent.keysym.sym == SDLK_ESCAPE)
			this->quit = true;
		break;
	}
	}

	switch (this->game->state())
	{
	case Game::State::GAME:
		handleGameEvent(event);
		break;
	case Game::State::DEAD:
		handleDeadEvent(event);
		break;
	}
}

void App::handleDeadEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
	{
		auto keyboardEvent = reinterpret_cast<const SDL_KeyboardEvent&>(event);
		switch (keyboardEvent.keysym.sym)
		{
		case SDLK_RETURN:
		case SDLK_SPACE:
			this->game->nextLevel();
			break;
		}
		break;
	}
	case SDL_JOYBUTTONDOWN:
		this->game->nextLevel();
		break;
	}
}

void App::handleGameEvent(const SDL_Event &event)
{
	switch (event.type)
	{
	case SDL_KEYDOWN:
	{
		auto keyboardEvent = reinterpret_cast<const SDL_KeyboardEvent&>(event);
		switch (keyboardEvent.keysym.sym)
		{
		case SDLK_UP:
			this->game->changeDirection(Game::Direction::UP);
			break;
		case SDLK_DOWN:
			this->game->changeDirection(Game::Direction::DOWN);
			break;
		case SDLK_RIGHT:
			this->game->changeDirection(Game::Direction::RIGHT);
			break;
		case SDLK_LEFT:
			this->game->changeDirection(Game::Direction::LEFT);
			break;
		}
		break;
	}
	case SDL_JOYAXISMOTION:
	{
		static const int X_AXIS = 0;
		static const int Y_AXIS = 1;
		auto joyEvent = reinterpret_cast<const SDL_JoyAxisEvent&>(event);
		if (joyEvent.axis == X_AXIS)
		{
			if (joyEvent.value > 0)
				this->game->changeDirection(Game::Direction::RIGHT);
			else if (joyEvent.value < 0)
				this->game->changeDirection(Game::Direction::LEFT);
		}
		else if (joyEvent.axis == Y_AXIS)
		{
			// up-down is reversed
			if (joyEvent.value < 0)
				this->game->changeDirection(Game::Direction::UP);
			else if (joyEvent.value > 0)
				this->game->changeDirection(Game::Direction::DOWN);
		}
		break;
	}
	}
}

}
