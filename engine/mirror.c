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

#include <stdio.h>
#include <error.h>
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/array.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "mirror.h"

static void create_mirror_bitmaps (int last_room, int room);

static ALLEGRO_BITMAP *mirror_bitmap[FLOORS + 2][PLACES + 1];

/* palace vga */
ALLEGRO_BITMAP *pv_mirror;

void
load_mirror (void)
{
  /* palace vga */
  pv_mirror = load_bitmap (PV_MIRROR);

  /* callbacks */
  register_room_callback (create_mirror_bitmaps);
}

void
unload_mirror (void)
{
  /* palace vga */
  al_destroy_bitmap (pv_mirror);

  /* callbacks */
  remove_room_callback (create_mirror_bitmaps);
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
      clear_bitmap (*b, TRANSPARENT);
    }
}

void
draw_mirror (ALLEGRO_BITMAP *bitmap, struct pos *p,
                 enum em em, enum vm vm)
{
  struct coord c;

  ALLEGRO_BITMAP *mirror = NULL;

  switch (em) {
  case DUNGEON:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: mirror = pv_mirror; break;
    }
    break;
  }

  if (hgc) mirror = apply_palette (mirror, hgc_palette);

  int i;
  for (i = 0; i < kid_nmemb; i++)
    draw_kid_if_at_pos (bitmap, &kid[i], p, vm);

  ALLEGRO_BITMAP *b = mirror_bitmap[p->floor + 1][p->place + 1];
  mirror_coord (p, &c);
  draw_bitmap_region (bitmap, b, c.x + 22, c.y + 3,
                      MIRROR_BITMAP_W, MIRROR_BITMAP_H, 0, 0, 0);
  draw_bitmapc (b, bitmap, mirror_reflex_coord (p, &c), ALLEGRO_FLIP_HORIZONTAL);
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
    case CGA: break;
    case EGA: break;
    case VGA: break;
    }
    break;
  case PALACE:
    switch (vm) {
    case CGA: break;
    case EGA: break;
    case VGA: mirror = pv_mirror; break;
    }
    break;
  }

  if (hgc) mirror = apply_palette (mirror, hgc_palette);

  ALLEGRO_BITMAP *b = mirror_bitmap[p->floor + 1][p->place + 1];
  draw_bitmapc (b, bitmap, mirror_reflex_coord (p, &c), ALLEGRO_FLIP_HORIZONTAL);
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
