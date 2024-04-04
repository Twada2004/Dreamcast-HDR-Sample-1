#ifndef _VERTEX32_H_INCLUDED_
#define _VERTEX32_H_INCLUDED_

typedef struct
{
	float w, x, y, z;
	float u, v;
	unsigned int base;
	unsigned int offset;
} vertex32_t __attribute__((aligned(32)));

extern int vertex32_set_zclip_polygon(void *dest, vertex32_t *src, int *index);
extern int vertex32_set_polygon(void *dest, vertex32_t *src, int *index);

#endif
