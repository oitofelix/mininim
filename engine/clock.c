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

#include <error.h>
#include "kernel/video.h"
#include "anim.h"
#include "clock.h"

/* variables */
ALLEGRO_BITMAP *clock_01, *clock_02, *clock_03, *clock_04, *clock_05, *clock_06,
  *clock_07, *clock_sand_01, *clock_sand_02, *clock_sand_03;

ALLEGRO_BITMAP *princess_room_clock = NULL;

void
load_clock (void)
{
  clock_01 = load_bitmap (CLOCK_01);
  clock_02 = load_bitmap (CLOCK_02);
  clock_03 = load_bitmap (CLOCK_03);
  clock_04 = load_bitmap (CLOCK_04);
  clock_05 = load_bitmap (CLOCK_05);
  clock_06 = load_bitmap (CLOCK_06);
  clock_07 = load_bitmap (CLOCK_07);
  clock_sand_01 = load_bitmap (CLOCK_SAND_01);
  clock_sand_02 = load_bitmap (CLOCK_SAND_02);
  clock_sand_03 = load_bitmap (CLOCK_SAND_03);
}

void
unload_clock (void)
{
  al_destroy_bitmap (clock_01);
  al_destroy_bitmap (clock_02);
  al_destroy_bitmap (clock_03);
  al_destroy_bitmap (clock_04);
  al_destroy_bitmap (clock_05);
  al_destroy_bitmap (clock_06);
  al_destroy_bitmap (clock_07);
  al_destroy_bitmap (clock_sand_01);
  al_destroy_bitmap (clock_sand_02);
  al_destroy_bitmap (clock_sand_03);
}

void
draw_clock (void)
{
  static int i = 0;
  ALLEGRO_BITMAP *sand;

  if (! princess_room_clock) return;

  switch (i % 3) {
  case 0: sand = clock_sand_01; break;
  case 1: sand = clock_sand_02; break;
  case 2: sand = clock_sand_03; break;
  default:
    error (-1, 0, "%s: arithmetic error", __func__);
  }

  draw_bitmap (princess_room_clock, screen, 153, 141, 0);
  draw_bitmap (sand, screen, 161, 157, 0);
  i++;
}
