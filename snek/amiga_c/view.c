#include "view.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH_LEN 65536
#define MAX_DIGITS 32

struct _SnekcView
{
	int calloc_crash_preventor;
};

SnekcView *snekc_view_new()
{
	SnekcView *view = (SnekcView *) calloc(1, sizeof(SnekcView));
	/*SnekcView *view = (SnekcView *) malloc(sizeof(SnekcView));*/
	return view;
error:
	snekc_view_free(view);
	return NULL;
}

void snekc_view_free(SnekcView *view)
{
	free(view);
}

void snekc_view_drawgame(SnekcView *view, SnekcGame *game)
{
}
