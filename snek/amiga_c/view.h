#ifndef idd9d54c34_47e9_44ea_8d80_3f55aaa92d55
#define idd9d54c34_47e9_44ea_8d80_3f55aaa92d55

#include <intuition/intuition.h>

#include "game.h"

struct _SnekcView;
typedef struct _SnekcView SnekcView;

SnekcView *snekc_view_new(struct Window *window);
void snekc_view_free(SnekcView *);

void snekc_view_drawgame(SnekcView *view, SnekcGame *game);

#endif
