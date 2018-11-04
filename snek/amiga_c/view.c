#include "view.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_PATH_LEN 65536
#define MAX_DIGITS 32

static const struct IntuiText m_score_value = {1, 0, JAM1, 0, 0, 0, "0", 0};
static const struct IntuiText m_score_label = {1, 0, JAM1, 0, 0, 0, "Score:", 0};

struct _SnekcView
{
	struct Window *window;
	struct IntuiText score_label;
	struct IntuiText score_value;
};

SnekcView *snekc_view_new(struct Window *window)
{
	SnekcView *view = (SnekcView *) calloc(1, sizeof(SnekcView));
	view->window = window;

	view->score_label = m_score_label;
	view->score_label.TopEdge = window->Height - 8;

	view->score_value = m_score_value;
	view->score_value.TopEdge = view->score_label.TopEdge;
	view->score_value.LeftEdge = IntuiTextLength(&view->score_label);

	view->score_label.NextText = &view->score_value;
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
	PrintIText(view->window->RPort, &view->score_label, 0, 0);
}
