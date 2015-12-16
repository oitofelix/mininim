/*
  spikes-floor.c -- spikes floor module;

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

#include <error.h>
#include "kernel/audio.h"
#include "kernel/array.h"
#include "physics.h"
#include "room.h"
#include "level.h"
#include "kid/kid.h"
#include "floor.h"
#include "spikes-floor.h"

ALLEGRO_BITMAP *spikes_floor_left, *spikes_floor_right,
  *spikes_left_01, *spikes_right_01,
  *spikes_left_02, *spikes_right_02, *spikes_left_03, *spikes_right_03,
  *spikes_left_04, *spikes_right_04, *spikes_left_05, *spikes_right_05,
  *spikes_01_fg, *spikes_02_fg, *spikes_03_fg, *spikes_04_fg,
  *spikes_05_fg;

ALLEGRO_SAMPLE *spikes_sample;
bool sample_spikes;

static struct spikes_floor *spikes_floor = NULL;
static size_t spikes_floor_nmemb = 0;

void
load_vdungeon_spikes_floor (void)
{
  spikes_floor_left = load_bitmap (VDUNGEON_SPIKES_FLOOR_LEFT);
  spikes_floor_right = load_bitmap (VDUNGEON_SPIKES_FLOOR_RIGHT);
  spikes_left_01 = load_bitmap (VDUNGEON_SPIKES_LEFT_01);
  spikes_right_01 = load_bitmap (VDUNGEON_SPIKES_RIGHT_01);
  spikes_left_02 = load_bitmap (VDUNGEON_SPIKES_LEFT_02);
  spikes_right_02 = load_bitmap (VDUNGEON_SPIKES_RIGHT_02);
  spikes_left_03 = load_bitmap (VDUNGEON_SPIKES_LEFT_03);
  spikes_right_03 = load_bitmap (VDUNGEON_SPIKES_RIGHT_03);
  spikes_left_04 = load_bitmap (VDUNGEON_SPIKES_LEFT_04);
  spikes_right_04 = load_bitmap (VDUNGEON_SPIKES_RIGHT_04);
  spikes_left_05 = load_bitmap (VDUNGEON_SPIKES_LEFT_05);
  spikes_right_05 = load_bitmap (VDUNGEON_SPIKES_RIGHT_05);
  spikes_01_fg = load_bitmap (VDUNGEON_SPIKES_01_FG);
  spikes_02_fg = load_bitmap (VDUNGEON_SPIKES_02_FG);
  spikes_03_fg = load_bitmap (VDUNGEON_SPIKES_03_FG);
  spikes_04_fg = load_bitmap (VDUNGEON_SPIKES_04_FG);
  spikes_05_fg = load_bitmap (VDUNGEON_SPIKES_05_FG);
}

void
unload_spikes_floor (void)
{
  al_destroy_bitmap (spikes_floor_left);
  al_destroy_bitmap (spikes_floor_right);
  al_destroy_bitmap (spikes_left_01);
  al_destroy_bitmap (spikes_right_01);
  al_destroy_bitmap (spikes_left_02);
  al_destroy_bitmap (spikes_right_02);
  al_destroy_bitmap (spikes_left_03);
  al_destroy_bitmap (spikes_right_03);
  al_destroy_bitmap (spikes_left_04);
  al_destroy_bitmap (spikes_right_04);
  al_destroy_bitmap (spikes_left_05);
  al_destroy_bitmap (spikes_right_05);
  al_destroy_bitmap (spikes_01_fg);
  al_destroy_bitmap (spikes_02_fg);
  al_destroy_bitmap (spikes_03_fg);
  al_destroy_bitmap (spikes_04_fg);
  al_destroy_bitmap (spikes_05_fg);
}

void
load_spikes_floor_samples (void)
{
  spikes_sample = load_sample (SPIKES_SAMPLE);
}

void
unload_spikes_floor_samples (void)
{
  al_destroy_sample (spikes_sample);
}

void
register_spikes_floor (struct pos *p)
{
  struct spikes_floor s;

  s.p = *p;
  s.i = 0;
  s.wait = SPIKES_WAIT;
  s.state = 0;
  s.inactive = false;
  s.murdered_kid = -1;

  spikes_floor =
    add_to_array (&s, 1, spikes_floor, &spikes_floor_nmemb,
                  spikes_floor_nmemb, sizeof (s));

  sort_spikes_floors ();
}

void
sort_spikes_floors (void)
{
  qsort (spikes_floor, spikes_floor_nmemb,
         sizeof (struct spikes_floor), compare_spikes_floors);
}

int
compare_spikes_floors (const void *s0, const void *s1)
{
  return cpos (&((struct spikes_floor *) s0)->p,
               &((struct spikes_floor *) s1)->p);
}

struct spikes_floor *
spikes_floor_at_pos (struct pos *p)
{
  struct spikes_floor s;
  s.p = *p;

  return bsearch (&s, spikes_floor, spikes_floor_nmemb, sizeof (s),
                  compare_spikes_floors);
}

void
break_spikes_floor (struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;
  s->p.room = -1;
  if (s->murdered_kid != -1)
    kid_die_suddenly (get_kid_by_id (s->murdered_kid));
  sort_spikes_floors ();
}

void
compute_spikes_floors (void)
{
  size_t i, j;
  struct coord nc; struct pos pm, np;

  for (i = 0; i < spikes_floor_nmemb; i++) {
    struct spikes_floor *s = &spikes_floor[i];

    if (s->inactive) continue;

    if (s->p.room == -1) {
      /* remove_spikes_floor (s); i--; */
      continue;
    }
    switch (s->i) {
    case 0: if (should_spikes_raise (&s->p)) {
        sample_spikes = true;
        s->i++;
        s->wait = 12;
        s->state = 1;
      } else s->state = 0;
      break;
    case 1: s->i++; s->state = 2; break;
    case 2: s->i++; s->state = 3; break;
    case 3: s->i++; s->state = 4; break;
    case 4: if (! should_spikes_raise (&s->p)) {
        if (s->wait-- == 0) {
          s->i++;
          s->state = 3;
        } else s->state = 5;
      } else s->state = 5;
      break;
    case 5: s->i++; s->state = 2; break;
    case 6: s->i = 0; s->state = 1; break;
    }

    /* spike kid */
    for (j = 0; j < kid_nmemb; j++) {
      struct anim *k = &kid[j];
      if (is_kid_dead (&k->f)
          || k->spikes_immune) continue;
      survey (_m, pos, &k->f, &nc, &pm, &np);
      if (peq (&pm, &s->p)
          && (((s->state >= 2 && s->state <= 4)
               && (is_kid_start_run (&k->f)
                   || is_kid_run (&k->f)
                   || is_kid_stop_run (&k->f)
                   || is_kid_run_jump_running (&k->f)))
              || (is_kid_couch (&k->f) && kid->fall)
              || is_kid_jump_landing (&k->f)
              || is_kid_run_jump_landing (&k->f))) {
        s->inactive = true;
        s->murdered_kid = k->id;
        s->state = 5;
        k->splash = true;
        k->current_lives = 0;
        k->death_pos = s->p;
        video_effect.color = RED;
        start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.1));
        sample_spiked = true;
        kid_die_spiked (k);
      }
    }
  }
}

void
sample_spikes_floors (void)
{
  if (sample_spikes) play_sample (spikes_sample);
  sample_spikes = false;
}

bool
should_spikes_raise_for_pos (struct pos *p, struct pos *pk)
{
  struct pos pb1, pb2;

  return peq (pk, p)
    || (peq (pk, prel (p, &pb1, -1, 0))
        && crel (p, -1, 0)->fg == NO_FLOOR)
    || (peq (pk, prel (p, &pb2, -2, 0))
        && crel (p, -1, 0)->fg == NO_FLOOR
        && crel (p, -2, 0)->fg == NO_FLOOR);
}

bool
should_spikes_raise (struct pos *p)
{
  int i;
  struct anim *k;
  struct coord nc;
  struct pos np, pmf, pm, pmba;

  for (i = 0; i < kid_nmemb; i++) {
    k = &kid[i];
    if (is_kid_dead (&k->f)) continue;
    survey (_mf, pos, &k->f, &nc, &pmf, &np);
    survey (_m, pos, &k->f, &nc, &pm, &np);
    survey (_mba, pos, &k->f, &nc, &pmba, &np);

    if (should_spikes_raise_for_pos (p, &pmf)
        || should_spikes_raise_for_pos (p, &pm)
        || should_spikes_raise_for_pos (p, &pmba))
      return true;
  }

  return false;
}

void
draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  draw_spikes_floor_floor (bitmap, p);
  draw_spikes (bitmap, p, s);
}

void
draw_spikes_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  draw_spikes_floor_floor_left (bitmap, p);
  draw_spikes_left (bitmap, p, s);
}

void
draw_spikes_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  draw_spikes_floor_floor_right (bitmap, p);
  draw_spikes_right (bitmap, p, s);
}

void
draw_spikes_floor_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_floor_base (bitmap, p);
  draw_spikes_floor_floor_left (bitmap, p);
  draw_spikes_floor_floor_right (bitmap, p);
}

void
draw_spikes_floor_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_floor_left, bitmap, floor_left_coord (p, &c), 0);
}

void
draw_spikes_floor_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_floor_right, bitmap, floor_right_coord (p, &c), 0);
}

void
draw_spikes (ALLEGRO_BITMAP *bitmap, struct pos *p,
             struct spikes_floor *s)
{
  draw_spikes_left (bitmap, p, s);
  draw_spikes_right (bitmap, p, s);
}

void
draw_spikes_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                  struct spikes_floor *s)
{
  switch (s->state) {
  case 0: break;
  case 1: draw_spikes_left_01 (bitmap, p); break;
  case 2: draw_spikes_left_02 (bitmap, p); break;
  case 3: draw_spikes_left_03 (bitmap, p); break;
  case 4: draw_spikes_left_04 (bitmap, p); break;
  case 5: draw_spikes_left_05 (bitmap, p); break;
  }
}

void
draw_spikes_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                   struct spikes_floor *s)
{
  switch (s->state) {
  case 0: break;
  case 1: draw_spikes_right_01 (bitmap, p); break;
  case 2: draw_spikes_right_02 (bitmap, p); break;
  case 3: draw_spikes_right_03 (bitmap, p); break;
  case 4: draw_spikes_right_04 (bitmap, p); break;
  case 5: draw_spikes_right_05 (bitmap, p); break;
  }
}

void
draw_spikes_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;

  switch (s->state) {
  case 0: break;
  case 1: draw_spikes_01_fg (bitmap, p); break;
  case 2: draw_spikes_02_fg (bitmap, p); break;
  case 3: draw_spikes_03_fg (bitmap, p); break;
  case 4: draw_spikes_04_fg (bitmap, p); break;
  case 5: draw_spikes_05_fg (bitmap, p); break;
  }
}

void
draw_spikes_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_spikes_left_01 (bitmap, p);
  draw_spikes_right_01 (bitmap, p);
}

void
draw_spikes_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_left_01, bitmap, spikes_left_01_coord (p, &c), 0);
}

void
draw_spikes_right_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_right_01, bitmap, spikes_right_01_coord (p, &c), 0);
}

void
draw_spikes_01_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_01_fg, bitmap, spikes_01_fg_coord (p, &c), 0);
}

struct coord *
spikes_left_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 51;
  c->room = p->room;
  return c;
}

struct coord *
spikes_right_01_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 53;
  c->room = p->room;
  return c;
}

struct coord *
spikes_01_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 56;
  c->room = p->room;
  return c;
}

void
draw_spikes_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_spikes_left_02 (bitmap, p);
  draw_spikes_right_02 (bitmap, p);
}

void
draw_spikes_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_left_02, bitmap, spikes_left_02_coord (p, &c), 0);
}

void
draw_spikes_right_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_right_02, bitmap, spikes_right_02_coord (p, &c), 0);
}

void
draw_spikes_02_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_02_fg, bitmap, spikes_02_fg_coord (p, &c), 0);
}

struct coord *
spikes_left_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
spikes_right_02_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
spikes_02_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 48;
  c->room = p->room;
  return c;
}

void
draw_spikes_03 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_spikes_left_03 (bitmap, p);
  draw_spikes_right_03 (bitmap, p);
}

void
draw_spikes_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_left_03, bitmap, spikes_left_03_coord (p, &c), 0);
}

void
draw_spikes_right_03 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_right_03, bitmap, spikes_right_03_coord (p, &c), 0);
}

void
draw_spikes_03_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_03_fg, bitmap, spikes_03_fg_coord (p, &c), 0);
}

struct coord *
spikes_left_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 34;
  c->room = p->room;
  return c;
}

struct coord *
spikes_right_03_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 41;
  c->room = p->room;
  return c;
}

struct coord *
spikes_03_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 37;
  c->room = p->room;
  return c;
}

void
draw_spikes_04 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_spikes_left_04 (bitmap, p);
  draw_spikes_right_04 (bitmap, p);
}

void
draw_spikes_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_left_04, bitmap, spikes_left_04_coord (p, &c), 0);
}

void
draw_spikes_right_04 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_right_04, bitmap, spikes_right_04_coord (p, &c), 0);
}

void
draw_spikes_04_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_04_fg, bitmap, spikes_04_fg_coord (p, &c), 0);
}

struct coord *
spikes_left_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 35;
  c->room = p->room;
  return c;
}

struct coord *
spikes_right_04_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 36;
  c->room = p->room;
  return c;
}

struct coord *
spikes_04_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 38;
  c->room = p->room;
  return c;
}

void
draw_spikes_05 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_spikes_left_05 (bitmap, p);
  draw_spikes_right_05 (bitmap, p);
}

void
draw_spikes_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_left_05, bitmap, spikes_left_05_coord (p, &c), 0);
}

void
draw_spikes_right_05 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_right_05, bitmap, spikes_right_05_coord (p, &c), 0);
}

void
draw_spikes_05_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (spikes_05_fg, bitmap, spikes_05_fg_coord (p, &c), 0);
}

struct coord *
spikes_left_05_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 35;
  c->room = p->room;
  return c;
}

struct coord *
spikes_right_05_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 37;
  c->room = p->room;
  return c;
}

struct coord *
spikes_05_fg_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 40;
  c->room = p->room;
  return c;
}
