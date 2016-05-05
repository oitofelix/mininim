/*
  wall-pv.c -- wall palace vga module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "mininim.h"

static ALLEGRO_COLOR wall_color (int i);
static void draw_wall_brick (ALLEGRO_BITMAP *bitmap, struct pos *p,
                                int row, int col);
static struct rect *wall_brick_rect (struct pos *p, int row, int col,
                                            struct rect *r);
static ALLEGRO_BITMAP *wall_mark (int i);
static struct frame *wall_mark_frame (struct pos *p, int i,
                                             struct frame *f);

/* palace vga */
ALLEGRO_BITMAP *pv_wall_mark_00, *pv_wall_mark_01, *pv_wall_mark_02,
  *pv_wall_mark_03, *pv_wall_mark_04, *pv_wall_mark_05, *pv_wall_mark_06,
  *pv_wall_mark_07, *pv_wall_mark_08, *pv_wall_mark_09, *pv_wall_mark_10,
  *pv_wall_mark_11, *pv_wall_mark_12, *pv_wall_mark_13, *pv_wall_mark_14;

void
load_wall_pv (void)
{
  /* palace vga */
  pv_wall_mark_00 = load_bitmap (PV_WALL_MARK_00);
  pv_wall_mark_01 = load_bitmap (PV_WALL_MARK_01);
  pv_wall_mark_02 = load_bitmap (PV_WALL_MARK_02);
  pv_wall_mark_03 = load_bitmap (PV_WALL_MARK_03);
  pv_wall_mark_04 = load_bitmap (PV_WALL_MARK_04);
  pv_wall_mark_05 = load_bitmap (PV_WALL_MARK_05);
  pv_wall_mark_06 = load_bitmap (PV_WALL_MARK_06);
  pv_wall_mark_07 = load_bitmap (PV_WALL_MARK_07);
  pv_wall_mark_08 = load_bitmap (PV_WALL_MARK_08);
  pv_wall_mark_09 = load_bitmap (PV_WALL_MARK_09);
  pv_wall_mark_10 = load_bitmap (PV_WALL_MARK_10);
  pv_wall_mark_11 = load_bitmap (PV_WALL_MARK_11);
  pv_wall_mark_12 = load_bitmap (PV_WALL_MARK_12);
  pv_wall_mark_13 = load_bitmap (PV_WALL_MARK_13);
  pv_wall_mark_14 = load_bitmap (PV_WALL_MARK_14);
}

void
unload_wall_pv (void)
{
  /* palace vga */
  destroy_bitmap (pv_wall_mark_00);
  destroy_bitmap (pv_wall_mark_01);
  destroy_bitmap (pv_wall_mark_02);
  destroy_bitmap (pv_wall_mark_03);
  destroy_bitmap (pv_wall_mark_04);
  destroy_bitmap (pv_wall_mark_05);
  destroy_bitmap (pv_wall_mark_06);
  destroy_bitmap (pv_wall_mark_07);
  destroy_bitmap (pv_wall_mark_08);
  destroy_bitmap (pv_wall_mark_09);
  destroy_bitmap (pv_wall_mark_10);
  destroy_bitmap (pv_wall_mark_11);
  destroy_bitmap (pv_wall_mark_12);
  destroy_bitmap (pv_wall_mark_13);
  destroy_bitmap (pv_wall_mark_14);
}

void
draw_wall_pv (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_wall_base_pv (bitmap, p);
  draw_wall_left_pv (bitmap, p);
  draw_wall_right (bitmap, p, PALACE, VGA);
  draw_wall_top (bitmap, p, PALACE, VGA);
}

void
draw_wall_base_pv (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_wall_brick (bitmap, p, 3, 0);
  return;
}

void
draw_wall_left_pv (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_wall_brick (bitmap, p, 0, 0);
  draw_wall_brick (bitmap, p, 1, 0);
  draw_wall_brick (bitmap, p, 1, 1);
  draw_wall_brick (bitmap, p, 2, 0);
  draw_wall_brick (bitmap, p, 2, 1);
  return;
}

void
draw_wall_brick (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 int row, int col)
{
  struct rect r;
  struct frame f0, f1;
  f0.flip = f1.flip = 0;

  struct pos np; npos (p, &np);
  ALLEGRO_COLOR c = mr.cell[mr.dx][mr.dy].wall_color[np.floor][row][np.place + col];
  /* c = compute_wall_color (p, row, col); */
  c = apply_hue_color (c);
  if (peq (p, &mouse_pos)) c = selection_palette (c);
  wall_brick_rect (p, row, col, &r);
  draw_filled_rect (bitmap, &r, c);

  wall_mark_frame (p, row, &f0);
  f0.b = apply_hue_palette (f0.b);
  if (peq (p, &mouse_pos)) f0.b = apply_palette (f0.b, selection_palette);
  wall_mark_frame (p, row + 1, &f1);
  f1.b = apply_hue_palette (f1.b);
  if (peq (p, &mouse_pos)) f1.b = apply_palette (f1.b, selection_palette);
  draw_frame (bitmap, &f0);
  draw_frame (bitmap, &f1);
}

ALLEGRO_COLOR
wall_color (int i)
{
  switch (i) {
  case 0: return PALACE_WALL_COLOR_00;
  case 1: return PALACE_WALL_COLOR_01;
  case 2: return PALACE_WALL_COLOR_02;
  case 3: return PALACE_WALL_COLOR_03;
  case 4: return PALACE_WALL_COLOR_04;
  case 5: return PALACE_WALL_COLOR_05;
  case 6: return PALACE_WALL_COLOR_06;
  case 7: return PALACE_WALL_COLOR_07;
  }
  return PALACE_WALL_COLOR_00;
}

void
generate_wall_colors_for_room (int room, ALLEGRO_COLOR wall_color_array[3][4][11])
{
	uint32_t orand_seed;
	int floor, row, col;
	int ocolor, bcolor, color;

	orand_seed = random_seed;
	random_seed = room;
	prandom(1);

	for (floor = 0; floor < 3; floor++) {
		for (row = 0; row < 4; row++) {
			if (row % 2) bcolor = 0;
			else bcolor = 4;
			ocolor = -1;
			for (col = 0; col <= 10; col++) {
				do {
					color = bcolor + prandom (3);
				} while (color == ocolor);
				wall_color_array[floor][row][col] = wall_color (color);
				ocolor = color;
			}
		}
	}

	random_seed = orand_seed;
}

void
generate_wall_colors (void)
{
  int x, y;
  for (y = mr.h - 1; y >= 0; y--)
    for (x = 0; x < mr.w; x++)
      if (mr.cell[x][y].room) generate_wall_colors_for_room (mr.cell[x][y].room, mr.cell[x][y].wall_color);
      else memcpy (mr.cell[x][y].wall_color, room0_wall_color, sizeof (room0_wall_color));
}

ALLEGRO_COLOR
compute_wall_color (struct pos *p, int row, int col)
{
  int r;
  struct pos np; npos (p, &np);
  r = prandom_seq (np.room, np.floor * 4 * 11
                   + row * 11
                   + np.place + col, 11, 3);
  return wall_color ((row % 2 ? 0 : 4) + r);
}

struct rect *
wall_brick_rect (struct pos *p, int row, int col,
                 struct rect *r)
{
  r->c.room = p->room;
  r->c.x = PLACE_WIDTH * p->place;
  r->c.y = PLACE_HEIGHT * p->floor + 3;

  switch (row) {
  case 0:
    r->w = PLACE_WIDTH;
    r->h = 20;
    break;
  case 1:
    r->c.y += 20;
    r->c.x += 16 * (col % 2);
    r->w = 16;
    r->h = 21;
    break;
  case 2:
    r->c.y += 41;
    r->c.x += 8 * (col % 2);
    r->w = (col % 2) ? 24 : 8;
    r->h = 19;
    break;
  case 3:
    r->c.y += 60;
    r->w = PLACE_WIDTH;
    r->h = 3;
    break;
  }

  return r;
}

ALLEGRO_BITMAP *
wall_mark (int i)
{
  switch (i) {
  case 0: return pv_wall_mark_00;
  case 1: return pv_wall_mark_01;
  case 2: return pv_wall_mark_02;
  case 3: return pv_wall_mark_03;
  case 4: return pv_wall_mark_04;
  case 5: return pv_wall_mark_05;
  case 6: return pv_wall_mark_06;
  case 7: return pv_wall_mark_07;
  case 8: return pv_wall_mark_08;
  case 9: return pv_wall_mark_09;
  case 10: return pv_wall_mark_10;
  case 11: return pv_wall_mark_11;
  case 12: return pv_wall_mark_12;
  case 13: return pv_wall_mark_13;
  case 14: return pv_wall_mark_14;
  }
  return NULL;
}

struct frame *
wall_mark_frame (struct pos *p, int i, struct frame *f)
{
  int r = prandom_seq_pos (p, i, 1, 2);
  f->c.room = p->room;

  switch (i) {
  case 0:
    f->b = wall_mark (r);
    f->c.x = PLACE_WIDTH * (p->place + 1) - 8;
    f->c.y = PLACE_HEIGHT * p->floor + 3;
    break;
  case 1:
    f->b = wall_mark (r + 3);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 17;
    break;
  case 2:
    f->b = wall_mark (r + 6);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 38;
    break;
  case 3:
    f->b = wall_mark (r + 9);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 58;
    break;
  case 4:
    f->b = wall_mark (r + 12);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 63;
    break;
  }

  return f;
}
