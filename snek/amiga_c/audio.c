#include "audio.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH 65536

struct _SnekcAudio
{
};

SnekcAudio *snekc_audio_new()
{
	SnekcAudio *audio;
	return audio;
error:
	snekc_audio_free(audio);
	return 0;
}

void snekc_audio_free(SnekcAudio *audio)
{
	free(audio);
}

void snekc_audio_play(SnekcAudio *audio, SnekcSfx sfx, int channel)
{
}
