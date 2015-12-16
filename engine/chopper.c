/*
  chopper.c -- chopper module;

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
#include "kernel/audio.h"
#include "kernel/array.h"
#include "kernel/random.h"
#include "anim.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "kid/kid.h"
#include "level.h"
#include "chopper.h"

ALLEGRO_BITMAP *chopper_bottom_01, *chopper_top_01, *chopper_fg_01,
  *chopper_bottom_02, *chopper_top_02, *chopper_fg_02, *chopper_03,
  *chopper_fg_03, *chopper_04, *chopper_fg_04, *chopper_bottom_05,
  *chopper_top_05, *chopper_fg_05;

ALLEGRO_SAMPLE *chopper_sample;

bool sample_chopper;

static struct chopper *chopper = NULL;
static size_t chopper_nmemb = 0;

void
load_vdungeon_chopper (void)
{
  chopper_bottom_01 = load_bitmap (VDUNGEON_CHOPPER_BOTTOM_01);
  chopper_top_01 = load_bitmap (VDUNGEON_CHOPPER_TOP_01);
  chopper_fg_01 = load_bitmap (VDUNGEON_CHOPPER_FG_01);
  chopper_bottom_02 = load_bitmap (VDUNGEON_CHOPPER_BOTTOM_02);
  chopper_top_02 = load_bitmap (VDUNGEON_CHOPPER_TOP_02);
  chopper_fg_02 = load_bitmap (VDUNGEON_CHOPPER_FG_02);
  chopper_03 = load_bitmap (VDUNGEON_CHOPPER_03);
  chopper_fg_03 = load_bitmap (VDUNGEON_CHOPPER_FG_03);
  chopper_04 = load_bitmap (VDUNGEON_CHOPPER_04);
  chopper_fg_04 = load_bitmap (VDUNGEON_CHOPPER_FG_04);
  chopper_bottom_05 = load_bitmap (VDUNGEON_CHOPPER_BOTTOM_05);
  chopper_top_05 = load_bitmap (VDUNGEON_CHOPPER_TOP_05);
  chopper_fg_05 = load_bitmap (VDUNGEON_CHOPPER_FG_05);
}

void
unload_chopper (void)
{
  al_destroy_bitmap (chopper_bottom_01);
  al_destroy_bitmap (chopper_top_01);
  al_destroy_bitmap (chopper_fg_01);
  al_destroy_bitmap (chopper_bottom_02);
  al_destroy_bitmap (chopper_top_02);
  al_destroy_bitmap (chopper_fg_02);
  al_destroy_bitmap (chopper_03);
  al_destroy_bitmap (chopper_fg_03);
  al_destroy_bitmap (chopper_04);
  al_destroy_bitmap (chopper_fg_04);
  al_destroy_bitmap (chopper_bottom_05);
  al_destroy_bitmap (chopper_top_05);
  al_destroy_bitmap (chopper_fg_05);
}

void
load_chopper_samples (void)
{
  chopper_sample = load_sample (CHOPPER_SAMPLE);
}

void
unload_chopper_samples (void)
{
  al_destroy_sample (chopper_sample);
}

void
register_chopper (struct pos *p)
{
  struct chopper c;

  c.p = *p;
  c.i = 0;
  c.wait = CHOPPER_WAIT;

  chopper =
    add_to_array (&c, 1, chopper, &chopper_nmemb, chopper_nmemb, sizeof (c));

  qsort (chopper, chopper_nmemb, sizeof (c), compare_choppers);
}

int
compare_choppers (const void *c0, const void *c1)
{
  return cpos (&((struct chopper *) c0)->p, &((struct chopper *) c1)->p);
}

struct chopper *
chopper_at_pos (struct pos *p)
{
  struct chopper c;
  c.p = *p;
  return bsearch (&c, chopper, chopper_nmemb, sizeof (c), compare_choppers);
}

bool
should_chomp (struct pos *p)
{
  int i;
  struct anim *k;
  struct coord nc;
  struct pos np, pm, _p;

  for (i = 0; i < kid_nmemb; i++) {
    k = &kid[i];
    if (is_kid_dead (&k->f)) continue;
    survey (_m, pos, &k->f, &nc, &pm, &np);
    int inc = p->place < pm.place ? +1 : -1;
    if (p->room == pm.room && p->floor == pm.floor) {
      for (_p = *p; _p.place != pm.place; _p.place += inc)
        if (con (&_p)->fg == WALL) return false;
      return true;
    }
  }

  return false;
}

void
compute_choppers (void)
{
  size_t i;

  for (i = 0; i < chopper_nmemb; i++) {
    struct chopper *c = &chopper[i];
    if (c->p.room == -1) {
      /* remove_chopper (d); i--; */
      continue;
    }

    switch (c->i) {
    case 0:
      if (c->wait-- <= 0 && should_chomp (&c->p)
          && (draw_cycle % CHOPPER_WAIT) ==
          prandom_pos (&c->p, 0, 1, CHOPPER_WAIT - 1)) c->i++; break;
    case 1: c->i++; sample_chopper = true; break;
    case 2: c->i++; break;
    case 3: c->i++; break;
    case 4: c->i = 0; c->wait = CHOPPER_WAIT; break;
    }
  }
}

void
sample_choppers (void)
{
  if (sample_chopper) play_sample (chopper_sample);
  sample_chopper = false;
}

void
draw_chopper_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct chopper *ch = chopper_at_pos (p);
  if (! ch) return;

  switch (ch->i) {
  case 0: draw_chopper_left_01 (bitmap, p); break;
  case 1: draw_chopper_left_02 (bitmap, p); break;
  case 2: draw_chopper_left_03 (bitmap, p); break;
  case 3: draw_chopper_left_04 (bitmap, p); break;
  case 4: draw_chopper_left_05 (bitmap, p); break;
  }
}

void
draw_chopper_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_bottom_01, bitmap, chopper_bottom_coord_01 (p, &c), 0);
  draw_bitmapc (chopper_top_01, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_bottom_02, bitmap, chopper_bottom_coord_02 (p, &c), 0);
  draw_bitmapc (chopper_top_02, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_03, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_04, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_bottom_05, bitmap, chopper_bottom_coord_05 (p, &c), 0);
  draw_bitmapc (chopper_top_05, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct chopper *ch = chopper_at_pos (p);
  if (! ch) return;

  switch (ch->i) {
  case 0: draw_chopper_fg_01 (bitmap, p); break;
  case 1: draw_chopper_fg_02 (bitmap, p); break;
  case 2: draw_chopper_fg_03 (bitmap, p); break;
  case 3: draw_chopper_fg_04 (bitmap, p); break;
  case 4: draw_chopper_fg_05 (bitmap, p); break;
  }
}

void
draw_chopper_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_fg_01, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_fg_02, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_fg_03, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_fg_04, bitmap, chopper_top_coord (p, &c), 0);
}

void
draw_chopper_fg_05 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (chopper_fg_05, bitmap, chopper_top_coord (p, &c), 0);
}

struct coord *
chopper_bottom_coord_01 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 42;
  c->room = p->room;
  return c;
}

struct coord *
chopper_top_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 3;
  c->room = p->room;
  return c;
}

struct coord *
chopper_bottom_coord_02 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 33;
  c->room = p->room;
  return c;
}

struct coord *
chopper_bottom_coord_05 (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 46;
  c->room = p->room;
  return c;
}
