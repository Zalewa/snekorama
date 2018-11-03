#pragma once

#include <SDL_mixer.h>
#include <list>
#include <map>
#include <string>

namespace Snek
{

struct PlayAudioSampleRequest
{
	std::string sample;
	int channel = -1;
};

class Audio
{
public:
	Audio();
	~Audio();

	void play(const std::list<PlayAudioSampleRequest> &requests);
	void play(const PlayAudioSampleRequest &request);

private:
	std::map<std::string, Mix_Chunk*> samples_;

	Mix_Chunk *getSample(const std::string &name);
};

}
