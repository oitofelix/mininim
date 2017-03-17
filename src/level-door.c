/*
  level-door.c -- level door module;

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

struct level_door *level_door = NULL;
size_t level_door_nmemb = 0;

struct level_door *
init_level_door (struct pos *p, struct level_door *d)
{
  int n, f;
  typed_int (ext (p), LEVEL_DOOR_STEPS, LEVEL_DOOR_FASES, &n, &f);

  npos (p, &d->p);
  d->broken = f;
  d->i = n;
  d->action = NO_LEVEL_DOOR_ACTION;
  d->priority = 0;

  if (semantics == LEGACY_SEMANTICS) {
    struct pos q;
    if (first_level_door_in_room_pos (p->l->start_pos.room, &q)
        && peq (&q, p))
      d->no_stairs = true;
    else d->no_stairs = false;
  } else d->no_stairs = peq (p, &p->l->start_pos);

  return d;
}

void
register_level_door (struct pos *p)
{
  struct level_door d;

  init_level_door (p, &d);

  level_door =
    add_to_array (&d, 1, level_door, &level_door_nmemb, level_door_nmemb, sizeof (d));

  qsort (level_door, level_door_nmemb, sizeof (d), compare_level_doors);
}

int
compare_level_doors (const void *d0, const void *d1)
{
  return cpos (&((struct level_door *) d0)->p, &((struct level_door *) d1)->p);
}

struct level_door *
copy_level_door (struct level_door *to, struct level_door *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct level_door *
level_door_at_pos (struct pos *p)
{
  struct level_door d;
  d.p = *p;

  struct level_door *dd;

 search:
  dd = bsearch (&d, level_door, level_door_nmemb, sizeof (d),
                compare_level_doors);

  if (dd && fg (p) != LEVEL_DOOR) {
    remove_level_door (dd);
    return NULL;
  } else if (! dd && fg (p) == LEVEL_DOOR) {
    register_level_door (p);
    goto search;
  }

  return dd;
}

void
remove_level_door (struct level_door *d)
{
  size_t i =  d - level_door;
  level_door =
    remove_from_array (level_door, &level_door_nmemb, i, 1, sizeof (*d));
}

int
fake_level_door_step (struct pos *p)
{
  return is_fake (p) ? fake_ext (p) : level_door_at_pos (p)->i;
}

bool
fake_level_door_stairs (struct pos *p)
{
  return is_fake (p) ? abs (ext (p)) % 2 : ! level_door_at_pos (p)->no_stairs;
}

bool
fake_level_door_broken (struct pos *p)
{
  return is_fake (p) ? abs (ext (p)) % 2 : level_door_at_pos (p)->broken;
}

void
break_level_door (struct pos *p)
{
  struct level_door *d = level_door_at_pos (p);
  if (! d) return;
  register_tile_undo
    (&undo, p,
     MIGNORE, MIGNORE, d->i + LEVEL_DOOR_STEPS, MIGNORE,
     NULL, false, "LOOSE FLOOR BREAKING");
  d->broken = true;
}

void
compute_level_doors (void)
{
  size_t i;

  for (i = 0; i < level_door_nmemb;) {
    struct level_door *d = &level_door[i];
    if (fg (&d->p) == LEVEL_DOOR) {
      i++; continue;
    }
    remove_level_door (d);
  }

  for (i = 0; i < level_door_nmemb; i++) {
    struct level_door *d = &level_door[i];
    switch (d->action) {
    case OPEN_LEVEL_DOOR:
      if (d->i % 5 == 2 && d->i > 2) {
        if (d->i == LEVEL_DOOR_STEPS - 2) alert_guards (&d->p);
        play_audio (&level_door_open_audio, &d->p, -1);
      }
      if (d->i > 0) {
        d->i--;
        register_changed_pos (&d->p);
      } else d->action = NO_LEVEL_DOOR_ACTION;
      break;
    case CLOSE_LEVEL_DOOR:
      if (d->i == 0) {
        alert_guards (&d->p);
        play_audio (&level_door_open_audio, &d->p, -1);
      }
      if (d->i < LEVEL_DOOR_STEPS - 1) {
        int r = 14 - (d->i % 15);
        d->i += r ? r : 15;
        register_changed_pos (&d->p);
        if (d->i >= LEVEL_DOOR_STEPS - 1) {
          d->i = LEVEL_DOOR_STEPS - 1;
          d->action = NO_LEVEL_DOOR_ACTION;
          play_audio (&level_door_close_audio, &d->p, -1);
        }
      }
      break;
    default:
      break;
    }
  }
}

struct level_door *
get_exit_level_door (struct level *l, int n)
{
  int i;
  for (i = 0; i < level_door_nmemb; i++) {
    struct level_door *d = &level_door[i];
    if (! peq (&d->p, &l->start_pos)
        && d->i == 0
        && n-- == 0) return d;
  }
  return NULL;
}

struct pos *
first_level_door_in_room_pos (int room, struct pos *p_ret)
{
  struct pos p; new_pos (&p, &global_level, room, -1, -1);
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      if (fg (&p) == LEVEL_DOOR) {
        *p_ret = p;
        return p_ret;
      }
  return NULL;
}

void
draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_object_part (bitmap, "LEVEL_DOOR", fake_level_door_stairs (p)
                    ? "ENTER" : "EXIT", p);

  draw_object_part (bitmap, "LEVEL_DOOR", fake_level_door_step (p) + 1,
                    p);
}

void
draw_level_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct actor *a)
{
  if (a && a->action != kid_stairs) return;

  push_drawn_rectangle (bitmap);

  draw_object_part (bitmap, "LEVEL_DOOR", "FRONT", p);

  draw_object_part (bitmap, "LEVEL_DOOR", fake_level_door_step (p) + 1,
                    p);

  redraw_drawn_rectangle (pop_drawn_rectangle (), p);
}

void
draw_level_door_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_object (bitmap, fake_level_door_broken (p)
               ? "BROKEN_FLOOR" : "FLOOR", p);
}
