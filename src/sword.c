/*
  sword.c -- sword module;

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

/* cga */
static ALLEGRO_BITMAP *c_normal_sword, *c_shiny_sword;

/* ega */
static ALLEGRO_BITMAP *e_normal_sword, *e_shiny_sword;

/* vga */
static ALLEGRO_BITMAP *v_normal_sword, *v_shiny_sword;

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
}

ALLEGRO_BITMAP *
sword_bitmap (void)
{
  /* switch (vm) { */
  /* case CGA: return c_normal_sword; */
  /* case EGA: return e_normal_sword; */
  /* case VGA: */
    return v_normal_sword;
  /* } */
  /* assert (false); */
  /* return NULL; */
}

ALLEGRO_BITMAP *
shiny_sword_bitmap (void)
{
  /* switch (vm) { */
  /* case CGA: return c_shiny_sword; */
  /* case EGA: return e_shiny_sword; */
  /* case VGA: */
    return v_shiny_sword;
  /* } */
  /* assert (false); */
  /* return NULL; */
}

void
draw_sword (ALLEGRO_BITMAP *bitmap, struct pos *p,
            bool start_pos)
{
  ALLEGRO_BITMAP *normal_sword = sword_bitmap (),
    *shiny_sword = shiny_sword_bitmap ();

  /* if (hgc) { */
  /*   normal_sword = apply_palette (normal_sword, hgc_palette); */
  /*   shiny_sword = apply_palette (shiny_sword, hgc_palette); */
  /* } */

  if (start_pos) {
    normal_sword = apply_palette (normal_sword, start_actor_palette);
    shiny_sword = apply_palette (shiny_sword, start_actor_palette);
  }

  struct coord c;
  ALLEGRO_BITMAP *sword = anim_cycle % 60 ? normal_sword : shiny_sword;
  seedp (p);
  draw_bitmapc (sword, bitmap, sword_coord (p, &c),
                prandom (1) ? ALLEGRO_FLIP_HORIZONTAL : 0);
  unseedp ();

  if (! start_pos) {
    push_clipping_rectangle (bitmap, OW (c.x), OH (c.y),
                             get_bitmap_width (sword),
                             get_bitmap_height (sword));
    draw_tile_fg_front (bitmap, p, NULL);
    pop_clipping_rectangle ();
  }
}

struct coord *
sword_coord (struct pos *p, struct coord *c)
{
  return
    new_coord (c, p->l, p->room,
               PLACE_WIDTH * p->place,
               PLACE_HEIGHT * p->floor + 50);
}

bool
is_sword_frame (struct frame *f)
{
  size_t i;
  for (i = 0; i < actor_nmemb; i++)
    if (f->b == actor[i].xf.b) return true;
  return false;
}
