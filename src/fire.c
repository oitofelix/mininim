/*
  fire.c -- fire module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

/* Just for reference! */

/* static ALLEGRO_COLOR e_fire_palette (ALLEGRO_COLOR c); */
/* static ALLEGRO_COLOR c_fire_palette (ALLEGRO_COLOR c); */

/* static ALLEGRO_BITMAP * */
/* get_fire_frame (int i) */
/* { */
/*   ALLEGRO_BITMAP *fire = NULL; */

/*   switch (i) { */
/*   case 0: fire = v_fire_00; break; */
/*   case 1: fire = v_fire_01; break; */
/*   case 2: fire = v_fire_02; break; */
/*   case 3: fire = v_fire_03; break; */
/*   case 4: fire = v_fire_04; break; */
/*   case 5: fire = v_fire_05; break; */
/*   case 6: fire = v_fire_06; break; */
/*   case 7: fire = v_fire_07; break; */
/*   case 8: fire = v_fire_08; break; */
/*   } */

/*   /\* switch (vm) { *\/ */
/*   /\* case CGA: fire = apply_palette (fire, c_fire_palette); break; *\/ */
/*   /\* case EGA: fire = apply_palette (fire, e_fire_palette); break; *\/ */
/*   /\* case VGA: break; *\/ */
/*   /\* } *\/ */

/*   /\* if (hgc) fire = apply_palette (fire, hgc_palette); *\/ */

/*   return fire; */
/* } */

/* void */
/* draw_princess_room_fire (ALLEGRO_BITMAP *bitmap) */
/* { */
/*   static int i = 0; */

/*   ALLEGRO_BITMAP *fire_0 = */
/*     get_fire_frame (prandom_uniq (FIRE_RANDOM_SEED_0 + i, 1, 8)); */
/*   ALLEGRO_BITMAP *fire_1 = */
/*     get_fire_frame (prandom_uniq (FIRE_RANDOM_SEED_1 + i, 1, 8)); */

/*   draw_bitmap (fire_0, bitmap, 93, 99, 0); */
/*   draw_bitmap (fire_1, bitmap, 211, 99, 0); */

/*   i++; */
/* } */

/* struct coord * */
/* fire_coord (struct pos *p, struct coord *c) */
/* { */
/*   return */
/*     new_coord (c, p->l, p->room, */
/*                PLACE_WIDTH * (p->place + 1) + 8, */
/*                PLACE_HEIGHT * p->floor + 5); */
/* } */

/* static ALLEGRO_COLOR */
/* e_fire_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, V_FIRE_COLOR_01)) return E_FIRE_COLOR_01; */
/*   else if (color_eq (c, V_FIRE_COLOR_02)) return E_FIRE_COLOR_02; */
/*   else return c; */
/* } */

/* static ALLEGRO_COLOR */
/* c_fire_palette (ALLEGRO_COLOR c) */
/* { */
/*   if (color_eq (c, V_FIRE_COLOR_01)) return C_FIRE_COLOR_01; */
/*   else if (color_eq (c, V_FIRE_COLOR_02)) return C_FIRE_COLOR_02; */
/*   else return c; */
/* } */
