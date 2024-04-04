
#ifndef _RENDER_H_INCLUDED_
#define _RENDER_H_INCLUDED_

//
// VRAM Layout (32 bit access)
//
// +--+--+-- 0xa5000000 / 0xa5400000
// |:::::|
// |:::::|	Texture area (64 bit access)
// |:::::|
// +--+--+-- Texture limit
// |  |  |
// |  |  |	OPB stack space (upward)
// |  |  |
// +--+--+-- OPB address (32 byte align)
// |  |  |
// |  |  |	Object Pointer Buffer
// |  |  |		Tile_w * Tile_h * Pass((OP0 + OM0 + TR0 + TM0 + PT0) + (OP1 + OM1 + TR1 + TM1 + PT1) + ....)
// |  |  |
// +--+--+-- Tile address (32 byte align)
// |  |  |
// |  |  |	Tile Matrix
// |  |  |		Tile_w * Tile_h * Pass * 24;
// |  |  |
// +--+--+-- Vertex address (1 Mbyte align)
// |  |  |
// |  |  |	Vertex Buffer
// |  |  |
// +--+--+-- Frame Buffer address
// |  |  |
// |  |  |	Frame Buffer
// |  |  |
// +--+--+-- 0xa53fffff / 0xa57fffff
//

#define RENDER_TEXTURE_LIMIT 0x001c0000
#define RENDER_VERTEX_BUFFER 0x00200000
#define RENDER_FRAME_WIDTH 640
#define RENDER_FRAME_HEIGHT 480
#define RENDER_FRAME_BUFFER (0x00400000 - (RENDER_FRAME_WIDTH * RENDER_FRAME_HEIGHT * 2))

/* Render struct (64byte) */
typedef struct
{
	int width;			 /* Width */
	int height;			 /* Height */
	int pixel_type;		 /* Alpha | Pixel type | Dither */
	int scaler;			 /* Scaler */
	int *tile;			 /* Tile index array (0:Vertical) */
	int bgcolor;		 /* Background color */
	void *texture;		 /* Texture render address (0:Frame render) */
	int pass_num;		 /* Pass num */
	int pass_binlist[8]; /* Pass list */
	/* binlist = ((PreSort << 29) | (PT << 16) | (TM << 12) | (TR << 8) | (OM << 4) | OP) */
} render_t;

extern void render_interrupt(unsigned int code);
extern void render_init(render_t *target0, render_t *target1);
extern int render_ready(void);
extern int render_nextpass(void);
extern void render_begin(void);
extern void render_end_type(int type);

#endif