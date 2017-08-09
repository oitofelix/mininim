/*
  potion.c -- potion module;

  Copyright (C) 2015, 2016, 2017 Bruno Félix Rezende Ribeiro
  <oitofelix@gnu.org>

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

static const char *potion_string (int e);

const char *
potion_string (int e)
{
  switch (e) {
  case NO_ITEM: return "NO_ITEM";
  case EMPTY_POTION: return "EMPTY_POTION";
  case SMALL_HP_POTION: return "SMALL_HP_POTION";
  case BIG_HP_POTION: return "BIG_HP_POTION";
  case SMALL_POISON_POTION: return "SMALL_POISON_POTION";
  case BIG_POISON_POTION: return "BIG_POISON_POTION";
  case FLOAT_POTION: return "FLOAT_POTION";
  case FLIP_POTION: return "FLIP_POTION";
  case ACTIVATION_POTION: return "ACTIVATION_POTION";
  case SWORD: return "SWORD";
  default: assert (false); break;
  }
}

void
draw_potion (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  push_drawn_rectangle (bitmap);
  int e = ext (p);
  const char *potion = potion_string (e);
  draw_object (bitmap, potion, p);
  struct drawn_rectangle *dr = pop_drawn_rectangle ();
  push_clipping_rectangle (dr->bitmap, dr->x, dr->y, dr->w, dr->h);
  draw_tile_fg_front (bitmap, p, NULL);
  struct pos pr; prel (p, &pr, +0, +1);
  draw_tile_fg_front (bitmap, &pr, NULL);
  pop_clipping_rectangle ();
}

/* ALLEGRO_COLOR */
/* e_hp_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, WHITE)) return E_HP_POTION_BUBBLE_COLOR; */
/*   else return c; */
/* } */

/* ALLEGRO_COLOR */
/* e_poison_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, WHITE)) return E_POISON_POTION_BUBBLE_COLOR; */
/*   else return c; */
/* } */

/* ALLEGRO_COLOR */
/* e_float_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, WHITE)) return E_FLOAT_POTION_BUBBLE_COLOR; */
/*   else return c; */
/* } */

/* ALLEGRO_COLOR */
/* e_flip_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, WHITE)) return E_FLIP_POTION_BUBBLE_COLOR; */
/*   else return c; */
/* } */

/* ALLEGRO_COLOR */
/* c_hp_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, WHITE)) return C_HP_POTION_BUBBLE_COLOR; */
/*   else return c; */
/* } */

/* ALLEGRO_COLOR */
/* c_poison_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, WHITE)) return C_POISON_POTION_BUBBLE_COLOR; */
/*   else return c; */
/* } */

/* ALLEGRO_COLOR */
/* c_float_palette (ALLEGRO_COLOR c) */
/* { */
/*   static int i = 0; */

/*   if (color_eq (c, WHITE)) { */
/*     i++; */
/*     return i % 2 */
/*       ? C_FLOAT_POTION_BUBBLE_COLOR_01 */
/*       : C_FLOAT_POTION_BUBBLE_COLOR_02; */
/*   } else return c; */
/* } */

/* ALLEGRO_COLOR */
/* c_flip_palette (ALLEGRO_COLOR c) */
/* { */
/*   static int i = 0; */

/*   if (color_eq (c, WHITE)) { */
/*     i++; */
/*     return i % 2 */
/*       ? C_FLIP_POTION_BUBBLE_COLOR_01 */
/*       : C_FLIP_POTION_BUBBLE_COLOR_02; */
/*   } else return c; */
/* } */
