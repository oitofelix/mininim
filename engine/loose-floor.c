/*
  loose-floor.c -- loose floor module;

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
#include "prince.h"
#include "kernel/video.h"
#include "kernel/random.h"
#include "kernel/array.h"
#include "anim.h"
#include "physics.h"
#include "kid/kid.h"
#include "room.h"
#include "floor.h"
#include "loose-floor.h"
#include "opener-floor.h"
#include "closer-floor.h"
#include "spikes-floor.h"

ALLEGRO_BITMAP *loose_floor_left_01, *loose_floor_right_01, *loose_floor_base_01,
  *loose_floor_left_02, *loose_floor_right_02, *loose_floor_base_02,
  *loose_floor_02, *broken_floor;

ALLEGRO_SAMPLE *loose_floor_01_sample, *loose_floor_02_sample, *loose_floor_03_sample,
  *broken_floor_sample;

bool sample_loose_floor_01, sample_loose_floor_02, sample_loose_floor_03,
  sample_broken_floor;

static struct loose_floor *loose_floor = NULL;
static size_t loose_floor_nmemb = 0;

void
load_vdungeon_loose_floor (void)
{
  loose_floor_left_01 = load_bitmap (VDUNGEON_LOOSE_FLOOR_LEFT_01);
  loose_floor_right_01 = load_bitmap (VDUNGEON_LOOSE_FLOOR_RIGHT_01);
  loose_floor_base_01 = load_bitmap (VDUNGEON_LOOSE_FLOOR_BASE_01);
  loose_floor_left_02 = load_bitmap (VDUNGEON_LOOSE_FLOOR_LEFT_02);
  loose_floor_right_02 = load_bitmap (VDUNGEON_LOOSE_FLOOR_RIGHT_02);
  loose_floor_base_02 = load_bitmap (VDUNGEON_LOOSE_FLOOR_BASE_02);
  /* used for loose floor falling animation */
  loose_floor_02 = create_loose_floor_02_bitmap ();
  broken_floor = create_broken_floor_bitmap ();
}

void
unload_loose_floor (void)
{
  al_destroy_bitmap (loose_floor_left_01);
  al_destroy_bitmap (loose_floor_right_01);
  al_destroy_bitmap (loose_floor_base_01);
  al_destroy_bitmap (loose_floor_left_02);
  al_destroy_bitmap (loose_floor_right_02);
  al_destroy_bitmap (loose_floor_base_02);
  al_destroy_bitmap (loose_floor_02);
}

void
load_loose_floor_samples (void)
{
  loose_floor_01_sample = load_sample (LOOSE_FLOOR_01_SAMPLE);
  loose_floor_02_sample = load_sample (LOOSE_FLOOR_02_SAMPLE);
  loose_floor_03_sample = load_sample (LOOSE_FLOOR_03_SAMPLE);
  broken_floor_sample = load_sample (BROKEN_FLOOR_SAMPLE);
}

void
unload_loose_floor_samples (void)
{
  al_destroy_sample (loose_floor_01_sample);
  al_destroy_sample (loose_floor_02_sample);
  al_destroy_sample (loose_floor_03_sample);
  al_destroy_sample (broken_floor_sample);
}

ALLEGRO_BITMAP *
create_loose_floor_02_bitmap (void)
{
  int wl = al_get_bitmap_width (loose_floor_left_02);
  int wr = al_get_bitmap_width (loose_floor_right_02);
  int w = wl + wr;
  int hl = al_get_bitmap_height (loose_floor_left_02);
  int hr = al_get_bitmap_height (loose_floor_right_02);
  int hb = al_get_bitmap_height (loose_floor_base_02);
  int h = max (hl, hr) + hb;

  ALLEGRO_BITMAP *bitmap = create_bitmap (w, h);
  clear_bitmap (bitmap, al_map_rgba (0, 0, 0, 0));
  draw_bitmap (loose_floor_base_02, bitmap, 0, 14, 0);
  draw_bitmap (loose_floor_left_02, bitmap, 0, 1, 0);
  draw_bitmap (loose_floor_right_02, bitmap, 32, 0, 0);

  return bitmap;
}

ALLEGRO_BITMAP *
create_broken_floor_bitmap (void)
{
  int wl = al_get_bitmap_width (broken_floor_left);
  int wr = al_get_bitmap_width (broken_floor_right);
  int w = wl + wr;
  int hl = al_get_bitmap_height (broken_floor_left);
  int hr = al_get_bitmap_height (broken_floor_right);
  int hb = al_get_bitmap_height (normal_floor_base);
  int h = max (hl, hr) + hb;

  ALLEGRO_BITMAP *bitmap = create_bitmap (w, h);
  clear_bitmap (bitmap, al_map_rgba (0, 0, 0, 0));
  draw_bitmap (normal_floor_base, bitmap, 0, 14, 0);
  draw_bitmap (broken_floor_left, bitmap, 0, 1, 0);
  draw_bitmap (broken_floor_right, bitmap, 32, 0, 0);

  return bitmap;
}

void
register_loose_floor (struct pos *p)
{
  struct loose_floor l;

  l.p = *p;
  l.i = 0;
  l.resist = LOOSE_FLOOR_RESISTENCE;
  l.action = NO_LOOSE_FLOOR_ACTION;
  l.state = 0;

  struct coord c; floor_left_coord (p, &c);
  l.f.b = loose_floor_02;
  l.f.c.room = p->room;
  l.f.c.x = c.x;
  l.f.c.y = c.y + 3;
  l.f.flip = 0;

  loose_floor =
    add_to_array (&l, 1, loose_floor, &loose_floor_nmemb,
                  loose_floor_nmemb, sizeof (l));

  sort_loose_floors ();
}

void
sort_loose_floors (void)
{
  qsort (loose_floor, loose_floor_nmemb, sizeof (struct loose_floor),
         compare_loose_floors);
}

int
compare_loose_floors (const void *l0, const void *l1)
{
  return cpos (&((struct loose_floor *) l0)->p,
               &((struct loose_floor *) l1)->p);
}

struct loose_floor *
loose_floor_at_pos (struct pos *p)
{
  struct loose_floor l;
  l.p = *p;

  return bsearch (&l, loose_floor, loose_floor_nmemb, sizeof (l),
                  compare_loose_floors);
}

void
remove_loose_floor (struct loose_floor *l)
{
  size_t i =  l - loose_floor;
  remove_from_array (loose_floor, &loose_floor_nmemb, i, 1, sizeof (*l));
}

void
release_loose_floor (struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (l->action != RELEASE_LOOSE_FLOOR
      && l->action != FALL_LOOSE_FLOOR) {
    l->action = RELEASE_LOOSE_FLOOR;
    l->i = 0;
  }
}

void
compute_loose_floors (void)
{
  size_t i;

  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];
    if (l->p.room == -1) {
      remove_loose_floor (l); i--;
      continue;
    }
    switch (l->action) {
    case SHAKE_LOOSE_FLOOR: compute_loose_floor_shake (l); break;
    case RELEASE_LOOSE_FLOOR: compute_loose_floor_release (l); break;
    case FALL_LOOSE_FLOOR: compute_loose_floor_fall (l); break;
    default: break;
    }
  }
}

void
compute_loose_floor_shake (struct loose_floor *l)
{
  switch (l->i) {
  case 0: l->state = 1;
    sample_random_loose_floor (); l->i++; break;
  case 1: l->state = 0; l->i++; break;
  case 2: l->state = 2;
    sample_random_loose_floor (); l->i++; break;
  case 3: l->state = 0;
    l->action = NO_LOOSE_FLOOR_ACTION; l->i = 0; break;
  }
}

void
compute_loose_floor_release (struct loose_floor *l)
{
  if (l->resist-- > 0) {
    l->state = 0;
    return;
  }
  switch (l->i) {
  case 0: l->state = 1;
    sample_random_loose_floor (); l->i++; break;
  case 1: l->state = 0; l->i++; break;
  case 2: l->state = 2;
    sample_random_loose_floor (); l->i++; break;
  case 3: l->state = 2; l->i++; break;
  case 4: l->state = 0; l->i++; break;
  case 5: l->state = 0; l->i++; break;
  case 6: l->state = 0; l->i++; break;
  case 7: l->state = 2;
    sample_random_loose_floor (); l->i++; break;
  case 8: l->state = 2; l->i++; break;
  case 9: l->state = 2; l->i++; break;
  case 10:
    con (&l->p)->fg = NO_FLOOR;
    l->state = 2;
    l->i = 0;
    l->f.id = &l->f;
    l->action = FALL_LOOSE_FLOOR;
    l->resist = LOOSE_FLOOR_RESISTENCE;
    break;
  }
}

void
compute_loose_floor_fall (struct loose_floor *l)
{
  int speed = 3 * ++l->i;
  if (speed > 33) speed = 33;

  struct frame nf;
  struct frame_offset fo;
  fo.b = l->f.b;
  fo.dx = 0;
  fo.dy = speed;
  next_frame (&l->f, &nf, &fo);
  struct coord mbo_f, mbo_nf;
  struct pos fpmbo_f, nfpmbo_f, fpmbo_nf, nfpmbo_nf;
  enum confg fcmbo_f, fcmbo_nf;
  fcmbo_f = survey (_mbo, posf, &l->f, &mbo_f, &fpmbo_f, &nfpmbo_f)->fg;
  fcmbo_nf = survey (_mbo, posf, &nf, &mbo_nf, &fpmbo_nf, &nfpmbo_nf)->fg;

  struct pos p;

  /* hit kid */
  int i;
  for (i = 0; i < kid_nmemb; i++) {
    struct coord kmt; struct pos np, kpmt;
    struct anim *k = &kid[i];
    survey (_mt, pos, &k->f, &kmt, &kpmt, &np);
    if (peq (&nfpmbo_f, &kpmt)
        && mbo_f.y <= kmt.y
        && mbo_nf.y >= kmt.y
        && ! k->hit_by_loose_floor
        && ! is_kid_dead (&k->f)
        && ! is_kid_hang_or_climb (&k->f)) {
      k->hit_by_loose_floor = true;
      k->splash = true;
      k->current_lives--;
      k->uncouch_slowly = true;
      sample_hit_wall = true;
      video_effect.color = RED;
      start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (0.1));
      if (k->current_lives <= 0) kid_die_suddenly (k);
      else kid_couch_suddenly (k);
    }
  }

  if (fcmbo_f == NO_FLOOR
      || peq (&fpmbo_f, &fpmbo_nf)) {
    /* the floor hit a rigid structure */
    if (fcmbo_nf == WALL
        || fcmbo_nf == PILLAR
        || fcmbo_nf == DOOR) prel (&fpmbo_nf, &p, -1, 0);
    /* the floor continue to fall */
    else {
      l->f = nf;
      if (fcmbo_nf == NO_FLOOR) l->p = fpmbo_nf;
      sort_loose_floors ();
      return;
    }
    /* the floor hit the ground */
  } else {
    struct loose_floor *m;
    p = fpmbo_f;
    switch (fcmbo_f) {
    case LOOSE_FLOOR: /* loose floor isn't ground */
      m = loose_floor_at_pos (&fpmbo_f);
      m->p.room = -1;
      l->f = nf;
      l->f.b = broken_floor;
      l->p = fpmbo_f;
      l->i = 0;
      con (&fpmbo_f)->fg = NO_FLOOR;
      sort_loose_floors ();
      sample_broken_floor = true;
      return;
    case OPENER_FLOOR: break_opener_floor (&fpmbo_f); break;
    case CLOSER_FLOOR: break_closer_floor (&fpmbo_f); break;
    case SPIKES_FLOOR: break_spikes_floor (&fpmbo_f); break;
    default: break;
    }
  }

  /* reach here only if the floor hit a rigid structure or the
     ground */
  con (&p)->fg = BROKEN_FLOOR;
  shake_loose_floor_row (&p);
  l->p.room = -1;
  sort_loose_floors ();
  sample_broken_floor = true;
}

void
shake_loose_floor_row (struct pos *p)
{
  struct pos _p = *p;

  struct loose_floor *l;
  for (_p.place = PLACES - 1; _p.place >= -1; _p.place--)
    if (con (&_p)->fg == LOOSE_FLOOR) {
      l = loose_floor_at_pos (&_p);
      if (l->action == NO_LOOSE_FLOOR_ACTION) {
        l->action = SHAKE_LOOSE_FLOOR;
        l->i = 0;
      }
    }
}

void
sample_random_loose_floor (void)
{
  switch (prandom (2)) {
  case 0: sample_loose_floor_01 = true;
  case 1: sample_loose_floor_02 = true;
  case 2: sample_loose_floor_03 = true;
  }
}

void
sample_loose_floors (void)
{
  if (sample_loose_floor_01) play_sample (loose_floor_01_sample);
  if (sample_loose_floor_02) play_sample (loose_floor_02_sample);
  if (sample_loose_floor_03) play_sample (loose_floor_03_sample);
  if (sample_broken_floor) play_sample (broken_floor_sample);

  sample_loose_floor_01 = sample_loose_floor_02 =
    sample_loose_floor_03 = sample_broken_floor = false;
}

void
draw_falling_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  if (l->action == FALL_LOOSE_FLOOR) {
    struct coord tr, br;
    struct pos fptr, nfptr, fpbr, nfpbr;
    survey (_tr, posf, &l->f, &tr, &fptr, &nfptr);
    survey (_br, posf, &l->f, &br, &fpbr, &nfpbr);
    draw_frame (bitmap, &l->f);
    draw_confg_base (bitmap, &fptr);
    draw_confg_left (bitmap, &fptr, true);
    draw_confg_base (bitmap, &fpbr);
    draw_confg_left (bitmap, &fpbr, true);
  } else return;
}

void
draw_loose_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  if (l->action == FALL_LOOSE_FLOOR) return;
  else {
    switch (l->state) {
    case 0: draw_floor (bitmap, p); break;
    case 1: draw_loose_floor_01 (bitmap, p); break;
    case 2: draw_loose_floor_02 (bitmap, p); break;
    }
  }
}

void
draw_loose_floor_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_floor_base (bitmap, p); break;
  case 1: draw_loose_floor_01_base (bitmap, p); break;
  case 2: draw_loose_floor_02_base (bitmap, p); break;
  }
}

void
draw_loose_floor_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_floor_left (bitmap, p); break;
  case 1: draw_loose_floor_01_left (bitmap, p); break;
  case 2: draw_loose_floor_02_left (bitmap, p); break;
  }
}

void
draw_loose_floor_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct loose_floor *l = loose_floor_at_pos (p);
  if (! l) return;

  switch (l->state) {
  case 0: draw_floor_right (bitmap, p); break;
  case 1: draw_loose_floor_01_right (bitmap, p); break;
  case 2: draw_loose_floor_02_right (bitmap, p); break;
  }
}

void
draw_loose_floor_01 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_loose_floor_01_base (bitmap, p);
  draw_loose_floor_01_left (bitmap, p);
  draw_loose_floor_01_right (bitmap, p);
}

void
draw_loose_floor_01_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (loose_floor_base_01, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_loose_floor_01_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (loose_floor_left_01, bitmap, loose_floor_left_coord (p, &c), 0);
}

void
draw_loose_floor_01_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (loose_floor_right_01, bitmap, loose_floor_right_coord (p, &c), 0);
}

void
draw_loose_floor_02 (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_loose_floor_02_base (bitmap, p);
  draw_loose_floor_02_left (bitmap, p);
  draw_loose_floor_02_right (bitmap, p);
}

void
draw_loose_floor_02_base (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (loose_floor_base_02, bitmap, floor_base_coord (p, &c), 0);
}

void
draw_loose_floor_02_left (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (loose_floor_left_02, bitmap, floor_left_coord (p, &c), 0);
}

void
draw_loose_floor_02_right (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  struct coord c;
  draw_bitmapc (loose_floor_right_02, bitmap, loose_floor_right_coord (p, &c), 0);
}

struct coord *
loose_floor_left_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * p->place;
  c->y = PLACE_HEIGHT * p->floor + 50 - 1;
  c->room = p->room;
  return c;
}

struct coord *
loose_floor_right_coord (struct pos *p, struct coord *c)
{
  c->x = PLACE_WIDTH * (p->place + 1);
  c->y = PLACE_HEIGHT * p->floor + 50 - 1;
  c->room = p->room;
  return c;
}
