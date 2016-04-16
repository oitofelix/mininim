/*
  potion.c -- potion module;

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
ALLEGRO_BITMAP *dc_small_potion, *dc_big_potion;

/* palace cga */
ALLEGRO_BITMAP *pc_small_potion, *pc_big_potion;

/* dungeon ega */
ALLEGRO_BITMAP *de_small_potion, *de_big_potion;

/* palace ega */
ALLEGRO_BITMAP *pe_small_potion, *pe_big_potion;

/* dungeon vga */
ALLEGRO_BITMAP *dv_small_potion, *dv_big_potion;

/* palace vga */
ALLEGRO_BITMAP *pv_small_potion, *pv_big_potion;

/* palettable */
ALLEGRO_BITMAP *bubble_00, *bubble_01, *bubble_02, *bubble_03, *bubble_04,
  *bubble_05, *bubble_06;

static struct coord *small_potion_coord (struct pos *p, struct coord *c);
static struct coord *small_potion_bubble_coord (struct pos *p, struct coord *c);
static struct coord *big_potion_coord (struct pos *p, struct coord *c);
static struct coord *big_potion_bubble_coord (struct pos *p, struct coord *c);
static ALLEGRO_COLOR empty_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR v_life_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR v_poison_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR v_float_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR v_flip_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_life_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_poison_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_float_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR e_flip_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_life_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_poison_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_float_palette (ALLEGRO_COLOR c);
static ALLEGRO_COLOR c_flip_palette (ALLEGRO_COLOR c);

void
load_potion (void)
{
  /* dungeon cga */
  dc_small_potion = load_bitmap (DC_SMALL_POTION);
  dc_big_potion = load_bitmap (DC_BIG_POTION);

  /* palace cga */
  pc_small_potion = load_bitmap (PC_SMALL_POTION);
  pc_big_potion = load_bitmap (PC_BIG_POTION);

  /* dungeon ega */
  de_small_potion = load_bitmap (DE_SMALL_POTION);
  de_big_potion = load_bitmap (DE_BIG_POTION);

  /* palace ega */
  pe_small_potion = load_bitmap (PE_SMALL_POTION);
  pe_big_potion = load_bitmap (PE_BIG_POTION);

  /* dungeon vga */
  dv_small_potion = load_bitmap (DV_SMALL_POTION);
  dv_big_potion = load_bitmap (DV_BIG_POTION);

  /* palace vga */
  pv_small_potion = load_bitmap (PV_SMALL_POTION);
  pv_big_potion = load_bitmap (PV_BIG_POTION);

  /* palettable */
  bubble_00 = load_bitmap (BUBBLE_00);
  bubble_01 = load_bitmap (BUBBLE_01);
  bubble_02 = load_bitmap (BUBBLE_02);
  bubble_03 = load_bitmap (BUBBLE_03);
  bubble_04 = load_bitmap (BUBBLE_04);
  bubble_05 = load_bitmap (BUBBLE_05);
  bubble_06 = load_bitmap (BUBBLE_06);
}

void
unload_potion (void)
{
  /* dungeon cga */
  destroy_bitmap (dc_small_potion);
  destroy_bitmap (dc_big_potion);

  /* palace cga */
  destroy_bitmap (pc_small_potion);
  destroy_bitmap (pc_big_potion);

  /* dungeon ega */
  destroy_bitmap (de_small_potion);
  destroy_bitmap (de_big_potion);

  /* palace ega */
  destroy_bitmap (pe_small_potion);
  destroy_bitmap (pe_big_potion);

  /* dungeon vga */
  destroy_bitmap (dv_small_potion);
  destroy_bitmap (dv_big_potion);

  /* palace vga */
  destroy_bitmap (pv_small_potion);
  destroy_bitmap (pv_big_potion);

  /* palettable */
  destroy_bitmap (bubble_00);
  destroy_bitmap (bubble_01);
  destroy_bitmap (bubble_02);
  destroy_bitmap (bubble_03);
  destroy_bitmap (bubble_04);
  destroy_bitmap (bubble_05);
  destroy_bitmap (bubble_06);
}

static ALLEGRO_BITMAP *
get_bubble_frame (int i)
{
  switch (i) {
  case 0: return bubble_00;
  case 1: return bubble_01;
  case 2: return bubble_02;
  case 3: return bubble_03;
  case 4: return bubble_04;
  case 5: return bubble_05;
  case 6: return bubble_06;
  default:
    error (-1, 0, "%s (%i): unknown bubble frame", __func__, i);
  }

  return NULL;
}

void
draw_potion (ALLEGRO_BITMAP *bitmap, struct pos *p,
             enum em em, enum vm vm)
{
  if (! is_potion (p)) return;

  ALLEGRO_BITMAP *small_potion = NULL,
    *big_potion = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA:
      small_potion = dc_small_potion;
      big_potion = dc_big_potion;
      break;
    case EGA:
      small_potion = de_small_potion;
      big_potion = de_big_potion;
      break;
    case VGA:
      small_potion = dv_small_potion;
      big_potion = dv_big_potion;
      break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA:
      small_potion = pc_small_potion;
      big_potion = pc_big_potion;
      break;
    case EGA:
      small_potion = pe_small_potion;
      big_potion = pe_big_potion;
      break;
    case VGA:
      small_potion = pv_small_potion;
      big_potion = pv_big_potion;
      break;
    }
    break;
  }

  ALLEGRO_BITMAP *bottle = NULL, *bubble = NULL;
  struct coord bottle_coord, bubble_coord;
  palette bubble_palette = NULL;

  seedp (p);
  enum item item = con (p)->ext.item;

  switch (item) {
  case EMPTY_POTION:
  case SMALL_LIFE_POTION:
  case SMALL_POISON_POTION:
  case ACTIVATION_POTION:
    bottle = small_potion;
    small_potion_coord (p, &bottle_coord);
    small_potion_bubble_coord (p, &bubble_coord);
    break;
  case BIG_LIFE_POTION:
  case BIG_POISON_POTION:
  case FLOAT_POTION:
  case FLIP_POTION:
    bottle = big_potion;
    big_potion_coord (p, &bottle_coord);
    big_potion_bubble_coord (p, &bubble_coord);
    break;
  default:
    error (-1, 0, "%s (%i): unknown potion type", __func__, item);
    break;
  }

  palette life_palette = NULL, poison_palette = NULL,
    float_palette = NULL, flip_palette = NULL;

  switch (vm) {
  case CGA:
    life_palette = c_life_palette;
    poison_palette = c_poison_palette;
    float_palette = c_float_palette;
    flip_palette = c_flip_palette;
    break;
  case EGA:
    life_palette = e_life_palette;
    poison_palette = e_poison_palette;
    float_palette = e_float_palette;
    flip_palette = e_flip_palette;
    break;
  case VGA:
    life_palette = v_life_palette;
    poison_palette = v_poison_palette;
    float_palette = v_float_palette;
    flip_palette = v_flip_palette;
    break;
  }

  switch (item) {
  case EMPTY_POTION: bubble_palette = empty_palette; break;
  case SMALL_LIFE_POTION:
  case BIG_LIFE_POTION: bubble_palette = life_palette; break;
  case SMALL_POISON_POTION:
  case BIG_POISON_POTION: bubble_palette = poison_palette; break;
  case FLOAT_POTION: bubble_palette = float_palette; break;
  case FLIP_POTION: bubble_palette = flip_palette; break;
  case ACTIVATION_POTION: bubble_palette = poison_palette; break;
  default:
    error (-1, 0, "%s (%i): unknown potion type", __func__, item);
    break;
  }

  if (hgc) bottle = apply_palette (bottle, hgc_palette);
  draw_bitmapc (bottle, bitmap, &bottle_coord, 0);
  bubble = get_bubble_frame (anim_cycle % 7);
  bubble = apply_palette (bubble, bubble_palette);
  if (hgc) bubble = apply_palette (bubble, hgc_palette);
  int r = prandom (1);
  bubble_coord.x -= r;
  draw_bitmapc (bubble, bitmap, &bubble_coord,
                r ? ALLEGRO_FLIP_HORIZONTAL : 0);
  unseedp ();
}

bool
is_potion (struct pos *p)
{
  return con (p)->fg == FLOOR
    && (con (p)->ext.item == EMPTY_POTION
        || con (p)->ext.item == SMALL_LIFE_POTION
        || con (p)->ext.item == BIG_LIFE_POTION
        || con (p)->ext.item == SMALL_POISON_POTION
        || con (p)->ext.item == BIG_POISON_POTION
        || con (p)->ext.item == FLOAT_POTION
        || con (p)->ext.item == FLIP_POTION
        || con (p)->ext.item == ACTIVATION_POTION);
}

struct coord *
small_potion_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) - 10;
  c->y = PLACE_HEIGHT * p->floor + 49;
  c->room = p->room;
  return c;
}

struct coord *
small_potion_bubble_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) - 7;
  c->y = PLACE_HEIGHT * p->floor + 40;
  c->room = p->room;
  return c;
}

struct coord *
big_potion_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) - 10;
  c->y = PLACE_HEIGHT * p->floor + 45;
  c->room = p->room;
  return c;
}

struct coord *
big_potion_bubble_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1) - 7;
  c->y = PLACE_HEIGHT * p->floor + 36;
  c->room = p->room;
  return c;
}

ALLEGRO_COLOR
empty_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return TRANSPARENT_COLOR;
  else return c;
}

ALLEGRO_COLOR
v_life_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return V_LIFE_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
v_poison_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return V_POISON_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
v_float_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return V_FLOAT_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
v_flip_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return V_FLIP_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
e_life_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return E_LIFE_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
e_poison_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return E_POISON_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
e_float_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return E_FLOAT_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
e_flip_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return E_FLIP_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
c_life_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return C_LIFE_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
c_poison_palette (ALLEGRO_COLOR c)
{
  if (color_eq (c, WHITE)) return C_POISON_POTION_BUBBLE_COLOR;
  else return c;
}

ALLEGRO_COLOR
c_float_palette (ALLEGRO_COLOR c)
{
  static int i = 0;

  if (color_eq (c, WHITE)) {
    i++;
    return i % 2
      ? C_FLOAT_POTION_BUBBLE_COLOR_01
      : C_FLOAT_POTION_BUBBLE_COLOR_02;
  } else return c;
}

ALLEGRO_COLOR
c_flip_palette (ALLEGRO_COLOR c)
{
  static int i = 0;

  if (color_eq (c, WHITE)) {
    i++;
    return i % 2
      ? C_FLIP_POTION_BUBBLE_COLOR_01
      : C_FLIP_POTION_BUBBLE_COLOR_02;
  } else return c;
}
