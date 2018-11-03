#pragma once

#include <memory>
#include <list>
#include <vector>

namespace Snek
{

struct PlayAudioSampleRequest;

struct Vec2
{
	int x = 0;
	int y = 0;

	bool operator==(const Vec2 &other) const
	{
		return x == other.x && y == other.y;
	}

	Vec2 operator+(const Vec2 &other) const
	{
		return {x + other.x, y + other.y};
	}

	bool isZero() const
	{
		return x == 0 && y == 0;
	}
};

class Entity
{
public:
	enum EntityType
	{
		APPLE,
		BODY,
		BRICK,
		HEAD,
	};

	Vec2 position;
	Vec2 velocity;

	Entity(EntityType type);

	EntityType type() const { return type_; }

private:
	EntityType type_;
};

struct Field
{
	std::unique_ptr<Entity> entity;
};

class Game
{
public:
	enum class Direction
	{
		UP, DOWN, LEFT, RIGHT, NONE
	};

	enum class State
	{
		GAME, DEAD
	};

	Game(Vec2 size);

	bool changeDirection(Direction direction);
	void nextLevel();
	void tick();

	std::list<PlayAudioSampleRequest> popAudioRequests();
	const auto &entities() const { return entities_; }
	int score() const { return score_; }
	State state() const { return state_; }


private:
	static const int APPLE_WORTH = 117;

	std::list<PlayAudioSampleRequest> audioRequests_;
	std::list<std::shared_ptr<Entity>> entities_;
	std::list<std::shared_ptr<Entity>> playerBody;
	std::shared_ptr<Entity> playerHead;
	int playerLength;
	int score_;
	Vec2 size;
	State state_;
	Direction nextDirection;

	void advance(const Vec2 &position);
	void die();
	void scorePoint();
	void spawnNewApple();

	void queueAudio(const PlayAudioSampleRequest &request);

	std::shared_ptr<Entity> entityAt(const Vec2 &pos);
	Vec2 movePoint(const Vec2 &point, const Vec2 &offset);
};

}
