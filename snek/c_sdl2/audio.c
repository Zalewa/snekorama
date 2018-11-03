#include "audio.h"

#include <SDL.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH 65536

struct _SnekcAudio
{
	Mix_Chunk *pop;
	Mix_Chunk *score;
	Mix_Chunk *smash;
};

static Mix_Chunk *load_sample(const char *name)
{
	Mix_Chunk *chunk;
	char path[MAX_PATH];
	strcpy(path, SDL_GetBasePath());
	strcat(path, name);
	chunk = Mix_LoadWAV(path);
	if (!chunk)
		fprintf(stderr, "failed to load audio chunk %s: %s\n",
			name, Mix_GetError());
	return chunk;
}

SnekcAudio *snekc_audio_new()
{
	SnekcAudio *audio;
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		fprintf(stderr, "could not initialize audio: %s\n", Mix_GetError());
		return 0;
	}
	audio = (SnekcAudio*) malloc(sizeof(SnekcAudio));
	if (audio)
	{
		audio->pop = load_sample("pop.wav");
		if (!audio->pop)
			goto error;
		audio->score = load_sample("score.wav");
		if (!audio->score)
			goto error;
		audio->smash = load_sample("smash.wav");
		if (!audio->smash)
			goto error;
	}
	return audio;
error:
	snekc_audio_free(audio);
	return 0;
}

void snekc_audio_free(SnekcAudio *audio)
{
	Mix_FreeChunk(audio->pop);
	Mix_FreeChunk(audio->score);
	Mix_FreeChunk(audio->smash);
	free(audio);
	Mix_CloseAudio();
}

void snekc_audio_play(SnekcAudio *audio, SnekcSfx sfx, int channel)
{
	Mix_Chunk *sample = 0;
	switch (sfx)
	{
	case SFX_POP:
		sample = audio->pop;
		break;
	case SFX_SCORE:
		sample = audio->score;
		break;
	case SFX_SMASH:
		sample = audio->smash;
		break;
	default:
		fprintf(stderr, "requested playback of unknown sfx sample\n");
		return;
	}
	Mix_PlayChannel(channel, sample, 0);
}
