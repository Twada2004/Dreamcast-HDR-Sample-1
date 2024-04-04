#ifndef _MATRIX_H_INCLUDED_
#define _MATRIX_H_INCLUDED_

extern void matrix_get(float *dest);
extern void matrix_set(float *src);
extern void matrix_apply(float *src);
extern void matrix_set_identity(void);
extern void matrix_set_identity2(void);
extern void matrix_transpose_rotate(void);
extern void matrix_transpose_inverce(void);
extern void matrix_apply_translate(float x, float y, float z);
extern void matrix_apply_scale(float x, float y, float z);
extern void matrix_apply_rotate_x(float angle);
extern void matrix_apply_rotate_y(float angle);
extern void matrix_apply_rotate_z(float angle);
extern void matrix_apply_screen(float width, float height);
extern void matrix_apply_projection(float fov, float aspect, float near_z);
extern void matrix_apply_lookat(float *position_3f, float *target_3f, float *up_3f);

extern void vector4_get_transform2(float *dest, float *src, int dest_skip, int num);

#endif
