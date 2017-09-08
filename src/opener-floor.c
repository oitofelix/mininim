/*
  opener-floor.c -- opener floor module;

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

struct opener_floor *opener_floor = NULL;
size_t opener_floor_nmemb = 0;

struct opener_floor *
init_opener_floor (struct pos *p, struct opener_floor *o)
{
  memset (o, 0, sizeof (*o));

  int n;
  typed_int (ext (p), p->l->event_nmemb, 1, &n, NULL);

  npos (p, &o->p);
  o->event = n;

  return o;
}

void
register_opener_floor (struct pos *p)
{
  struct opener_floor o;

  init_opener_floor (p, &o);

  opener_floor =
    add_to_array (&o, 1, opener_floor, &opener_floor_nmemb,
                  opener_floor_nmemb, sizeof (o));

  qsort (opener_floor, opener_floor_nmemb, sizeof (o),
         compare_opener_floors);
}

int
compare_opener_floors (const void *o0, const void *o1)
{
  return cpos (&((struct opener_floor *) o0)->p,
               &((struct opener_floor *) o1)->p);
}

struct opener_floor *
copy_opener_floor (struct opener_floor *to, struct opener_floor *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct opener_floor *
opener_floor_at_pos (struct pos *p)
{
  struct opener_floor o;
  o.p = *p;

  struct opener_floor *oo;

 search:
  oo = bsearch (&o, opener_floor, opener_floor_nmemb, sizeof (o),
                compare_opener_floors);

  if (oo && fg (p) != OPENER_FLOOR) {
    remove_opener_floor (oo);
    return NULL;
  } else if (! oo && fg (p) == OPENER_FLOOR) {
    register_opener_floor (p);
    goto search;
  }

  return oo;
}

struct opener_floor *
opener_floor_by_event (struct pos *p, int event, int dir)
{
  struct opener_floor *o;
  if (p) o = opener_floor_at_pos (p);
  else o = &opener_floor[0];

  if (! o) {
    o = &opener_floor[0];
    p = NULL;
  }

  int i;

  if (dir < 0)
    for (i = o - opener_floor - (p ? 1 : 0); i >= 0; i--) {
      if (opener_floor[i].event == event) return &opener_floor[i];
    }
  else
    for (i = o - opener_floor + (p ? 1 : 0);
         i < opener_floor_nmemb; i++) {
      if (opener_floor[i].event == event) return &opener_floor[i];
    }

  return NULL;
}

void
remove_opener_floor (struct opener_floor *o)
{
  size_t i =  o - opener_floor;
  opener_floor =
    remove_from_array (opener_floor, &opener_floor_nmemb, i, 1, sizeof (*o));
}

void
press_opener_floor (struct pos *p, struct actor *a)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  o->pressed = true;

  if (! o->prev_pressed) {
    kid_haptic (a, KID_HAPTIC_COLLISION);
    register_changed_pos (&o->p);
    o->prev_pressed = true;
    o->priority = anim_cycle;
  }
}

void
break_opener_floor (struct pos *p)
{
  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;
  open_door (o->p.l, o->event, anim_cycle, true);
  register_tile_undo
    (&undo, p,
     BROKEN_FLOOR, MIGNORE, NO_ITEM, MIGNORE,
     NULL, false, "LOOSE FLOOR BREAKING");
}

void
unpress_opener_floors (void)
{
  size_t i;
  for (i = 0; i < opener_floor_nmemb; i++) {
    opener_floor[i].prev_pressed =
      opener_floor[i].pressed;
    opener_floor[i].pressed = false;
  }
}

void
register_changed_opener_floors (void)
{
  size_t i;
  for (i = 0; i < opener_floor_nmemb; i++) {
    struct opener_floor *o = &opener_floor[i];
    if (o->prev_pressed != o->pressed)
      register_changed_pos (&o->p);
  }
}

void
compute_opener_floors (void)
{
  size_t i;

  for (i = 0; i < opener_floor_nmemb;) {
    struct opener_floor *o = &opener_floor[i];
    if (fg (&o->p) == OPENER_FLOOR) {
      i++; continue;
    }
    remove_opener_floor (o);
  }

  for (i = 0; i < opener_floor_nmemb; i++) {
    struct opener_floor *o = &opener_floor[i];
    if (o->pressed) {
      if (! o->noise) {
        alert_guards (&o->p);
        play_audio (&opener_floor_audio, &o->p, -1);
        o->noise = true;
      }
      open_door (o->p.l, o->event, o->priority, false);
    } else o->noise = false;
  }
}

void
draw_opener_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (is_fake (p)) {
    draw_object (bitmap, "OPENER_FLOOR", p);
    return;
  }

  struct opener_floor *o = opener_floor_at_pos (p);
  if (! o) return;

  if (o->pressed) draw_object (bitmap, "FLOOR", p);
  else draw_object (bitmap, "OPENER_FLOOR", p);
}
