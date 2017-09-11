/*
  physics.c -- physics module;

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

struct tile *
tile (struct pos *p)
{
  struct pos np; npos (p, &np);
  return &np.l->tile[np.room][np.floor][np.place];
}

struct tile *
tile_rel (struct pos *p, int floor, int place)
{
  struct pos pr;
  return tile (prel (p, &pr, floor, place));
}

enum tile_bg
bg_val (int b)
{
  return typed_int (b, TILE_BGS, 1, NULL, NULL);
}

enum tile_fg
fg_val (int f)
{
  return typed_int (f, TILE_FGS, 1, NULL, NULL);
}

int
ext_val (int f, int e)
{
  switch (fg_val (f)) {
  case FLOOR: case BROKEN_FLOOR: case SKELETON_FLOOR:
  case STUCK_FLOOR: case HIDDEN_FLOOR: case PILLAR:
  case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
    return typed_int (e, TILE_ITEMS, 1, NULL, NULL);

  case LOOSE_FLOOR:
    return typed_int (e, LOOSE_FLOOR_STEPS, LOOSE_FLOOR_FASES, NULL, NULL);

  case SPIKES_FLOOR:
    return typed_int (e, SPIKES_STEPS, SPIKES_FASES, NULL, NULL);

  case OPENER_FLOOR:
  case CLOSER_FLOOR:
    return e;
    /* return typed_int (e, EVENTS, 1, NULL, NULL); */

  case DOOR:
    return typed_int (e, DOOR_STEPS, DOOR_FASES, NULL, NULL);

  case LEVEL_DOOR:
    return typed_int (e, LEVEL_DOOR_STEPS, LEVEL_DOOR_FASES, NULL, NULL);

  case CHOMPER:
    return typed_int (e, CHOMPER_STEPS, CHOMPER_FASES, NULL, NULL);

  case CARPET:
  case TCARPET:
    return typed_int (e, CARPET_DESIGNS, 1, NULL, NULL);

  default: return 0;
  }
}

union tile_state *
clear_tile (struct pos *p, void *data)
{
  set_tile (p, NO_FLOOR, BRICKS_5, NO_ITEM, NO_FAKE);
  return NULL;
}

union tile_state *
random_tile (struct pos *p, void *data)
{
  set_tile (p, prandom (TILE_FGS - 1), prandom (TILE_BGS - 1),
           prandom_max (), is_fake (p) ? prandom (TILE_FGS - 1) : NO_FAKE);
  return NULL;
}

union tile_state *
unfake_tile (struct pos *p, void *data)
{
  set_fake (p, NO_FAKE);
  return NULL;
}

union tile_state *
fg_fake_exchange (struct pos *p, void *data)
{
  enum tile_fg f = fg (p);
  int ff =  fake (p);
  set_fg (p, ff);
  set_fake (p, f);
  return NULL;
}

struct pos *
set_tile (struct pos *p, int f, int b, int e, int ff)
{
  set_fg (p, f);
  set_bg (p, b);
  set_ext (p, e);
  set_fake (p, ff);
  return p;
}

enum tile_bg
bg_tile (struct tile *t)
{
  return bg_val (t->bg);
}

enum tile_bg
bg (struct pos *p)
{
  return bg_tile (tile (p));
}

enum tile_fg
fg_tile (struct tile *t)
{
  return fg_val (t->fg);
}

enum tile_fg
fg (struct pos *p)
{
  return fg_tile (tile (p));
}

enum tile_fg
fake_tile (struct tile *t)
{
  if (t->fake < 0) return fg_tile (t);
  else return fg_val (t->fake);
}

enum tile_fg
fake (struct pos *p)
{
  return fake_tile (tile (p));
}

int
ext_tile (struct tile *t)
{
  return ext_val (t->fg, t->ext);
}

int
ext (struct pos *p)
{
  return ext_tile (tile (p));
}

int
fake_ext (struct pos *p)
{
  struct tile *c = tile (p);
  return ext_val (fake (p), c->ext);
}

enum tile_bg
set_bg (struct pos *p, int b)
{
  return tile (p)->bg = bg_val (b);
}

enum tile_fg
set_fg (struct pos *p, int f)
{
  return tile (p)->fg = fg_val (f);
}

enum tile_fg
set_fake (struct pos *p, int ff)
{
  enum tile_fg f = fg_val (ff);
  if (ff < 0) return tile (p)->fake = NO_FAKE;
  else return tile (p)->fake = (f == fg (p) ? -1 : f);
}

int
set_ext (struct pos *p, int e)
{
  struct tile *c = tile (p);
  return c->ext = ext_val (c->fg, e);
}

enum tile_bg
bg_rel (struct pos *p, int floor, int place)
{
  return bg_val (tile_rel (p, floor, place)->bg);
}

enum tile_fg
fg_rel (struct pos *p, int floor, int place)
{
  return fg_val (tile_rel (p, floor, place)->fg);
}

enum tile_fg
fake_rel (struct pos *p, int floor, int place)
{
  struct tile *c = tile_rel (p, floor, place);
  return fg_val (c->fake < 0 ? c->fg : c->fake);
}

int
ext_rel (struct pos *p, int floor, int place)
{
  struct tile *c = tile_rel (p, floor, place);
  return ext_val (c->fg, c->ext);
}

enum tile_bg
set_bg_rel (struct pos *p, int floor, int place, int b)
{
  return tile_rel (p, floor, place)->bg = bg_val (b);
}

enum tile_fg
set_fg_rel (struct pos *p, int floor, int place, int f)
{
  return tile_rel (p, floor, place)->fg = fg_val (f);
}

int
set_ext_rel (struct pos *p, int floor, int place, int e)
{
  struct tile *c = tile_rel (p, floor, place);
  return c->ext = ext_val (c->fg, e);
}

struct level_event *
event (struct level *l, int e)
{
  return &l->event[typed_int (e, l->event_nmemb, 1, NULL, NULL)];
}

struct guard *
guard (struct level *l, int g)
{
  return &l->guard[typed_int (g, l->guard_nmemb, 1, NULL, NULL)];
}

struct room_linking *
llink (struct level *l, int r)
{
  return &l->link[room_val (l, r)];
}

bool
traversable_cs (enum tile_fg t)
{
  return t == NO_FLOOR
    || t == BIG_PILLAR_TOP
    || t == TCARPET
    || arch_top_cs (t);
}

bool
is_traversable (struct pos *p)
{
  enum tile_fg t = fg (p);
  return traversable_cs (t);
}

bool
is_traversable_fake (struct pos *p)
{
  enum tile_fg t = fake (p);
  return traversable_cs (t);
}

bool
critical_cs (enum tile_fg t)
{
  return traversable_cs (t)
    || t == LOOSE_FLOOR;
}

bool
is_critical (struct pos *p)
{
  return critical_cs (fg (p));
}

bool
is_collidable_at_left (struct pos *p, struct frame *f)
{
  struct chomper *c;

  switch (fg (p)) {
  case WALL:
    return true;
  case CHOMPER:
    c = chomper_at_pos (p);
    if (! c->inactive) return false;
    switch (c->i) {
    case 0: case 4:
      return false;
    case 1: {
      if (! f) return true;
      struct actor *a = get_actor_by_id (f->parent_id);
      if (! a) return true;
      return ! is_kid_run_jump_air (a);
    }
    case 2: case 3:
      return true;
    default: return false;
    }
  case MIRROR: return true;
  default: return false;
  }
}

bool
potentially_collidable_at_left_cs (enum tile_fg t)
{
  return t == WALL  || t == CHOMPER || t == MIRROR;
}

bool
is_potentially_collidable_at_left (struct pos *p)
{
  return potentially_collidable_at_left_cs (fg (p));
}

bool
is_collidable_at_right (struct pos *p, struct frame *f)
{
  struct coord tf;
  switch (fg (p)) {
  case WALL: case TCARPET: case CARPET:
    return true;
  case DOOR:
    if (! f) return false;
    _tf (f, &tf);
    struct actor *a = get_actor_by_id (f->parent_id);
    if (a && a->action == kid_crouch && door_at_pos (p)->i >= 32)
      return true;
    else if (a && a->action == kid_crouch) return false;
    else if (tf.y <= door_grid_tip_y (p) - DOOR_GRID_TIP_THRESHOLD)
      return true;
    else return false;
  default: return false;
  }
}

bool
potentially_collidable_at_right_cs (enum tile_fg t)
{
  return t == WALL  || t == DOOR
    || t == TCARPET || t == CARPET;
}

bool
is_potentially_collidable_at_right (struct pos *p)
{
  return potentially_collidable_at_right_cs (fg (p));
}

bool
is_shockwave_medium (struct pos *p)
{
  enum tile_fg f = fg (p);
  return ! is_traversable (p)
    && f != WALL
    && (f != LOOSE_FLOOR ||
        loose_floor_at_pos (p)->action != SHAKE_LOOSE_FLOOR);
}

bool
is_pillar (struct pos *p)
{
  enum tile_fg t = fg (p);
  return t == PILLAR
    || t == BIG_PILLAR_TOP
    || t == BIG_PILLAR_BOTTOM
    || t == ARCH_BOTTOM;
}

bool
is_rigid_tile (struct pos *p)
{
  enum tile_fg t = fg (p);
  return is_pillar (p)
    || t == WALL || t == DOOR || t == CHOMPER || t == MIRROR
    || is_arch_top (p) || is_carpet (p);
}

bool
carpet_cs (enum tile_fg t)
{
  return t == CARPET || t == TCARPET;
}

bool
is_carpet (struct pos *p)
{
  enum tile_fg t = fg (p);
  return carpet_cs (t);
}

bool
is_arch_top (struct pos *p)
{
  return arch_top_cs (fg (p));
}

bool
arch_top_cs (enum tile_fg t)
{
  return t == ARCH_TOP_MID
    || t == ARCH_TOP_SMALL
    || t == ARCH_TOP_LEFT
    || t == ARCH_TOP_RIGHT;
}

bool
is_door (struct pos *p)
{
  enum tile_fg t = fg (p);
  return t == DOOR
    || t == LEVEL_DOOR;
}

bool
is_floor (struct pos *p)
{
  enum tile_fg t = fg (p);
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == SKELETON_FLOOR
    || t == LOOSE_FLOOR
    || t == SPIKES_FLOOR
    || t == OPENER_FLOOR
    || t == CLOSER_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR;
}

bool
is_floor_like (struct pos *p)
{
  enum tile_fg t = fg (p);
  return is_floor (p)
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == DOOR
    || t == LEVEL_DOOR
    || t == CHOMPER
    || t == MIRROR
    || t == CARPET
    || t == ARCH_BOTTOM;
}

bool
item_fg_cs (enum tile_fg t)
{
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == SKELETON_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == ARCH_BOTTOM;
}

bool
is_item_fg (struct pos *p)
{
  return item_fg_cs (fg (p));
}

bool
potion_ext_cs (int t)
{
  return t == EMPTY_POTION
    || t == SMALL_HP_POTION
    || t == BIG_HP_POTION
    || t == SMALL_POISON_POTION
    || t == BIG_POISON_POTION
    || t == FLOAT_POTION
    || t == FLIP_POTION
    || t == ACTIVATION_POTION;
}

bool
is_potion_ext (struct pos *p)
{
  enum item t = ext (p);
  return potion_ext_cs (t);
}

bool
is_potion (struct pos *p)
{
  return is_item_fg (p) && is_potion_ext (p);
}

bool
is_sword (struct pos *p)
{
  return is_item_fg (p) && ext (p) == SWORD;
}

bool
is_item (struct pos *p)
{
  return is_potion (p) || is_sword (p);
}

bool
is_fake (struct pos *p)
{
  return tile (p)->fake >= 0 && fg (p) != fake (p);
}

bool
fall_fg_cs (enum tile_fg t)
{
  return t == LOOSE_FLOOR;
}

bool
is_fall_fg (struct pos *p)
{
  return fall_fg_cs (fg (p));
}

bool
event_fg_cs (enum tile_fg t)
{
  return t == OPENER_FLOOR
    || t == CLOSER_FLOOR;
}

bool
is_event_fg (struct pos *p)
{
  return event_fg_cs (fg (p));
}

bool
step_fg_cs (enum tile_fg t)
{
  return t == SPIKES_FLOOR
    || t == DOOR
    || t == LEVEL_DOOR
    || t == CHOMPER;
}

bool
is_step_fg (struct pos *p)
{
  return step_fg_cs (fg (p));
}

bool
design_fg_cs (enum tile_fg t)
{
  return t == CARPET
    || t == TCARPET;
}

bool
is_design_fg (struct pos *p)
{
  return design_fg_cs (fg (p));
}

struct pos *
first_tile_fg (struct pos *p0, struct pos *p1, tile_fg_set cs, struct pos *p)
{
  struct pos np0, np1;

  npos (p0, &np0);
  pos2room (p1, np0.room, &np1);

  if (np0.room != np1.room
      || np0.floor != np1.floor) {
    invalid_pos (p);
    return p;
  }

  int inc = np0.place < np1.place ? +1 : -1;

  for (*p = np0; p->place != np1.place; prel (p, p, +0, inc))
    if (cs (fg (p))) return p;

  if (cs (fg (&np1))) {
    *p = np1;
    return p;
  }

  invalid_pos (p);
  return p;
}

struct pos *
next_pos_by_pred (struct pos *p, int dir, pos_pred pred, void *data)
{
  struct pos q = *p;

  if (q.room < 0 || q.floor < 0 || q.place < 0) new_pos (&q, q.l, 0, 0, -1);
  if (q.room > p->l->room_nmemb - 1 || q.floor > FLOORS - 1
      || q.place > PLACES - 1)
    new_pos (&q, q.l, p->l->room_nmemb - 1, FLOORS - 1, PLACES);

  if (dir < 0) {
    goto loop_prev;

    for (q.room = p->l->room_nmemb - 1; q.room >= 0; q.room--)
      for (q.floor = FLOORS - 1; q.floor >= 0; q.floor--)
        for (q.place = PLACES - 1; q.place >= 0; q.place--) {
          if (pred (&q, data)) {
            *p = q;
            return p;
          }

        loop_prev:;
        }
  } else {
    goto loop_next;

    for (q.room = 0; q.room < p->l->room_nmemb; q.room++)
      for (q.floor = 0; q.floor < FLOORS; q.floor++)
        for (q.place = 0; q.place < PLACES; q.place++) {
          if (pred (&q, data)) {
            *p = q;
            return p;
          }

        loop_next:;
        }
  }

  return NULL;
}

bool
is_event_at_pos (struct pos *p, void *data)
{
  int *event = (int *) data;
  enum tile_fg t = fg (p);
  int e = ext (p);

  return (t == OPENER_FLOOR || t == CLOSER_FLOOR)
    && (e == *event || e == -*event - 1);
}

int
next_int_by_pred (int *n, int dir, int min, int max,
                  int_pred pred, void *data)
{
  int m = *n;

  if (m < min) m = min - 1;
  if (m > max) m = max + 1;

  if (dir < 0) {
    goto loop_prev;

    for (m = max; m >= min; m--) {
      if (pred (m, data)) {
        *n = m;
        return m;
      }

    loop_prev:;
    }
  } else {
    goto loop_next;

    for (m = min; m <= max; m++) {
      if (pred (m, data)) {
        *n = m;
        return m;
      }

    loop_next:;
    }
  }

  return -1;
}

bool
is_pos_at_event (int e, void *_p)
{
  struct pos *p = (struct pos *) _p;
  return peq (&event (p->l, e)->p, p);
}

void
exchange_event_pos (struct pos *p0, struct pos *p1)
{
  if (peq (p0, p1)) return;
  size_t i;
  for (i = 0; i < p0->l->event_nmemb; i++)
    if (peq (&event (p0->l, i)->p, p0))
      event (p1->l, i)->p = *p1;
    else if (peq (&event (p1->l, i)->p, p1))
      event (p0->l, i)->p = *p0;
}

void
exchange_guard_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  if (peq (p0, p1)) return;
  int i;
  for (i = 0; i < p0->l->guard_nmemb; i++)
    if (peq (&guard (p0->l, i)->p, p0)) {
      guard (p1->l, i)->p = *p1;
      if (invert_dir)
        guard (p1->l, i)->dir = (guard (p1->l, i)->dir == LEFT) ? RIGHT : LEFT;
    } else if (peq (&guard (p1->l, i)->p, p1)) {
      guard (p0->l, i)->p = *p0;
      if (invert_dir)
        guard (p0->l, i)->dir = (guard (p0->l, i)->dir == LEFT) ? RIGHT : LEFT;
    }
}

void
exchange_kid_start_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  if (peq (p0, p1)) return;
  if (peq (&p0->l->start_pos, p0)) {
    p1->l->start_pos = *p1;
    if (invert_dir)
      p1->l->start_dir = p1->l->start_dir == LEFT ? RIGHT : LEFT;
  } else if (peq (&p1->l->start_pos, p1)) {
    p0->l->start_pos = *p0;
    if (invert_dir)
      p0->l->start_dir = p0->l->start_dir == LEFT ? RIGHT : LEFT;
  }
}

void
exchange_actor_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  if (peq (p0, p1)) return;
  int i, j;
  for (i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];

    struct pos p;
    survey (_m, pos, &a->f, NULL, &p, NULL);

    if (peq (&p, p0)) {
      place_at_pos (&a->f, _m, p1, &a->f.c);
      place_on_the_ground (&a->f, &a->f.c);
      for (j = 0; j < 2; j++) {
        if (is_valid_pos (&a->df_pos[j]))
          get_new_rel_pos (&p, &a->df_pos[j], p1, &a->df_pos[j]);
        if (is_valid_pos (&a->df_posb[j]))
          get_new_rel_pos (&p, &a->df_posb[j], p1, &a->df_posb[j]);
      }
      if (invert_dir) invert_frame_dir (&a->f, &a->f);
    } else if (peq (&p, p1)) {
      place_at_pos (&a->f, _m, p0, &a->f.c);
      place_on_the_ground (&a->f, &a->f.c);
      for (j = 0; j < 2; j++) {
        if (is_valid_pos (&a->df_pos[j]))
          get_new_rel_pos (&p, &a->df_pos[j], p0, &a->df_pos[j]);
        if (is_valid_pos (&a->df_posb[j]))
          get_new_rel_pos (&p, &a->df_posb[j], p0, &a->df_posb[j]);
      }
      if (invert_dir) invert_frame_dir (&a->f, &a->f);
    }
  }
}

void
exchange_falling_floor_pos (struct pos *p0, struct pos *p1)
{
  if (peq (p0, p1)) return;
  size_t i;
  for (i = 0; i < loose_floor_nmemb; i++) {
    struct loose_floor *l = &loose_floor[i];
    if (l->action != FALL_LOOSE_FLOOR) continue;
    if (peq (&l->p, p0)) {
      l->f.c.x = p1->place * PLACE_WIDTH;
      l->f.c.y = p1->floor * PLACE_HEIGHT + fmod (l->f.c.y, PLACE_HEIGHT);
      l->p = *p1;
    } else if (peq (&l->p, p1)) {
      l->f.c.x = p0->place * PLACE_WIDTH;
      l->f.c.y = p0->floor * PLACE_HEIGHT + fmod (l->f.c.y, PLACE_HEIGHT);
      l->p = *p0;
    }
  }
}

void
invert_tile_dir (struct pos *p)
{
  int c = ext (p);

  switch (fg (p)) {
  case CARPET: case TCARPET:
    if (c == 2) set_ext (p, 4);
    else if (c == 3) set_ext (p, 5);
    else if (c == 4) set_ext (p, 2);
    else if (c == 5) set_ext (p, 3);
    break;
  case ARCH_TOP_LEFT:
    set_fg (p, ARCH_TOP_RIGHT);
    break;
  case ARCH_TOP_RIGHT:
    set_fg (p, ARCH_TOP_LEFT);
    break;
  default: break;
  }
}

void
mirror_pos (struct pos *p0, struct pos *p1, bool invert_dir)
{
  union tile_state cons0, cons1;
  struct tile con0, con1;
  if (peq (p0, p1)) return;

  con0 = *tile (p0); copy_to_tile_state (&cons0, p0);
  con1 = *tile (p1); copy_to_tile_state (&cons1, p1);
  *tile (p0) = con1; copy_from_tile_state (p0, &cons1);
  *tile (p1) = con0; copy_from_tile_state (p1, &cons0);
  /* if (should_init (&con0, &con1)) { */
    init_tile_at_pos (p0);
    init_tile_at_pos (p1);
  /* } */
  if (invert_dir) {
    invert_tile_dir (p0);
    invert_tile_dir (p1);
  }
  exchange_event_pos (p0, p1);
  exchange_guard_pos (p0, p1, invert_dir);
  exchange_kid_start_pos (p0, p1, invert_dir);
  exchange_actor_pos (p0, p1, invert_dir);
  exchange_falling_floor_pos (p0, p1);

  register_changed_pos (p0);
  register_changed_pos (p1);
}

union tile_state *
decorate_tile (struct pos *p, void *data)
{
  struct pos np;

  enum tile_fg f = fg (p);
  enum tile_bg b = bg (p);
  int e = ext (p);

  switch (f) {
  case FLOOR: case BROKEN_FLOOR: case SKELETON_FLOOR:
    if (f == FLOOR && e != NO_ITEM) break;
    switch (prandom (2)) {
    case 0: set_fg (p, FLOOR); break;
    case 1: set_fg (p, BROKEN_FLOOR); break;
    case 2:
      if (is_critical (prel (p, &np, +0, -1))) break;
      set_fg (p, SKELETON_FLOOR);
      break;
    }
    break;
  case PILLAR: case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
    if (f == BIG_PILLAR_BOTTOM
        && fg_rel (p, -1, +0) == BIG_PILLAR_TOP) break;
    if (f == ARCH_BOTTOM
        && fg_rel (p, -1, +0) == ARCH_TOP_MID) break;
    switch (prandom (2)) {
    case 0: set_fg (p, PILLAR); break;
    case 1:
      if (fg_rel (p, -1, +1) == NO_FLOOR) break;
      set_fg (p, BIG_PILLAR_BOTTOM); break;
    case 2: set_fg (p, ARCH_BOTTOM); break;
    }
    break;
  default: break;
  }

  if (b != WINDOW && b != BALCONY) {
    if (is_floor (p)
        || f == NO_FLOOR
        || f == CHOMPER
        || f == MIRROR
        || f == ARCH_BOTTOM) {
      switch (prandom (4)) {
      case 0: set_bg (p, BRICKS_2); break;
      case 1: set_bg (p, BRICKS_3); break;
      case 2: set_bg (p, BRICKS_4); break;
      case 3: set_bg (p, BRICKS_5); break;
      case 4: set_bg (p, TORCH); break;
      }
    } else if (f == DOOR) {
      switch (prandom (4)) {
      case 0: set_bg (p, BRICKS_2); break;
      case 1: set_bg (p, BRICKS_3); break;
      case 2: set_bg (p, BRICKS_4); break;
      case 3: set_bg (p, BRICKS_5); break;
      }
    } else if (f == WALL
               && (wall_correlation (p) == SWS
                   || wall_correlation (p) == WWS))
      set_bg (p, BRICKS_5);
    else set_bg (p, NO_BRICKS);
  }

  if (is_carpet (p)) {
    switch (e) {
    case 0:
    case 1:
      set_ext (p, prandom (1) ? 0 : 1);
      set_bg (p, BRICKS_5);
      break;
    case 2:
    case 3:
      set_ext (p, prandom (1) ? 2 : 3);
      set_bg (p, BRICKS_5);
      break;
    case 4:
    case 5:
      set_ext (p, prandom (1) ? 4 : 5);
      set_bg (p, BRICKS_5);
      break;
    default: break;
    }
  }

  return NULL;
}

struct tile_copy *
copy_tile (struct tile_copy *c, struct pos *p)
{
  c->c = *tile (p);
  copy_to_tile_state (&c->cs, p);
  return c;
}

union tile_state *
paste_tile (struct pos *p, struct tile_copy *c)
{
  *tile (p) = c->c;
  return &c->cs;
}

struct room_copy *
copy_room (struct room_copy *rc, struct level *l, int room)
{
  struct pos p; new_pos (&p, l, room, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      copy_tile (&rc->c[p.floor][p.place], &p);
  return rc;
}

struct level *
paste_room (struct level *l, int room, struct room_copy *rc, char *desc)
{
  struct pos p; new_pos (&p, l, room, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      apply_to_place (&p, (pos_trans) paste_tile,
                      &rc->c[p.floor][p.place], NULL);
  end_undo_set (&undo, desc);
  register_changed_room (room);
  return l;
}

void
apply_to_place (struct pos *p, pos_trans f, void *data, char *desc)
{
  struct tile c0, c1;
  c0 = *tile (p);
  union tile_state *ts = f (p, data);
  c1 = *tile (p);
  *tile (p) = c0;
  register_tile_undo (&undo, p,
                      c1.fg, c1.bg, c1.ext, c1.fake,
                      ts, (intptr_t) f, desc);
}

void
apply_to_place_rect_sel (struct rect_sel *rs, pos_trans f,
                         void *data, char *desc)
{
  for (size_t room = 1; room < rs->level->room_nmemb; room++) {
    if (! is_room_in_rect_sel (rs, room)) continue;
    struct pos p; new_pos (&p, rs->level, room, -1, -1);
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++)
        if (is_pos_in_rect_sel (rs, &p))
          apply_to_place (&p, f, data, NULL);
  }
  end_undo_set (&undo, desc);
}

void
apply_to_room (struct level *l, int room, pos_trans f,
               void *data, char *desc)
{
  struct pos p; new_pos (&p, l, room, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      apply_to_place (&p, f, data, NULL);
  end_undo_set (&undo, desc);
  register_changed_room (room);
}

void
apply_to_room_rect_sel (struct rect_sel *rs, pos_trans f,
                        void *data, char *desc)
{
  for (size_t room = 1; room < rs->level->room_nmemb; room++) {
    if (! is_room_in_rect_sel (rs, room)) continue;
    apply_to_room (rs->level, room, f, data, NULL);
  }
  end_undo_set (&undo, desc);
}

void
apply_to_level (struct level *l, pos_trans f, void *data, char *desc)
{
  for (int i = 1; i < l->room_nmemb; i++) apply_to_room (l, i, f, data, NULL);
  end_undo_set (&undo, desc);
}

void
apply_to_scope (struct pos *p, struct rect_sel *rs, pos_trans f, void *data,
                char *base_desc, enum scope scope)
{
  char *desc;
  switch (scope) {
  case PLACE_SCOPE:
    desc = xasprintf ("%s (PLACE)", base_desc);
    apply_to_place (p, f, data, desc);
    break;
  case PLACE_RECT_SEL_SCOPE:
    desc = xasprintf ("%s (PLACE RECT. SEL.)", base_desc);
    apply_to_place_rect_sel (rs, f, data, desc);
    break;
  case ROOM_SCOPE:
    desc = xasprintf ("%s (ROOM)", base_desc);
    apply_to_room (p->l, p->room, f, data, desc);
    break;
  case ROOM_RECT_SEL_SCOPE:
    desc = xasprintf ("%s (ROOM RECT. SEL.)", base_desc);
    apply_to_room_rect_sel (rs, f, data, desc);
    break;
  case LEVEL_SCOPE:
    desc = xasprintf ("%s (LEVEL)", base_desc);
    apply_to_level (p->l, f, data, desc);
    break;
  default: assert (false); return;
  }
  al_free (desc);
}

enum tile_diff
tile_diff (struct tile *c0, struct tile *c1)
{
  enum tile_fg fg0 = fg_val (c0->fg);
  enum tile_bg bg0 = bg_val (c0->bg);
  int ext0 = ext_val (fg0, c0->ext);
  int fake0 = c0->fake;

  enum tile_fg fg1 = fg_val (c1->fg);
  enum tile_bg bg1 = bg_val (c1->bg);
  int ext1 = ext_val (fg1, c1->ext);
  int fake1 = c1->fake;

  if (fg0 == fg1 && bg0 == bg1 && ext0 == ext1 && fake0 == fake1)
    return TILE_DIFF_NO_DIFF;

  if (fg0 != fg1 && bg0 == bg1 && ext0 == ext1 && fake0 == fake1)
    return TILE_DIFF_FG;

  if (fg0 == fg1 && bg0 != bg1 && ext0 == ext1 && fake0 == fake1)
    return TILE_DIFF_BG;

  if (fg0 == fg1 && bg0 == bg1 && ext0 != ext1 && fake0 == fake1)
    return TILE_DIFF_EXT;

  if (fg0 == fg1 && bg0 == bg1 && ext0 == ext1 && fake0 != fake1)
    return TILE_DIFF_FAKE;

  return TILE_DIFF_MIXED;
}

struct level *
mirror_room_h (struct level *l, int room)
{
  struct pos p0, p1;
  new_pos (&p0, l, room, -1, -1);
  for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
    for (p0.place = 0; p0.place < PLACES / 2; p0.place++) {
      reflect_pos_h (&p0, &p1);
      mirror_pos (&p0, &p1, true);
    }
  register_changed_room (room);
  return l;
}

struct level *
mirror_level_h (struct level *l)
{
  int i;
  for (i = 1; i < l->room_nmemb; i++) {
    mirror_room_h (l, i);
    mirror_link (l, i, LEFT, RIGHT);
  }
  return l;
}



bool
is_immediately_accessible_pos (struct pos *to, struct pos *from,
                               struct frame *f)
{
  if (cutscene) return true;

  if (peq (to, from)) return true;

  if (peqr (to, from, +0, -1))
    return ! is_collidable_at_left (from, f)
      && ! is_collidable_at_right (to, f);

  if (peqr (to, from, +0, +1))
    return ! is_collidable_at_right (from, f)
      && ! is_collidable_at_left (to, f);

  if (peqr (to, from, +1, +0))
    return is_traversable (from);

  struct pos from_b;
  prel (from, &from_b, +1, +0);

  if (peqr (to, from, +1, -1))
    return is_traversable (from)
      && ! is_collidable_at_left (&from_b, f)
      && ! is_collidable_at_right (to, f);

  if (peqr (to, from, +1, +1))
    return is_traversable (from)
      && ! is_collidable_at_right (&from_b, f)
      && ! is_collidable_at_left (to, f);

  return false;
}

bool
is_accessible (struct frame *f, coord_f cf, int dx0, int dx1)
{
  struct pos p0, p1;
  surveyo (cf, dx0, +0, pos, f, NULL, &p0, NULL);
  surveyo (cf, dx1, +0, pos, f, NULL, &p1, NULL);
  return is_immediately_accessible_pos (&p1, &p0, f);
}

struct frame *
move_frame (struct frame *f, coord_f cf, int dx,
            lua_Number move_left, lua_Number move_right)
{
  /* positive move_dx moves foward, negative move_dx moves backward */
  struct pos p, np;
  struct frame nf = *f;
  lua_Number move_dx = nf.dir == LEFT ? move_left : move_right;
  int inc = nf.dir == LEFT ? -1 : +1;
  nf.c.x += inc * move_dx;
  surveyo (cf, dx, +0, pos, f, NULL, &p, NULL);
  surveyo (cf, dx, +0, pos, &nf, NULL, &np, NULL);
  while (! is_immediately_accessible_pos (&np, &p, &nf)
         && fabs (nf.c.x - f->c.x) < ORIGINAL_WIDTH) {
    nf.c.x += nf.c.x < f->c.x ? +1 : -1;
    surveyo (cf, dx, +0, pos, &nf, NULL, &np, NULL);
  }
  *f = nf;
  return f;
}

bool
uncollide (struct frame *f, struct frame_offset *fo, coord_f cf,
           int left, int right,
           struct frame_offset *fo_ret,
           struct collision_info *ci_ret)
{
  struct frame nf;
  struct frame_offset nfo = *fo;

  struct collision_info ci;
  invalid_pos (&ci.kid_p);
  invalid_pos (&ci.tile_p);

  if (cutscene) {
    if (fo_ret) *fo_ret = nfo;
    if (ci_ret) *ci_ret = ci;
    return false;
  }

  int dx = f->dir == LEFT ? left : right;

  struct pos p, op;
  surveyo (cf, dx - 8, +0, pos, f, NULL, &op, NULL);

  next_frame (f, &nf, &nfo);
  frame2room (&nf, f->c.room, &nf.c);
  surveyo (cf, dx, +0, pos, &nf, NULL, &p, NULL);

  /* find out increment */
  int inc = 0;
  int inc_pos = 0;
  pos2room (&p, op.room, &p);
  if (p.place < op.place) {
    inc_pos = -1;
    struct frame nf2;
    struct frame_offset nfo2 = *fo;
    nfo2.dx += +1;
    next_frame (f, &nf2, &nfo2);
    frame2room (&nf2, f->c.room, &nf2.c);
    inc = nf2.c.x > nf.c.x ? +1 : -1;
  } else if (p.place > op.place) {
    inc_pos = +1;
    struct frame nf2;
    struct frame_offset nfo2 = *fo;
    nfo2.dx += +1;
    next_frame (f, &nf2, &nfo2);
    frame2room (&nf2, f->c.room, &nf2.c);
    inc = nf2.c.x < nf.c.x ? +1 : -1;
  }

  /* uncollide */
  bool c = false;
  struct frame_offset cnfo;
  while (! is_immediately_accessible_pos (&p, &op, &nf)
         && fabs (nfo.dx) < ORIGINAL_WIDTH) {
    struct pos pp = p;
    nfo.dx += inc;
    next_frame (f, &nf, &nfo);
    frame2room (&nf, f->c.room, &nf.c);
    surveyo (cf, dx, +0, pos, &nf, NULL, &p, NULL);
    /* p.floor = op.floor; */
    op.floor = p.floor;
    if (peqr (&pp, &p, +0, inc_pos)
        && ! is_immediately_accessible_pos (&pp, &p, &nf)) {
      if (inc_pos == -1) {
        if (is_collidable_at_left (&p, &nf))
          ci.tile_p = p;
        else if (is_collidable_at_right (&pp, &nf))
          ci.tile_p = pp;
      } else {
        if (is_collidable_at_right (&p, &nf))
          ci.tile_p = p;
        else if (is_collidable_at_left (&pp, &nf))
          ci.tile_p = pp;
      }

      c = true;
      cnfo = nfo;
      ci.kid_p = pp;
    }
  }

  if (fo_ret) *fo_ret = c ? cnfo : nfo;
  if (ci_ret) *ci_ret = ci;

  return c;
}

bool
uncollide_static (struct frame *f,
                  struct frame_offset *fo,
                  coord_f cf_front,
                  int front_left, int front_right,
                  coord_f cf_back,
                  int back_left, int back_right,
                  struct frame_offset *fo_ret)
{
  struct pos pf0, pb0, pf1, pb1;
  struct frame_offset fo0 = *fo, fo1 = *fo;
  struct frame nf0, nf1;

  int dx_front = f->dir == LEFT ? front_left : front_right;
  int dx_back = f->dir == LEFT ? back_left : back_right;

  next_frame (f, &nf0, &fo0);
  frame2room (&nf0, f->c.room, &nf0.c);
  surveyo (cf_front, dx_front, +0, pos, &nf0, NULL, &pf0, NULL);
  surveyo (cf_back, dx_back, +0, pos, &nf0, NULL, &pb0, NULL);

  next_frame (f, &nf1, &fo1);
  frame2room (&nf1, f->c.room, &nf1.c);
  surveyo (cf_front, dx_front, +0, pos, &nf1, NULL, &pf1, NULL);
  surveyo (cf_back, dx_back, +0, pos, &nf1, NULL, &pb1, NULL);

  bool c = ! is_immediately_accessible_pos (&pf0, &pb0, &nf0)
    || ! is_immediately_accessible_pos (&pf1, &pb1, &nf1);

  if (! fo_ret) return c;

  while (! is_immediately_accessible_pos (&pf0, &pb0, &nf0)
         && fabs (fo0.dx) <= PLACE_WIDTH) {
    fo0.dx++;
    next_frame (f, &nf0, &fo0);
    frame2room (&nf0, f->c.room, &nf0.c);
    surveyo (cf_front, dx_front, +0, pos, &nf0, NULL, &pf0, NULL);
    surveyo (cf_back, dx_back, +0, pos, &nf0, NULL, &pb0, NULL);
  }

  while (! is_immediately_accessible_pos (&pf1, &pb1, &nf1)
         && fabs (fo1.dx) <= PLACE_WIDTH) {
    fo1.dx--;
    next_frame (f, &nf1, &fo1);
    frame2room (&nf1, f->c.room, &nf1.c);
    surveyo (cf_front, dx_front, +0, pos, &nf1, NULL, &pf1, NULL);
    surveyo (cf_back, dx_back, +0, pos, &nf1, NULL, &pb1, NULL);
  }

  int d0 = fabs (fo0.dx - fo->dx);
  int d1 = fabs (fo1.dx - fo->dx);
  *fo_ret = (d0 < d1) ? fo0 : fo1;
  return c;
}

int
dist_collision (struct frame *f, coord_f cf, int left, int right,
                struct collision_info *ci_ret)
{
  if (cutscene) return PLACE_WIDTH + 1;

  struct frame_offset fo;
  fo.b = f->b;
  fo.dx = fo.dy = 0;

  int inc = cf_inc (f, cf);

  while (! uncollide (f, &fo, cf, left, right, NULL, ci_ret)
         && fabs (fo.dx) <= PLACE_WIDTH)
    fo.dx += inc;

  return fabs (fo.dx);
}

void
enforce_wall_collision (struct actor *a)
{
  struct frame *f = &a->f;

  struct pos pl, pr;
  int dx = +2;
  surveyo (_ml, dx, +0, pos, f, NULL, &pl, NULL);
  surveyo (_mr, dx, +0, pos, f, NULL, &pr, NULL);

  if (fg (&pl) != WALL || fg (&pr) != WALL)
    return;

  struct pos pll, prr;
  prel (&pl, &pll, +0, -1);
  prel (&pr, &prr, +0, +1);

  int inc;
  if (! is_collidable_at_right (&pll, f)
      && is_collidable_at_left (&prr, f)) inc = -1;
  else if (is_collidable_at_right (&pll, f)
           && ! is_collidable_at_left (&prr, f)) inc = +1;
  else if (f->dir == LEFT) inc = +1;
  else inc = -1;

  int max = PLACES;

  surveyo (_ml, dx, +0, pos, f, NULL, NULL, &pl);
  surveyo (_mr, dx, +0, pos, f, NULL, NULL, &pr);
  pos2room (&pr, pl.room, &pr);

  while (fg (&pl) == WALL
         && fg (&pr) == WALL
         && abs (pl.place - pr.place) <= 1
         && max--) {
    f->c.x += PLACE_WIDTH * inc;
    surveyo (_ml, dx, +0, pos, f, NULL, NULL, &pl);
    surveyo (_mr, dx, +0, pos, f, NULL, NULL, &pr);
    pos2room (&pr, pl.room, &pr);
  }
}

bool
is_in_danger (struct pos *p0, struct frame *f, coord_f cf, bool *misstep_ret)
{
  struct frame_offset fo;
  fo.b = f->b;
  fo.dx = -1;
  fo.dy = 0;

  struct pos p;
  surveyo (cf, +0, +0, pos, f, NULL, &p, NULL);

  if (uncollide (f, &fo, cf, +0, +6, NULL, NULL))
    return true;

  if (peq (p0, &p)) return false;

  bool misstep = is_falling (f, cf, +0, +0)
    || fg (&p) == LOOSE_FLOOR;

  if (misstep && misstep_ret) *misstep_ret = misstep;

  return misstep
    || fg (&p) == CLOSER_FLOOR
    || (f->dir == LEFT && fg (p0) == CHOMPER)
    || (f->dir == RIGHT && fg (&p) == CHOMPER);
}




int
dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                 int margin, bool reverse)
{
  if (cutscene) return PLACE_WIDTH + 1;

  struct frame _f = *f;

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int inc = (_f.dir == LEFT) ? r * -1 : r * +1;

  int i = 0;

  struct pos np, _np;
  survey (cf, pf, &_f, NULL, NULL, &np);

  do {
    i += inc;
    _f.c.x += inc;
    nframe (&_f, &_f.c);
    survey (cf, pf, &_f, NULL, NULL, &_np);
  } while (np.place == _np.place
           && abs (i) < PLACE_WIDTH);

  return abs (i);
}

bool
is_on_tile (struct frame *f, coord_f cf, pos_f pf,
           int margin, bool reverse, int min_dist, enum tile_fg t)
{
  struct frame _f = *f;
  int dn = dist_next_place (&_f, cf, pf, margin, reverse);

  _f.c.x += (_f.dir == LEFT) ? -margin : +margin;
  int r = reverse ? -1 : 1;
  int dir = (_f.dir == LEFT) ? r * -1: r * +1;

  struct pos p;
  survey (cf, pf, &_f, NULL, &p, NULL);

  return dn <= min_dist && fg_rel (&p, 0, dir) == t;
}

int
dist_tile (struct frame *f, coord_f cf, pos_f pf,
          int margin, bool reverse, enum tile_fg t)
{
  if (cutscene) return PLACE_WIDTH + 1;

  int dn = dist_next_place (f, cf, pf, margin, reverse);
  if (is_on_tile (f, cf, pf, margin, reverse, dn, t)) return dn;
  else return PLACE_WIDTH + 1;
}

int
dist_chomper (struct frame *f, bool reverse)
{
  if (cutscene) return PLACE_WIDTH + 1;

  struct pos ptf, ptfr;

  struct frame _f = *f;
  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  survey (_tf, pos, &_f, NULL, &ptf, NULL);
  enum tile_fg ctf = fg (&ptf);
  prel (&ptf, &ptfr, +0, +1);
  enum tile_fg ctfr = fg (&ptfr);

  if (_f.dir == LEFT && ctf == CHOMPER)
    return dist_next_place (&_f, _tf, pos, +0, false);
  else if (_f.dir == RIGHT && ctfr == CHOMPER)
    return dist_next_place (&_f, _tf, pos, +0, false);

  return PLACE_WIDTH + 1;
}

int
dist_fall (struct frame *f, bool reverse)
{
  if (cutscene) return PLACE_WIDTH + 1;

  int dnf, dbpt, datm, dats, datl, datr, dtc;

  dnf = dist_tile (f, _bf, pos, -4, reverse, NO_FLOOR);
  dbpt = dist_tile (f, _bf, pos, -4, reverse, BIG_PILLAR_TOP);
  datm = dist_tile (f, _bf, pos, -4, reverse, ARCH_TOP_MID);
  dats = dist_tile (f, _bf, pos, -4, reverse, ARCH_TOP_SMALL);
  datl = dist_tile (f, _bf, pos, -4, reverse, ARCH_TOP_LEFT);
  datr = dist_tile (f, _bf, pos, -4, reverse, ARCH_TOP_RIGHT);
  dtc = dist_tile (f, _bf, pos, -4, reverse, TCARPET);

  if (dnf <= PLACE_WIDTH) return dnf;
  if (dbpt <= PLACE_WIDTH) return dbpt;
  if (datm <= PLACE_WIDTH) return datm;
  if (dats <= PLACE_WIDTH) return dats;
  if (datl <= PLACE_WIDTH) return datl;
  if (datr <= PLACE_WIDTH) return datr;
  if (dtc <= PLACE_WIDTH) return dtc;

  return PLACE_WIDTH + 1;
}



bool
hangable_cs (enum tile_fg t, enum dir d)
{
  return t == FLOOR
    || t == BROKEN_FLOOR
    || t == LOOSE_FLOOR
    || t == SKELETON_FLOOR
    || t == SPIKES_FLOOR
    || t == OPENER_FLOOR
    || t == CLOSER_FLOOR
    || t == STUCK_FLOOR
    || t == HIDDEN_FLOOR
    || t == PILLAR
    || t == BIG_PILLAR_BOTTOM
    || t == DOOR
    || t == LEVEL_DOOR
    || (t == CHOMPER && d == LEFT)
    || t == ARCH_BOTTOM
    || (t == CARPET && d == RIGHT)
    || (t == MIRROR && d == LEFT);
}

bool
is_hangable (struct pos *p, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;

  enum tile_fg f = fg (p);
  enum tile_fg fr = fg_rel (p, +0, +1);

  struct pos pa; prel (p, &pa, -1, 0);
  struct pos hanged_pos; prel (p, &hanged_pos, -1, dir);
  enum tile_fg fh = fg (&hanged_pos);

  return hangable_cs (fh, d)
    && (fh != LOOSE_FLOOR
        || loose_floor_at_pos (&hanged_pos)->action != RELEASE_LOOSE_FLOOR)
    && is_traversable (&pa)
    && ! (d == LEFT && f == CHOMPER)
    && ! (d == LEFT && f == MIRROR)
    && ! (d == RIGHT && fr == CHOMPER)
    && ! (d == RIGHT && fr == MIRROR)
    && ! (d == RIGHT && is_carpet (p))
    && ! (d == RIGHT && is_carpet (&pa))
    && ! (d == RIGHT && f == DOOR)
    && ! (d == RIGHT && f == WALL)
    && ! (d == LEFT && f == WALL);
}

bool
can_hang (struct actor *a, bool reverse, struct pos *hang_pos)
{
  struct frame _f = a->f;

  if (reverse) _f.dir = (_f.dir == LEFT) ? RIGHT : LEFT;

  nframe (&_f, &_f.c);

  struct coord tb; _tb (&_f, &tb);

  struct pos pbf, pmbo, pbb, pbbb;
  survey (_bf, pos, &_f, NULL, &pbf, NULL);
  survey (_mbo, pos, &_f, NULL, &pmbo, NULL);
  survey (_bb, pos, &_f, NULL, &pbb, NULL);
  prel (&pbb, &pbbb, +0, (_f.dir == LEFT) ? +1 : -1);

  bool hbf = is_hangable (&pbf, _f.dir);
  bool hmbo = is_hangable (&pmbo, _f.dir);
  bool hbb = is_hangable (&pbb, _f.dir);
  bool hbbb = is_hangable (&pbbb, _f.dir);

  if (! hbf && ! hmbo && ! hbb && !hbbb)
    return false;

  if (hbbb) *hang_pos = pbbb;
  if (hbb) *hang_pos = pbb;
  if (hmbo) *hang_pos = pmbo;
  if (hbf) *hang_pos = pbf;

  pos2room (hang_pos, _f.c.room, hang_pos);

  /* for fall */
  struct coord ch;
  ch.x = PLACE_WIDTH * (hang_pos->place + 1) - 1;
  ch.y = PLACE_HEIGHT * hang_pos->floor;

  float d = dist_coord (&tb, &ch);

  /* if (reverse) printf ("dist_coord = %f\n", d); */

  if (is_actor_fall (a) && d > (reverse ? 22 : 18)) return false;

  return true;
}

struct pos *
get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p)
{
  int dir = (d == LEFT) ? -1 : +1;
  prel (hang_pos, p, -1, dir);
  pos2room (p, hang_pos->room, p);
  return p;
}

enum tile_fg
get_hanged_tile (struct pos *hang_pos, enum dir d)
{
  struct pos p;
  return fg (get_hanged_pos (hang_pos, d, &p));
}

bool
is_free (struct pos *hang_pos, enum dir d)
{
  int dir = (d == LEFT) ? -1 : +1;
  struct pos ptf; prel (hang_pos, &ptf, 0, dir);
  enum tile_fg ctf = fg (&ptf);
  return ! (ctf == WALL || (ctf == DOOR && d == LEFT)
            || (is_carpet (&ptf) && d == LEFT));
}





bool
is_constrained_pos (struct pos *p, struct frame *f)
{
  struct pos p_1, p1, p2;
  int d = f->dir == LEFT ? -1 : +1;
  prel (p, &p_1, +0, d * -1);
  prel (p, &p1, +0, d * +1);
  prel (p, &p2, +0, d * +2);
  return is_traversable (&p_1)
    && ((f->dir == LEFT &&
         ((! is_traversable (&p1)
           && is_traversable (&p2))
          || (! is_traversable (&p1)
              && ! is_collidable_at_left (p, f)
              && ! is_collidable_at_right (&p1, f)
              && (is_collidable_at_left (&p1, f)
                  || is_collidable_at_right (&p2, f)))))
        || (f->dir == RIGHT &&
            ((! is_traversable (&p1)
              && is_traversable (&p2))
             || (! is_traversable (&p1)
                 && ! is_collidable_at_right (p, f)
                 && ! is_collidable_at_left (&p1, f)
                 && (is_collidable_at_right (&p1, f)
                     || is_collidable_at_left (&p2, f))))));
}

bool
is_in_front_level_door (struct frame *f, struct pos *p)
{
  struct pos ptr, ptl, p_ret;
  surveyo (_tr, +8, 0, pos, f, NULL, &ptr, NULL);
  surveyo (_tl, +8, 0, pos, f, NULL, &ptl, NULL);

  if (fg (&ptr) == LEVEL_DOOR)
    p_ret = ptr;
  else if (fg (&ptl) == LEVEL_DOOR)
    p_ret = ptl;
  else invalid_pos (&p_ret);

  if (is_valid_pos (&p_ret)) {
    if (p) *p = p_ret;
    return true;
  }

  return false;
}

bool
is_in_front_open_level_door (struct frame *f, struct pos *p)
{
  struct pos p_ret;

  if (is_in_front_level_door (f, &p_ret)) {
    struct level_door *d =
      level_door_at_pos (&p_ret);
    if (d->action != CLOSE_LEVEL_DOOR
        && d->i <= LEVEL_DOOR_OPEN_MAX_STEP) {
      if (p) *p = p_ret;
      return true;
    }
  }

  return false;
}

bool
is_falling (struct frame *f, coord_f cf, int dx0, int dx1)
{
  /* if (! f->b) return false; */
  struct pos p0, p1;
  if (dx0 != dx1) surveyo (cf, dx0, +0, pos, f, NULL, &p0, NULL);
  surveyo (cf, dx1, +0, pos, f, NULL, &p1, NULL);
  return (dx0 == dx1 || is_immediately_accessible_pos (&p1, &p0, f))
    && is_traversable (&p1);
}





void
update_depressible_floor (struct actor *a, int dx0, int dx1)
{
  if (cutscene) return;

  struct coord c0, c1;
  struct pos p0, p1;

  int dir = (a->f.dir == LEFT) ? -1 : 1;

  _bf (&a->f, &c0);
  c0.x += dir * dx0;
  pos (&c0, &p0);

  _bf (&a->f, &c1);
  c1.x += dir * dx1;
  pos (&c1, &p1);

  press_depressible_floor (&p0, a);
  a->df_pos[0] = p0;

  press_depressible_floor (&p1, a);
  a->df_pos[1] = p1;
}

void
keep_depressible_floor (struct actor *a)
{
  if (is_kid_hang_or_climb (a)) {
    struct pos hanged_pos;
    clear_depressible_floor (a);
    get_hanged_pos (&a->hang_pos, a->f.dir, &hanged_pos);
    press_depressible_floor (&hanged_pos, a);
  } else {
    struct pos *p0 = &a->df_pos[0];
    struct pos *p1 = &a->df_pos[1];

    press_depressible_floor (p0, a);
    press_depressible_floor (p1, a);
  }
}

void
clear_depressible_floor (struct actor *a)
{
  invalid_pos (&a->df_pos[0]);
  invalid_pos (&a->df_pos[1]);
}

void
save_depressible_floor (struct actor *a)
{
  a->df_posb[0] = a->df_pos[0];
  a->df_posb[1] = a->df_pos[1];
}

void
restore_depressible_floor (struct actor *a)
{
  a->df_pos[0] = a->df_posb[0];
  a->df_pos[1] = a->df_posb[1];
}

void
press_depressible_floor (struct pos *p, struct actor *a)
{
  if (cutscene || ! is_valid_pos (p)) return;

  switch (fg (p)) {
  case OPENER_FLOOR: press_opener_floor (p, a); break;
  case CLOSER_FLOOR: press_closer_floor (p, a); break;
  case LOOSE_FLOOR: release_loose_floor (p, a); break;
  case HIDDEN_FLOOR: unhide_hidden_floor (p); break;
  default: break;
  }
}

void
unhide_hidden_floor (struct pos *p)
{
  if (fg (p) != HIDDEN_FLOOR) return;
  else {
    register_tile_undo
      (&undo, p,
       FLOOR, MIGNORE, MIGNORE, MIGNORE,
       NULL, false, "UNHIDE FLOOR");
  }
}

void
keep_all_depressible_floors (void)
{
  size_t i;
  for (i = 0; i < actor_nmemb; i++)
    keep_depressible_floor (&actor[i]);
}





void
activate_tile (struct pos *p)
{
  struct door *d;
  struct closer_floor *c;
  struct opener_floor *o;

  if (! p || ! is_valid_pos (p)) return;

  switch (fg (p)) {
  case SPIKES_FLOOR:
    spikes_floor_at_pos (p)->activate = true; break;
  case DOOR:
    d = door_at_pos (p);
    d->action = (d->i) ? OPEN_DOOR : CLOSE_DOOR;
    break;
  case OPENER_FLOOR:
    o = opener_floor_at_pos (p);
    o->noise = true;
    press_opener_floor (p, NULL); break;
  case CLOSER_FLOOR:
    c = closer_floor_at_pos (p);
    c->noise = true;
    press_closer_floor (p, NULL); break;
  case HIDDEN_FLOOR: unhide_hidden_floor (p); break;
  case LOOSE_FLOOR: release_loose_floor (p, NULL); break;
  case CHOMPER: chomper_at_pos (p)->activate = true; break;
  case LEVEL_DOOR: level_door_at_pos (p)->action = OPEN_LEVEL_DOOR;
    break;
  default: break;
  }
}
