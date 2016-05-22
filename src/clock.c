/*
  clock.c -- clock module;

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

/* cga */
ALLEGRO_BITMAP *c_clock_00, *c_clock_01, *c_clock_02,
  *c_clock_03, *c_clock_04, *c_clock_05, *c_clock_06;

/* ega */
ALLEGRO_BITMAP *e_clock_00, *e_clock_01, *e_clock_02,
  *e_clock_03, *e_clock_04, *e_clock_05, *e_clock_06;

/* vga */
ALLEGRO_BITMAP *v_clock_00, *v_clock_01, *v_clock_02,
  *v_clock_03, *v_clock_04, *v_clock_05, *v_clock_06;

/* palettable */
ALLEGRO_BITMAP *clock_sand_00, *clock_sand_01, *clock_sand_02;

void
load_clock (void)
{
  /* cga */
  c_clock_00 = load_bitmap (C_CLOCK_00);
  c_clock_01 = load_bitmap (C_CLOCK_01);
  c_clock_02 = load_bitmap (C_CLOCK_02);
  c_clock_03 = load_bitmap (C_CLOCK_03);
  c_clock_04 = load_bitmap (C_CLOCK_04);
  c_clock_05 = load_bitmap (C_CLOCK_05);
  c_clock_06 = load_bitmap (C_CLOCK_06);

  /* ega */
  e_clock_00 = load_bitmap (E_CLOCK_00);
  e_clock_01 = load_bitmap (E_CLOCK_01);
  e_clock_02 = load_bitmap (E_CLOCK_02);
  e_clock_03 = load_bitmap (E_CLOCK_03);
  e_clock_04 = load_bitmap (E_CLOCK_04);
  e_clock_05 = load_bitmap (E_CLOCK_05);
  e_clock_06 = load_bitmap (E_CLOCK_06);

  /* vga */
  v_clock_00 = load_bitmap (V_CLOCK_00);
  v_clock_01 = load_bitmap (V_CLOCK_01);
  v_clock_02 = load_bitmap (V_CLOCK_02);
  v_clock_03 = load_bitmap (V_CLOCK_03);
  v_clock_04 = load_bitmap (V_CLOCK_04);
  v_clock_05 = load_bitmap (V_CLOCK_05);
  v_clock_06 = load_bitmap (V_CLOCK_06);

  /* palettable */
  clock_sand_00 = load_bitmap (CLOCK_SAND_00);
  clock_sand_01 = load_bitmap (CLOCK_SAND_01);
  clock_sand_02 = load_bitmap (CLOCK_SAND_02);
}

void
unload_clock (void)
{
  /* cga */
  destroy_bitmap (c_clock_00);
  destroy_bitmap (c_clock_01);
  destroy_bitmap (c_clock_02);
  destroy_bitmap (c_clock_03);
  destroy_bitmap (c_clock_04);
  destroy_bitmap (c_clock_05);
  destroy_bitmap (c_clock_06);

  /* ega */
  destroy_bitmap (e_clock_00);
  destroy_bitmap (e_clock_01);
  destroy_bitmap (e_clock_02);
  destroy_bitmap (e_clock_03);
  destroy_bitmap (e_clock_04);
  destroy_bitmap (e_clock_05);
  destroy_bitmap (e_clock_06);

  /* vga */
  destroy_bitmap (v_clock_00);
  destroy_bitmap (v_clock_01);
  destroy_bitmap (v_clock_02);
  destroy_bitmap (v_clock_03);
  destroy_bitmap (v_clock_04);
  destroy_bitmap (v_clock_05);
  destroy_bitmap (v_clock_06);

  /* palettable */
  destroy_bitmap (clock_sand_00);
  destroy_bitmap (clock_sand_01);
  destroy_bitmap (clock_sand_02);
}

ALLEGRO_COLOR
c_clock_sand_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, CLOCK_SAND_COLOR)) return C_CLOCK_SAND_COLOR;
  return c;
}

ALLEGRO_COLOR
e_clock_sand_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, CLOCK_SAND_COLOR)) return E_CLOCK_SAND_COLOR;
  return c;
}

ALLEGRO_COLOR
v_clock_sand_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, CLOCK_SAND_COLOR)) return V_CLOCK_SAND_COLOR;
  return c;
}

void
draw_clock (ALLEGRO_BITMAP *bitmap, int i, enum vm vm)
{
  if (i == -1) return;

  ALLEGRO_BITMAP *clock = NULL,
    *clock_00 = NULL,
    *clock_01 = NULL,
    *clock_02 = NULL,
    *clock_03 = NULL,
    *clock_04 = NULL,
    *clock_05 = NULL,
    *clock_06 = NULL,
    *clock_sand = NULL;

  palette pal = NULL;

  switch (vm) {
  case CGA:
    clock_00 = c_clock_00;
    clock_01 = c_clock_01;
    clock_02 = c_clock_02;
    clock_03 = c_clock_03;
    clock_04 = c_clock_04;
    clock_05 = c_clock_05;
    clock_06 = c_clock_06;
    pal = c_clock_sand_palette;
    break;
  case EGA:
    clock_00 = e_clock_00;
    clock_01 = e_clock_01;
    clock_02 = e_clock_02;
    clock_03 = e_clock_03;
    clock_04 = e_clock_04;
    clock_05 = e_clock_05;
    clock_06 = e_clock_06;
    pal = e_clock_sand_palette;
    break;
  case VGA:
    clock_00 = v_clock_00;
    clock_01 = v_clock_01;
    clock_02 = v_clock_02;
    clock_03 = v_clock_03;
    clock_04 = v_clock_04;
    clock_05 = v_clock_05;
    clock_06 = v_clock_06;
    pal = v_clock_sand_palette;
    break;
  }

  switch (i) {
  case 0: clock = clock_00; break;
  case 1: clock = clock_01; break;
  case 2: clock = clock_02; break;
  case 3: clock = clock_03; break;
  case 4: clock = clock_04; break;
  case 5: clock = clock_05; break;
  case 6: clock = clock_06; break;
  }

  switch (anim_cycle % 3) {
  case 0: clock_sand = clock_sand_00; break;
  case 1: clock_sand = clock_sand_01; break;
  case 2: clock_sand = clock_sand_02; break;
  }

  clock_sand = apply_palette (clock_sand, pal);

  if (hgc) {
    clock = apply_palette (clock, hgc_palette);
    clock_sand = apply_palette (clock_sand, hgc_palette);
  }

  draw_bitmap (clock, bitmap, 153, 141, 0);
  if (i < 6) draw_bitmap (clock_sand, bitmap, 162, 157, 0);
}

int
get_clock_by_time_left (void)
{
  float f = ((float) play_time) / time_limit;
  f = f < 0 ? 0 : f;
  f = f > 1 ? 1 : f;
  return f * 6;
}
