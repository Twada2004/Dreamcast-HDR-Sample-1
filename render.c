#include "render.h"

volatile static struct render_state
{
	int current;
	int current_pass;
	int binlist;
	int render_busy;
	int video_flip;

	int none[3];
} state __attribute__((aligned(32)));

volatile static struct render_internal
{
	int width;		/* Width */
	int height;		/* Height */
	int pixel_type; /* Alpha | Pixel type | Dither */
	int scaler;		/* Scaler */

	int tile; /* Tile buffer address (Internal) */
	int opb;  /* Object Pointer Buffer address (Internal) */

	void *texture;		 /* Texture render address (0:Frame render) */
	int pass_num;		 /* Pass num */
	int pass_binlist[8]; /* Pass list */
} render[2] __attribute__((aligned(32)));

static int video_init(void)
{
	volatile unsigned int *videobase = (volatile unsigned int *)0xa05f8000;
	int width = RENDER_FRAME_WIDTH;
	int height = RENDER_FRAME_HEIGHT;
	int pixel_type = 1;
	int cable_type;
	int video_region;

	/* Get and set cable type */
	*(volatile unsigned int *)0xff80002c = ((*(volatile unsigned int *)0xff80002c) & 0xfff0ffff) | 0x000a0000;
	cable_type = (unsigned int)((*(volatile unsigned short *)0xff800030) & 0x300);
	*(volatile unsigned int *)0xa0702c00 = ((*(volatile unsigned int *)0xa0702c00) & 0xfffffcff) | cable_type;
	cable_type >>= 8;

	/* Store video output region (0 = NTSC, 1 = PAL) */
	video_region = (*(unsigned char *)0x8c000074) - 0x30;

	if (!(cable_type & 2))
	{
		/* VGA */

		videobase[0x0e8 / 4] |= 0x00160008;											  /* Video disable */
		videobase[0x05c / 4] = (1 << 20) | ((height - 1) << 10) | ((width >> 1) - 1); /* Framebuffer size */
		videobase[0x044 / 4] = (1 << 23) | (pixel_type << 2);						  /* Display read mode */
		videobase[0x0cc / 4] = 0x00150208;											  /* VBL offset */
		videobase[0x0d0 / 4] = 0x00000100;											  /* Sync mode */
		videobase[0x0dc / 4] = 0x00280208;											  /* start and end border */
		videobase[0x0e0 / 4] = 0x03f1933f;											  /* Sync width */
		videobase[0x0f0 / 4] = 0x00280028;											  /* V offset */
		videobase[0x0c8 / 4] = 0x03450000;											  /* M */
		videobase[0x0d4 / 4] = 0x007e0345;											  /* Horizontal border */
		videobase[0x0d8 / 4] = 0x020c0359;											  /* HV counter */
		videobase[0x0ec / 4] = 0x000000a8;											  /* Horizontal pos */
	}
	else if (video_region)
	{
		/* Interlace PAL */

		videobase[0x0e8 / 4] |= 0x00160008;																	  /* Video disable */
		videobase[0x05c / 4] = (((width >> 1) + 1) << 20) | (((height >> 1) - 1) << 10) | ((width >> 1) - 1); /* Framebuffer size */
		videobase[0x044 / 4] = (pixel_type << 2);															  /* Display read mode */
		videobase[0x0cc / 4] = 0x00150136;																	  /* VBL offset */
		videobase[0x0d0 / 4] = 0x00000190;																	  /* Sync mode */
		videobase[0x0dc / 4] = 0x002c026c;																	  /* start and end border */
		videobase[0x0e0 / 4] = 0x07d6a53f;																	  /* Sync width */
		videobase[0x0f0 / 4] = 0x002e002d;																	  /* V offset */
		videobase[0x0c8 / 4] = 0x034b0000;																	  /* M */
		videobase[0x0d4 / 4] = 0x008d034b;																	  /* Horizontal border */
		videobase[0x0d8 / 4] = 0x0270035f;																	  /* HV counter */
		videobase[0x0ec / 4] = 0x000000ae;																	  /* Horizontal pos */
	}
	else
	{
		/* Interlace NTSC */

		videobase[0x0e8 / 4] |= 0x00160008;																	  /* Video disable */
		videobase[0x05c / 4] = (((width >> 1) + 1) << 20) | (((height >> 1) - 1) << 10) | ((width >> 1) - 1); /* Framebuffer size */
		videobase[0x044 / 4] = (pixel_type << 2);															  /* Display read mode */
		videobase[0x0cc / 4] = 0x00150104;																	  /* VBL offset */
		videobase[0x0d0 / 4] = 0x00000150;																	  /* Sync mode */
		videobase[0x0dc / 4] = 0x00240204;																	  /* start and end border */
		videobase[0x0e0 / 4] = 0x07d6c63f;																	  /* Sync width */
		videobase[0x0f0 / 4] = 0x00120012;																	  /* V offset */
		videobase[0x0c8 / 4] = 0x03450000;																	  /* M */
		videobase[0x0d4 / 4] = 0x007e0345;																	  /* Horizontal border */
		videobase[0x0d8 / 4] = 0x020c0359;																	  /* HV counter */
		videobase[0x0ec / 4] = 0x000000a8;																	  /* Horizontal pos */
	}

	/* Set video */
	videobase[0x04c / 4] = width >> 2;								 /* Stride value (disp_w * 2) / 8 */
	videobase[0x050 / 4] = 0xa5400000 - (width * height << 1);		 /* Framebuffer odd */
	videobase[0x054 / 4] = 0xa5400000 - (width * (height - 1) << 1); /* Framebuffer even */

	/* Enable video */
	videobase[0x0e8 / 4] &= ~8; /* Video enable */
	videobase[0x044 / 4] |= 1;	/* Video enable */

	return (cable_type != 0);
}

static void video_flip(void)
{
	volatile unsigned int *video = (volatile unsigned int *)0xa05f8054;

	*video ^= 0x00400000;
	*--video ^= 0x00400000;
}

static void vblank_wait(void)
{
	volatile unsigned int *vbl = (volatile unsigned int *)0xa05f810c;

	while (!(*vbl & 0x01ff))
		;

	while (*vbl & 0x01ff)
		;
}

static void vram_clear(void)
{
	volatile unsigned int *dest = (volatile unsigned int *)0xe1000000;
	int i = 0x20000;

	/* QACR0 QACR1 */
	(*(volatile unsigned int *)(void *)0xff000038) = 0x10;
	(*(volatile unsigned int *)(void *)0xff00003c) = 0x10;

	asm volatile(
		"fschg\n\t"
		"fldi0	fr2\n\t"
		"fldi0	fr3\n\t"
		"add	#64, %[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fmov.d	dr2, @-%[d]\n\t"
		"fschg\n\t"
		"1:\n\t"
		"pref	@%[d]\n\t"
		"add	#32, %[d]\n\t"
		"dt		%[n]\n\t"
		"pref	@%[d]\n\t"
		"bf/s	1b\n\t"
		"add	#32, %[d]\n"
		: [d] "+&r"(dest)
		: [n] "r"(i)
		: "t", "fr2", "fr3");
}

void render_set_bgcolor(int target, unsigned int color0, unsigned int color1, unsigned int color2)
{
	unsigned int dest = 0xa5000000 | RENDER_VERTEX_BUFFER | (target << 22);
	volatile unsigned int *d = (volatile unsigned int *)(0xe0000000 | (dest & 0x1fffffff));
	volatile float *f = (volatile float *)d;

	/* QACR0 QACR1 */
	(*(volatile unsigned int *)(void *)0xff000038) = ((unsigned int)dest >> 24) & 0x1c;
	(*(volatile unsigned int *)(void *)0xff00003c) = ((unsigned int)dest >> 24) & 0x1c;

	d[0] = 0x90800000; /* Flag 1 */
	d[1] = 0x20800440; /* Flag 2 */
	d[2] = 0;		   /* Flag 3 */
	f[3] = 0.0f;	   /* x0 */
	f[4] = 480.0f;	   /* y0 */
	f[5] = 1.0f;	   /* z0 */
	d[6] = color0;	   /* color0 */
	f[7] = 0.0f;	   /* x1 */
	asm("pref @%0"
		:
		: "r"(d));
	f[8] = 0.0f;	/* y1 */
	f[9] = 1.0f;	/* z1 */
	d[10] = color1; /* color1 */
	f[11] = 640.0f; /* x2 */
	f[12] = 0.0f;	/* y2 */
	f[13] = 1.0f;	/* z2 */
	d[14] = color2; /* color2 */
	d[15] = 0;
	asm("pref @%0"
		:
		: "r"(d + 8));
}

void render_end_type(int type)
{
	unsigned int *d = (unsigned int *)0xe0000000;

	/* QACR0 QACR1 */
	(*(volatile unsigned int *)(void *)0xff000038) = 0x10; /* 0x10000000 */
	(*(volatile unsigned int *)(void *)0xff00003c) = 0x10; /* 0x10000000 */

	/* Set dummy */
	d[0] = (type << 24) | 0x80840012;
	d[1] = 0;
	d[2] = 0;
	d[3] = 0;
	d[4] = 0xffffffff;
	d[5] = 0xffffffff;
	d[6] = 0xffffffff;
	d[7] = 0xffffffff;
	asm("pref @%0"
		:
		: "r"(d));
	d += 8;

	/* Set EOL */
	d[0] = 0;
	d[1] = 0;
	d[2] = 0;
	d[3] = 0;
	d[4] = 0;
	d[5] = 0;
	d[6] = 0;
	d[7] = 0;
	asm("pref @%0"
		:
		: "r"(d));
	d += 8;
}

void render_interrupt(unsigned int code)
{
	switch (code)
	{
	case 6: /* YUV done */
		break;
	case 7: /* OP done */
		state.binlist &= 0xfffffff0;
		break;
	case 8: /* OM done */
		state.binlist &= 0xffffff0f;
		break;
	case 9: /* TR done */
		state.binlist &= 0xfffff0ff;
		break;
	case 10: /* TM done */
		state.binlist &= 0xffff0fff;
		break;
	case 21: /* PT done */
		state.binlist &= 0xfff0ffff;
		break;
	case 2: /* Render done */
		state.render_busy = 0;
		if (state.video_flip)
		{
			vblank_wait();
			video_flip();
			state.video_flip = 0;
		}
		break;

	case 5: /* V-Blank */
		break;
	}
}

int render_ready(void)
{
	volatile unsigned int *regs = (volatile unsigned int *)0xa05f8000;
	volatile struct render_internal *target = &(render[state.current]);
	int offset = state.current << 22;
	int tile_w = target->width >> 5;
	int tile_h = target->height >> 5;

	while (state.binlist)
		;
	state.binlist = target->pass_binlist[0] & 0x0fffffff;

	regs[0x008 / 4] = 1; /* Reset TA */
	regs[0x008 / 4] = 0;

	regs[0x124 / 4] = target->opb & 0x007fffff;				/* Object Pointer Buffer */
	regs[0x12c / 4] = offset | RENDER_TEXTURE_LIMIT;		/* Texture limit */
	regs[0x128 / 4] = (offset | RENDER_VERTEX_BUFFER) + 96; /* Vertex buffer + (BG Plane) */
	regs[0x130 / 4] = offset | RENDER_FRAME_BUFFER;			/* Vertex buffer limit */
	regs[0x164 / 4] = target->opb & 0x007fffff;				/* OPB Stack */
	regs[0x13c / 4] = ((tile_h - 1) << 16) | (tile_w - 1);	/* Tile size */
	regs[0x140 / 4] = target->pass_binlist[0] | 0x00100000; /* OPB size */

	regs[0x144 / 4] = 0x80000000; /* Render begin */

	/* Pass update */
	state.current_pass = 1;

	return regs[0x144 / 4];
}

int render_nextpass(void)
{
	volatile unsigned int *regs = (volatile unsigned int *)0xa05f8000;
	volatile struct render_internal *target = &(render[state.current]);
	int tile_w = target->width >> 5;
	int tile_h = target->height >> 5;
	int binlist, size;

	/* Calculate prev OPB size */
	binlist = target->pass_binlist[state.current_pass - 1];
	size = (16 << (binlist & 3)) & -32; /* Opaque polygon */
	binlist >>= 4;
	size += (16 << (binlist & 3)) & -32; /* Opaque modifier */
	binlist >>= 4;
	size += (16 << (binlist & 3)) & -32; /* Translucent polygon */
	binlist >>= 4;
	size += (16 << (binlist & 3)) & -32; /* Translucent modifier */
	binlist >>= 4;
	size += (16 << (binlist & 3)) & -32; /* Punch-through */
	size *= tile_w * tile_h;

	while (state.binlist)
		;
	state.binlist = target->pass_binlist[state.current_pass] & 0x0fffffff;

	regs[0x124 / 4] += size;					  /* Object Pointer Buffer */
	regs[0x140 / 4] = state.binlist | 0x00100000; /* OPB size */
	regs[0x160 / 4] = 0x80000000;				  /* Next Pass begin */

	/* Pass update */
	state.current_pass++;

	return regs[0x160 / 4];
}

void render_begin(void)
{
	volatile unsigned int *regs = (volatile unsigned int *)0xa05f8000;
	volatile struct render_internal *target = &(render[state.current]);
	int offset = state.current << 22;
	int screen_w, screen_h;
	int screen;

	while (state.binlist)
		;

	while (state.render_busy)
		;

	/* Render output */
	if (target->texture)
	{
		/* Texture render */
		screen_w = target->width;
		screen_h = target->height;
		screen = ((int)target->texture & 0x00ffffff) | 0x01000000;
	}
	else
	{
		/* Frame render */
		screen_w = RENDER_FRAME_WIDTH;
		screen_h = RENDER_FRAME_HEIGHT;
		screen = regs[0x050 / 4] ^ 0x00400000;
		state.video_flip = 1;
	}

	regs[0x008 / 4] = 2;							 /* TA liset */
	regs[0x008 / 4] = 0;							 /* TA ready */
	regs[0x0f4 / 4] = target->scaler;				 /* Scaler */
	regs[0x02c / 4] = target->tile & 0x007fffff;	 /* Tile buffer */
	regs[0x020 / 4] = offset | RENDER_VERTEX_BUFFER; /* Vertex buffer */
	regs[0x060 / 4] = screen;						 /* Even frame */
	regs[0x064 / 4] = screen + (screen_w << 1);		 /* Odd frame */
	regs[0x068 / 4] = (screen_w - 1) << 16;			 /* Pixel clip X */
	regs[0x06c / 4] = (screen_h - 1) << 16;			 /* Pixel clip Y */
	regs[0x04c / 4] = (screen_w << 1) >> 3;			 /* Frame width */
	regs[0x048 / 4] = target->pixel_type;			 /* Pixel type */
	regs[0x014 / 4] = 0xffffffff;					 /* Begin render */

	/* Update state */
	state.current = 1 - state.current;
	state.render_busy = 1;
}

static void target_set_buffer(struct render_internal *target, void *buf_addr)
{
	int *tile_addr = (int *)buf_addr;
	int *tile = 0;
	int opb_addr;
	int *index = (int *)target->tile;
	int w = target->width >> 5;
	int h = target->height >> 5;
	int p = target->pass_num;
	int i;

	/* Get buffer addr */
	tile_addr -= w * h * p * 6;
	tile_addr = (int *)(((unsigned int)tile_addr - 64) & -32);
	opb_addr = (int)tile_addr & 0x00ffffff;

	for (i = p; i; i--)
	{
		int flags = target->pass_binlist[i - 1] & 0x20000000; /* Pre-sort */
		int binlist = target->pass_binlist[i - 1] & (~0x20000000);
		int opb[5];
		int size[5];
		int y;

		/* Set flags */
		if (i != 1)
			flags |= (1 << 30); /* Do not depth clear */
		if (i != p)
			flags |= (1 << 28); /* Do not frame flush */

		/* Set OPB size */
		size[0] = (16 << (binlist & 3)) & -32; /* Opaque polygon */
		binlist >>= 4;
		size[1] = (16 << (binlist & 3)) & -32; /* Opaque modifier */
		binlist >>= 4;
		size[2] = (16 << (binlist & 3)) & -32; /* Translucent polygon */
		binlist >>= 4;
		size[3] = (16 << (binlist & 3)) & -32; /* Translucent modifier */
		binlist >>= 4;
		size[4] = (16 << (binlist & 3)) & -32; /* Punch-through */

		/* Calculate OPB address */
		opb[4] = opb_addr - size[4] * w * h; /* Punch-through */
		opb[3] = opb[4] - size[3] * w * h;	 /* Translucent modifier */
		opb[2] = opb[3] - size[2] * w * h;	 /* Translucent polygon */
		opb[1] = opb[2] - size[1] * w * h;	 /* Opaque modifier */
		opb[0] = opb[1] - size[0] * w * h;	 /* Opaque polygon */
		opb_addr = opb[0];

		for (y = 0; y < h; y++)
		{
			int x;

			for (x = 0; x < w; x++)
			{
				if (index)
					tile = tile_addr + ((index[y * w + x] * p + i) * 6);
				else
					tile = tile_addr + (((x * h + y) * p + i) * 6);

				*--tile = size[4] ? opb[4] : 0x80000000; /* Punch-through */
				opb[4] += size[4];
				*--tile = size[3] ? opb[3] : 0x80000000; /* Translucent modifier */
				opb[3] += size[3];
				*--tile = size[2] ? opb[2] : 0x80000000; /* Translucent polygon */
				opb[2] += size[2];
				*--tile = size[1] ? opb[1] : 0x80000000; /* Opaque modifier */
				opb[1] += size[1];
				*--tile = size[0] ? opb[0] : 0x80000000; /* Opaque polygon */
				opb[0] += size[0];

				*--tile = flags | (y << 8) | (x << 2); /* Flags */
			}
		}

		if (i == p)
		{
			/* End of tile */
			*tile |= 0x80000000;
		}
	}

	target->tile = (int)tile_addr;
	target->opb = opb_addr;
}

void render_init(render_t *target0, render_t *target1)
{
	volatile unsigned int *videobase = (volatile unsigned int *)0xa05f8000;
	int cable_type;

	/* Init video */
	cable_type = video_init();

	/* Clear VRAM */
	vram_clear();

	/* Init regester */
	videobase[0x0a8 / 4] = 0x15d1c951; /* M (Unknown magic value) */
	videobase[0x0a0 / 4] = 0x00000020; /* M */
	videobase[0x040 / 4] = 0x00000000; /* Border color*/
	videobase[0x110 / 4] = 0x00093f39; /* M */
	videobase[0x098 / 4] = 0x00800408; /* M */
	videobase[0x084 / 4] = 0x03f80000; /* texture clip distance (float) */
	videobase[0x030 / 4] = 0x00000101; /* M */
	videobase[0x0b0 / 4] = 0x007f7f7f; /* Fog table color */
	videobase[0x0b4 / 4] = 0x007f7f7f; /* Fog vertex color */
	videobase[0x0b8 / 4] = 0x0000ff07; /* Fog density */
	videobase[0x0c0 / 4] = 0x00000000; /* Color clamp min */
	videobase[0x0bc / 4] = 0xffffffff; /* Color clamp max */
	videobase[0x080 / 4] = 0x00000007; /* M */
	videobase[0x088 / 4] = 0x00000000; /* Background depth */
	videobase[0x074 / 4] = 0x00000180; /* Cheap shadow */
	videobase[0x078 / 4] = 0x3f800000; /* polygon culling (float) */
	videobase[0x07c / 4] = 0x0027df77; /* M */
	videobase[0x0e4 / 4] = 0x00000014; /* Stride width */
	videobase[0x118 / 4] = 0x00008040; /* M */
	videobase[0x11c / 4] = 0x00000080; /* PT alpha value */

	/* PVR reset */
	videobase[0x008 / 4] = 0xffffffff; /* PVR reset */
	videobase[0x008 / 4] = 0x00000000; /* PVR ready */

	/* Wait VBL */
	vblank_wait();

	/* Init Render state */
	state.current = 0;
	state.current_pass = 0;
	state.binlist = 0;
	state.render_busy = 0;
	state.video_flip = 0;

	/* Set BG color */
	videobase[0x088 / 4] = 0x00000000;												/* BG Depth (float) */
	videobase[0x08c / 4] = 0x01000000 | ((RENDER_VERTEX_BUFFER & 0x000ffffc) << 1); /* BG Plane */
	render_set_bgcolor(0, target0->bgcolor, target0->bgcolor, target0->bgcolor);
	render_set_bgcolor(1, target1->bgcolor, target1->bgcolor, target1->bgcolor);

	/* Init Target */
	render[0] = *(struct render_internal *)target0;
	render[1] = *(struct render_internal *)target1;

	/* Create buffer */
	target_set_buffer((struct render_internal *)&render[0], (void *)(0xa5000000 | RENDER_VERTEX_BUFFER));
	target_set_buffer((struct render_internal *)&render[1], (void *)(0xa5400000 | RENDER_VERTEX_BUFFER));

	/* Set flicker-free filter */
	if (cable_type)
	{
		if (!render[0].texture)
			render[0].scaler |= 1;
		if (!render[1].texture)
			render[1].scaler |= 1;
	}
}
