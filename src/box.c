/*
  box.c -- box module;

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

static ALLEGRO_COLOR e_box_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_box_palette (ALLEGRO_COLOR c);

/* vga */
static ALLEGRO_BITMAP *v_box_00, *v_box_01, *v_box_02;

void
load_box (void)
{
  /* vga */
  v_box_00 = load_bitmap (V_BOX_00);
  v_box_01 = load_bitmap (V_BOX_01);
  v_box_02 = load_bitmap (V_BOX_02);
}

void
unload_box (void)
{
  /* vga */
  destroy_bitmap (v_box_00);
  destroy_bitmap (v_box_01);
  destroy_bitmap (v_box_02);
}

static ALLEGRO_BITMAP *
get_box_frame (int i, enum vm vm)
{
  ALLEGRO_BITMAP *box = NULL;

  switch (i) {
  case 0: box = v_box_00; break;
  case 1: box = v_box_01; break;
  case 2: box = v_box_02; break;
  }

  switch (vm) {
  case CGA: box = apply_palette (box, c_box_palette); break;
  case EGA: box = apply_palette (box, e_box_palette); break;
  case VGA: break;
  }

  if (hgc) box = apply_palette (box, hgc_palette);

  return box;
}

void
draw_box (ALLEGRO_BITMAP* bitmap, struct pos *p, enum vm vm)
{
  struct coord c;
  if (! peq (&mouse_pos, p)) return;

  ALLEGRO_BITMAP *box = get_box_frame (anim_cycle % 3, vm);
  draw_bitmapc (box, bitmap, box_coord (p, &c), 0);
}

struct coord *
box_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor - 13;
  c->room = p->room;
  return c;
}

static ALLEGRO_COLOR
e_box_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_BOX_COLOR)) return E_BOX_COLOR;
  else return c;
}

static ALLEGRO_COLOR
c_box_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_BOX_COLOR)) return C_BOX_COLOR;
  else return c;
}
