/*
  chomper.c -- chomper module;

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

struct chomper *chomper = NULL;
size_t chomper_nmemb = 0;

static void draw_blade (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        struct chomper *ch, int i, lua_Number w);

struct chomper *
init_chomper (struct pos *p, struct chomper *c)
{
  npos (p, &c->p);

  int n, f;
  typed_int (ext (p), CHOMPER_STEPS, CHOMPER_FASES, &n, &f);

  c->i = n % (CHOMPER_STEPS - 1);
  c->blood = f;
  c->inactive = (n != 0);

  c->wait = CHOMPER_WAIT;
  c->alert = false;
  c->activate = false;

  return c;
}

void
register_chomper (struct pos *p)
{
  struct chomper c;

  init_chomper (p, &c);

  chomper =
    add_to_array (&c, 1, chomper, &chomper_nmemb, chomper_nmemb, sizeof (c));

  qsort (chomper, chomper_nmemb, sizeof (c), compare_chompers);
}

int
compare_chompers (const void *c0, const void *c1)
{
  return cpos (&((struct chomper *) c0)->p, &((struct chomper *) c1)->p);
}

struct chomper *
copy_chomper (struct chomper *to, struct chomper *from)
{
  struct pos p = to->p;
  *to = *from;
  to->p = p;
  return to;
}

struct chomper *
chomper_at_pos (struct pos *p)
{
  struct chomper c;
  c.p = *p;

  struct chomper *cc;

 search:
  cc = bsearch (&c, chomper, chomper_nmemb, sizeof (c),
                compare_chompers);

  if (cc && fg (p) != CHOMPER) {
    remove_chomper (cc);
    return NULL;
  } else if (! cc && fg (p) == CHOMPER) {
    register_chomper (p);
    goto search;
  }

  return cc;
}

void
remove_chomper (struct chomper *c)
{
  size_t i =  c - chomper;
  chomper =
    remove_from_array (chomper, &chomper_nmemb, i, 1, sizeof (*c));
}

bool
should_chomp (struct pos *p)
{
  int i;

  for (i = 0; i < actor_nmemb; i++) {
    struct actor *a = &actor[i];
    if (a->type != KID
        || (is_actor_dead (a) && a->id != 0)) continue;
    if (is_pos_seeing (p, a, LEFT) || is_pos_seeing (p, a, RIGHT))
      return true;
  }

  return false;
}

void
compute_chompers (void)
{
  size_t i, j;

  for (i = 0; i < chomper_nmemb;) {
    struct chomper *c = &chomper[i];
    if (fg (&c->p) == CHOMPER) {
      i++; continue;
    }
    remove_chomper (c);
  }

  for (i = 0; i < chomper_nmemb; i++) {
    struct chomper *c = &chomper[i];

    if (c->inactive) continue;

    switch (c->i) {
    case 0:
      if (! c->alert) c->alert = ! should_chomp (&c->p);
      if ((c->wait-- <= 0 && should_chomp (&c->p)
           && (anim_cycle % CHOMPER_WAIT) ==
           prandom_pos (&c->p, CHOMPER_WAIT - 1))
          || c->activate) {
        c->i++;
        register_changed_pos (&c->p);
      }
      break;
    case 1:
      c->i++;
      if (c->alert) {
        alert_guards (&c->p);
        c->alert = false;
      }
      play_audio (&chomper_audio, &c->p, -1);
      kid_haptic_for_range (&c->p, _ml, 3 * PLACE_WIDTH, KID_HAPTIC_CHOMPER);
      register_changed_pos (&c->p);
      break;
    case 2:
      c->i++;
      register_changed_pos (&c->p);
      break;
    case 3:
      c->i++;
      register_changed_pos (&c->p);
      break;
    case 4:
      c->i = 0; c->wait = CHOMPER_WAIT; c->activate = false;
      register_changed_pos (&c->p);
      break;
    }

    if (c->i != 1 && c->i != 2 ) continue;

    /* chomp kid */
    for (j = 0; j < actor_nmemb; j++) {
      struct actor *a = &actor[j];
      struct pos pl; prel (&c->p, &pl, +0, -1);
      if (a->type == MOUSE
          || is_actor_fall (a)
          || a->immortal
          || a->chomper_immune
          || is_collidable_at_right (&pl, &a->f))
        continue;
      struct pos pbf, pbb;
      surveyo (_bf, -2, +0, pos, &a->f, NULL, &pbf, NULL);
      surveyo (_bb, +0, +0, pos, &a->f, NULL, &pbb, NULL);
      pos2room (&pbf, c->p.room, &pbf);
      pos2room (&pbb, c->p.room, &pbb);
      if (a->ignore_danger != 1
          && (((pbf.room == c->p.room
             && pbf.floor == c->p.floor)
            || (pbb.room == c->p.room
                && pbb.floor == c->p.floor))
           && ((a->f.dir == LEFT && pbf.place < c->p.place
                && pbb.place >= c->p.place)
               || (a->f.dir == RIGHT && pbf.place >= c->p.place
                   && pbb.place < c->p.place)))
          && (! is_actor_dead (a) || a->action != kid_die_chomped)) {
        if (a->type != SKELETON) c->blood = true;
        a->splash = true;
        a->p = c->p;
        a->death_reason = CHOMPER_DEATH;
        if (a->type == SKELETON)
          play_audio (&skeleton_audio, &c->p, -1);
        else play_audio (&chomped_audio, &c->p, -1);
        a->next_action = actor_die_chomped;
      }
    }
  }
}

void
draw_blade (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chomper *ch,
            int i, lua_Number w)
{
  draw_object_index_part_width (bitmap, "CHOMPER", i, "MAIN", p, w);

  struct rect rl; rect_object_index_part (&rl, "CHOMPER", i, "MAIN", p);
  struct rect rb; rect_object_index_part (&rb, "CHOMPER", i, "BLOOD", p);

  double bw = w - fabs (rb.c.x - rl.c.x);

  if (ch && ch->blood)
    draw_object_index_part_width (bitmap, "CHOMPER", i, "BLOOD", p, bw);
}

void
draw_chomper (ALLEGRO_BITMAP *bitmap, struct pos *p)
{
  if (is_fake (p)) {
    draw_blade (bitmap, p, NULL, 3, FULL_WIDTH);
    /* draw_blade (bitmap, p, NULL, abs (ext (p)) % 5 + 1, FULL_WIDTH); */
    return;
  }

  struct chomper *ch = chomper_at_pos (p);
  if (! ch) return;

  draw_blade (bitmap, p, ch, ch->i + 1, FULL_WIDTH);
}

/* TODO: query video mode for CHOMPER_FG_WIDTH */

void
draw_chomper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f)
{
  switch (should_draw_face (p, f, LEFT)) {
  case SHOULD_DRAW_FULL: {
    struct chomper *ch = chomper_at_pos (p);
    if (! ch) return;
    draw_blade (bitmap, p, ch, ch->i + 1, CHOMPER_FG_WIDTH + 3);
    break;
  }
  case SHOULD_DRAW_PART:
    draw_blade (bitmap, p, NULL, abs (ext (p)) % 5 + 1,
                CHOMPER_FG_WIDTH);
    break;
  case SHOULD_DRAW_NONE: default: {
    struct chomper *ch = chomper_at_pos (p);
    if (! ch) return;
    draw_blade (bitmap, p, ch, ch->i + 1, CHOMPER_FG_WIDTH);
    break;
  }
  }
}
