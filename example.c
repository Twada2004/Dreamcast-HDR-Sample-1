#include <kos.h>
#include "render.h"
#include "matrix.h"
#include "vertex32.h"
/* Mesh data */
#include "mesh_data/stage.h"
#include "mesh_data/wall.h"
#include "mesh_data/sky.h"

/* Plane */
pvr_sprite_hdr_t plane_head = {0xa000000b, 0xd0000000, 0x008020c0, 0x04000000, 0xffffffff, 0, 0, 0};
pvr_sprite_txr_t plane = {0xf0000000, 0.0f, 256.0f, 200.0f, 0.0f, 0.0f, 200.0f, 512.0f, 0.0f, 200.0f, 512.0f, 256.0f, 0, 0x00003f80, 0, 0x3f800000};
/* Luminance extraction */
pvr_sprite_hdr_t inv_head = {0xa0000000, 0xd0000000, 0x009020c0, 0x00000000, 0xffffffff, 0, 0, 0};
pvr_sprite_hdr_t add_head = {0xa0000000, 0xd0000000, 0x009020c0, 0x00000000, 0x80ffffff, 0, 0, 0};
pvr_sprite_hdr_t pow_head = {0xa0000000, 0xd0000000, 0x008920c0, 0x00000000, 0xffffffff, 0, 0, 0};
pvr_sprite_col_t effect_plane[] = {
    {0xf0000000, 0.0f, 96.0f, 100.0f, 0.5f, 0.0f, 100.0f, 128.0f, 0.0f, 100.0f, 128.0f, 96.0f, 0, 0, 0, 0},
    {0xf0000000, 0.0f, 96.0f, 100.1f, 0.5f, 0.0f, 100.1f, 128.0f, 0.0f, 100.1f, 128.0f, 96.0f, 0, 0, 0, 0},
    {0xf0000000, 0.0f, 96.0f, 100.2f, 0.5f, 0.0f, 100.2f, 128.0f, 0.0f, 100.2f, 128.0f, 96.0f, 0, 0, 0, 0},
    {0xf0000000, 0.0f, 96.0f, 100.3f, 0.5f, 0.0f, 100.3f, 128.0f, 0.0f, 100.3f, 128.0f, 96.0f, 0, 0, 0, 0},
};
/* Gaussian blur */
pvr_poly_hdr_t blur0_head = {0x808c000e, 0xd2000000, 0x0091bfc0, 0x04000000, 0, 0, 0, 0};
pvr_poly_hdr_t blur1_head = {0x808c000e, 0xd2000000, 0x0091bfc0, 0x04000000, 0, 0, 0, 0};
pvr_vertex_t blur0_plane[] = {
    {0xe0000000, 160.5f, 96.5f, 10.0f, 0.00000f + (0.0f / 512.0f), 0.7500f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 160.5f, 00.5f, 10.0f, 0.00000f + (0.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 288.5f, 96.5f, 10.0f, 0.25000f + (0.0f / 512.0f), 0.7500f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 288.5f, 00.5f, 10.0f, 0.25000f + (0.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 48.5f, 10.0f, 0.31250f + (-0.5f / 512.0f), 0.7500f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 00.5f, 10.0f, 0.31250f + (-0.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 48.5f, 10.0f, 0.56250f + (-0.5f / 512.0f), 0.7500f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 00.5f, 10.0f, 0.56250f + (-0.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 88.5f, 10.0f, 0.59375f + (-0.5f / 512.0f), 0.3750f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 64.5f, 10.0f, 0.59375f + (-0.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 88.5f, 10.0f, 0.71875f + (-0.5f / 512.0f), 0.3750f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 64.5f, 10.0f, 0.71875f + (-0.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 384.5f, 96.5f, 10.0f, 0.31250f + (0.0f / 512.0f), 0.7500f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 00.5f, 10.0f, 0.31250f + (0.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 512.5f, 96.5f, 10.0f, 0.56250f + (0.0f / 512.0f), 0.7500f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 512.5f, 00.5f, 10.0f, 0.56250f + (0.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
};
pvr_vertex_t blur1_plane[] = {
    {0xe0000000, 160.5f, 96.5f, 10.1f, 0.00000f + (1.0f / 512.0f), 0.7500f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 160.5f, 00.5f, 10.1f, 0.00000f + (1.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 288.5f, 96.5f, 10.1f, 0.25000f + (1.0f / 512.0f), 0.7500f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 288.5f, 00.5f, 10.1f, 0.25000f + (1.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 160.5f, 96.5f, 10.2f, 0.00000f + (0.0f / 512.0f), 0.7500f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 160.5f, 00.5f, 10.2f, 0.00000f + (0.0f / 512.0f), 0.0000f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 288.5f, 96.5f, 10.2f, 0.25000f + (0.0f / 512.0f), 0.7500f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 288.5f, 00.5f, 10.2f, 0.25000f + (0.0f / 512.0f), 0.0000f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 160.5f, 96.5f, 10.3f, 0.00000f + (1.0f / 512.0f), 0.7500f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 160.5f, 00.5f, 10.3f, 0.00000f + (1.0f / 512.0f), 0.0000f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 288.5f, 96.5f, 10.3f, 0.25000f + (1.0f / 512.0f), 0.7500f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 288.5f, 00.5f, 10.3f, 0.25000f + (1.0f / 512.0f), 0.0000f + (1.0f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 48.5f, 10.1f, 0.31250f + (1.5f / 512.0f), 0.7500f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 00.5f, 10.1f, 0.31250f + (1.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 48.5f, 10.1f, 0.56250f + (1.5f / 512.0f), 0.7500f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 00.5f, 10.1f, 0.56250f + (1.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 48.5f, 10.2f, 0.31250f + (-0.5f / 512.0f), 0.7500f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 00.5f, 10.2f, 0.31250f + (-0.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 48.5f, 10.2f, 0.56250f + (-0.5f / 512.0f), 0.7500f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 00.5f, 10.2f, 0.56250f + (-0.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 48.5f, 10.3f, 0.31250f + (1.5f / 512.0f), 0.7500f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 304.5f, 00.5f, 10.3f, 0.31250f + (1.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 48.5f, 10.3f, 0.56250f + (1.5f / 512.0f), 0.7500f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 00.5f, 10.3f, 0.56250f + (1.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 88.5f, 10.1f, 0.59375f + (1.5f / 512.0f), 0.3750f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 64.5f, 10.1f, 0.59375f + (1.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 88.5f, 10.1f, 0.71875f + (1.5f / 512.0f), 0.3750f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 64.5f, 10.1f, 0.71875f + (1.5f / 512.0f), 0.0000f + (-0.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 88.5f, 10.2f, 0.59375f + (-0.5f / 512.0f), 0.3750f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 64.5f, 10.2f, 0.59375f + (-0.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 88.5f, 10.2f, 0.71875f + (-0.5f / 512.0f), 0.3750f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 64.5f, 10.2f, 0.71875f + (-0.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 88.5f, 10.3f, 0.59375f + (1.5f / 512.0f), 0.3750f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 336.5f, 64.5f, 10.3f, 0.59375f + (1.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 368.5f, 88.5f, 10.3f, 0.71875f + (1.5f / 512.0f), 0.3750f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xf0000000, 368.5f, 64.5f, 10.3f, 0.71875f + (1.5f / 512.0f), 0.0000f + (1.5f / 128.0f), 0x40ffffff, 0x00000000},
    {0xe0000000, 384.5f, 96.5f, 10.1f, 0.59375f + (0.0f / 512.0f), 0.3750f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 00.5f, 10.1f, 0.59375f + (0.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 512.5f, 96.5f, 10.1f, 0.71875f + (0.0f / 512.0f), 0.3750f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 512.5f, 00.5f, 10.1f, 0.71875f + (0.0f / 512.0f), 0.0000f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 96.5f, 10.2f, 0.65625f + (0.0f / 512.0f), 0.6875f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 00.5f, 10.2f, 0.65625f + (0.0f / 512.0f), 0.5000f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 512.5f, 96.5f, 10.2f, 0.71875f + (0.0f / 512.0f), 0.6875f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 512.5f, 00.5f, 10.2f, 0.71875f + (0.0f / 512.0f), 0.5000f + (0.0f / 128.0f), 0xffffffff, 0x00000000},
};
/* Twiddle */
pvr_poly_hdr_t twid0_head = {0x808c000e, 0xd2000000, 0x00900fc0, 0x04000000, 0, 0, 0, 0};
pvr_poly_hdr_t twid1_head = {0x808c000e, 0xd2000000, 0x00900fc0, 0x00000000, 0, 0, 0, 0};
pvr_vertex_t twid0_plane[] = {
    {0xe0000000, 000.5f, 160.5f, 10.0f, 0.75f + (0.5f / 512.0f), 1.0f + (0.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 000.5f, 128.5f, 10.0f, 0.75f + (0.5f / 512.0f), 0.0f + (0.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 128.5f, 160.5f, 10.0f, 1.00f + (0.5f / 512.0f), 1.0f + (0.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 128.5f, 128.5f, 10.0f, 1.00f + (0.5f / 512.0f), 0.0f + (0.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 128.5f, 160.5f, 10.0f, 0.75f + (0.5f / 512.0f), 1.0f + (1.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 128.5f, 128.5f, 10.0f, 0.75f + (0.5f / 512.0f), 0.0f + (1.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 256.5f, 160.5f, 10.0f, 1.00f + (0.5f / 512.0f), 1.0f + (1.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 256.5f, 128.5f, 10.0f, 1.00f + (0.5f / 512.0f), 0.0f + (1.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 256.5f, 160.5f, 10.0f, 0.75f + (0.5f / 512.0f), 1.0f + (2.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 256.5f, 128.5f, 10.0f, 0.75f + (0.5f / 512.0f), 0.0f + (2.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 160.5f, 10.0f, 1.00f + (0.5f / 512.0f), 1.0f + (2.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 384.5f, 128.5f, 10.0f, 1.00f + (0.5f / 512.0f), 0.0f + (2.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 160.5f, 10.0f, 0.75f + (0.5f / 512.0f), 1.0f + (3.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 128.5f, 10.0f, 0.75f + (0.5f / 512.0f), 0.0f + (3.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 512.5f, 160.5f, 10.0f, 1.00f + (0.5f / 512.0f), 1.0f + (3.5f / 128.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 512.5f, 128.5f, 10.0f, 1.00f + (0.5f / 512.0f), 0.0f + (3.5f / 128.0f), 0xffffffff, 0x00000000},
};
pvr_vertex_t twid1_plane[] = {
    {0xe0000000, 000.5f, 256.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.75f + (0.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 000.5f, 160.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.00f + (0.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 128.5f, 256.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.75f + (0.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 128.5f, 160.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.00f + (0.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 128.5f, 256.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.75f + (1.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 128.5f, 160.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.00f + (1.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 256.5f, 256.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.75f + (1.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 256.5f, 160.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.00f + (1.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 256.5f, 256.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.75f + (2.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 256.5f, 160.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.00f + (2.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 256.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.75f + (2.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 384.5f, 160.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.00f + (2.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 256.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.75f + (3.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 384.5f, 160.5f, 10.0f, 0.0f + (0.5f / 128.0f), 0.00f + (3.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xe0000000, 512.5f, 256.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.75f + (3.5f / 512.0f), 0xffffffff, 0x00000000},
    {0xf0000000, 512.5f, 160.5f, 10.0f, 1.0f + (0.5f / 128.0f), 0.00f + (3.5f / 512.0f), 0xffffffff, 0x00000000},
};
/* HDR effects */
pvr_sprite_hdr_t tone_head = {0xa0000000, 0xd0000000, 0x008020c0, 0x00000000, 0xffffffff, 0, 0, 0};
pvr_sprite_col_t tone_plane = {0xf0000000, 0.0f, 480.0f, 100.0f, 0.0f, 0.0f, 100.0f, 640.0f, 0.5f, 100.0f, 640.0f, 480.0f, 0, 0, 0, 0};
pvr_sprite_hdr_t bloom_head = {0xa000000e, 0xd2000000, 0x0091bfc0, 0x00000000, 0xffffffff, 0, 0, 0};
pvr_sprite_txr_t bloom_plane = {0xf0000000, 0.0f, 480.0f, 100.0f, 0.0f, 0.0f, 100.0f, 640.0f, 0.5f, 100.0f, 640.0f, 480.0f, 0, 0x00003f40, 0, 0x3f800000};

/* Stage */
pvr_poly_hdr_t stage_head = {0x808e000a, 0xd2000000, 0x009024c0, 0x00000000, 0, 0, 0, 0};
pvr_poly_hdr_t wall_head = {0x808e0002, 0xca000000, 0x009024c0, 0x00000000, 0, 0, 0, 0};
pvr_poly_hdr_t sky_head = {0x808e000a, 0xd2000000, 0x009024c0, 0x00000000, 0, 0, 0, 0};
pvr_vertex_t *stage01_workbuf;
pvr_vertex_t *stage02_workbuf;
pvr_vertex_t *wall_workbuf;
pvr_vertex_t *sky_workbuf;
pvr_vertex_t vert_workbuffer[1024 * 128] __attribute__((aligned(32)));

int tex_load(char *filename, unsigned int *dest, int *tex_w, int *tex_h)
{
    struct
    {
        char id[4]; // 'DTEX'
        short width;
        short height;
        int type;
        int size;
    } header;
    int file = 0;

    file = fs_open(filename, 0);
    if (file == -1)
        return 0;

    /* load header */
    fs_read(file, &header, 16);
    *tex_w = (int)header.width;
    *tex_h = (int)header.height;

    fs_read(file, (void *)dest, header.size);
    fs_close(file);

    return header.size;
}

static pvr_vertex_t *workbuf_init(pvr_vertex_t *workbuf, float *uv, unsigned int *vcolor, int vert_num)
{
    for (int i = 0; i < vert_num; i++)
    {
        workbuf[i].u = uv[i * 2 + 0];
        workbuf[i].v = uv[i * 2 + 1];
        workbuf[i].argb = vcolor[i * 2 + 0];
        workbuf[i].oargb = vcolor[i * 2 + 1];
    }
    return &workbuf[vert_num];
}

void draw_init(void)
{
    unsigned int *addr = (unsigned int *)0xa4000000 + (512 * 256 / 2);
    int size = 0;
    int w, h;

    /* Plane */
    plane_head.cmd |= (0 << 24);                               // Type
    plane_head.mode2 |= (1 << 29) | (0 << 26);                 // (SRC | DST)
    plane_head.mode2 |= (6 << 3) | 5;                          // U size | V size
    plane_head.mode3 |= (0 << 31) | (1 << 27);                 // Mipmap | Pixel type
    plane_head.mode3 |= ((((unsigned int)0) & 0x7fffff) >> 3); // Texture address
    /* Luminance extraction */
    inv_head.cmd |= (2 << 24);               // Type
    inv_head.mode2 |= (3 << 29) | (0 << 26); // (SRC | DST)
    add_head.cmd |= (2 << 24);               // Type
    add_head.mode2 |= (4 << 29) | (1 << 26); // (SRC | DST)
    pow_head.cmd |= (2 << 24);               // Type
    pow_head.mode2 |= (2 << 29) | (1 << 26); // (SRC | DST)
    /* Gaussian blur */
    blur0_head.cmd |= (0 << 24);                               // Type
    blur0_head.mode2 |= (4 << 29) | (0 << 26);                 // (SRC | DST)
    blur0_head.mode2 |= (6 << 3) | 4;                          // U size | V size
    blur0_head.mode3 |= (0 << 31) | (1 << 27);                 // Mipmap | Pixel type
    blur0_head.mode3 |= ((((unsigned int)0) & 0x7fffff) >> 3); // Texture address
    blur1_head.cmd |= (2 << 24);                               // Type
    blur1_head.mode2 |= (4 << 29) | (1 << 26);                 // (SRC | DST)
    blur1_head.mode2 |= (6 << 3) | 4;                          // U size | V size
    blur1_head.mode3 |= (0 << 31) | (1 << 27);                 // Mipmap | Pixel type
    blur1_head.mode3 |= ((((unsigned int)0) & 0x7fffff) >> 3); // Texture address
    /* Twiddle */
    twid0_head.cmd |= (0 << 24);                                           // Type
    twid0_head.mode2 |= (1 << 29) | (0 << 26);                             // (SRC | DST)
    twid0_head.mode2 |= (6 << 3) | 4;                                      // U size | V size
    twid0_head.mode3 |= (0 << 31) | (1 << 27);                             // Mipmap | Pixel type
    twid0_head.mode3 |= ((((unsigned int)0) & 0x7fffff) >> 3);             // Texture address
    twid1_head.cmd |= (0 << 24);                                           // Type
    twid1_head.mode2 |= (1 << 29) | (0 << 26);                             // (SRC | DST)
    twid1_head.mode2 |= (4 << 3) | 6;                                      // U size | V size
    twid1_head.mode3 |= (0 << 31) | (1 << 27);                             // Mipmap | Pixel type
    twid1_head.mode3 |= ((((unsigned int)512 * 128 * 2) & 0x7fffff) >> 3); // Texture address
    /* HDR effects */
    tone_head.cmd |= (2 << 24);                                            // Type
    tone_head.mode2 |= (2 << 29) | (1 << 26);                              // (SRC | DST)
    bloom_head.cmd |= (2 << 24);                                           // Type
    bloom_head.mode2 |= (1 << 29) | (1 << 26);                             // (SRC | DST)
    bloom_head.mode2 |= (4 << 3) | 4;                                      // U size | V size
    bloom_head.mode3 |= (0 << 31) | (1 << 27);                             // Mipmap | Pixel type
    bloom_head.mode3 |= ((((unsigned int)512 * 224 * 2) & 0x7fffff) >> 3); // Texture address

    /* Stage */
    size = tex_load("/rd/stage.tex", addr, &w, &h);
    stage_head.cmd |= (0 << 24);                                  // Type
    stage_head.mode2 |= (1 << 29) | (0 << 26);                    // (SRC | DST)
    stage_head.mode2 |= (5 << 3) | 5;                             // U size | V size
    stage_head.mode3 |= (0 << 31) | (0 << 27);                    // Mipmap | Pixel type
    stage_head.mode3 |= ((((unsigned int)addr) & 0x7fffff) >> 3); // Texture address
    addr += size / 4;
    /* Wall */
    wall_head.cmd |= (2 << 24);               // Type
    wall_head.mode2 |= (4 << 29) | (1 << 26); // (SRC | DST)
    /* Sky */
    size = tex_load("/rd/sky2.tex", addr, &w, &h);
    sky_head.cmd |= (0 << 24);                                  // Type
    sky_head.mode2 |= (1 << 29) | (0 << 26);                    // (SRC | DST)
    sky_head.mode2 |= (6 << 3) | 5;                             // U size | V size
    sky_head.mode3 |= (0 << 31) | (1 << 27);                    // Mipmap | Pixel type
    sky_head.mode3 |= ((((unsigned int)addr) & 0x7fffff) >> 3); // Texture address
    addr += size / 4;

    /* Lighting */
    float light[3] = {0.0f, 1.0f, -1.0f};
    float color[3] = {1.0f, 0.75f, 0.3f};
    float ambient = 0.25f;
    for (int i = 0; i < stage01_vert_num; i++)
    {
        float *normal = &stage01_norm_list[i * 4];
        float tmp;
        //tmp = fipr(light[0], light[1], light[2], 0.0f, normal[0], normal[1], normal[2], normal[3]);
        tmp = light[0] * normal[0] + light[1] * normal[1] + light[2] * normal[2];
        if (tmp < 0.0f)
            tmp = 0.0f;
        tmp += ambient;
        if (tmp > 1.0f)
            tmp = 1.0f;
        tmp *= 255.0f * 0.5f;
        stage01_vcolor_list[i * 2 + 0] = 0xff000000 | ((int)(tmp * color[0]) << 16) | ((int)(tmp * color[1]) << 8) | (int)(tmp * color[2]);
    }
    /* Init workbuffer */
    stage01_workbuf = vert_workbuffer;
    stage02_workbuf = workbuf_init(stage01_workbuf, stage01_uv_list, stage01_vcolor_list, stage01_vert_num);
    wall_workbuf = workbuf_init(stage02_workbuf, stage02_uv_list, stage02_vcolor_list, stage02_vert_num);
    sky_workbuf = workbuf_init(wall_workbuf, wall_uv_list, wall_vcolor_list, wall_vert_num);
    workbuf_init(sky_workbuf, sky_uv_list, sky_vcolor_list, sky_vert_num);
}

void camera_get_view(matrix_t *view)
{
    static float cam_pos[3] = {0.0f, 0.0f, 0.0f};
    static float cam_tar[3] = {0.0f, 0.0f, 0.0f};
    static float cam_up[3] = {0.0f, 1.0f, 0.0f};
    static float cam_pos_next[3] = {0.0f, 10.0f, 7.5f};
    static float cam_tar_next[3] = {0.0f, 0.0f, 0.0f};
    static float cam_up_next[3] = {0.0f, 1.0f, 0.0f};
    static float inter = 0.05f;
    maple_device_t *dev = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if (dev)
    {
        static float dy = 0.0f;
        static float dis = 20.0f;
        static float high = 10.0f;
        cont_state_t *st = (cont_state_t *)maple_dev_status(dev);
        dy += (float)st->joyx * (0.1f / 127.0f);
        high -= (float)st->joyy * (0.25f / 127.0f);
        if (st->rtrig)
            dis -= (float)st->rtrig * 0.005f;
        if (st->ltrig)
            dis += (float)st->ltrig * 0.005f;
        cam_pos_next[0] = fsin(dy) * dis;
        cam_pos_next[2] = fcos(dy) * dis;
        cam_pos_next[1] = high;
    }
    cam_pos[0] += inter * (cam_pos_next[0] - cam_pos[0]);
    cam_pos[1] += inter * (cam_pos_next[1] - cam_pos[1]);
    cam_pos[2] += inter * (cam_pos_next[2] - cam_pos[2]);
    cam_tar[0] += inter * (cam_tar_next[0] - cam_tar[0]);
    cam_tar[1] += inter * (cam_tar_next[1] - cam_tar[1]);
    cam_tar[2] += inter * (cam_tar_next[2] - cam_tar[2]);
    cam_up[0] += inter * (cam_up_next[0] - cam_up[0]);
    cam_up[1] += inter * (cam_up_next[1] - cam_up[1]);
    cam_up[2] += inter * (cam_up_next[2] - cam_up[2]);
    matrix_set_identity();
    matrix_apply_lookat(cam_pos, cam_tar, cam_up);
    matrix_get((float *)view);
}

void draw_frame(int bloom_flag, float tone_level)
{
    unsigned int user_clip[8] = {0x20000000, 0, 0, 0, 0, 0, 3, 2};
    static matrix_t view __attribute__((aligned(32)));

    camera_get_view(&view);

    matrix_set_identity2();
    matrix_apply_screen(128.0f, 96.0f);
    matrix_apply_projection((3.14159265f / 3.0f), (640.0f / 480.0f), 0.125f);
    matrix_apply((float *)&view);
    vector4_get_transform2((float *)stage01_workbuf, (float *)stage01_vert_list, 16, stage01_vert_num);
    vector4_get_transform2((float *)stage02_workbuf, (float *)stage02_vert_list, 16, stage02_vert_num);
    vector4_get_transform2((float *)wall_workbuf, (float *)wall_vert_list, 16, wall_vert_num);
    vector4_get_transform2((float *)sky_workbuf, (float *)sky_vert_list, 16, sky_vert_num);
    render_ready();

    /**** Opaque polygon ****/

    pvr_prim(&user_clip, sizeof(user_clip));
    pvr_prim(&stage_head, sizeof(stage_head));
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)stage01_workbuf, stage01_face_list);
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)stage02_workbuf, stage02_face_list);
    pvr_prim(&sky_head, sizeof(sky_head));
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)sky_workbuf, sky_face_list);
    render_end_type(0);

    /**** Opaque modifier ****/

    render_end_type(1);

    /**** Translucent polygon ****/

    pvr_prim(&wall_head, sizeof(wall_head));
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)wall_workbuf, wall_face_list);
    render_end_type(2);

    /**** Punch-through ****/

    render_end_type(4);

    /**** HDR effect pass ****/
    render_nextpass();
    pvr_prim(&blur0_head, sizeof(blur0_head));
    pvr_prim(blur0_plane, sizeof(blur0_plane));
    pvr_prim(&twid0_head, sizeof(twid0_head));
    pvr_prim(twid0_plane, sizeof(twid0_plane));
    pvr_prim(&twid1_head, sizeof(twid1_head));
    pvr_prim(twid1_plane, sizeof(twid1_plane));
    render_end_type(0);
    pvr_prim(&inv_head, sizeof(inv_head));
    pvr_prim(&effect_plane[0], sizeof(effect_plane[0]));
    add_head.argb = PVR_PACK_COLOR(((1.0f - tone_level) * 0.5f + 0.5f), 1.0f, 1.0f, 1.0f);
    pvr_prim(&add_head, sizeof(add_head));
    pvr_prim(&effect_plane[1], sizeof(effect_plane[1]));
    pvr_prim(&inv_head, sizeof(inv_head));
    pvr_prim(&effect_plane[2], sizeof(effect_plane[2]));
    pvr_prim(&pow_head, sizeof(pow_head));
    pvr_prim(&effect_plane[3], sizeof(effect_plane[3]));
    pvr_prim(&blur1_head, sizeof(blur1_head));
    pvr_prim(blur1_plane, sizeof(blur1_plane));
    render_end_type(2);

    render_begin();

    matrix_set_identity2();
    matrix_apply_screen(640.0f, 480.0f);
    matrix_apply_projection((3.14159265f / 3.0f), (640.0f / 480.0f), 0.125f);
    mat_apply(&view);
    vector4_get_transform2((float *)stage01_workbuf, (float *)stage01_vert_list, 16, stage01_vert_num);
    vector4_get_transform2((float *)stage02_workbuf, (float *)stage02_vert_list, 16, stage02_vert_num);
    vector4_get_transform2((float *)wall_workbuf, (float *)wall_vert_list, 16, wall_vert_num);
    vector4_get_transform2((float *)sky_workbuf, (float *)sky_vert_list, 16, sky_vert_num);
    render_ready();

    /**** Opaque polygon ****/

    //pvr_prim(&plane_head, sizeof(plane_head));
    //pvr_prim(&plane, sizeof(plane));
    user_clip[6] = 19;
    user_clip[7] = 14;
    pvr_prim(&user_clip, sizeof(user_clip));
    pvr_prim(&stage_head, sizeof(stage_head));
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)stage01_workbuf, stage01_face_list);
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)stage02_workbuf, stage02_face_list);
    pvr_prim(&sky_head, sizeof(sky_head));
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)sky_workbuf, sky_face_list);
    render_end_type(0);

    /**** Opaque modifier ****/

    render_end_type(1);

    /**** Translucent polygon ****/

    pvr_prim(&wall_head, sizeof(wall_head));
    vertex32_set_zclip_polygon((void *)0x10000000, (vertex32_t *)wall_workbuf, wall_face_list);
    render_end_type(2);

    /**** Punch-through ****/

    render_end_type(4);

    /**** HDR effect pass ****/
    render_nextpass();
    if(bloom_flag)
    {
        tone_head.argb = PVR_PACK_COLOR(tone_level , tone_level, tone_level, tone_level);
        pvr_prim(&tone_head, sizeof(tone_head));
        pvr_prim(&tone_plane, sizeof(tone_plane));
        pvr_prim(&bloom_head, sizeof(bloom_head));
        pvr_prim(&bloom_plane, sizeof(bloom_plane));
    }
    render_end_type(2);

    render_begin();
}

/* romdisk */
extern uint8 romdisk_boot[];
KOS_INIT_ROMDISK(romdisk_boot);

int main(void)
{
    int tile_unique_index[] = {
        0, 4, 8, 12, 16, 20, 24, 28, 32, 36, 40, 44, 48, 52, 56, 60,
        1, 5, 9, 13, 17, 21, 25, 29, 33, 37, 41, 45, 49, 53, 57, 61,
        2, 6, 10, 14, 18, 22, 26, 30, 34, 38, 42, 46, 50, 54, 58, 62,
        3, 7, 11, 15, 19, 23, 27, 31, 35, 39, 43, 47, 51, 55, 59, 63,
        64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79,
        80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95,
        96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
        112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127,
    };
    render_t render[2] = {
        {512,                         /* Width */
         256,                         /* Height */
         (0x80 << 16) | 1 | (0 << 3), /* Alpha | Pixel type | Dither */
         0x400,                       /* Scaler */
         tile_unique_index,           /* Tile index array (0:Vertical) */
         0xff000000,                  /* Background color */
         (void *)0xa4000000,          /* Texture render address (0:Frame render) */
         2,                           /* Pass num */
         {0x00010212, 0x20000101, 0, 0, 0, 0, 0, 0}
         /* binlist = ((PreSort << 29) | (PT << 16) | (TM << 12) | (TR << 8) | (OM << 4) | OP) */},
        {640,                         /* Width */
         480,                         /* Height */
         (0x80 << 16) | 1 | (1 << 3), /* Alpha | Pixel type | Dither */
         0x400,                       /* Scaler */
         0,                           /* Tile index array (0:Vertical) */
         0xff0000ff,                  /* Background color */
         0,                           /* Texture render address (0:Frame render) */
         2,                           /* Pass num */
         {0x00010212, 0x20000100, 0, 0, 0, 0, 0, 0}
         /* binlist = ((PreSort << 29) | (PT << 16) | (TM << 12) | (TR << 8) | (OM << 4) | OP) */},
    };
    int done = 0;
    int bloom_flag = 1;
    float tone_level = 1.0f;

    render_init(&render[0], &render[1]);

    /* Init interrupt */
    asic_evt_set_handler(ASIC_EVT_PVR_OPAQUEDONE, (asic_evt_handler)render_interrupt, 0);
    asic_evt_enable(ASIC_EVT_PVR_OPAQUEDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_OPAQUEMODDONE, (asic_evt_handler)render_interrupt, 0);
    asic_evt_enable(ASIC_EVT_PVR_OPAQUEMODDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_TRANSDONE, (asic_evt_handler)render_interrupt, 0);
    asic_evt_enable(ASIC_EVT_PVR_TRANSDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_TRANSMODDONE, (asic_evt_handler)render_interrupt, 0);
    asic_evt_enable(ASIC_EVT_PVR_TRANSMODDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_PTDONE, (asic_evt_handler)render_interrupt, 0);
    asic_evt_enable(ASIC_EVT_PVR_PTDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_RENDERDONE_TSP, (asic_evt_handler)render_interrupt, 0);
    asic_evt_enable(ASIC_EVT_PVR_RENDERDONE_TSP, ASIC_IRQ_DEFAULT);

    draw_init();
    /* keep drawing frames until start is pressed */
    while (!done)
    {
        maple_device_t *dev;
        cont_state_t *st;

        /* Check reset */
        dev = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
        if (dev)
        {
            /* Reset */
            st = (cont_state_t *)maple_dev_status(dev);
            if ((st->buttons & 0x60e) == 0x60e)
                done = 1;
            /* Bloom ON/OFF */
            if ((st->buttons & CONT_A))
            {
                bloom_flag = 1;
            }
            if ((st->buttons & CONT_B))
            {
                bloom_flag = 0;
            }
            /* Tone level */
            if ((st->buttons & CONT_DPAD_UP))
            {
                tone_level += 0.1f;
                if (tone_level > 1.0f)
                    tone_level = 1.0f;
            }
            else if ((st->buttons & CONT_DPAD_DOWN))
            {
                tone_level -= 0.1f;
                if (tone_level < 0.0f)
                    tone_level = 0.0f;
            }
        }
        draw_frame(bloom_flag, tone_level);
    }

    /* Stop anything that might be going on */
    PVR_SET(PVR_RESET, PVR_RESET_ALL);
    PVR_SET(PVR_RESET, PVR_RESET_NONE);

    asic_evt_set_handler(ASIC_EVT_PVR_OPAQUEDONE, NULL, 0);
    asic_evt_disable(ASIC_EVT_PVR_OPAQUEDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_OPAQUEMODDONE, NULL, 0);
    asic_evt_disable(ASIC_EVT_PVR_OPAQUEMODDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_TRANSDONE, NULL, 0);
    asic_evt_disable(ASIC_EVT_PVR_TRANSDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_TRANSMODDONE, NULL, 0);
    asic_evt_disable(ASIC_EVT_PVR_TRANSMODDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_PTDONE, NULL, 0);
    asic_evt_disable(ASIC_EVT_PVR_PTDONE, ASIC_IRQ_DEFAULT);
    asic_evt_set_handler(ASIC_EVT_PVR_RENDERDONE_TSP, NULL, 0);
    asic_evt_disable(ASIC_EVT_PVR_RENDERDONE_TSP, ASIC_IRQ_DEFAULT);

    return 0;
}
