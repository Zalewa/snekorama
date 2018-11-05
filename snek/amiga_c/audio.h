#ifndef id29721a72_349f_4553_9a8b_4a8d0b5a81eb
#define id29721a72_349f_4553_9a8b_4a8d0b5a81eb

struct _SnekcAudio;
typedef struct _SnekcAudio SnekcAudio;

extern SnekcAudio *g_audio;

typedef enum
{
	SFX_POP = 0,
	SFX_SCORE,
	SFX_SMASH,

	NUM_SFX
} SnekcSfx;

SnekcAudio *snekc_audio_new();
void snekc_audio_free(SnekcAudio*);

void snekc_audio_play(SnekcAudio *audio, SnekcSfx sfx, int channel);

#endif
