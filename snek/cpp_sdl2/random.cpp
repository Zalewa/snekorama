#include "random.hpp"

#include <chrono>
#include <random>
#include <iostream>

namespace Snek
{
static std::mt19937 randomEngine;
}

void Snek::initRandom()
{
	// std::random_device may return the same sequence upon
	// each run as the implementations are allowed to fall-back
	// to deterministic generators with seeds. We need to
	// extra-randomize the seed for the engine with a clock.
	//
	// See: https://stackoverflow.com/q/45069219/1089357
	//
	// For me rd() would always return 3499211612 as its first value.
	std::random_device rd;
	auto clocks = static_cast<decltype(rd())>(
		std::chrono::system_clock::now().time_since_epoch().count());
	randomEngine = std::mt19937(rd() ^ clocks);
}

int Snek::randomInt(int min, int max)
{
	return std::uniform_int_distribution<int>(min, max)(randomEngine);
}
