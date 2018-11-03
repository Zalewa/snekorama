#include "audio.hpp"

using namespace std::string_literals;

#include "error.hpp"
#include <SDL.h>
#include <algorithm>
#include <vector>

namespace Snek
{

Audio::Audio()
{
	if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
		throw Error("could not initialize audio: "s + Mix_GetError());
}

Audio::~Audio()
{
	std::for_each(samples_.begin(), samples_.end(), [](const auto &pair) {
			Mix_FreeChunk(pair.second);
		});
}

void Audio::play(const std::list<PlayAudioSampleRequest> &requests)
{
	// We need to play the autochannel sounds last or else
	// we risk overwriting the auto-picked channel.
	std::vector<PlayAudioSampleRequest> sorted = {requests.begin(), requests.end()};
	std::sort(sorted.begin(), sorted.end(),
		[](auto &r1, auto &r2) -> bool
		{
			return r1.channel >= 0 || r2.channel < 0;
		});
	for (auto &req : sorted)
		play(req);
}

void Audio::play(const PlayAudioSampleRequest &request)
{
	Mix_Chunk *sample = getSample(request.sample);
	Mix_PlayChannel(request.channel, sample, 0);
}

Mix_Chunk *Audio::getSample(const std::string &name)
{
	auto it = this->samples_.find(name);
	if (it != this->samples_.end())
		return it->second;
	std::string path = SDL_GetBasePath() + name;
	Mix_Chunk *chunk = Mix_LoadWAV(path.c_str());
	if (chunk == nullptr)
		throw Error("failed to load audio chunk '"s + name + "': "s + Mix_GetError());
	this->samples_[name] = chunk;
	return chunk;
}

}
