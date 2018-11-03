#include "view.h"

#include <stdio.h>
#include <string.h>

#define MAX_PATH_LEN 65536
#define MAX_DIGITS 32

const SDL_Rect SPRITE_APPLE = {0, 32, 32, 32};
const SDL_Rect SPRITE_BODY = {0, 0, 32, 32};
const SDL_Rect SPRITE_BRICK = {32, 32, 32, 32};
const SDL_Rect SPRITE_HEAD = {32, 0, 32, 32};
const SDL_Rect SPRITE_HEAD_DEAD = {64, 0, 32, 32};

struct _SnekcView
{
	SDL_Renderer *renderer;

	SDL_Texture *texture_score;
	SDL_Texture *texture_digits;
	SDL_Texture *texture_sprites;

	int one_digit_width;
	int digit_height;
};

SDL_Texture *load_texture(SDL_Renderer *renderer, const char *name)
{
	const char *dir = SDL_GetBasePath();
	char path[MAX_PATH_LEN] = "";
	SDL_Surface *surface;
	SDL_Texture *texture;

	strcpy(path, dir);
	strcat(path, name);

	surface = SDL_LoadBMP(path);
	if (!surface)
		return NULL;
	SDL_SetColorKey(surface, SDL_TRUE, 0);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	return texture;
}

void render_digits(SnekcView *view, long score, SDL_Rect dst)
{
	int full_size_width;
	int rendered_digit_width;
	unsigned digit_idx;
	char digits[MAX_DIGITS];
	SDL_Rect digit_rect = {0, 0, 0, 0};
	SDL_Rect destination_rect = {0, 0, 0, 0};

	snprintf(digits, MAX_DIGITS, "%ld", score);
	if (digits[0] == 0)
		return;

	digit_rect.w = view->one_digit_width;
	digit_rect.h = view->digit_height;

	destination_rect.y = dst.y;
	destination_rect.h = dst.h;

	full_size_width = view->one_digit_width * strlen(digits);
	rendered_digit_width = view->one_digit_width;
	if (full_size_width > dst.w)
		rendered_digit_width = dst.w / strlen(digits);
	destination_rect.w = rendered_digit_width;

	for (digit_idx = 0; digit_idx < strlen(digits); ++digit_idx)
	{
		char digit = digits[digit_idx];
		int offset = digit_idx * rendered_digit_width;

		if (digit < '0' || digit > '9')
		{
			fprintf(stderr, "tried to render a non-digit as a number\n");
			return;
		}
		digit_rect.x = (digit - '0') * view->one_digit_width;
		destination_rect.x = dst.x + offset;
		SDL_RenderCopy(view->renderer, view->texture_digits,
			&digit_rect, &destination_rect);
	}
}

const SDL_Rect *sprite(SnekcGame *game, Identity identity)
{
	switch (identity)
	{
	case ID_APPLE:
		return &SPRITE_APPLE;
	case ID_BODY:
		return &SPRITE_BODY;
	case ID_BRICK:
		return &SPRITE_BRICK;
	case ID_HEAD:
		if (snekc_game_state(game) == STATE_DEAD)
			return &SPRITE_HEAD_DEAD;
		else
			return &SPRITE_HEAD;
	}
	return 0;
}

double velocity_to_rotation(Vec2 v)
{
	if (v.y > 0)
		return 180.;
	if (v.x > 0)
		return 90.;
	if (v.x < 0)
		return 270.;
	return 0.;
}

void render_game(SnekcView *view, SnekcGame *game)
{
	Entity *list = snekc_game_entity_list(game);
	Entity *node = list;
	while (node)
	{
		const SDL_Rect *sprite_ = sprite(game, node->identity);
		/* TODO - 32 to const int */
		SDL_Rect destination = {
			0, 0,
			32, 32
		};
		destination.x = node->position.x * 32;
		destination.y = node->position.y * 32;
		SDL_RenderCopyEx(view->renderer, view->texture_sprites,
			sprite_, &destination,
			velocity_to_rotation(node->velocity),
			0, 0);
		node = (Entity*) node->next;
	}
}

void render_score(SnekcView *view, SnekcGame *game)
{
	static const SDL_Rect panel_rect = {0, 512, 512, 640 - 512};
	static const SDL_Rect score_label_rect = {0, 512, 256, 128};
	static const SDL_Rect score_rect = {256, 540, 256, 80};

	SDL_SetRenderDrawColor(view->renderer, 0, 0, 0, 0xff);
	SDL_RenderFillRect(view->renderer, &panel_rect);
	SDL_RenderCopy(view->renderer, view->texture_score,
		0, &score_label_rect);
	render_digits(view, snekc_game_score(game), score_rect);
}

SnekcView *snekc_view_new(SDL_Renderer *renderer)
{
	int digit_width;
	const char* error = NULL;
	SnekcView *view = (SnekcView *) calloc(1, sizeof(SnekcView));
	view->renderer = renderer;

	/** This piece of code could've been done better, but it only
		loads 3 textures and I didn't want to bother. */
	view->texture_score = load_texture(renderer, "score.bmp");
	if (!view->texture_score)
	{
		error = "score.bmp";
		goto error_texture;
	}
	view->texture_digits = load_texture(renderer, "digits.bmp");
	if (!view->texture_digits)
	{
		error = "digits.bmp";
		goto error_texture;
	}
	if (SDL_QueryTexture(view->texture_digits, 0, 0,
			&digit_width, &view->digit_height) != 0)
	{
		fprintf(stderr, "could not obtain digit texture params: %s\n", SDL_GetError());
		goto error;
	}
	view->one_digit_width = digit_width / 10;
	view->texture_sprites = load_texture(renderer, "sprites.bmp");
	if (!view->texture_sprites)
	{
		error = "sprites.bmp";
		goto error_texture;
	}
	return view;

error_texture:
	fprintf(stderr, "failed to load texture %s: %s\n", error, SDL_GetError());
error:
	snekc_view_free(view);
	return NULL;
}

void snekc_view_free(SnekcView *view)
{
	if (view->texture_score)
		SDL_DestroyTexture(view->texture_score);
	if (view->texture_digits)
		SDL_DestroyTexture(view->texture_digits);
	if (view->texture_sprites)
		SDL_DestroyTexture(view->texture_sprites);
	free(view);
}

void snekc_view_drawgame(SnekcView *view, SnekcGame *game)
{
	SDL_SetRenderDrawColor(view->renderer, 0, 0x0, 0x80, 0xff);
	SDL_RenderClear(view->renderer);

	render_game(view, game);
	render_score(view, game);

	SDL_RenderPresent(view->renderer);
}
