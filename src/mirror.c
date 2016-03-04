/*
  mirror.c -- mirror module;

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

static void create_mirror_bitmaps (int last_room, int room);

static ALLEGRO_BITMAP *mirror_bitmap[FLOORS + 2][PLACES + 1];

/* dungeon cga */
ALLEGRO_BITMAP *dc_mirror;

/* palace cga */
ALLEGRO_BITMAP *pc_mirror;

/* dungeon ega */
ALLEGRO_BITMAP *de_mirror;

/* palace ega */
ALLEGRO_BITMAP *pe_mirror;

/* dungeon vga */
ALLEGRO_BITMAP *dv_mirror;

/* palace vga */
ALLEGRO_BITMAP *pv_mirror;

struct mirror *mirror = NULL;
size_t mirror_nmemb = 0;

void
load_mirror (void)
{
  /* dungeon cga */
  dc_mirror = load_bitmap (DC_MIRROR);

  /* palace cga */
  pc_mirror = load_bitmap (PC_MIRROR);

  /* dungeon ega */
  de_mirror = load_bitmap (DE_MIRROR);

  /* palace ega */
  pe_mirror = load_bitmap (PE_MIRROR);

  /* dungeon vga */
  dv_mirror = load_bitmap (DV_MIRROR);

  /* palace vga */
  pv_mirror = load_bitmap (PV_MIRROR);

  /* callbacks */
  register_room_callback (create_mirror_bitmaps);
}

void
unload_mirror (void)
{
  /* dungeon cga */
  al_destroy_bitmap (dc_mirror);

  /* palace cga */
  al_destroy_bitmap (pc_mirror);

  /* dungeon ega */
  al_destroy_bitmap (de_mirror);

  /* palace ega */
  al_destroy_bitmap (pe_mirror);

  /* dungeon vga */
  al_destroy_bitmap (dv_mirror);

  /* palace vga */
  al_destroy_bitmap (pv_mirror);

  /* callbacks */
  remove_room_callback (create_mirror_bitmaps);
}

void
register_mirror (struct pos *p)
{
  struct mirror m;

  m.p = *p;
  m.kid_crossing = -1;

  mirror =
    add_to_array (&m, 1, mirror, &mirror_nmemb,
                  mirror_nmemb, sizeof (m));

  qsort (mirror, mirror_nmemb, sizeof (m),
         compare_mirrors);
}

int
compare_mirrors (const void *o0, const void *o1)
{
  return cpos (&((struct mirror *) o0)->p,
               &((struct mirror *) o1)->p);
}

struct mirror *
mirror_at_pos (struct pos *p)
{
  struct mirror m;
  m.p = *p;

  return bsearch (&m, mirror, mirror_nmemb, sizeof (m),
                  compare_mirrors);
}

void
remove_mirror (struct mirror *m)
{
  size_t i =  m - mirror;
  mirror =
    remove_from_array (mirror, &mirror_nmemb, i, 1, sizeof (*m));
}

void
uncross_mirrors (void)
{
  size_t i;
  for (i = 0; i < mirror_nmemb; i++)
    mirror[i].kid_crossing = -1;
}

static void
create_mirror_bitmaps (int last_room, int room)
{
  struct coord c;
  struct pos p;
  p.room = room;

  for (p.floor = FLOORS; p.floor >= -1; p.floor--)
    for (p.place = -1; p.place < PLACES; p.place++) {
      ALLEGRO_BITMAP **b = &mirror_bitmap[p.floor + 1][p.place + 1];
      if (*b) {
        al_destroy_bitmap (*b);
        *b = NULL;
      }
      if (con (&p)->fg != MIRROR) continue;

      mirror_coord (&p, &c);

      *b = create_bitmap (MIRROR_BITMAP_W, MIRROR_BITMAP_H);
      clear_bitmap (*b, TRANSPARENT_COLOR);
    }
}

void
update_mirror_bitmap (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;

  ALLEGRO_BITMAP *b = mirror_bitmap[p->floor + 1][p->place + 1];
  if (! b) return;

  int i;
  for (i = 0; i < anima_nmemb; i++)
    draw_anim_if_at_pos (bitmap, &anima[i], p, vm);

  mirror_coord (p, &c);
  draw_bitmap_region (bitmap, b, c.x + 22, c.y + 3,
                      MIRROR_BITMAP_W, MIRROR_BITMAP_H, 0, 0,
                      ALLEGRO_FLIP_HORIZONTAL);
}

void
draw_mirror (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  if (con (p)->fg != MIRROR) return;

  struct coord c;

  ALLEGRO_BITMAP *mirror = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: mirror = dc_mirror; break;
    case EGA: mirror = de_mirror; break;
    case VGA: mirror = dv_mirror; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: mirror = pc_mirror; break;
    case EGA: mirror = pe_mirror; break;
    case VGA: mirror = pv_mirror; break;
    }
    break;
  }

  mirror = apply_hue_palette (mirror);
  if (hgc) mirror = apply_palette (mirror, hgc_palette);
  if (peq (p, &mouse_pos))
    mirror = apply_palette (mirror, selection_palette);

  ALLEGRO_BITMAP *b = mirror_bitmap[p->floor + 1][p->place + 1];
  if (b) draw_bitmapc (b, bitmap, mirror_reflex_coord (p, &c), 0);
  draw_bitmapc (mirror, bitmap, mirror_coord (p, &c), 0);
}

void
draw_mirror_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *mirror = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: mirror = dc_mirror; break;
    case EGA: mirror = de_mirror; break;
    case VGA: mirror = dv_mirror; break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: mirror = pc_mirror; break;
    case EGA: mirror = pe_mirror; break;
    case VGA: mirror = pv_mirror; break;
    }
    break;
  }

  mirror = apply_hue_palette (mirror);
  if (hgc) mirror = apply_palette (mirror, hgc_palette);
  if (peq (p, &mouse_pos))
    mirror = apply_palette (mirror, selection_palette);

  ALLEGRO_BITMAP *b = mirror_bitmap[p->floor + 1][p->place + 1];
  if (b) draw_bitmapc (b, bitmap, mirror_reflex_coord (p, &c), 0);

  int h = al_get_bitmap_height (mirror);
  draw_bitmap_regionc (mirror, bitmap, 0, 0, 22, h, mirror_coord (p, &c), 0);
}

struct coord *
mirror_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place - 3;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
mirror_reflex_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place + 3;
  c->y = PLACE_HEIGHT * p->floor + 6;
  c->room = p->room;
  return c;
}
