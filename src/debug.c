/*
  debug.c -- debug module;

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

void
save_guard_bitmap (enum anim_type type, int style, enum vm vm)
{
  char *filename = NULL, *prefix = NULL;
  switch (type) {
  case GUARD: prefix = "guard"; break;
  case FAT_GUARD: prefix = "fat-guard"; break;
  case VIZIER: prefix = "vizier"; break;
  case SKELETON: prefix = "skeleton"; break;
  case SHADOW: prefix = "shadow"; break;
  default: prefix = "invalid"; break;

  }
  xasprintf (&filename, "%s-%i.png", prefix, style);
  ALLEGRO_BITMAP *b = get_guard_normal_bitmap (type);
  b = apply_guard_palette (b, type, style, vm);
  save_bitmap (filename, b);
  al_free (filename);
}

void
save_guard_bitmaps (enum vm vm)
{
  int type, style;
  for (type = SHADOW; type <= VIZIER; type++)
    for (style = 0; style <= 7; style++)
      save_guard_bitmap (type, style, vm);
}
