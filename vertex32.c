#include "vertex32.h"

static void commit_vert(vertex32_t *d, vertex32_t *p, int eos)
{
	asm volatile(
		"fschg\n\t"
		"add	#14, %[end]\n\t"
		"fmov	@%[src]+, dr0\n\t"
		"shld	%[shift], %[end]\n\t"
		"fmov	@%[src]+, dr2\n\t"
		"lds	%[end], fpul\n\t"
		"fmov	@%[src]+, dr4\n\t"
		"add	#32, %[dest]\n\t"
		"fmov	@%[src]+, dr6\n\t"
		"fsts	fpul, fr0\n\t"
		"fmov	dr6,@-%[dest]\n\t"
		"fmov	dr4,@-%[dest]\n\t"
		"fmov	dr2,@-%[dest]\n\t"
		"fmov	dr0,@-%[dest]\n\t"
		"pref	@%[dest]\n\t"
		"fschg\n"
		: [dest] "+&r"(d), [src] "+&r"(p), [end] "+r"(eos)
		: [shift] "r"(28)
		: "fpul", "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7");
}

static void commit_vert_inter(vertex32_t *d, vertex32_t *p, vertex32_t *q, int eos)
{
	unsigned int p_rb;
	unsigned int p_ag;
	unsigned int q_rb;
	unsigned int q_ag;

	asm volatile(
		"fschg\n\t"

		"fmov	@%[src0]+, dr0\n\t"
		"fmov	@%[src1]+, dr8\n\t"
		"mov	#28, %[rb0]\n\t"
		"fmov	dr0, dr2\n\t"
		"add	#14, %[ii]\n\t"
		"fmov	dr0, dr6\n\t"
		"shld	%[rb0], %[ii]\n\t"

		"fldi1	fr1\n\t"
		"fsub	fr1, fr0\n\t"
		"fsub	fr8, fr6\n\t"
		"lds	%[ii], fpul\n\t"
		"fdiv	fr6, fr0\n\t"

		"fmov	@%[src0]+, dr4\n\t"
		"fmov	@%[src1]+, dr10\n\t"

		"fmul	fr2, fr3\n\t"
		"add	#32, %[dest]\n\t"
		"fmul	fr8, fr9\n\t"
		"mov	#-1, %[ii]\n\t"
		"fmul	fr2, fr4\n\t"
		"extu.b	%[ii], %[ii]\n\t"
		"fmul	fr8, fr10\n\t"
		"fldi1	fr5\n\t"

		"fsub	fr3, fr9\n\t"
		"fsts	fpul, fr2\n\t"
		"fsub	fr4, fr10\n\t"
		"lds	%[ii], fpul\n\t"
		"float	fpul, fr1\n\t"

		"fmac	fr0, fr9, fr3\n\t"
		"fmac	fr0, fr10, fr4\n\t"

		"fmov	@%[src0]+, dr6\n\t"
		"fmul	fr0, fr1\n\t"
		"fmov	@%[src1]+, dr8\n\t"
		"fsub	fr6, fr8\n\t"
		"ftrc	fr1, fpul\n\t"
		"fsub	fr7, fr9\n\t"
		"fmac	fr0, fr8, fr6\n\t"
		"sts	fpul, %[ii]\n\t"
		"fmac	fr0, fr9, fr7\n\t"

		"mov.l	@(4, %[src0]), %[rb0]\n\t"
		"mov	%[rb0], %[ag0]\n\t"
		"and	%[mask], %[rb0]\n\t"
		"shlr8	%[ag0]\n\t"
		"mov.l	@(4, %[src1]), %[rb1]\n\t"
		"and	%[mask], %[ag0]\n\t"
		"mov	%[rb1], %[ag1]\n\t"
		"and	%[mask], %[rb1]\n\t"
		"shlr8	%[ag1]\n\t"
		"and	%[mask], %[ag1]\n\t"

		"sub	%[ag0], %[ag1]\n\t"
		"mul.l	%[ii], %[ag1]\n\t"
		"sub	%[rb0], %[rb1]\n\t"
		"sts	macl, %[ag1]\n\t"
		"shlr8	%[ag1]\n\t"
		"add	%[ag1], %[ag0]\n\t"
		"mul.l	%[ii], %[rb1]\n\t"
		"and	%[mask], %[ag0]\n\t"
		"shll8	%[ag0]\n\t"
		"sts	macl, %[rb1]\n\t"
		"shlr8	%[rb1]\n\t"
		"add	%[rb1], %[rb0]\n\t"
		"and	%[mask], %[rb0]\n\t"
		"or		%[ag0], %[rb0]\n"
		"mov.l	%[rb0], @-%[dest]\n\t"

		"mov.l	@%[src0], %[rb0]\n\t"
		"mov	%[rb0], %[ag0]\n\t"
		"and	%[mask], %[rb0]\n\t"
		"shlr8	%[ag0]\n\t"
		"mov.l	@%[src1], %[rb1]\n\t"
		"and	%[mask], %[ag0]\n\t"
		"mov	%[rb1], %[ag1]\n\t"
		"and	%[mask], %[rb1]\n\t"
		"shlr8	%[ag1]\n\t"
		"and	%[mask], %[ag1]\n\t"

		"sub	%[ag0], %[ag1]\n\t"
		"mul.l	%[ii], %[ag1]\n\t"
		"sub	%[rb0], %[rb1]\n\t"
		"sts	macl, %[ag1]\n\t"
		"shlr8	%[ag1]\n\t"
		"add	%[ag1], %[ag0]\n\t"
		"mul.l	%[ii], %[rb1]\n\t"
		"and	%[mask], %[ag0]\n\t"
		"shll8	%[ag0]\n\t"
		"sts	macl, %[rb1]\n\t"
		"shlr8	%[rb1]\n\t"
		"add	%[rb1], %[rb0]\n\t"
		"and	%[mask], %[rb0]\n\t"
		"or		%[ag0], %[rb0]\n"
		"mov.l	%[rb0], @-%[dest]\n\t"

		"fmov	dr6,@-%[dest]\n\t"
		"fmov	dr4,@-%[dest]\n\t"
		"fmov	dr2,@-%[dest]\n\t"

		"pref	@%[dest]\n\t"
		"fschg\n"
		: [dest] "+&r"(d), [src0] "+&r"(p), [src1] "+&r"(q), [ii] "+r"(eos), [rb0] "=&r"(p_rb), [rb1] "=&r"(q_rb), [ag0] "=&r"(p_ag), [ag1] "=&r"(q_ag)
		: [mask] "r"(0x00ff00ff)
		: "macl", "fpul", "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");
}

int vertex32_set_zclip_polygon(void *dest, vertex32_t *src, int *index)
{
	vertex32_t *d = dest;
	vertex32_t *p = src;
	int size = *index++;

	/* Use SQ */
	if ((unsigned int)d & 0x10000000)
	{
		/* QACR0 QACR1 */
		(*(volatile unsigned int *)(void *)0xff000038) = 0x10; /* 0x10000000 */
		(*(volatile unsigned int *)(void *)0xff00003c) = 0x10; /* 0x10000000 */

		dest = d = (vertex32_t *)(((unsigned int)d & 0x0fffffff) | 0xe0000000);
	}

	while (size)
	{
		int strip_num = 0;
		int clip = 0;
		int eos = 0;
		float w;

		w = p[*index++].w;

		if (w > 1.0f)
		{
			w = p[*index++].w;

			commit_vert(d++, &p[index[-2]], 0);

			if (w > 1.0f)
			{
				/* 0, 1 inside */
				commit_vert(d++, &p[index[-1]], 0);

				clip = 6;
			}
			else
			{
				/* 0 inside, 1 outside */
				commit_vert_inter(d++, &p[index[-2]], &p[index[-1]], 0);

				clip = 2;
			}
		}
		else
		{
			w = p[*index++].w;

			if (w > 1.0f)
			{
				/* 0 outside and 1 inside */
				commit_vert_inter(d++, &p[index[-1]], &p[index[-2]], 0);
				commit_vert(d++, &p[index[-1]], 0);

				clip = 4;
			}
		}

		/* Third point and more. */
		for (strip_num = 2; !eos; strip_num++)
		{
			int index_1 = *index++;

			/* End of strip. */
			if (index_1 < 0)
			{
				index_1 = -index_1;
				eos = 1;
			}

			/* Inside check. */
			w = p[index_1].w;

			/* clip code. */
			clip >>= 1;

			if (w > 1.0f)
			{
				clip |= 4;
			}
			else if (!clip)
			{
				continue;
			}

			if (clip == 7)
			{
				/* all inside */
				commit_vert(d++, &p[index_1], eos);
				continue;
			}
			else
			{
				switch (clip)
				{
				case 1: /* 0 inside, 1 and 2 outside */
					/* Pause strip */
					commit_vert_inter(d++, &p[index[-3]], &p[index_1], 1);
					break;
				case 3: /* 0 and 1 inside, 2 outside */
					commit_vert_inter(d++, &p[index[-3]], &p[index_1], 0);
					commit_vert(d++, &p[index[-2]], 0);
				case 2: /* 0 outside, 1 inside, 2 outside */
					commit_vert_inter(d++, &p[index[-2]], &p[index_1], eos);
					break;
				case 4: /* 0 and 1 outside, 2 inside */
					commit_vert_inter(d++, &p[index_1], &p[index[-3]], 0);
					if (strip_num & 0x01)
					{
					case 5: /* 0 inside, 1 outside and 2 inside */
						/* Turn over */
						commit_vert(d++, &p[index_1], 0);
					}
					commit_vert_inter(d++, &p[index_1], &p[index[-2]], 0);
					commit_vert(d++, &p[index_1], eos);
					break;
				case 6: /* 0 outside, 1 and 2 inside */
					commit_vert_inter(d++, &p[index_1], &p[index[-3]], 0);
					commit_vert(d++, &p[index[-2]], 0);
					commit_vert(d++, &p[index_1], eos);
					break;
				default:
					break;
				}
			}
		}

		size -= strip_num;
	}

	return (int)((unsigned int)d - (unsigned int)dest);
}

int vertex32_set_polygon(void *dest, vertex32_t *src, int *index)
{
	vertex32_t *d = dest;
	vertex32_t *p = src;
	int size = *index++;
	unsigned int hdr = 0xe0000000;
	unsigned int end = 0xf0000000;

	/* Use SQ */
	if ((unsigned int)d & 0x10000000)
	{
		/* QACR0 QACR1 */
		(*(volatile unsigned int *)(void *)0xff000038) = 0x10; /* 0x10000000 */
		(*(volatile unsigned int *)(void *)0xff00003c) = 0x10; /* 0x10000000 */

		dest = d = (vertex32_t *)(((unsigned int)d & 0x0fffffff) | 0xe0000000);
	}

	asm volatile(
		"fschg\n\t"
		:
		:);

	while (size)
	{
		int eos = hdr;

		for (; eos == hdr; size--)
		{
			int idx = *index++;

			if (idx < 0)
			{
				idx = -idx;
				eos = end;
			}

			asm volatile(
				"fmov	@%[src]+, dr0\n\t"
				"or		%[f], %[e]\n\t"
				"fmov	@%[src]+, dr2\n\t"
				"lds	%[e], fpul\n\t"
				"fmov	@%[src]+, dr4\n\t"
				"add	#32, %[dest]\n\t"
				"fmov	@%[src]+, dr6\n\t"
				"fsts	fpul, fr0\n\t"
				"fmov	dr6,@-%[dest]\n\t"
				"fmov	dr4,@-%[dest]\n\t"
				"fmov	dr2,@-%[dest]\n\t"
				"fmov	dr0,@-%[dest]\n\t"
				"pref	@%[dest]\n"
				: [e] "+r"(eos)
				: [f] "r"(hdr), [dest] "r"(d++), [src] "r"(&p[idx])
				: "fpul", "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7");
		}
	}

	asm volatile(
		"fschg\n\t"
		:
		:);

	return (int)((unsigned int)d - (unsigned int)dest);
}
