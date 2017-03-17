/*
  spikes-floor.c -- spikes floor module;

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

struct spikes_floor *spikes_floor = NULL;
size_t spikes_floor_nmemb = 0;

struct spikes_floor *
init_spikes_floor (struct pos *p, struct spikes_floor *s)
{
  npos (p, &s->p);
  s->wait = SPIKES_WAIT;
  s->activate = false;

  switch (ext (p)) {
  case 0: s->i = 0; s->state = 0; break;
  case 1: s->i = 1; s->state = 1; break;
  case 2: s->i = 2; s->state = 2; break;
  case 3: s->i = 3; s->state = 3; break;
  case 4: s->i = 4; s->state = 4; break;
  case 5: s->i = 4; s->state = 5; break;
  case 6: s->i = 5; s->state = 3; break;
  case 7: s->i = 6; s->state = 2; break;
  case 8: s->i = 0; s->state = 1; break;
  case 9: s->i = 0; s->state = 0; break;
  }

  s->inactive = (ext (p) != 0);

  return s;
}

void
register_spikes_floor (struct pos *p)
{
  struct spikes_floor s;

  init_spikes_floor (p, &s);

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
copy_spikes_floor (struct spikes_floor *to, struct spikes_floor *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  if (from->inactive && get_actor_dead_at_pos (&from->p))
    to->inactive = false;
  return to;
}

struct spikes_floor *
spikes_floor_at_pos (struct pos *p)
{
  struct spikes_floor s;
  s.p = *p;

  struct spikes_floor *ss;

 search:
  ss = bsearch (&s, spikes_floor, spikes_floor_nmemb, sizeof (s),
                compare_spikes_floors);

  if (ss && fg (p) != SPIKES_FLOOR) {
    remove_spikes_floor (ss);
    return NULL;
  } else if (! ss && fg (p) == SPIKES_FLOOR) {
    register_spikes_floor (p);
    goto search;
  }

  return ss;
}

void
remove_spikes_floor (struct spikes_floor *s)
{
  size_t i =  s - spikes_floor;
  spikes_floor =
    remove_from_array (spikes_floor, &spikes_floor_nmemb, i, 1, sizeof (*s));
}

void
break_spikes_floor (struct pos *p)
{
  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (! s) return;
  struct actor *a = get_actor_dead_at_pos (p);
  if (a) actor_die_suddenly (a);
  remove_spikes_floor (s);
}

void
compute_spikes_floors (void)
{
  size_t i, j;

  for (i = 0; i < spikes_floor_nmemb;) {
    struct spikes_floor *s = &spikes_floor[i];
    if (fg (&s->p) == SPIKES_FLOOR) {
      i++; continue;
    }
    remove_spikes_floor (s);
  }

  for (i = 0; i < spikes_floor_nmemb; i++) {
    struct spikes_floor *s = &spikes_floor[i];

    if (! s->inactive) {
      int state = s->state;

      switch (s->i) {
      case 0: if (should_spikes_raise (&s->p) || s->activate) {
          alert_guards (&s->p);
          kid_haptic_for_range (&s->p, _m, 3 * PLACE_WIDTH,
                                KID_HAPTIC_SPIKES);
          play_audio (&spikes_audio, &s->p, -1);
          s->i++;
          s->wait = 12;
          s->state = 1;
        } else if (s->state != 0) s->state = 0;
        break;
      case 1:
        s->i++;
        s->state = 2;
        break;
      case 2:
        s->i++;
        s->state = 3;
        break;
      case 3:
        s->i++;
        s->state = 4;
        break;
      case 4: if (! should_spikes_raise (&s->p)) {
          if (s->wait-- == 0) {
            s->i++;
            s->state = 3;
          } else s->state = 5;
        } else {
          s->state = 5;
        }
        break;
      case 5:
        s->i++;
        s->state = 2;
        break;
      case 6:
        s->i = 0;
        s->state = 1;
        s->activate = false;
        break;
      }

      if (state != s->state)
        register_changed_pos (&s->p);
    }

    /* spike kid */
    for (j = 0; j < actor_nmemb; j++) {
      struct actor *a = &actor[j];
      if (is_kid_dead (a)
          || a->immortal
          || a->spikes_immune
          || ext (&s->p) >= 5
          || get_actor_dead_at_pos (&s->p)) continue;
      struct pos pbf, pmbo;
      surveyo (_bf, -4, +0, pos, &a->f, NULL, &pbf, NULL);
      survey (_mbo, pos, &a->f, NULL, &pmbo, NULL);
      if ((peq (&pmbo, &s->p) && peq (&pbf, &s->p))
          && ((((s->state >= 2 && s->state <= 4)
                || ext (&s->p) > 0)
               && (a->action == kid_run
                   || is_kid_run_jump_running (a)))
              || (a->action == kid_crouch && a->fall && a->i < 3
                  && ! a->float_timer)
              || (a->action == kid_jump && a->i >= 10 && a->i <= 12)
              || is_kid_run_jump_landing (a))) {
        a->p = s->p;
        if (a->action == kid_crouch && a->fall) actor_die_spiked (a);
        else a->next_action = actor_die_spiked;
      }
    }
  }
}

bool
should_spikes_raise_for_pos (struct pos *p, struct pos *pk)
{
  struct pos pb1, pb2, np;

  return peq (pk, p)
    || (peq (pk, prel (p, &pb1, -1, 0))
        && is_traversable (prel (p, &np, -1, +0)))
    || (peq (pk, prel (p, &pb2, -2, 0))
        && is_traversable (prel (p, &np, -1, +0))
        && is_traversable (prel (p, &np, -2, +0)));
}

bool
should_spikes_raise (struct pos *p)
{
  int i;
  struct pos pml, pm, pmr;

  for (i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];
    if (is_actor_dead (a)) continue;
    survey (_ml, pos, &a->f, NULL, &pml, NULL);
    surveyo (_m, -2, +0, pos, &a->f, NULL, &pm, NULL);
    survey (_mr, pos, &a->f, NULL, &pmr, NULL);

    if ((should_spikes_raise_for_pos (p, &pml)
         && ! is_collidable_at_left (&pm, &a->f))
        || should_spikes_raise_for_pos (p, &pm)
        || (should_spikes_raise_for_pos (p, &pmr)
            && ! is_collidable_at_right (&pm, &a->f)))
      return true;
  }

  return false;
}

void
draw_spikes_floor (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (is_fake (p)) {
    int i = abs (ext (p)) % 6;
    draw_object_index_part (bitmap, "SPIKES_FLOOR", i + 1, "MAIN", p);
    return;
  }

  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (s) draw_object_index_part (bitmap, "SPIKES_FLOOR", s->state + 1,
                                 "MAIN", p);
}

void
draw_spikes_floor_fg (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (is_fake (p)) {
    int i = abs (ext (p)) % 6;
    if (i) draw_object_index_part (bitmap, "SPIKES_FLOOR", i + 1,
                                   "FRONT", p);
    return;
  }

  struct spikes_floor *s = spikes_floor_at_pos (p);
  if (s && s->state)
    draw_object_index_part (bitmap, "SPIKES_FLOOR", s->state + 1,
                            "FRONT", p);
}
