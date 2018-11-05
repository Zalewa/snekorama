#ifndef id6238bd16_b38b_454d_81bc_743c92197848
#define id6238bd16_b38b_454d_81bc_743c92197848

typedef struct
{
	int x, y;
} Vec2;

Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_zero();

/** Non-zero if equal, zero if different. */
int vec2_equal(Vec2 a, Vec2 b);
int vec2_is_zero(Vec2 a);

#endif
