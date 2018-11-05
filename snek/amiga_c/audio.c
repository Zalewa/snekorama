#include "audio.h"

#include <datatypes/soundclass.h>
#include <devices/audio.h>
#include <proto/datatypes.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH 65536

typedef struct
{
	Object *dtp;
	UBYTE *address;
	ULONG length;
	UWORD period;
	UWORD volume;
} SfxSample;

static char *m_sfx_names[NUM_SFX] =
{
	"pop.8svx",
	"score.8svx",
	"smash.8svx"
};

struct _SnekcAudio
{
	SfxSample sfx[NUM_SFX];
	struct MsgPort *msgport_audio;
	struct IOAudio *audio_io;
};

/**
 * 1 - first left
 * 2 - first right
 * 4 - second left
 * 8 - second right
 *
 * This will allocate only one of the channels. To allocate
 * more than one, we need to binary OR them.
 */
static UBYTE channels[] = {1, 2, 4, 8};

SnekcAudio *snekc_audio_new()
{
	int idx;
	LONG error;
	struct IOAudio *io;
	SnekcAudio *audio = (SnekcAudio *)calloc(1, sizeof(SnekcAudio));

	/* Load sound files. */
	for (idx = 0; idx < NUM_SFX; ++idx)
	{
		ULONG period = 0;
		ULONG volume = 0;
		Object *obj = NewDTObject(m_sfx_names[idx],
			DTA_SourceType, DTST_FILE,
			DTA_GroupID, GID_SOUND,
			TAG_END);
		if (!obj)
		{
			long error = IoErr();
			fprintf(stderr, "failed to load sound %s: %d\n", m_sfx_names[idx], error);
			PrintFault(error, 0);
			goto error;
		}
		audio->sfx[idx].dtp = obj;
		GetDTAttrs(obj, SDTA_Sample, &audio->sfx[idx].address, TAG_END);
		GetDTAttrs(obj, SDTA_SampleLength, &audio->sfx[idx].length, TAG_END);
		GetDTAttrs(obj, SDTA_Period, &period, TAG_END);
		GetDTAttrs(obj, SDTA_Volume, &volume, TAG_END);
		audio->sfx[idx].volume = (UWORD) volume;
		audio->sfx[idx].period = (UWORD) period;
	}

	/* Init Paula. */
	audio->msgport_audio = (struct MsgPort *) CreateMsgPort();
	if (!audio->msgport_audio)
	{
		fprintf(stderr, "cannot create MsgPort for audio\n");
		goto error;
	}

	io = (struct IOAudio *) CreateIORequest(
		audio->msgport_audio, sizeof(struct IOAudio));
	if (!io)
	{
		fprintf(stderr, "cannot create IO request for audio\n");
		goto error;
	}
	audio->audio_io = io;

	/* Open the audio device and allocate a channel. */
	io->ioa_Request.io_Message.mn_ReplyPort = audio->msgport_audio;
	io->ioa_Request.io_Message.mn_Node.ln_Pri = 127;
	io->ioa_Request.io_Command = ADCMD_ALLOCATE;
	io->ioa_Request.io_Flags  = ADIOF_NOWAIT;
	io->ioa_AllocKey = 0;
	io->ioa_Data = channels;
	io->ioa_Length = sizeof(channels);

	error = OpenDevice(AUDIONAME, 0L, (struct IORequest *) io, 0L);
	if (error != 0)
	{
		fprintf(stderr, "cannot open audio device\n");
		goto error;
	}

	return audio;
error:
	snekc_audio_free(audio);
	return 0;
}

void snekc_audio_free(SnekcAudio *audio)
{
	int i;

	/* Close device. */
	if (audio->audio_io)
	{
		if (audio->audio_io->ioa_Request.io_Device)
		{
			/* Stop any sound that is being played. */
			audio->audio_io->ioa_Request.io_Command = CMD_STOP;
			BeginIO((struct IORequest *) audio->audio_io);

			/* Only then close the device. */
			CloseDevice((struct IORequest *) audio->audio_io);
		}
		DeleteIORequest((struct IORequest *) audio->audio_io);
	}
	if (audio->msgport_audio)
		DeleteMsgPort(audio->msgport_audio);

	/* Delete samples. */
	for (i = 0; i < NUM_SFX; ++i)
	{
		if (audio->sfx[i].dtp)
			DisposeDTObject(audio->sfx[i].dtp);
	}
	free(audio);
}

void snekc_audio_play(SnekcAudio *audio, SnekcSfx sfx, int channel)
{
	struct IOAudio *io = audio->audio_io;
	SfxSample *sample = &audio->sfx[sfx];
	/* First, let's check if we're already playing a sample. */
	if (!CheckIO((struct IORequest *) audio->audio_io))
	{
		/* We're playing something. */

		/* If this is the score sfx it takes priority over
		   all else, so let's not overwrite it. */
		if (io->ioa_Data == audio->sfx[SFX_SCORE].address)
			return;

		/* Let's abort if we're playing anything else. If we
		   don't abort and try to play something else, audio
		   will break and the whole system will crash. */
		io->ioa_Request.io_Command = ADCMD_FINISH;
		io->ioa_Request.io_Flags = ADIOF_SYNCCYCLE;
		BeginIO((struct IORequest *) io);
		WaitIO((struct IORequest *) io);
	}

	/* Play the sample with requested number. */
	io->ioa_Request.io_Command = CMD_WRITE;
	io->ioa_Request.io_Flags = ADIOF_PERVOL;
	io->ioa_Data = sample->address;
	io->ioa_Length = sample->length;
	io->ioa_Volume = sample->volume;
	io->ioa_Period = sample->period;
	io->ioa_Cycles = 1;
	BeginIO((struct IORequest *) io);
}
