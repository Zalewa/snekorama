#include "view.hpp"

#include "app.hpp"
#include "error.hpp"
#include "game.hpp"
#include <SDL.h>
#include <algorithm>
#include <map>

using namespace std::string_literals;

namespace Snek
{

class Texture
{
public:
	Texture(SDL_Renderer *renderer, std::string filename)
	{
		std::string surfacePath = SDL_GetBasePath() + filename;
		SDL_Surface *surface = SDL_LoadBMP(surfacePath.c_str());
		if (surface == nullptr)
			throw Error("cannot load texture surface: "s + SDL_GetError());
		SDL_SetColorKey(surface, SDL_TRUE, 0);
		this->texture_ = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
		if (this->texture_ == nullptr)
			throw Error("cannot create texture: "s + SDL_GetError());
		if (SDL_QueryTexture(this->texture_, nullptr, nullptr,
				&this->width_, &this->height_) != 0)
		{
			std::string error = "cannot query texture: "s + SDL_GetError();
			SDL_DestroyTexture(this->texture_);
			this->texture_ = nullptr;
			throw Error(error);
		}
	}

	~Texture()
	{
		if (this->texture_ != nullptr)
			SDL_DestroyTexture(this->texture_);
	}

	int width() const { return width_; }
	int height() const { return height_; }
	SDL_Texture *sdl() { return texture_; }

private:
	SDL_Texture *texture_;
	int width_;
	int height_;
};

static const int FIELD_SIZE = 32;

typedef SDL_Rect Sprite;

constexpr static SDL_Rect mkfield(int col, int row)
{
	return {col * FIELD_SIZE, row * FIELD_SIZE, FIELD_SIZE, FIELD_SIZE};
}

constexpr static Sprite mksprite(int col, int row)
{
	return {col * FIELD_SIZE, row * FIELD_SIZE, FIELD_SIZE, FIELD_SIZE};
}

namespace Sprites
{
const Sprite SNEK_BODY = mksprite(0, 0);
const Sprite SNEK_HEAD = mksprite(1, 0);
const Sprite APPLE = mksprite(0, 1);
const Sprite BRICK = mksprite(1, 1);
const Sprite SNEK_DEAD_HEAD = mksprite(2, 0);

static auto mkEntitySprites()
{
	std::map<Entity::EntityType, const Sprite*> m = {
		{ Entity::EntityType::APPLE, &APPLE },
		{ Entity::EntityType::BODY, &SNEK_BODY },
		{ Entity::EntityType::BRICK, &BRICK },
		{ Entity::EntityType::HEAD, &SNEK_HEAD }
	};
	return m;
}

const std::map<Entity::EntityType, const Sprite*> entitySprites = mkEntitySprites();
}

class SpriteDraw
{
public:
	SpriteDraw(SDL_Renderer *renderer)
		: renderer(renderer)
	{
	}

	void draw(const Sprite &sprite, const SDL_Rect &destination, double angle)
	{
		SDL_RenderCopyEx(this->renderer, get(),
			&sprite, &destination,
			angle, nullptr, SDL_FLIP_NONE);
	}

private:
	SDL_Renderer *renderer;
	std::unique_ptr<Texture> sprites;

	SDL_Texture *get()
	{
		if (this->sprites == nullptr)
			this->sprites = std::make_unique<Texture>(this->renderer, "sprites.bmp");
		return this->sprites->sdl();
	}
};

class NumberDraw
{
public:
	NumberDraw(SDL_Renderer *renderer)
		: renderer(renderer)
	{
	}

	void draw(long number, const SDL_Rect &dst)
	{
		draw(std::to_string(number), dst);
	}

	void draw(const std::string &number, const SDL_Rect &dst)
	{
		if (number.empty())
			return;

		int fullSizeWidth = oneDigitWidth() * number.size();
		int renderedDigitWidth = oneDigitWidth();
		if (fullSizeWidth > dst.w)
			renderedDigitWidth = dst.w / number.length();

		SDL_Texture *texture = get()->sdl();
		for (unsigned digitIdx = 0; digitIdx < number.size(); ++digitIdx)
		{
			char digit = number[digitIdx];
			if (digit < '0' || digit > '9')
				throw Error("tried to render a non-digit as a number");
			int offset = digitIdx * renderedDigitWidth;
			SDL_Rect digitRect = this->digit(static_cast<int>(digit - '0'));
			SDL_Rect destination = {dst.x + offset, dst.y, renderedDigitWidth, dst.h};
			SDL_RenderCopy(this->renderer, texture,
				&digitRect, &destination);
		}
	}

private:
	std::unique_ptr<Texture> digits;
	SDL_Renderer *renderer;

	Texture *get()
	{
		if (this->digits == nullptr)
			this->digits = std::make_unique<Texture>(this->renderer, "digits.bmp");
		return this->digits.get();
	}

	SDL_Rect digit(int digit)
	{
		static int height = get()->height();
		int x = digit * oneDigitWidth();
		return {x, 0, oneDigitWidth(), height };
	}

	int oneDigitWidth()
	{
		// There are 10 digits on the texture.
		return get()->width() / 10;
	}
};

View::View(App *app, Game *game)
	: app(app), game(game)
{
	this->numbers = std::make_shared<NumberDraw>(app->renderer);
	this->sprites = std::make_shared<SpriteDraw>(app->renderer);
	this->scoreLabel = std::make_shared<Texture>(app->renderer, "score.bmp");
}

void View::render()
{
	SDL_SetRenderDrawColor(app->renderer, 0, 0x0, 0x80, 0xff);
	SDL_RenderClear(app->renderer);

	renderScore();
	renderGamefield();

	SDL_RenderPresent(app->renderer);
}

void View::renderGamefield()
{
	for (auto &entity : this->game->entities())
	{
		const Sprite *sprite = Sprites::entitySprites.at(entity->type());
		if (entity->type() == Entity::EntityType::HEAD &&
			this->game->state() == Game::State::DEAD)
		{
			sprite = &Sprites::SNEK_DEAD_HEAD;
		}
		if (sprite == nullptr)
			throw Error("cannot find sprite for this entity type");
		SDL_Rect target = mkfield(entity->position.x, entity->position.y);
		const Vec2 velocity = entity->velocity;
		double rotation = 0.0;
		if (velocity.x > 0)
			rotation = 90.0;
		else if (velocity.x < 0)
			rotation = 270.0;
		else if (velocity.y > 0)
			rotation = 180.0;
		this->sprites->draw(*sprite, target, rotation);
	}
}

void View::renderScore()
{
	SDL_SetRenderDrawColor(this->app->renderer, 0, 0, 0, 0xff);
	SDL_Rect panelRect = {0, 512, 512, 640 - 512};
	SDL_RenderFillRect(this->app->renderer, &panelRect);

	SDL_Rect scoreRect = {0, 512, 256, 128};
	SDL_RenderCopy(this->app->renderer, this->scoreLabel->sdl(),
		nullptr, &scoreRect);

	this->numbers->draw(this->game->score(), {256, 540, 256, 80});
}

}
