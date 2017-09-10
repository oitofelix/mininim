/*
  closer-floor.c -- closer floor module;

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

struct closer_floor *closer_floor = NULL;
size_t closer_floor_nmemb = 0;

struct closer_floor *
init_closer_floor (struct pos *p, struct closer_floor *c)
{
  memset (c, 0, sizeof (*c));

  int n;
  typed_int (ext (p), p->l->event_nmemb, 1, &n, NULL);

  npos (p, &c->p);
  c->event = n;

  return c;
}


void
register_closer_floor (struct pos *p)
{
  struct closer_floor c;

  init_closer_floor (p, &c);

  closer_floor =
    add_to_array (&c, 1, closer_floor, &closer_floor_nmemb,
                  closer_floor_nmemb, sizeof (c));

  qsort (closer_floor, closer_floor_nmemb, sizeof (c),
         compare_closer_floors);
}

int
compare_closer_floors (const void *c0, const void *c1)
{
  return cpos (&((struct closer_floor *) c0)->p,
               &((struct closer_floor *) c1)->p);
}

struct closer_floor *
copy_closer_floor (struct closer_floor *to, struct closer_floor *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct closer_floor *
closer_floor_at_pos (struct pos *p)
{
  struct closer_floor c;
  c.p = *p;

  struct closer_floor *cc;

 search:
  cc = bsearch (&c, closer_floor, closer_floor_nmemb, sizeof (c),
                compare_closer_floors);

  if (cc && fg (p) != CLOSER_FLOOR) {
    remove_closer_floor (cc);
    return NULL;
  } else if (! cc && fg (p) == CLOSER_FLOOR) {
    register_closer_floor (p);
    goto search;
  }

  return cc;
}

struct closer_floor *
closer_floor_by_event (struct pos *p, int event, int dir)
{
  struct closer_floor *c;
  if (p) c = closer_floor_at_pos (p);

  if (! p || ! c) {
    c = &closer_floor[dir < 0 ? closer_floor_nmemb - 1 : 0];
    p = NULL;
  }

  if (dir < 0) for (size_t i = (c - closer_floor) - (p ? 1 : 0); i >= 0; i--) {
      if (closer_floor[i].event == event) return &closer_floor[i];
    } else for (size_t i = (c - closer_floor) + (p ? 1 : 0);
                i < closer_floor_nmemb; i++) {
      if (closer_floor[i].event == event) return &closer_floor[i];
    }

  return NULL;
}

void
remove_closer_floor (struct closer_floor *c)
{
  size_t i =  c - closer_floor;
  closer_floor =
    remove_from_array (closer_floor, &closer_floor_nmemb, i, 1, sizeof (*c));
}

void
press_closer_floor (struct pos *p, struct actor *a)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;
  c->pressed = true;

  if (! c->prev_pressed) {
    kid_haptic (a, KID_HAPTIC_COLLISION);
    register_changed_pos (&c->p);
    c->prev_pressed = true;
    c->priority = anim_cycle;
  }
}

void
break_closer_floor (struct pos *p)
{
  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;
  close_door (c->p.l, c->event, anim_cycle);
  register_tile_undo
    (&undo, p,
     BROKEN_FLOOR, MIGNORE, NO_ITEM, MIGNORE,
     NULL, false, "LOOSE FLOOR BREAKING");
}

void
unpress_closer_floors (void)
{
  size_t i;
  for (i = 0; i < closer_floor_nmemb; i++) {
    closer_floor[i].prev_pressed =
      closer_floor[i].pressed;
    closer_floor[i].pressed = false;
  }
}

void
register_changed_closer_floors (void)
{
  size_t i;
  for (i = 0; i < closer_floor_nmemb; i++) {
    struct closer_floor *c = &closer_floor[i];
    if (c->prev_pressed != c->pressed)
      register_changed_pos (&c->p);
  }
}

void
compute_closer_floors (void)
{
  size_t i;

  for (i = 0; i < closer_floor_nmemb;) {
    struct closer_floor *c = &closer_floor[i];
    if (fg (&c->p) == CLOSER_FLOOR) {
      i++; continue;
    }
    remove_closer_floor (c);
  }

  for (i = 0; i < closer_floor_nmemb; i++) {
    struct closer_floor *c = &closer_floor[i];
    if (c->pressed) {
      if (! c->noise) {
        alert_guards (&c->p);
        play_audio (&closer_floor_audio, &c->p, -1);
        c->noise = true;
      }
      close_door (c->p.l, c->event, c->priority);
    } else c->noise = false;
  }
}

void
draw_closer_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (is_fake (p)) {
    draw_object_part (bitmap, "CLOSER_FLOOR", "UNPRESSED", p);
    return;
  }

  struct closer_floor *c = closer_floor_at_pos (p);
  if (! c) return;

  if (c->pressed) {
    struct pos pl; prel (p, &pl, +0, +1);
    draw_object_part (bitmap, "CLOSER_FLOOR", "PRESSED", p);
  } else draw_object_part (bitmap, "CLOSER_FLOOR", "UNPRESSED", p);
}
