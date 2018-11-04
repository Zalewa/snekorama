#include "game.h"

#include "audio.h"
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

const int APPLE_SCORE = 117;

struct _SnekcGame
{
	Vec2 size;
	GameState state;
	long score;

	Entity *entity_list;

	Entity *player_head;
	int player_length;
	Vec2 player_next_velocity;
	Vec2 prev_tail_position;
};

Entity *entity_new()
{
	static long no = 1;
	Entity *entity = calloc(1, sizeof(Entity));
	listnode_init(entity);
	entity->no = no++;
	entity->position = vec2_zero();
	entity->velocity = vec2_zero();
	entity->identity = SNEKID_BRICK;
	return entity;
}

void entity_free(Entity *entity)
{
	free(entity);
}

Entity *find_entity_at(SnekcGame *game, Vec2 pos)
{
	Entity *entity = game->entity_list;
	while (entity != 0)
	{
		if (vec2_equal(entity->position, pos))
			return entity;
		entity = (Entity*) entity->next;
	}
	return 0;
}

int has_body(SnekcGame *game)
{
	return game->player_length > 1;
}

void clear_game(SnekcGame *game)
{
	if (game->entity_list != 0)
		list_foreach(game->entity_list, &entity_free);
	game->entity_list = 0;
	game->player_head = 0;
	game->score = 0;
}

Vec2 find_free_position(SnekcGame *game)
{
	Vec2 pos;
	Entity *collidee;
	do
	{
		pos.x = rand() % game->size.x;
		pos.y = rand() % game->size.y;
		collidee = find_entity_at(game, pos);
	} while (collidee != 0);
	return pos;
}

void spawn_new_apple(SnekcGame *game)
{
	Entity *apple = entity_new();
	apple->position = find_free_position(game);
	apple->identity = SNEKID_APPLE;
	list_add(game->entity_list, apple);
}

int is_apple(Entity *entity)
{
	return entity->identity == SNEKID_APPLE;
}

void score(SnekcGame *game)
{
	Entity *apple = (Entity*)list_find(game->entity_list, is_apple);

	list_remove(apple);
	entity_free(apple);
	game->player_length += 1;
	game->score += APPLE_SCORE;
	spawn_new_apple(game);
	snekc_audio_play(g_audio, SFX_SCORE, 1);
}

void move_player(SnekcGame *game, Vec2 newpos)
{
	Entity *entity = game->entity_list;
	Entity *head = game->player_head;
	Entity *tail = entity_new();
	long actual_player_length = 1;

	/* Add new tail chunk. */
	tail->identity = SNEKID_BODY;
	tail->position = head->position;
	list_add(game->entity_list, tail);
	tail = 0;

	/* Reposition the head. */
	head->position = newpos;

	/* Remove excessive tail chunks. */
	for (; entity; entity = (Entity*)entity->next)
	{
		if (entity->identity == SNEKID_BODY)
		{
			++actual_player_length;
			if (!tail)
				tail = entity;
		}
	}
	if (actual_player_length > game->player_length)
	{
		game->prev_tail_position = tail->position;
		list_remove(tail);
		free(tail);
	}

	/* Play step sound. */
	snekc_audio_play(g_audio, SFX_POP, 2);
}

void advance_player(SnekcGame *game)
{
	Entity *collidee;
	Entity *head = game->player_head;
	Vec2 newpos;

	newpos = vec2_add(head->position, head->velocity);
	if (newpos.x < 0)
		newpos.x = game->size.x - ((-1 * newpos.x) % game->size.x);
	else if (newpos.x >= game->size.x)
		newpos.x = newpos.x % game->size.x;
	if (newpos.y < 0)
		newpos.y = game->size.y - ((-1 * newpos.y) % game->size.y);
	else if (newpos.y >= game->size.y)
		newpos.y = newpos.y % game->size.y;

	collidee = find_entity_at(game, newpos);
	if (collidee)
	{
		switch (collidee->identity)
		{
		case SNEKID_BRICK:
		case SNEKID_BODY:
			snekc_audio_play(g_audio, SFX_SMASH, 1);
			game->state = STATE_DEAD;
			break;
		case SNEKID_APPLE:
			score(game);
			move_player(game, newpos);
			break;
		default:
			fprintf(stderr, "unhandled collision type %d\n",
				collidee->identity);
			fflush(stderr);
			break;
		}
	}
	else
	{
		move_player(game, newpos);
	}
}

SnekcGame *snekc_game_new()
{
	SnekcGame *game = malloc(sizeof(SnekcGame));
	game->state = STATE_ALIVE;
	game->score = 0;
	game->entity_list = 0;
	game->player_head = 0;
	game->player_next_velocity = vec2_zero();
	game->prev_tail_position.x = game->prev_tail_position.y = -1;
	return game;
}

void snekc_game_free(SnekcGame *game)
{
	clear_game(game);
	free(game);
}

void snekc_game_change_direction(SnekcGame *game, Direction direction)
{
	Vec2 *v = &game->player_head->velocity;
	Vec2 *nextv = &game->player_next_velocity;

	if (has_body(game) != 0)
	{
		/* Prohibit 180 turns. */
		if ((v->x > 0 && direction == DIR_LEFT) ||
			(v->x < 0 && direction == DIR_RIGHT) ||
			(v->y < 0 && direction == DIR_DOWN) ||
			(v->y > 0 && direction == DIR_UP))
			return;
	}
	nextv->x = nextv->y = 0;
	switch (direction)
	{
	case DIR_UP:
		nextv->y = -1;
		break;
	case DIR_DOWN:
		nextv->y = 1;
		break;
	case DIR_LEFT:
		nextv->x = -1;
		break;
	case DIR_RIGHT:
		nextv->x = 1;
		break;
	}
}

void snekc_game_next_level(SnekcGame *game, Vec2 size, int num_bricks)
{
	Entity *head;
	int i;

	clear_game(game);
	game->size = size;
	game->player_length = 1;
	game->state = STATE_ALIVE;

	/* Spawn player head. */
	head = entity_new();
	game->entity_list = head;
	game->player_head = head;
	head->identity = SNEKID_HEAD;
	head->position.x = size.x / 2;
	head->position.y = size.y / 2;

	/* Spawn bricks. */
	for (i = 0; i < num_bricks; ++i)
	{
		Entity *brick = entity_new();
		brick->position = find_free_position(game);
		brick->identity = SNEKID_BRICK;
		list_add(game->entity_list, brick);
	}

	spawn_new_apple(game);
}

void snekc_game_tick(SnekcGame *game)
{
	if (game->state == STATE_DEAD)
		return;

	/* Convert the velocity change request into the player's
	   actual velocity. */
	if (!vec2_is_zero(game->player_next_velocity))
	{
		game->player_head->velocity = game->player_next_velocity;
		game->player_next_velocity = vec2_zero();
	}

	if (!vec2_is_zero(game->player_head->velocity))
		advance_player(game);
}

GameState snekc_game_state(SnekcGame *game)
{
	return game->state;
}

long snekc_game_score(SnekcGame *game)
{
	return game->score;
}

Vec2 snekc_game_size(SnekcGame *game)
{
	return game->size;
}

Entity *snekc_game_entity_list(SnekcGame *game)
{
	return game->entity_list;
}

Vec2 snekc_game_prev_tail_position(SnekcGame *game)
{
	return game->prev_tail_position;
}
