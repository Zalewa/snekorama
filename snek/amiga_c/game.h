#ifndef id6cda10c0_1f55_4456_a019_0f5bc172676e
#define id6cda10c0_1f55_4456_a019_0f5bc172676e

#include "list.h"
#include "vec2.h"

typedef enum
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT
} Direction;

typedef enum
{
	ID_HEAD,
	ID_BODY,
	ID_BRICK,
	ID_APPLE
} Identity;

typedef enum
{
	STATE_ALIVE,
	STATE_DEAD
} GameState;

typedef struct
{
	LIST_NODE
	long no;
	Vec2 position;
	Vec2 velocity;
	Identity identity;
} Entity;

Entity *entity_new();
void entity_free(Entity*);

struct _SnekcGame;
typedef struct _SnekcGame SnekcGame;

SnekcGame *snekc_game_new();
void snekc_game_free(SnekcGame*);

void snekc_game_change_direction(SnekcGame *game, Direction direction);
void snekc_game_next_level(SnekcGame *game, Vec2 size, int num_bricks);
void snekc_game_tick(SnekcGame *game);

GameState snekc_game_state(SnekcGame *game);
long snekc_game_score(SnekcGame *game);
Entity *snekc_game_entity_list(SnekcGame *game);

#endif
