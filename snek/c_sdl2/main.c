#include <SDL.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "audio.h"
#include "game.h"
#include "view.h"

#define TICK_FREQUENCY 10
#define TICK_PERIOD (1000 / TICK_FREQUENCY)

const Vec2 GAME_SIZE = {16, 16};
const int NUM_BRICKS = 4;

const int JOY_X_AXIS = 0;
const int JOY_Y_AXIS = 1;

SnekcAudio *g_audio = NULL;
SDL_Window *g_window = NULL;
SDL_Renderer *g_renderer = NULL;

SDL_Joystick **g_joysticks = NULL;
int g_num_joysticks = 0;
int g_quit = 0;

const char *init_joysticks()
{
	int joyIdx;

	g_num_joysticks = SDL_NumJoysticks();
	if (g_num_joysticks < 0)
		return "it was impossible to obtain joysticks";

	g_joysticks = (SDL_Joystick**)calloc(g_num_joysticks, sizeof(SDL_Joystick*));
	for (joyIdx = 0; joyIdx < g_num_joysticks; ++joyIdx)
	{
		SDL_Joystick *joystick = SDL_JoystickOpen(joyIdx);
		if (joystick)
			g_joysticks[joyIdx] = joystick;
		else
			fprintf(stderr, "couldn't open joystick %d: %s\n",
				joyIdx, SDL_GetError());
	}

	return NULL;
}

int init()
{
	const char *error = NULL;

	/* Init random. */
	srand(time(NULL));

	/* Init SDL. */
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) != 0)
	{
		error = "could not init SDL";
		goto error;
	}

	/* Init video. */
	g_window = SDL_CreateWindow("Snek!",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		512, 640,
		SDL_WINDOW_SHOWN);
	if (g_window == NULL)
	{
		error = "could not create window";
		goto error;
	}

	g_renderer = SDL_CreateRenderer(g_window, -1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (g_renderer == NULL)
	{
		error = "could not create renderer";
		goto error;
	}

	/* Open joysticks. */
	error = init_joysticks();
	if (error != NULL)
		goto error;

	/* Init audio. */
	g_audio = snekc_audio_new();
	if (!g_audio)
		goto other_error;

	return 0;
error:
	fprintf(stderr, "%s: %s\n", error, SDL_GetError());
	return 1;
other_error:
	return 2;
}

void destroy()
{
	if (g_audio)
		snekc_audio_free(g_audio);
	g_audio = NULL;

	int joy_idx;
	for (joy_idx = 0; joy_idx < g_num_joysticks; ++joy_idx)
	{
		SDL_Joystick *joy = g_joysticks[joy_idx];
		if (joy)
			SDL_JoystickClose(joy);
	}
	free(g_joysticks);

	if (g_renderer)
		SDL_DestroyRenderer(g_renderer);
	g_renderer = NULL;

	if (g_window)
		SDL_DestroyWindow(g_window);
	g_window = NULL;
}

void new_game(SnekcGame *game)
{
	snekc_game_next_level(game, GAME_SIZE, NUM_BRICKS);
}

/** Return non-zero when event is handled. */
int handle_event(const SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_QUIT:
		g_quit = 1;
		return 1;
	case SDL_KEYUP:
	{
		SDL_KeyboardEvent *key_event = (SDL_KeyboardEvent*) event;
		if (key_event->keysym.sym == SDLK_ESCAPE)
		{
			g_quit = 1;
			return 1;
		}
		break;
	}
	}
	return 0;
}

void handle_game_alive_event(SnekcGame *game, const SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent*)event)->keysym.sym)
		{
		case SDLK_RIGHT:
			snekc_game_change_direction(game, DIR_RIGHT);
			break;
		case SDLK_LEFT:
			snekc_game_change_direction(game, DIR_LEFT);
			break;
		case SDLK_UP:
			snekc_game_change_direction(game, DIR_UP);
			break;
		case SDLK_DOWN:
			snekc_game_change_direction(game, DIR_DOWN);
			break;
		}
		break;
	case SDL_JOYAXISMOTION:
	{
		SDL_JoyAxisEvent *joy_event = (SDL_JoyAxisEvent*) event;
		if (joy_event->axis == JOY_X_AXIS)
		{
			if (joy_event->value > 0)
				snekc_game_change_direction(game, DIR_RIGHT);
			else if (joy_event->value < 0)
				snekc_game_change_direction(game, DIR_LEFT);
		}
		else if (joy_event->axis == JOY_Y_AXIS)
		{
			if (joy_event->value > 0)
				snekc_game_change_direction(game, DIR_DOWN);
			else if (joy_event->value < 0)
				snekc_game_change_direction(game, DIR_UP);
		}
		break;
	}
	}
}

void handle_game_dead_event(SnekcGame *game, const SDL_Event *event)
{
	switch (event->type)
	{
	case SDL_KEYDOWN:
		switch (((SDL_KeyboardEvent*)event)->keysym.sym)
		{
		case SDLK_RETURN:
		case SDLK_SPACE:
			new_game(game);
			break;
		}
		break;
	case SDL_JOYBUTTONDOWN:
		new_game(game);
		break;
	}
}

void handle_game_event(SnekcGame *game, const SDL_Event *event)
{
	switch (snekc_game_state(game))
	{
	case STATE_ALIVE:
		handle_game_alive_event(game, event);
		break;
	case STATE_DEAD:
		handle_game_dead_event(game, event);
		break;
	default:
		fprintf(stderr, "unhandled game state %d\n", snekc_game_state(game));
		break;
	}
}

int run()
{
	int ec = 0;
	SnekcGame *game = snekc_game_new();
	SnekcView *view = snekc_view_new(g_renderer);
	unsigned next_tick = 0;
	SDL_Event event;

	if (!view)
		goto error;

	new_game(game);
	while (g_quit == 0)
	{
		while (SDL_PollEvent(&event) != 0)
		{
			if (handle_event(&event) == 0)
				handle_game_event(game, &event);
		}
		if (SDL_GetTicks() > next_tick)
		{
			next_tick = SDL_GetTicks() + TICK_PERIOD;
			snekc_game_tick(game);
		}
		snekc_view_drawgame(view, game);
	}

	goto end;
error:
	ec = 1;
end:
	if (view)
		snekc_view_free(view);
	if (game)
		snekc_game_free(game);
	return ec;
}

int main(int argc, char *argv[])
{
	int ec;
	(void) argc;
	(void) argv;

	ec = init();
	if (ec != 0)
		goto end;

	ec = run();
end:
	destroy();
	return ec;
}
