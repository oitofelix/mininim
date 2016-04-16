/*
  sword.c -- sword module;

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

struct frameset sword_frameset[SWORD_FRAMESET_NMEMB];
static void init_sword_frameset (void);

/* cga */
static ALLEGRO_BITMAP *c_normal_sword, *c_shiny_sword;

/* ega */
static ALLEGRO_BITMAP *e_normal_sword, *e_shiny_sword;

/* vga */
static ALLEGRO_BITMAP *v_normal_sword, *v_shiny_sword;

/* palettable */
static ALLEGRO_BITMAP *sword_00, *sword_01, *sword_02, *sword_03,
  *sword_04, *sword_05, *sword_06, *sword_07, *sword_08, *sword_09,
  *sword_10, *sword_11, *sword_12, *sword_13, *sword_14, *sword_15,
  *sword_16, *sword_17, *sword_18, *sword_19, *sword_20, *sword_21,
  *sword_22, *sword_23, *sword_24, *sword_25, *sword_26, *sword_27,
  *sword_28, *sword_29, *sword_30, *sword_31, *sword_32, *sword_33;

static struct coord *sword_coord (struct pos *p, struct coord *c);

void
load_sword (void)
{
  /* cga */
  c_normal_sword = load_bitmap (C_NORMAL_SWORD);
  c_shiny_sword = load_bitmap (C_SHINY_SWORD);

  /* ega */
  e_normal_sword = load_bitmap (E_NORMAL_SWORD);
  e_shiny_sword = load_bitmap (E_SHINY_SWORD);

  /* vga */
  v_normal_sword = load_bitmap (V_NORMAL_SWORD);
  v_shiny_sword = load_bitmap (V_SHINY_SWORD);

  /* palettable */
  sword_00 = load_bitmap (SWORD_00);
  sword_01 = load_bitmap (SWORD_01);
  sword_02 = load_bitmap (SWORD_02);
  sword_03 = load_bitmap (SWORD_03);
  sword_04 = load_bitmap (SWORD_04);
  sword_05 = load_bitmap (SWORD_05);
  sword_06 = load_bitmap (SWORD_06);
  sword_07 = load_bitmap (SWORD_07);
  sword_08 = load_bitmap (SWORD_08);
  sword_09 = load_bitmap (SWORD_09);
  sword_10 = load_bitmap (SWORD_10);
  sword_11 = load_bitmap (SWORD_11);
  sword_12 = load_bitmap (SWORD_12);
  sword_13 = load_bitmap (SWORD_13);
  sword_14 = load_bitmap (SWORD_14);
  sword_15 = load_bitmap (SWORD_15);
  sword_16 = load_bitmap (SWORD_16);
  sword_17 = load_bitmap (SWORD_17);
  sword_18 = load_bitmap (SWORD_18);
  sword_19 = load_bitmap (SWORD_19);
  sword_20 = load_bitmap (SWORD_20);
  sword_21 = load_bitmap (SWORD_21);
  sword_22 = load_bitmap (SWORD_22);
  sword_23 = load_bitmap (SWORD_23);
  sword_24 = load_bitmap (SWORD_24);
  sword_25 = load_bitmap (SWORD_25);
  sword_26 = load_bitmap (SWORD_26);
  sword_27 = load_bitmap (SWORD_27);
  sword_28 = load_bitmap (SWORD_28);
  sword_29 = load_bitmap (SWORD_29);
  sword_30 = load_bitmap (SWORD_30);
  sword_31 = load_bitmap (SWORD_31);
  sword_32 = load_bitmap (SWORD_32);
  sword_33 = load_bitmap (SWORD_33);

  /* frameset */
  init_sword_frameset ();
}

void
unload_sword (void)
{
  /* cga */
  destroy_bitmap (c_normal_sword);
  destroy_bitmap (c_shiny_sword);

  /* ega */
  destroy_bitmap (e_normal_sword);
  destroy_bitmap (e_shiny_sword);

  /* vga */
  destroy_bitmap (v_normal_sword);
  destroy_bitmap (v_shiny_sword);

  /* palettable */
  destroy_bitmap (sword_00);
  destroy_bitmap (sword_01);
  destroy_bitmap (sword_02);
  destroy_bitmap (sword_03);
  destroy_bitmap (sword_04);
  destroy_bitmap (sword_05);
  destroy_bitmap (sword_06);
  destroy_bitmap (sword_07);
  destroy_bitmap (sword_08);
  destroy_bitmap (sword_09);
  destroy_bitmap (sword_10);
  destroy_bitmap (sword_11);
  destroy_bitmap (sword_12);
  destroy_bitmap (sword_13);
  destroy_bitmap (sword_14);
  destroy_bitmap (sword_15);
  destroy_bitmap (sword_16);
  destroy_bitmap (sword_17);
  destroy_bitmap (sword_18);
  destroy_bitmap (sword_19);
  destroy_bitmap (sword_20);
  destroy_bitmap (sword_21);
  destroy_bitmap (sword_22);
  destroy_bitmap (sword_23);
  destroy_bitmap (sword_24);
  destroy_bitmap (sword_25);
  destroy_bitmap (sword_26);
  destroy_bitmap (sword_27);
  destroy_bitmap (sword_28);
  destroy_bitmap (sword_29);
  destroy_bitmap (sword_30);
  destroy_bitmap (sword_31);
  destroy_bitmap (sword_32);
  destroy_bitmap (sword_33);
}

void
init_sword_frameset (void)
{
  struct frameset frameset[SWORD_FRAMESET_NMEMB] =
    {{sword_00,+0,+18},{sword_01,-7,+5},{sword_02,-17,+4},
     {sword_03,-18,+20},{sword_04,-16,+13},{sword_05,+9,+1},
     {sword_06,-7,+18},{sword_07,+0,+17},{sword_08,-7,-4},
     {sword_09,-7,-8},{sword_10,-11,+13},{sword_11,-10,+14},
     {sword_12,-22,+12},{sword_13,-28,+10},{sword_14,-13,-5},
     {sword_15,-1,-11},{sword_16,+0,+0},{sword_17,-15,+15},
     {sword_18,-14,+4},{sword_19,-22,+10},{sword_20,-7,-11},
     {sword_21,-14,-11},{sword_22,-14,-11},{sword_23,-14,-5},
     {sword_24,-28,+3},{sword_25,-28,+8},{sword_26,-21,+7},
     {sword_27,-14,+5},{sword_28,-14,+2},{sword_29,+0,+12},
     {sword_30,-12,+23},{sword_31,+0,+0},{sword_32,-14,+9},
     {sword_33,-5,+12}};

  memcpy (&sword_frameset, &frameset,
          SWORD_FRAMESET_NMEMB * sizeof (struct frameset));
}

void
draw_sword (ALLEGRO_BITMAP *bitmap, struct pos *p, enum vm vm)
{
  ALLEGRO_BITMAP *normal_sword = NULL,
    *shiny_sword = NULL;

  switch (vm) {
  case CGA:
    normal_sword = c_normal_sword;
    shiny_sword = c_shiny_sword;
    break;
    break;
  case EGA:
    normal_sword = e_normal_sword;
    shiny_sword = e_shiny_sword;
    break;
  case VGA:
    normal_sword = v_normal_sword;
    shiny_sword = v_shiny_sword;
    break;
  }

  if (hgc) {
    normal_sword = apply_palette (normal_sword, hgc_palette);
    shiny_sword = apply_palette (shiny_sword, hgc_palette);
  }

  if (edit == EDIT_KID && peq (p, &level.start_pos)) {
    normal_sword = apply_palette (normal_sword, start_anim_palette);
    shiny_sword = apply_palette (shiny_sword, start_anim_palette);
  }

  struct coord c;
  /* if (! is_sword (p)) return; */
  ALLEGRO_BITMAP *sword = anim_cycle % 60 ? normal_sword : shiny_sword;
  seedp (p);
  draw_bitmapc (sword, bitmap, sword_coord (p, &c),
                prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);
  unseedp ();
}

bool
is_sword (struct pos *p)
{
  return con (p)->fg == FLOOR
    && con (p)->ext.item == SWORD;
}

struct coord *
sword_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 50;
  c->room = p->room;
  return c;
}
