/*
  door.c -- door module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

struct door *door = NULL;
size_t door_nmemb = 0;

struct door *
init_door (struct pos *p, struct door *d)
{
  npos (p, &d->p);
  d->i = ext (p);
  d->action = NO_DOOR_ACTION;
  d->wait = DOOR_WAIT;
  d->noise = false;
  d->priority = 0;
  return d;
}

void
register_door (struct pos *p)
{
  struct door d;

  init_door (p, &d);

  door =
    add_to_array (&d, 1, door, &door_nmemb, door_nmemb, sizeof (d));

  qsort (door, door_nmemb, sizeof (d), compare_doors);
}

int
compare_doors (const void *d0, const void *d1)
{
  return cpos (&((struct door *) d0)->p, &((struct door *) d1)->p);
}

struct door *
copy_door (struct door *to, struct door *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct door *
door_at_pos (struct pos *p)
{
  struct door d;
  d.p = *p;

  struct door *dd;

 search:
  dd = bsearch (&d, door, door_nmemb, sizeof (d), compare_doors);

  if (dd && fg (p) != DOOR) {
    remove_door (dd);
    return NULL;
  } else if (! dd && fg (p) == DOOR) {
    register_door (p);
    goto search;
  }

  return dd;
}

void
remove_door (struct door *d)
{
  size_t i =  d - door;
  door =
    remove_from_array (door, &door_nmemb, i, 1, sizeof (*d));
}

void
compute_doors (void)
{
  size_t i;

  for (i = 0; i < door_nmemb;) {
    struct door *d = &door[i];
    if (fg (&d->p) == DOOR) {
      i++; continue;
    }
    remove_door (d);
  }

  for (i = 0; i < door_nmemb; i++) {
    struct door *d = &door[i];
    switch (d->action) {
    case OPEN_DOOR:
    case STAY_OPEN_DOOR:
      if (d->i == 0 && d->wait == 0
          && d->action != STAY_OPEN_DOOR)
        d->action = CLOSE_DOOR;
      else if (d->i == 0 && d->wait > 0) {
        if (! d->noise) {
          play_audio (&door_end_audio, &d->p, -1);
          d->noise = true;
        }

        d->wait--;
      }
      else if (d->i > 0) {
        if (d->i % 2 == 0) {
          if (d->i == DOOR_STEPS - 2) alert_guards (&d->p);
          play_audio (&door_open_audio, &d->p, -1);
        }
        d->i--;
        d->wait = DOOR_WAIT;
        register_changed_pos (&d->p);
      }
      break;
    case CLOSE_DOOR:
      if (d->i < DOOR_STEPS - 1) {
        if (d->wait++ % 4 == 0) {
          if (d->i == 0) alert_guards (&d->p);
          play_audio (&door_close_audio, &d->p, -1);
          d->i++;
          d->noise = false;
          register_changed_pos (&d->p);
        }
      } else if (d->i == DOOR_STEPS - 1) {
        play_audio (&door_end_audio, &d->p, -1);
        d->action = NO_DOOR_ACTION;
        d->wait = DOOR_WAIT;
        d->noise = false;
      }
      break;
    case ABRUPTLY_CLOSE_DOOR:
      if (d->i < DOOR_STEPS - 1) {
        int r = 11 - (d->i % 12);
        d->i += r ? r : 12;
        register_changed_pos (&d->p);
        if (d->i >= DOOR_STEPS - 1) {
          d->i = DOOR_STEPS - 1;
          alert_guards (&d->p);
          play_audio (&door_abruptly_close_audio, &d->p, -1);
        }
      } else {
        d->action = NO_DOOR_ACTION;
        d->wait = DOOR_WAIT;
        d->noise = false;
      }
      break;
    default:
      break;
    }
  }
}

void
open_door (struct level *l, int e, uint64_t priority, bool stay_open)
{
  struct pos *p;

  struct door *d;
  struct level_door *ld;

  do {
    p = &event (l, e)->p;
    switch (fg (p)) {
    case DOOR:
      d = door_at_pos (p);
      if (! d) continue;
      if (d->action == STAY_OPEN_DOOR
          || (d->action == NO_DOOR_ACTION && d->i == 0)
          || d->action == ABRUPTLY_CLOSE_DOOR)
          continue;
      if (d->priority <= priority) {
        d->action = stay_open ? STAY_OPEN_DOOR : OPEN_DOOR;
        d->wait = DOOR_WAIT;
        d->priority = priority;
      }
      break;
    case LEVEL_DOOR:
      ld = level_door_at_pos (p);
      if (! ld) continue;
      if (ld->priority <= priority) {
        ld->action = OPEN_LEVEL_DOOR;
        ld->priority = priority;
      }
      break;
    default:
      activate_tile (p);
      break;
    }
  } while (event (l, e++)->next
           && e < l->event_nmemb);
}

void
close_door (struct level *l, int e, uint64_t priority)
{
  struct pos *p;

  struct door *d;
  struct level_door *ld;

  do {
    p = &event (l, e)->p;
    switch (fg (p)) {
    case DOOR:
      d = door_at_pos (p);
      if (! d) continue;
      if (d->priority <= priority) {
        d->action = ABRUPTLY_CLOSE_DOOR;
        d->priority = priority;
      }
      break;
    case LEVEL_DOOR:
      ld = level_door_at_pos (p);
      if (! ld) continue;
      if (ld->priority <= priority) {
        ld->action = semantics == LEGACY_SEMANTICS
          ? OPEN_LEVEL_DOOR : CLOSE_LEVEL_DOOR;
        ld->priority = priority;
      }
      break;
    default:
      activate_tile (p);
      break;
    }
  } while (event (l, e++)->next
           && e < l->event_nmemb);
}

int
door_grid_tip_y (struct pos *p)
{
  struct rect r;
  rect_object_index_part (&r, "DOOR", door_at_pos (p)->i + 1, "TIP", p);
  return r.c.y;
}

int
fake_door_step (struct pos *p)
{
  return is_fake (p) ? fake_ext (p) : door_at_pos (p)->i;
}

void
draw_door (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  draw_object_part (bitmap, "DOOR", "MAIN", p);
  draw_object_part (bitmap, "DOOR", fake_door_step (p) + 1, p);
}

void
draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  draw_object_part (bitmap, "DOOR", "FRONT", p);

  push_drawn_rectangle (bitmap);
  enum should_draw s = should_draw_face (p, f, RIGHT);
  int w = s == SHOULD_DRAW_PART ? DOOR_FG_WIDTH : FULL_WIDTH;

  if (s != SHOULD_DRAW_NONE
      && (s != SHOULD_DRAW_FULL
          || should_draw_door_grid (p, f)))
    draw_object_part_width (bitmap, "DOOR", fake_door_step (p) + 1, p, w);

  redraw_drawn_rectangle (pop_drawn_rectangle (), p);
}

bool
should_draw_door_grid (struct pos *p, struct frame *f)
{
  if (! f) return false;

  struct pos pl, pa, pmt, ptb;

  struct actor *a = get_actor_by_id (f->parent_id);
  struct pos *hang_pos = &a->hang_pos;

  survey (_mt, pos, f, NULL, &pmt, NULL);
  survey (_tb, pos, f, NULL, &ptb, NULL);

  prel (p, &pl, +0, -1);
  prel (p, &pa, -1, +0);

  return
    (((peq (&ptb, p) || peq (&ptb, &pl)
       || peq (&ptb, &pa))
      && (peq (&pmt, p) || peq (&pmt, &pl)
          || peq (&pmt, &pa)))
     || (a && is_kid_successfully_climbing_at_pos
         (a, hang_pos, p))
     || (a && a->action == kid_turn_run
         && f->dir == LEFT))
    && ! (a && is_kid_successfully_climbing_at_pos
          (a, hang_pos, &pa));
}
