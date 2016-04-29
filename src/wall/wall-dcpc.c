/*
  wall-dcpc.c -- wall dungeon cga, palace cga module;

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

/* dungeon cga */
ALLEGRO_BITMAP *dc_wall_base, *dc_wall_left;

/* palace cga */
ALLEGRO_BITMAP *pc_wall_base, *pc_wall_left;

void
load_wall_dcpc (void)
{
  /* dungeon cga */
  dc_wall_base = load_bitmap (DC_WALL_BASE);
  dc_wall_left = load_bitmap (DC_WALL_LEFT);

  /* palace cga */
  pc_wall_base = load_bitmap (PC_WALL_BASE);
  pc_wall_left = load_bitmap (PC_WALL_LEFT);
}

void
unload_wall_dcpc (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_wall_base);
  destroy_bitmap (dc_wall_left);

  /* palace cga */
  destroy_bitmap (pc_wall_base);
  destroy_bitmap (pc_wall_left);
}

void
draw_wall_dcpc (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em)
{
  draw_wall_base_dcpc (bitmap, p, em);
  draw_wall_left_dcpc (bitmap, p, em);
  draw_wall_right (bitmap, p, em, vm);
  draw_wall_top (bitmap, p, em, vm);
}

void
draw_wall_base_dcpc (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em)
{
  ALLEGRO_BITMAP *wall_base = NULL;

  if (em == DUNGEON) wall_base = dc_wall_base;
  else wall_base = pc_wall_base;

  if (hgc) wall_base = apply_palette (wall_base, hgc_palette);
  if (peq (p, &mouse_pos))
    wall_base = apply_palette (wall_base, selection_palette);

  struct coord c;
  draw_bitmapc (wall_base, bitmap, wall_base_coord (p, &c), 0);
}

void
draw_wall_left_dcpc (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em)
{
  ALLEGRO_BITMAP *wall_left = NULL;

  if (em == DUNGEON) wall_left = dc_wall_left;
  else wall_left = pc_wall_left;

  if (hgc) wall_left = apply_palette (wall_left, hgc_palette);
  if (peq (p, &mouse_pos))
    wall_left = apply_palette (wall_left, selection_palette);

  struct coord c;
  draw_bitmapc (wall_left, bitmap, wall_coord (p, &c), 0);
}
