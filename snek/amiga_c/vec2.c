#include "vec2.h"

Vec2 vec2_add(Vec2 a, Vec2 b)
{
	Vec2 c;
	c.x = a.x + b.x;
	c.y = a.y + b.y;
	return c;
}

int vec2_equal(Vec2 a, Vec2 b)
{
	return a.x == b.x && a.y == b.y;
}

int vec2_is_zero(Vec2 a)
{
	return a.x == 0 && a.y == 0;
}

Vec2 vec2_zero()
{
	Vec2 v = {0, 0};
	return v;
}
