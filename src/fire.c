/*
  fire.c -- fire module;

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

static ALLEGRO_COLOR e_fire_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_fire_palette (ALLEGRO_COLOR c);

/* vga */
static ALLEGRO_BITMAP *v_fire_00, *v_fire_01, *v_fire_02, *v_fire_03,
  *v_fire_04, *v_fire_05, *v_fire_06, *v_fire_07, *v_fire_08;

void
load_fire (void)
{
  /* vga */
  v_fire_00 = load_bitmap (V_FIRE_00);
  v_fire_01 = load_bitmap (V_FIRE_01);
  v_fire_02 = load_bitmap (V_FIRE_02);
  v_fire_03 = load_bitmap (V_FIRE_03);
  v_fire_04 = load_bitmap (V_FIRE_04);
  v_fire_05 = load_bitmap (V_FIRE_05);
  v_fire_06 = load_bitmap (V_FIRE_06);
  v_fire_07 = load_bitmap (V_FIRE_07);
  v_fire_08 = load_bitmap (V_FIRE_08);
}

void
unload_fire (void)
{
  /* vga */
  destroy_bitmap (v_fire_00);
  destroy_bitmap (v_fire_01);
  destroy_bitmap (v_fire_02);
  destroy_bitmap (v_fire_03);
  destroy_bitmap (v_fire_04);
  destroy_bitmap (v_fire_05);
  destroy_bitmap (v_fire_06);
  destroy_bitmap (v_fire_07);
  destroy_bitmap (v_fire_08);
}

static ALLEGRO_BITMAP *
get_fire_frame (int i, enum vm vm)
{
  ALLEGRO_BITMAP *fire = NULL;

  switch (i) {
  case 0: fire = v_fire_00; break;
  case 1: fire = v_fire_01; break;
  case 2: fire = v_fire_02; break;
  case 3: fire = v_fire_03; break;
  case 4: fire = v_fire_04; break;
  case 5: fire = v_fire_05; break;
  case 6: fire = v_fire_06; break;
  case 7: fire = v_fire_07; break;
  case 8: fire = v_fire_08; break;
  }

  switch (vm) {
  case CGA: fire = apply_palette (fire, c_fire_palette); break;
  case EGA: fire = apply_palette (fire, e_fire_palette); break;
  case VGA: break;
  }

  if (hgc) fire = apply_palette (fire, hgc_palette);

  return fire;
}

void
draw_fire (ALLEGRO_BITMAP* bitmap, struct pos *p, enum vm vm)
{
  struct coord c;
  if (con (p)->bg != TORCH) return;

  ALLEGRO_BITMAP *fire =
    get_fire_frame (prandom_pos_uniq (p, anim_cycle, 1, 8), vm);

  if (peq (p, &mouse_pos))
    fire = apply_palette (fire, selection_palette);

  int flip = prandom_pos (p, 1);
  fire_coord (p, &c);
  c.x -= flip;
  draw_bitmapc (fire, bitmap, &c, flip ? ALLEGRO_FLIP_HORIZONTAL : 0);
}

void
draw_princess_room_fire (ALLEGRO_BITMAP *bitmap, enum vm vm)
{
  static int i = 0;

  ALLEGRO_BITMAP *fire_0 = get_fire_frame (prandom_uniq (FIRE_RANDOM_SEED_0 + i, 1, 8), vm);
  ALLEGRO_BITMAP *fire_1 = get_fire_frame (prandom_uniq (FIRE_RANDOM_SEED_1 + i, 1, 8), vm);

  draw_bitmap (fire_0, bitmap, 93, 99, 0);
  draw_bitmap (fire_1, bitmap, 211, 99, 0);

  i++;
}

struct coord *
fire_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) + 8;
  c->y = PLACE_HEIGHT * p->floor + 5;
  c->room = p->room;
  return c;
}

static ALLEGRO_COLOR
e_fire_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_FIRE_COLOR_01)) return E_FIRE_COLOR_01;
  else if (color_eq (c, V_FIRE_COLOR_02)) return E_FIRE_COLOR_02;
  else return c;
}

static ALLEGRO_COLOR
c_fire_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, V_FIRE_COLOR_01)) return C_FIRE_COLOR_01;
  else if (color_eq (c, V_FIRE_COLOR_02)) return C_FIRE_COLOR_02;
  else return c;
}
