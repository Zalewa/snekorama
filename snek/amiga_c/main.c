/* Amiga includes */
#include <proto/exec.h>
#include <devices/timer.h>
#include <intuition/intuition.h>
#include <intuition/screens.h>

/* stdlib includes */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

/* Snek includes */
#include "audio.h"
#include "game.h"
#include "view.h"

#define TICK_FREQUENCY 10L
#define TICK_PERIOD (1000000L / TICK_FREQUENCY)

const Vec2 GAME_SIZE = {16, 16};
const int NUM_BRICKS = 4;

SnekcAudio *g_audio = NULL;
int g_quit = 0;

/* Amiga specific stuff. */
#define KEY_ESC 0x45
#define KEY_CURSOR_UP 0x4c
#define KEY_CURSOR_DOWN 0x4d
#define KEY_CURSOR_RIGHT 0x4e
#define KEY_CURSOR_LEFT 0x4f
#define KEY_CURSOR_UP 0x4c
#define KEY_ENTER 0x44
#define KEY_NUMPAD_RETURN 0x43

struct Screen *g_amiscreen = 0;
struct Window *g_amiwindow = 0;
ULONG g_window_signal;

/* TODO Do we need this as g_? */
BOOL g_timer_was_sent = FALSE;
ULONG g_timer_signal;
struct MsgPort *g_msgport_timer = NULL;
struct timerequest *g_timer_io = NULL;
struct timeval g_timeval;


static void send_timer_request()
{
	g_timeval.tv_secs = 0;
	g_timeval.tv_micro = TICK_PERIOD;
	g_timer_io->tr_time = (struct timeval)g_timeval;
	SendIO((struct IORequest *) g_timer_io);
}

static void signals_timer()
{
	while (TRUE)
	{
		struct IntuiMessage *msg =
			(struct IntuiMessage *) GetMsg(g_msgport_timer);
		if (!msg)
			break;
	}
	send_timer_request();
}

/**
 * We wish the game to run on its own screen.
 */
static BOOL init_screen()
{
	struct NewScreen new_screen = {
		0, 0, STDSCREENWIDTH, STDSCREENHEIGHT, 8, 0, 1,
		HIRES, CUSTOMSCREEN | SCREENQUIET,
		NULL, NULL, NULL, NULL
	};
	g_amiscreen = (struct Screen *) OpenScreen(&new_screen);
	if (!g_amiscreen)
	{
		fprintf(stderr, "cannot create screen\n");
		return FALSE;
	}
	return TRUE;
}

/**
 * The window will be created on the separate screen which
 * must be acquired beforehand. It will take the whole size
 * of that screen.
 */
static BOOL init_window()
{
	if (!g_amiscreen)
	{
		fprintf(stderr, "screen must be init first\n");
		return FALSE;
	}
	g_amiwindow = (struct Window *) OpenWindowTags(NULL,
		WA_Left, 0, WA_Top, 0,
		WA_Width, g_amiscreen->Width,
		WA_Height, g_amiscreen->Height,
		WA_CustomScreen, g_amiscreen,
		WA_IDCMP, IDCMP_CLOSEWINDOW | IDCMP_MOUSEBUTTONS | IDCMP_RAWKEY,
		WA_Flags, WFLG_CLOSEGADGET | WFLG_BORDERLESS | WFLG_RMBTRAP | WFLG_ACTIVATE,
		TAG_DONE);
	if (!g_amiwindow)
	{
		fprintf(stderr, "cannot create window\n");
		return FALSE;
	}
	g_window_signal = 1L << g_amiwindow->UserPort->mp_SigBit;
	return TRUE;
}

/**
 * Acquire Amiga timer.
 */
static BOOL init_timer()
{
	LONG error;

	g_msgport_timer = CreateMsgPort();
	if (g_msgport_timer == 0)
	{
		fprintf(stderr, "cannot create MsgPort for timer\n");
		return FALSE;
	}
	g_timer_io = (struct timerequest *)
		CreateIORequest(g_msgport_timer, sizeof(struct timerequest));
	if (g_timer_io == 0)
	{
		fprintf(stderr, "cannot create IO request for timer\n");
		return FALSE;
	}

	error = OpenDevice(TIMERNAME, UNIT_VBLANK,
		(struct IORequest *) g_timer_io, 0);
	if (error != 0)
	{
		fprintf(stderr, "error when opening timer device: %ld\n", error);
		return FALSE;
	}

	g_timer_signal = 1L << g_msgport_timer->mp_SigBit;
	g_timer_io->tr_node.io_Command = TR_ADDREQUEST;

	send_timer_request();
	g_timer_was_sent = TRUE;

	return TRUE;
}

/**
 * Acquire system-wide resources and prepare them to be
 * used by other routines of the program.
 */
static BOOL init()
{
	/* Init random. */
	srand(time(NULL));

	/* Init Amiga windowing system on a separate screen. */
	if (!init_screen())
		return FALSE;
	if (!init_window())
		return FALSE;
	if (!init_timer())
		return FALSE;
	return TRUE;
}

/**
 * Release the timer acquired by init_timer().
 */
static void destroy_timer()
{
	if (g_timer_io)
	{
		if (g_timer_was_sent)
		{
			AbortIO((struct IORequest *) g_timer_io);
			WaitIO((struct IORequest *) g_timer_io);
		}
		CloseDevice((struct IORequest *) g_timer_io);
		DeleteIORequest(g_timer_io);
	}

	if (g_msgport_timer)
		DeleteMsgPort(g_msgport_timer);
}

/**
 * Release the system-wide resources that were acquired by init().
 * This function is capable of cleaning-up after a partial init(),
 * allowing it to be also called when there's an error condition
 * and the program must quit prematurely.
 */
static void destroy()
{
	destroy_timer();

	if (g_amiwindow)
		CloseWindow(g_amiwindow);
	if (g_amiscreen)
		CloseScreen(g_amiscreen);
}

static void new_game(SnekcGame *game)
{
	snekc_game_next_level(game, GAME_SIZE, NUM_BRICKS);
}

static void handle_game_alive_event(SnekcGame *game, struct IntuiMessage *msg)
{
	switch (msg->Class)
	{
	case IDCMP_RAWKEY:
		switch (msg->Code)
		{
		case KEY_CURSOR_UP:
			snekc_game_change_direction(game, DIR_UP);
			break;
		case KEY_CURSOR_DOWN:
			snekc_game_change_direction(game, DIR_DOWN);
			break;
		case KEY_CURSOR_LEFT:
			snekc_game_change_direction(game, DIR_LEFT);
			break;
		case KEY_CURSOR_RIGHT:
			snekc_game_change_direction(game, DIR_RIGHT);
			break;
		}
		break;
	}
}

static void handle_game_dead_event(SnekcGame *game, struct IntuiMessage *msg)
{
	switch (msg->Class)
	{
	case IDCMP_RAWKEY:
		switch (msg->Code)
		{
		case KEY_ENTER:
		case KEY_NUMPAD_RETURN:
			new_game(game);
			break;
		}
		break;
	}
}

static void handle_game_event(SnekcGame *game, struct IntuiMessage *msg)
{
	if (snekc_game_state(game) == STATE_ALIVE)
		handle_game_alive_event(game, msg);
	else
		handle_game_dead_event(game, msg);
}

static BOOL handle_event(struct IntuiMessage *msg)
{
	switch (msg->Class)
	{
	case IDCMP_RAWKEY:
		switch (msg->Code)
		{
		case KEY_ESC:
			g_quit = 1;
			return TRUE;
		}
		break;
	case IDCMP_CLOSEWINDOW:
		g_quit = 1;
		return TRUE;
	}
	return FALSE;
}

static int run()
{
	int ec = 0;
	SnekcGame *game = snekc_game_new();
	SnekcView *view = snekc_view_new(g_amiwindow);
	unsigned next_tick = 0;

	if (!view)
		goto error;

	new_game(game);
	while (g_quit == 0)
	{
		ULONG signals = Wait(g_timer_signal | g_window_signal);
		if (signals & g_window_signal)
		{
			struct IntuiMessage *msg;
			while (msg = (struct IntuiMessage *) GetMsg(g_amiwindow->UserPort))
			{
				if (!handle_event(msg))
					handle_game_event(game, msg);
				ReplyMsg((struct Message *) msg);
			}
		}
		if (signals & g_timer_signal)
		{
			signals_timer();
			snekc_game_tick(game);
			snekc_view_drawgame(view, game);
		}
	}

	goto end;
error:
	ec = 1;
end:
	if (view)
		snekc_view_free(view);
	if (game)
		snekc_game_free(game);
	return ec;
}

int main(int argc, char *argv[])
{
	int ec;
	(void) argc;
	(void) argv;

	ec = init();
	if (!ec)
		goto end;

	ec = run();
end:
	destroy();
	return ec;
}
