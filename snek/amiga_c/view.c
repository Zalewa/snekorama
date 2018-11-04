#include "view.h"

#include "game.h"

#include <proto/graphics.h>
#include <proto/datatypes.h>
#include <intuition/screens.h>
#include <datatypes/pictureclass.h>

#include <stdlib.h>
#include <stdio.h>

#define NUM_PENS 32
static ULONG m_pens[NUM_PENS];

typedef struct
{
	ULONG red, green, blue;
} Color;
/**
 * Color palette has been choosen by converting the sprites.bmp
 * asset to sprites.iff and examining what color values were
 * chosen by the converter program (ImageStudio for Amiga).
 *
 * Note: the automatic conversion produced a mightly ugly picture.
 */
static const Color colors[NUM_PENS] = {
	{4, 2, 4},
	{7, 152, 29},
	{169, 182, 43},
	{164, 2, 4},
	{6, 205, 40},
	{144, 82, 15},
	{6, 75, 12},
	{81, 140, 30},
	{215, 214, 215},
	{6, 175, 34},
	{111, 94, 20},
	{216, 13, 4},
	{143, 99, 20},
	{99, 4, 4},
	{193, 203, 60},
	{4, 233, 46},
	{85, 117, 25},
	{19, 55, 12},
	{183, 54, 7},
	{4, 104, 20},
	{100, 52, 12},
	{203, 43, 5},
	{206, 216, 103},
	{234, 236, 234},
	{68, 162, 35},
	{240, 6, 4},
	{47, 4, 4},
	{50, 122, 20},
	{171, 70, 11},
	{46, 182, 42},
	{46, 141, 28},
	{113, 116, 24}
};


static const struct IntuiText m_score_value = {1, 0, JAM1, 0, 0, 0, "0", 0};
static const struct IntuiText m_score_label = {1, 0, JAM1, 0, 0, 0, "Score:", 0};

struct _SnekcView
{
	struct Window *window;
	struct IntuiText score_label;
	struct IntuiText score_value;
	struct RastPort sprites;
	char *score_buffer;
};

/**
 * Set the color palette that will be used by the game to display
 * graphics.
 */
static BOOL init_colormap(SnekcView *view)
{
	ULONG i;
	struct ColorMap *colormap = view->window->WScreen->ViewPort.ColorMap;

	for (i = 0; i < NUM_PENS; ++i)
	{
		ULONG red = (colors[i].red << 24) | 0xffffff;
		ULONG green = (colors[i].green << 24) | 0xffffff;
		ULONG blue = (colors[i].blue << 24) | 0xffffff;

		/* This is mandatory to properly display the graphics. */
		SetRGB32(&view->window->WScreen->ViewPort, i, red, green, blue);

		/* ObtainPen function always fails on a custom screen.
		   I don't know why. */
		ULONG pen = ObtainPen(colormap, i,
			red,
			green,
			blue, PEN_EXCLUSIVE);
		if ((ULONG)-1 == pen)
			pen = ObtainBestPenA(colormap, red, green, blue, NULL);
		m_pens[i] = pen;
	}
	return TRUE;
}

static void free_colormap(SnekcView *view)
{
	struct ColorMap *colormap = view->window->WScreen->ViewPort.ColorMap;
	int i;
	/* If ObtainPen always fails, should we call ReleasePen?
	   These pens are obtained through ObtainBestPen. */
	for (i = 0; i < NUM_PENS; ++i)
		ReleasePen(colormap, m_pens[i]);
}

static BOOL load_bitmap(SnekcView *view, const char *name, struct RastPort *dst_rastport)
{
	/* The tutorial locked the public screen here. Because we
	   use a custom screen, we don't need to lock it. */
	BOOL result = FALSE;
	ULONG width, height, depth;
	struct BitMap *bitmap, *alloc_bitmap;

	/* Note: PDTA_Remap TRUE doesn't preserve all colors.
	   It seems only the first 4 colors are used.
	   I currently don't know why.

	   Nevertheless, the imported picture already uses
	   the same colormap as the program, so we don't
	   really need to remap.

	   Still, it would be good to know why this doesn't work.
	*/
	Object *obj = (Object *) NewDTObject((APTR) name,
		DTA_GroupID, GID_PICTURE,
		PDTA_Remap, FALSE,
		PDTA_Screen, view->window->WScreen,
		TAG_END);
	if (!obj)
	{
		fprintf(stderr, "cannot load %s\n", name);
		goto end;
	}
	DoDTMethod(obj, NULL, NULL, DTM_PROCLAYOUT, NULL, TRUE);
	GetDTAttrs(obj,
		PDTA_DestBitMap, &bitmap,
		TAG_END);
	width = GetBitMapAttr(bitmap, BMA_WIDTH);
	height = GetBitMapAttr(bitmap, BMA_HEIGHT);
	depth = GetBitMapAttr(bitmap, BMA_DEPTH);

	alloc_bitmap = AllocBitMap(width, height, depth, BMF_DISPLAYABLE | BMF_CLEAR, NULL);
	if (!alloc_bitmap)
	{
		fprintf(stderr, "cannot alloc bitmap %s\n", name);
		goto end;
	}

	BltBitMap(bitmap, 0, 0,
		alloc_bitmap, 0, 0,
		width, height,
		0xc0, 0xff, NULL);
	InitRastPort(dst_rastport);
	dst_rastport->BitMap = alloc_bitmap;

	result = TRUE;
end:
	if (obj)
		DisposeDTObject(obj);
	return result;
}

SnekcView *snekc_view_new(struct Window *window)
{
	SnekcView *view = (SnekcView *) calloc(1, sizeof(SnekcView));
	view->window = window;
	view->score_buffer = (char*)malloc(256);
	if (!init_colormap(view))
		goto error;

	view->score_label = m_score_label;
	view->score_label.TopEdge = window->Height - 8;

	sprintf(view->score_buffer, "0");
	view->score_value = m_score_value;
	view->score_value.IText = view->score_buffer;
	view->score_value.TopEdge = view->score_label.TopEdge;
	view->score_value.LeftEdge = IntuiTextLength(&view->score_label);

	view->score_label.NextText = &view->score_value;

	view->sprites.BitMap = NULL;
	if (!load_bitmap(view, "sprites.iff", &view->sprites))
		goto error;

	return view;
error:
	snekc_view_free(view);
	return NULL;
}

void snekc_view_free(SnekcView *view)
{
	if (view->sprites.BitMap)
		FreeBitMap(view->sprites.BitMap);
	free(view->score_buffer);
	free_colormap(view);
	free(view);
}

void snekc_view_drawgame(SnekcView *view, SnekcGame *game)
{
	sprintf(view->score_buffer, "%d", snekc_game_score(game));
	BltBitMapRastPort(view->sprites.BitMap, 0, 0, view->window->RPort, 0, 0, 128, 64, 0xc0);
	PrintIText(view->window->RPort, &view->score_label, 0, 0);
}
