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

#include <stdio.h>
#include <error.h>
#include "prince.h"
#include "kernel/video.h"
#include "kernel/random.h"
#include "engine/room.h"
#include "engine/anim.h"
#include "wall.h"
#include "wall-pv.h"

static ALLEGRO_COLOR palace_wall_color_array[4][3][11];

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

  /* callbacks */
  register_room_callback (compute_palace_wall_color_array);
}

void
unload_wall_pv (void)
{
  /* palace vga */
  al_destroy_bitmap (pv_wall_mark_00);
  al_destroy_bitmap (pv_wall_mark_01);
  al_destroy_bitmap (pv_wall_mark_02);
  al_destroy_bitmap (pv_wall_mark_03);
  al_destroy_bitmap (pv_wall_mark_04);
  al_destroy_bitmap (pv_wall_mark_05);
  al_destroy_bitmap (pv_wall_mark_06);
  al_destroy_bitmap (pv_wall_mark_07);
  al_destroy_bitmap (pv_wall_mark_08);
  al_destroy_bitmap (pv_wall_mark_09);
  al_destroy_bitmap (pv_wall_mark_10);
  al_destroy_bitmap (pv_wall_mark_11);
  al_destroy_bitmap (pv_wall_mark_12);
  al_destroy_bitmap (pv_wall_mark_13);
  al_destroy_bitmap (pv_wall_mark_14);

  /* callbacks */
  remove_room_callback (compute_palace_wall_color_array);
}

void
draw_pv_wall (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_pv_wall_base (bitmap, p);
  draw_pv_wall_left (bitmap, p);
  draw_wall_right (bitmap, p, PALACE, VGA);
}

void
draw_pv_wall_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_pv_wall_brick (bitmap, p, 3, 0);
  return;
}

void
draw_pv_wall_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_pv_wall_brick (bitmap, p, 0, 0);
  draw_pv_wall_brick (bitmap, p, 1, 0);
  draw_pv_wall_brick (bitmap, p, 1, 1);
  draw_pv_wall_brick (bitmap, p, 2, 0);
  draw_pv_wall_brick (bitmap, p, 2, 1);
  return;
}

void
draw_pv_wall_brick (ALLEGRO_BITMAP *bitmap, struct pos *p,
                    int row, int col)
{
  struct rect r;
  struct frame f0, f1;
  f0.flip = f1.flip = 0;

  ALLEGRO_COLOR c = palace_wall_color_array[p->floor][row][p->place + col];
  palace_wall_brick_rect (p, row, col, &r);
  draw_filled_rect (bitmap, &r, c);

  palace_wall_mark_frame (p, row, &f0);
  palace_wall_mark_frame (p, row + 1, &f1);
  draw_frame (bitmap, &f0);
  draw_frame (bitmap, &f1);
}

ALLEGRO_COLOR
palace_wall_color (int i)
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
compute_palace_wall_color_array (int last_room, int room)
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
					color = bcolor + prandom(3);
				} while (color == ocolor);
				palace_wall_color_array[floor][row][col] =
          palace_wall_color (color);
				ocolor = color;
			}
		}
	}

	random_seed = orand_seed;
}

ALLEGRO_COLOR
compute_palace_wall_color (struct pos *p, int row, int col)
{
  int r;
  struct pos np; npos (p, &np);
  r = prandom_seq (np.room, np.floor * 4 * 11
                   + row * 11
                   + np.place + col, 11, 3);
  return palace_wall_color ((row % 2 ? 0 : 4) + r);
}

struct rect *
palace_wall_brick_rect (struct pos *p, int row, int col,
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
palace_wall_mark (int i)
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
palace_wall_mark_frame (struct pos *p, int i, struct frame *f)
{
  int r = prandom_seq_pos (p, i, 1, 2);
  f->c.room = p->room;

  switch (i) {
  case 0:
    f->b = palace_wall_mark (r);
    f->c.x = PLACE_WIDTH * (p->place + 1) - 8;
    f->c.y = PLACE_HEIGHT * p->floor + 3;
    break;
  case 1:
    f->b = palace_wall_mark (r + 3);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 17;
    break;
  case 2:
    f->b = palace_wall_mark (r + 6);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 38;
    break;
  case 3:
    f->b = palace_wall_mark (r + 9);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 58;
    break;
  case 4:
    f->b = palace_wall_mark (r + 12);
    f->c.x = PLACE_WIDTH * p->place;
    f->c.y = PLACE_HEIGHT * p->floor + 63;
    break;
  }

  return f;
}
