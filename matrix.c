// ****************************************************************
// ****************************************************************
//  XMTRX register layout
//
//  reg\vec4  | fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------+-----------
//  fr[n + 0] | fr0       | fr4       | fr8       | fr12
// -----------+-----------+-----------+-----------+-----------
//  fr[n + 1] | fr1       | fr5       | fr9       | fr13
// -----------+-----------+-----------+-----------+-----------
//  fr[n + 2] | fr2       | fr6       | fr10      | fr14
// -----------+-----------+-----------+-----------+-----------
//  fr[n + 3] | fr3       | fr7       | fr11      | fr15
// -----------+-----------+-----------+-----------+-----------
// ****************************************************************
// ****************************************************************

void matrix_get(float *dest)
{
	asm volatile(
		"pref	@%[d]\n\t"
		"fschg\n\t"
		"add	#64,%[d]\n\t"
		"fmov.d	xd14,@-%[d]\n\t"
		"fmov.d	xd12,@-%[d]\n\t"
		"fmov.d	xd10,@-%[d]\n\t"
		"fmov.d	xd8,@-%[d]\n\t"
		"fmov.d	xd6,@-%[d]\n\t"
		"fmov.d	xd4,@-%[d]\n\t"
		"fmov.d	xd2,@-%[d]\n\t"
		"fmov.d	xd0,@-%[d]\n\t"
		"fschg\n"
		: [d] "+&r"(dest)
		:
		: "memory");
}

void matrix_set(float *src)
{
	asm volatile(
		"pref	@%[s]\n\t"
		"fschg\n\t"
		"add	#32,%[s]\n\t"
		"pref	@%[s]\n\t"
		"add	#-32,%[s]\n\t"
		"fmov.d	@%[s]+,xd0\n\t"
		"fmov.d	@%[s]+,xd2\n\t"
		"fmov.d	@%[s]+,xd4\n\t"
		"fmov.d	@%[s]+,xd6\n\t"
		"fmov.d	@%[s]+,xd8\n\t"
		"fmov.d	@%[s]+,xd10\n\t"
		"fmov.d	@%[s]+,xd12\n\t"
		"fmov.d	@%[s]+,xd14\n\t"
		"fschg\n"
		:
		: [s] "r"(src)
		:);
}

void matrix_apply(float *src)
{
	asm volatile(
		"pref	@%[s]\n\t"
		"fschg\n\t"
		"add	#32,%[s]\n\t"
		"pref	@%[s]\n\t"
		"add	#-32,%[s]\n\t"
		"fmov.d	@%[s]+,dr0\n\t"
		"fmov.d	@%[s]+,dr2\n\t"
		"ftrv	xmtrx, fv0\n\t"
		"fmov.d	@%[s]+,dr4\n\t"
		"fmov.d	@%[s]+,dr6\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fmov.d	@%[s]+,dr8\n\t"
		"fmov.d	@%[s]+,dr10\n\t"
		"ftrv	xmtrx, fv8\n\t"
		"fmov.d	@%[s]+,dr12\n\t"
		"fmov.d	@%[s]+,dr14\n\t"
		"ftrv	xmtrx, fv12\n\t"
		"frchg\n\t"
		"fschg\n"
		:
		: [s] "r"(src)
		: "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "fr12", "fr13", "fr14", "fr15");
}

// ****************************************************************
// void matrix_set_identity(void)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	1.0f      | 0.0f      | 0.0f      | 0.0f
// 	0.0f      | 1.0f      | 0.0f      | 0.0f
// 	0.0f      | 0.0f      | 1.0f      | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_set_identity(void)
{
	asm volatile(
		"fschg\n\t"
		"frchg\n\t"
		"fldi1	fr0\n\t"
		"fldi0	fr1\n\t"
		"fldi0	fr2\n\t"
		"fldi0	fr3\n\t"
		"fldi0	fr4\n\t"
		"fldi1	fr5\n\t"
		"fmov	dr2,dr6\n\t"
		"fmov	dr2,dr8\n\t"
		"fmov	dr0,dr10\n\t"
		"fmov	dr2,dr12\n\t"
		"fmov	dr4,dr14\n\t"
		"frchg\n"
		"fschg\n"
		:
		:
		:);
}

// ****************************************************************
// void matrix_set_identity2(void)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	0.0f      | 0.0f      | 0.0f      | 1.0f
// 	1.0f      | 0.0f      | 0.0f      | 0.0f
// 	0.0f      | 1.0f      | 0.0f      | 0.0f
// 	0.0f      | 0.0f      | 1.0f      | 0.0f
// ****************************************************************
void matrix_set_identity2(void)
{
	asm volatile(
		"fschg\n\t"
		"frchg\n\t"
		"fldi0	fr0\n\t"
		"fldi1	fr1\n\t"
		"fldi0	fr2\n\t"
		"fldi0	fr3\n\t"
		"fldi1	fr6\n\t"
		"fldi0	fr7\n\t"
		"fmov	dr0,dr10\n\t"
		"fmov	dr2,dr4\n\t"
		"fmov	dr2,dr8\n\t"
		"fmov	dr2,dr14\n\t"
		"fmov	dr6,dr12\n\t"
		"frchg\n"
		"fschg\n"
		:
		:
		:);
}

// ****************************************************************
// void matrix_transpose_rotate(void)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	fr0       | fr1       | fr2       | fr12
// 	fr4       | fr5       | fr6       | fr13
// 	fr8       | fr9       | fr10      | fr14
// 	fr3       | fr7       | fr11      | fr15
// ****************************************************************
void matrix_transpose_rotate(void)
{
	asm volatile(
		"frchg\n\t"
		"flds	fr1, fpul\n\t"
		"fmov	fr4, fr1\n\t"
		"fsts	fpul, fr4\n\t"
		"flds	fr2, fpul\n\t"
		"fmov	fr8, fr2\n\t"
		"fsts	fpul, fr2\n\t"
		"flds	fr6, fpul\n\t"
		"fmov	fr9, fr6\n\t"
		"fsts	fpul, fr9\n\t"
		"frchg\n"
		:
		:
		: "fpul");
}

// ****************************************************************
// void matrix_transpose_inverce(void)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	fr0       | fr1       | fr2       | -fv12･fv0
// 	fr4       | fr5       | fr6       | -fv12･fv4
// 	fr8       | fr9       | fr10      | -fv12･fv8
// 	fr3       | fr7       | fr11      | fr15
// ****************************************************************
void matrix_transpose_inverce(void)
{
	asm volatile(
		"frchg\n\t"
		"fneg	fr12\n\t"
		"fneg	fr13\n\t"
		"fneg	fr14\n\t"
		"fldi0	fr15\n\t"
		"fldi0	fr3\n\t"
		"fipr	fv12, fv0\n\t"
		"fldi0	fr7\n\t"
		"fipr	fv12, fv4\n\t"
		"fldi0	fr11\n\t"
		"fipr	fv12, fv8\n\t"

		"fmov	fr3, fr12\n\t"
		"fmov	fr7, fr13\n\t"
		"fmov	fr11, fr14\n\t"
		"fmov	fr1, fr15\n\t"
		"fmov	fr4, fr1\n\t"
		"fmov	fr15, fr4\n\t"
		"fmov	fr2, fr15\n\t"
		"fmov	fr8, fr2\n\t"
		"fmov	fr15, fr2\n\t"
		"fmov	fr6, fr15\n\t"
		"fmov	fr9, fr6\n\t"
		"fmov	fr15, fr9\n\t"

		"fldi0	fr3\n\t"
		"fldi0	fr7\n\t"
		"fldi0	fr11\n\t"
		"fldi1	fr15\n\t"
		"frchg\n"
		:
		:
		:);
}

// ****************************************************************
// void matrix_apply_translate(float x, float y, float z)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	1.0f      | 0.0f      | 0.0f      | x
// 	0.0f      | 1.0f      | 0.0f      | y
// 	0.0f      | 0.0f      | 1.0f      | z
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_translate(float x, float y, float z)
{
	asm volatile(
		"fldi1	fr7\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fschg\n\t"
		"fmov	dr4, xd12\n\t"
		"fmov	dr6, xd14\n\t"
		"fschg\n"
		:
		: [tx] "f"(x), [ty] "f"(y), [tz] "f"(z)
		: "fr7");
}

// ****************************************************************
// void matrix_apply_scale(float x, float y, float z)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	x         | 0.0f      | 0.0f      | 0.0f
// 	0.0f      | y         | 0.0f      | 0.0f
// 	0.0f      | 0.0f      | z         | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_scale(float x, float y, float z)
{
	asm volatile(
		"fschg\n\t"
		"fmov	xd0, dr8\n\t"
		"fmul	fr4, fr8\n\t"
		"fmul	fr4, fr9\n\t"
		"fmov	xd2, dr10\n\t"
		"fmul	fr4, fr10\n\t"
		"fmul	fr4, fr11\n\t"
		"fmov	dr8, xd0\n\t"
		"fmov	dr10, xd2\n\t"
		"fmov	xd4, dr8\n\t"
		"fmul	fr5, fr8\n\t"
		"fmul	fr5, fr9\n\t"
		"fmov	xd6, dr10\n\t"
		"fmul	fr5, fr10\n\t"
		"fmul	fr5, fr11\n\t"
		"fmov	dr8, xd4\n\t"
		"fmov	dr10, xd6\n\t"
		"fmov	xd8, dr8\n\t"
		"fmul	fr6, fr8\n\t"
		"fmul	fr6, fr9\n\t"
		"fmov	xd10, dr10\n\t"
		"fmul	fr6, fr10\n\t"
		"fmul	fr6, fr11\n\t"
		"fmov	dr8, xd8\n\t"
		"fmov	dr10, xd10\n\t"
		"fschg\n"
		:
		: [sx] "f"(x), [sy] "f"(y), [sz] "f"(z)
		: "fr8", "fr9", "fr10", "fr11");
}

// ****************************************************************
// void matrix_apply_rotate_x(float a)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	1.0f      | 0.0f      | 0.0f      | 0.0f
// 	0.0f      | cos(a)    | -sin(a)   | 0.0f
// 	0.0f      | sin(a)    | cos(a)    | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_rotate_x(float angle)
{
	angle *= 10430.37835f;

	asm volatile(
		"ftrc	%[a], fpul\n\t"
		".short	0xf4fd\n\t" /* fsca dr4 */
		"fldi0	fr8\n\t"
		"fldi0	fr11\n\t"
		"fmov	fr5, fr10\n\t"
		"fmov	fr4, fr9\n\t"
		"fneg	fr9\n\t"
		"ftrv	xmtrx, fv8\n\t"
		"fmov	fr4, fr6\n\t"
		"fldi0	fr7\n\t"
		"fldi0	fr4\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fschg\n\t"
		"fmov	dr8, xd8\n\t"
		"fmov	dr10, xd10\n\t"
		"fmov	dr4, xd4\n\t"
		"fmov	dr6, xd6\n\t"
		"fschg\n"
		:
		: [a] "f"(angle)
		: "fpul", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");
}

// ****************************************************************
// void matrix_apply_rotate_y(float a)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	cos(a)    | 0.0f      | sin(a)    | 0.0f
// 	0.0f      | 1.0f      | 0.0f      | 0.0f
// 	-sin(a)   | 0.0f      | cos(a)    | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_rotate_y(float angle)
{
	angle *= 10430.37835f;

	asm volatile(
		"ftrc	%[a], fpul\n\t"
		".short	0xf6fd\n\t" /* fsca dr6 */
		"fldi0	fr9\n\t"
		"fldi0	fr11\n\t"
		"fmov	fr6, fr8\n\t"
		"fmov	fr7, fr10\n\t"
		"ftrv	xmtrx, fv8\n\t"
		"fmov	fr7, fr4\n\t"
		"fldi0	fr5\n\t"
		"fneg	fr6\n\t"
		"fldi0	fr7\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fschg\n\t"
		"fmov	dr8, xd8\n\t"
		"fmov	dr10, xd10\n\t"
		"fmov	dr4, xd0\n\t"
		"fmov	dr6, xd2\n\t"
		"fschg\n"
		:
		: [a] "f"(angle)
		: "fpul", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");
}

// ****************************************************************
// void matrix_apply_rotate_z(float angle)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	cos(a)    | -sin(a)   | 0.0f      | 0.0f
// 	sin(a)    | cos(a)    | 0.0f      | 0.0f
// 	0.0f      | 0.0f      | 1.0f      | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_rotate_z(float angle)
{
	angle *= 10430.37835f;

	asm volatile(
		"ftrc	%[a], fpul\n\t"
		".short	0xf8fd\n\t" /* fsca dr8 */
		"fldi0	fr10\n\t"
		"fldi0	fr11\n\t"
		"fmov	fr8, fr5\n\t"
		"fneg	fr8\n\t"
		"ftrv	xmtrx, fv8\n\t"
		"fmov	fr9, fr4\n\t"
		"fschg\n\t"
		"fmov	dr10, dr6\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fmov	dr8, xd4\n\t"
		"fmov	dr10, xd6\n\t"
		"fmov	dr4, xd0\n\t"
		"fmov	dr6, xd2\n\t"
		"fschg\n"
		:
		: [a] "f"(angle)
		: "fpul", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");
}

// ****************************************************************
// matrix_apply_screen(float w, float h)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	w*0.5f    | 0.0f      | 0.0f      | w*0.5f
// 	0.0f      | -h*0.5f   | 0.0f      | h*0.5f
// 	0.0f      | 0.0f      | 1.0f      | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_screen(float width, float height)
{
	width *= 0.5f;
	height *= 0.5f;

	asm volatile(
		"fldi0	fr6\n\t"
		"fldi1	fr7\n\t"
		"fmov	fr4, fr8\n\t"
		"fmov	fr5, fr9\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fschg\n\t"
		"fneg	fr9\n\t"
		"fmov	xd0, dr10\n\t"
		"fmul	fr8, fr10\n\t"
		"fmul	fr8, fr11\n\t"
		"fmov	dr4, xd12\n\t"
		"fmov	dr6, xd14\n\t"
		"fmov	xd2, dr4\n\t"
		"fmul	fr8, fr4\n\t"
		"fmul	fr8, fr5\n\t"
		"fmov	dr10, xd0\n\t"
		"fmov	xd4, dr6\n\t"
		"fmul	fr9, fr6\n\t"
		"fmul	fr9, fr7\n\t"
		"fmov	dr4, xd2\n\t"
		"fmov	xd6, dr10\n\t"
		"fmul	fr9, fr10\n\t"
		"fmul	fr9, fr11\n\t"
		"fmov	dr6, xd4\n\t"
		"fmov	dr10, xd6\n\t"
		"fschg\n"
		:
		: [w] "f"(width), [h] "f"(height)
		: "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");
}

// ****************************************************************
// void matrix_apply_projection(float f, float a, float nz)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
// cot(f)/a/nz| 0.0f      | 0.0f      | 0.0f
// 	0.0f      | cot(f)/nz | 0.0f      | 0.0f
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// 	0.0f      | 0.0f      | 1.0f/nz   | 0.0f
// ****************************************************************
void matrix_apply_projection(float fov, float aspect, float near_z)
{
	fov *= 10430.37835f * 0.5f;

	asm volatile(
		"fldi1	fr7\n\t"
		"fdiv	%[a], fr7\n\t"
		"fschg\n\t"
		"fmul	%[n], %[n]\n\t"
		".short	0xf67d\n\t" /* fsrra fr6 */
		"ftrc	%[f], fpul\n\t"
		".short	0xf4fd\n\t" /* fsca	dr4 */
		"fdiv	fr4, fr5\n\t"

		"fmov	xd12, dr8\n\t"
		"fmul	%[n], fr8\n\t"
		"fmov	xd14, dr10\n\t"
		"fmul	%[n], fr9\n\t"
		"fmul	%[n], fr10\n\t"
		"fmul	%[n], fr11\n\t"

		"fmov	xd8, xd12\n\t"
		"fmov	xd10, xd14\n\t"
		"fmov	dr8, xd8\n\t"
		"fmov	dr10, xd10\n\t"

		"fmul	%[n], fr5\n\t"
		"fmov	xd4, dr8\n\t"
		"fmul	fr5, fr8\n\t"
		"fmul	fr5, fr9\n\t"
		"fmov	xd6, dr10\n\t"
		"fmul	fr5, fr10\n\t"
		"fmul	fr5, fr11\n\t"
		"fmov	dr8, xd4\n\t"
		"fmov	dr10, xd6\n\t"

		"fmov	xd0, dr8\n\t"
		"fmul	fr7, fr5\n\t"
		"fmov	xd2, dr10\n\t"
		"fmul	fr5, fr8\n\t"
		"fmul	fr5, fr9\n\t"
		"fmul	fr5, fr10\n\t"
		"fmul 	fr5, fr11\n\t"
		"fmov	dr8, xd0\n\t"
		"fmov	dr10, xd2\n\t"
		"fschg\n"
		:
		: [f] "f"(fov), [a] "f"(aspect), [n] "f"(near_z)
		: "fpul", "fr7", "fr8", "fr9", "fr10", "fr11");
}

// ****************************************************************
// void matrix_apply_lookat(float *P, float *T, float *U)
// ****************************************************************
//  fv[n + 0] | fv[n + 4] | fv[n + 8] | fv[n + 12]
// -----------+-----------+-----------+-----------
//	Xx=(YxU)  | Xy=(YxU)  | Xz=(YxU)  | -P･X
// 	Yx=(XxZ)  | Yy=(XxZ)  | Yz=(XxZ)  | -P･Y
// 	Zx=(P-T)  | Zy=(P-T)  | Zz=(P-T)  | -P･Z
// 	0.0f      | 0.0f      | 0.0f      | 1.0f
// ****************************************************************
void matrix_apply_lookat(float *position_3f, float *target_3f, float *up_3f)
{
	asm volatile(
		"fmov.s @%[t]+, fr8\n\t"
		"fmov.s @%[t]+, fr9\n\t"
		"fmov.s	@%[t]+, fr10\n\t"

		"fmov.s @%[p]+, fr12\n\t"
		"fmov.s @%[p]+, fr13\n\t"
		"fmov.s @%[p]+, fr14\n\t"
		"fldi0	fr15\n\t"

		/* z = position - target */
		"fsub	fr12, fr8\n\t"
		"fsub	fr13, fr9\n\t"
		"fsub	fr14, fr10\n\t"
		"fldi0	fr11\n\t"
		"fipr	fv8, fv8\n\t"

		"fmov.s @%[u]+, fr4\n\t"
		"fmov.s @%[u]+, fr5\n\t"
		"fmov.s @%[u]+, fr6\n\t"

		".short	0xfb7d\n\t" /* fsrra fr11 */
		"fmul	fr11, fr8\n\t"
		"fmul	fr11, fr9\n\t"
		"fmul	fr11, fr10\n\t"
		"fldi0	fr11\n\t"
		"fipr	fv12, fv8\n\t"

		/* x = cross(z, up) */
		"fmov	fr10, fr15\n\t"
		"fmul	fr5, fr15\n\t"
		"fmov	fr9, fr0\n\t"
		"fmul	fr6, fr0\n\t"
		"fmov	fr8, fr3\n\t"
		"fmul	fr6, fr3\n\t"
		"fsub	fr15, fr0\n\t"
		"fmov	fr10, fr1\n\t"
		"fmul	fr4, fr1\n\t"
		"fmov	fr8, fr2\n\t"
		"fmul	fr5, fr2\n\t"
		"fmov	fr9, fr15\n\t"
		"fmul	fr4, fr15\n\t"
		"fsub	fr3, fr1\n\t"
		"fsub	fr15, fr2\n\t"
		"fldi0	fr3\n\t"
		"fldi0	fr15\n\t"
		"fipr	fv0, fv0\n\t"

		".short	0xf37d\n\t" /* fsrra fr3 */
		"fmul	fr3, fr0\n\t"
		"fmul	fr3, fr1\n\t"
		"fmul	fr3, fr2\n\t"
		"fldi0	fr3\n\t"
		"fipr	fv12, fv0\n\t"

		/* y = cross(x, z) */
		"fmov	fr2, fr15\n\t"
		"fmul	fr9, fr15\n\t"
		"fmov	fr1, fr4\n\t"
		"fmul	fr10, fr4\n\t"
		"fmov	fr0, fr7\n\t"
		"fmul	fr10, fr7\n\t"
		"fsub	fr15, fr4\n\t"
		"fmov	fr2, fr5\n\t"
		"fmul	fr8, fr5\n\t"
		"fmov	fr0, fr6\n\t"
		"fmul	fr9, fr6\n\t"
		"fmov	fr1, fr15\n\t"
		"fmul	fr8, fr15\n\t"
		"fsub	fr7, fr5\n\t"
		"fsub	fr15, fr6\n\t"

		"fldi0	fr7\n\t"
		"fldi0	fr15\n\t"
		"fipr	fv12, fv4\n\t"

		"fneg	fr3\n\t"
		"fneg	fr11\n\t"
		"fneg	fr7\n\t"
		"fmov	fr3, fr12\n\t"
		"fmov	fr7, fr13\n\t"
		"fmov	fr11, fr14\n\t"
		"fldi1	fr15\n\t"
		"ftrv	xmtrx, fv12\n\t"

		"fmov	fr1, fr7\n\t"
		"fmov	fr2, fr11\n\t"
		"fmov	fr4, fr1\n\t"
		"fmov	fr8, fr2\n\t"
		"fldi0	fr3\n\t"
		"ftrv	xmtrx, fv0\n\t"

		"fmov	fr7, fr4\n\t"
		"fmov	fr6, fr7\n\t"
		"fmov	fr9, fr6\n\t"
		"fmov	fr7, fr9\n\t"
		"fldi0	fr7\n\t"
		"ftrv	xmtrx, fv4\n\t"

		"fmov	fr11, fr8\n\t"
		"fldi0	fr11\n\t"
		"ftrv	xmtrx, fv8\n\t"

		"frchg\n"
		:
		: [p] "r"(position_3f), [t] "r"(target_3f), [u] "r"(up_3f)
		: "fr0", "fr1", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11", "fr12", "fr13", "fr14", "fr15");
}

void vector4_get_transform2(float *dest, float *src, int dest_skip, int num)
{
	dest_skip = (dest_skip & -8) + 32;

	asm volatile(
		"pref	@%[d]\n\t"
		"pref	@%[s]\n\t"
		"shlr	%[n]\n\t"
		"add	#16, %[d]\n\t"
		"bf/s	1f\n\t"
		"fschg\n\t"
		"fmov.d	@%[s]+, dr8\n\t"
		"fmov.d	@%[s]+, dr10\n\t"
		"ftrv	xmtrx, fv8\n\t"
		"pref	@%[s]\n\t"
		"fldi1	fr3\n\t"
		"fdiv	fr8, fr3\n\t"
		"cmp/pl	%[n]\n\t"
		"fmul	fr3, fr10\n\t"
		"fmul	fr3, fr11\n\t"
		"fmul	fr3, fr9\n\t"
		"fmov.d	dr10, @-%[d]\n\t"
		"fmov.d	dr8, @-%[d]\n\t"
		"add	%[skip], %[d]\n\t"
		"bf/s	2f\n\t"
		"pref	@%[d]\n\t"
		"1:\n\t"
		"fmov.d	@%[s]+, dr4\n\t"
		"fmov.d	@%[s]+, dr6\n\t"
		"pref	@%[s]\n\t"
		"ftrv	xmtrx, fv4\n\t"
		"fmov.d	@%[s]+, dr8\n\t"
		"fmov.d	@%[s]+, dr10\n\t"
		"fldi1	fr2\n\t"
		"fdiv	fr4, fr2\n\t"
		"pref	@%[s]\n\t"
		"ftrv	xmtrx, fv8\n\t"
		"fmul	fr2, fr6\n\t"
		"fmul	fr2, fr7\n\t"
		"fmul	fr2, fr5\n\t"
		"fldi1	fr3\n\t"
		"fdiv	fr8, fr3\n\t"
		"fmov.d	dr6, @-%[d]\n\t"
		"fmov.d	dr4, @-%[d]\n\t"
		"add	%[skip], %[d]\n\t"
		"pref	@%[d]\n\t"
		"fmul	fr3, fr10\n\t"
		"fmul	fr3, fr11\n\t"
		"fmul	fr3, fr9\n\t"
		"dt		%[n]\n\t"
		"fmov.d	dr10, @-%[d]\n\t"
		"fmov.d	dr8, @-%[d]\n\t"
		"add	%[skip], %[d]\n\t"
		"bf/s	1b\n\t"
		"pref	@%[d]\n\t"
		"2:\n\t"
		"fschg\n"
		: [d] "+&r"(dest), [s] "+&r"(src)
		: [skip] "r"(dest_skip), [n] "r"(num)
		: "t", "fr2", "fr3", "fr4", "fr5", "fr6", "fr7", "fr8", "fr9", "fr10", "fr11");
}
