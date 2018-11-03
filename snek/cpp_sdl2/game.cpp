#include "game.hpp"

#include "audio.hpp"
#include "error.hpp"
#include "random.hpp"

namespace Snek
{

static const int CHANNEL_WORLD = -1;
static const int CHANNEL_PLAYER = 0;

Entity::Entity(Entity::EntityType type)
	: type_(type)
{
}


Game::Game(Vec2 size)
	: score_(0), size(size)
{
	nextLevel();
}

bool Game::changeDirection(Direction direction)
{
	auto &headVelocity = this->playerHead->velocity;
	switch (direction)
	{
	case Direction::UP:
		if (headVelocity.y > 0)
			return false;
		break;
	case Direction::DOWN:
		if (headVelocity.y < 0)
			return false;
		break;
	case Direction::LEFT:
		if (headVelocity.x > 0)
			return false;
		break;
	case Direction::RIGHT:
		if (headVelocity.x < 0)
			return false;
		break;
	default:
		throw Error("unknown direction type");
	}
	this->nextDirection = direction;
	return true;
}

void Game::nextLevel()
{
	if (this->state_ == State::DEAD)
		this->score_ = 0;
	this->state_ = State::GAME;
	this->entities_.clear();
	this->playerBody.clear();
	this->playerLength = 1;
	this->nextDirection = Direction::NONE;

	Vec2 headPos = {size.x / 2, size.y / 2};
	auto head = std::make_shared<Entity>(Entity::HEAD);
	head->position = headPos;
	this->entities_.push_back(head);
	this->playerHead = head;

	const int NUM_BRICKS = 4; // TODO somewhere else
	for (int i = 0; i < NUM_BRICKS; ++i)
	{
		Vec2 brickPos = {
			randomInt(0, size.x - 1),
			randomInt(0, size.y - 1)
		};
		if (entityAt(brickPos) != nullptr)
			continue;
		auto brick = std::make_shared<Entity>(Entity::BRICK);
		brick->position = brickPos;
		this->entities_.push_back(brick);
	}

	spawnNewApple();
}

void Game::tick()
{
	if (this->state_ == State::DEAD)
		return;

	switch (this->nextDirection)
	{
	case Direction::UP:
		this->playerHead->velocity = {0, -1};
		break;
	case Direction::DOWN:
		this->playerHead->velocity = {0, 1};
		break;
	case Direction::LEFT:
		this->playerHead->velocity = {-1, 0};
		break;
	case Direction::RIGHT:
		this->playerHead->velocity = {1, 0};
		break;
	case Direction::NONE:
		// pass
		break;
	default:
		throw Error("unknown direction type");
	}

	if (!this->playerHead->velocity.isZero())
	{
		Vec2 newHeadPosition = movePoint(
			this->playerHead->position,
			this->playerHead->velocity);
		auto collidee = entityAt(newHeadPosition);
		bool stopped = false;
		if (collidee != nullptr)
		{
			switch (collidee->type())
			{
			case Entity::BRICK:
			case Entity::BODY:
				die();
				stopped = true;
				queueAudio({"smash.wav", CHANNEL_PLAYER});
				break;
			case Entity::APPLE:
				scorePoint();
				queueAudio({"score.wav", CHANNEL_WORLD});
				break;
			default:
				throw Error("unhandled collidee");
			}
		}
		if (!stopped)
		{
			advance(newHeadPosition);
			queueAudio({"pop.wav", CHANNEL_PLAYER});
		}
	}
}

void Game::advance(const Vec2 &position)
{
	Vec2 prevPosition = this->playerHead->position;
	this->playerHead->position = position;

	auto neck = std::make_shared<Entity>(Entity::BODY);
	neck->position = prevPosition;
	entities_.push_back(neck);
	playerBody.push_front(neck);
	while (static_cast<int>(playerBody.size()) >= this->playerLength)
	{
		auto tail = playerBody.back();
		playerBody.pop_back();
		entities_.remove(tail);
	}
}

void Game::die()
{
	this->state_ = State::DEAD;
}

void Game::scorePoint()
{
	this->playerLength += 1;
	this->score_ += APPLE_WORTH;
	spawnNewApple();
}

void Game::spawnNewApple()
{
	Vec2 applePos;
	do
	{
		applePos = {
			randomInt(0, size.x - 1),
			randomInt(0, size.y - 1)
		};
	} while(entityAt(applePos) != nullptr);

	for (auto it = entities_.begin(); it != entities_.end(); ++it)
	{
		if ((*it)->type() == Entity::APPLE)
		{
			entities_.erase(it);
			break;
		}
	}

	auto apple = std::make_shared<Entity>(Entity::APPLE);
	apple->position = applePos;
	entities_.push_back(apple);
}

std::shared_ptr<Entity> Game::entityAt(const Vec2 &pos)
{
	for (auto entity : this->entities_)
	{
		if (entity->position == pos)
			return entity;
	}
	return nullptr;
}

Vec2 Game::movePoint(const Vec2 &point, const Vec2 &offset)
{
	Vec2 newPoint = point + offset;
	if (newPoint.x < 0)
		newPoint.x = this->size.x + (newPoint.x % this->size.x);
	else if (newPoint.x >= this->size.x)
		newPoint.x %= this->size.x;
	if (newPoint.y < 0)
		newPoint.y = this->size.y + (newPoint.y % this->size.y);
	else if (newPoint.y >= this->size.y)
		newPoint.y %= this->size.y;
	return newPoint;
}

void Game::queueAudio(const PlayAudioSampleRequest &request)
{
	this->audioRequests_.push_back(request);
}

std::list<PlayAudioSampleRequest> Game::popAudioRequests()
{
	auto requests = this->audioRequests_;
	this->audioRequests_.clear();
	return requests;
}

}
