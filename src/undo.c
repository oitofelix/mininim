/*
  undo.c -- undo module;

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

#include "mininim.h"

void
register_undo (struct undo *u, void *data, undo_f f, char *desc)
{
  if (! u->pass || ! u->count) u->current = -1;

  size_t i;
  for (i = u->current + 1; i < u->count; i++)
    al_free (u->pass[i].data);

  u->count = u->current + 2;
  u->pass = xrealloc (u->pass, u->count * sizeof (* u->pass));
  u->current++;

  u->pass[u->current].data = data;
  u->pass[u->current].f = f;
  u->pass[u->current].desc = desc;
}

void
free_undo (struct undo *u)
{
  size_t i;
  for (i = 0; i < u->count; i++)
    al_free (u->pass[i].data);

  u->count = 0;
  u->current = -1;
}

bool
can_undo (struct undo *u, int dir)
{
  return ! (u->current == -1 && dir < 0)
    && ! (u->current == u->count - 1 && dir >= 0)
    && u->pass && u->count;
}

bool
undo_pass (struct undo *u, int dir, char **desc)
{
  if (! can_undo (u, dir)) {
    desc = NULL;
    return false;
  }
  if (dir >= 0) u->current++;
  u->pass[u->current].f (u->pass[u->current].data, dir);
  if (desc) *desc = u->pass[u->current].desc;
  if (dir < 0) u->current--;
  return true;
}

void
ui_undo_pass (struct undo *u, int dir, char *prefix)
{
  char *text;
  char *dir_str = (dir >= 0) ? "REDO" : "UNDO";
  static char *undo_msg = NULL;

  bool b = can_undo (u, dir);

  if (undo_msg) al_free (undo_msg);

  if (! b) {
    if (prefix) xasprintf (&undo_msg, "NO FURTHER %s %s", prefix, dir_str);
    else xasprintf (&undo_msg, "NO FURTHER %s", dir_str);
    editor_msg (undo_msg, 24);
    return;
  }

  undo_pass (u, dir, &text);

  if (prefix) xasprintf (&undo_msg, "%s %s: %s", prefix, dir_str, text);
  else xasprintf (&undo_msg, "%s: %s", dir_str, text);
  editor_msg (undo_msg, 24);
}

/*********************/
/* CONSTRUCTION UNDO */
/*********************/

void
register_con_undo (struct undo *u, struct pos *p,
                   enum confg fg, enum conbg bg, int ext,
                   bool should_destroy, bool should_register, bool should_prepare,
                   bool ignore_intermediate, char *desc)
{
  struct con c;

  c.fg = (fg != MIGNORE) ? fg : con (p)->fg;
  c.bg = (bg != MIGNORE) ? bg : con (p)->bg;
  c.ext = (ext != MIGNORE) ? (union conext) ext : con (p)->ext;

  /* if (! memcmp (con (p), &c, sizeof (c))) return; */

  struct con_undo *prev_data = u->count
    ? (struct con_undo *) u->pass[u->current].data
    : NULL;

  enum con_diff cd = con_diff (con (p), &c);

  if (ignore_intermediate
      && prev_data
      && u->pass[u->current].f == (undo_f) con_undo
      && peq (&prev_data->p, p)
      && cd == con_diff (&prev_data->b, &prev_data->f)
      && cd != CON_DIFF_MIXED
      && cd != CON_DIFF_NO_DIFF)
    undo_pass (u, -1, NULL);

  if (! memcmp (con (p), &c, sizeof (c))) return;

  struct con_undo *d = xmalloc (sizeof (struct con_undo));
  d->p = *p;
  d->b = *con (p);
  d->f = c;
  register_undo (u, d, (undo_f) con_undo, desc);

  if (should_destroy) destroy_con_at_pos (p);
  *con (p) = d->f;
  if (should_register) register_con_at_pos (p);
  if (should_prepare) prepare_con_at_pos (p);

  register_changed_pos (p, -1);
}

void
con_undo (struct con_undo *d, int dir)
{
  destroy_con_at_pos (&d->p);
  *con (&d->p) = (dir >= 0) ? d->f : d->b;
  register_con_at_pos (&d->p);
  prepare_con_at_pos (&d->p);
  register_changed_pos (&d->p, -1);
}

/*********************/
/* EXCHANGE POS UNDO */
/*********************/

void
register_exchange_pos_undo (struct undo *u, struct pos *p0, struct pos *p1,
                            bool prepare, bool invert_dir, char *desc)
{
  if (peq (p0, p1)) return;

  struct exchange_pos_undo *d = xmalloc (sizeof (struct exchange_pos_undo));
  d->p0 = *p0;
  d->p1 = *p1;
  d->prepare = prepare;
  d->invert_dir = invert_dir;
  register_undo (u, d, (undo_f) exchange_pos_undo, desc);
  exchange_pos_undo (d, +1);
}

void
exchange_pos_undo (struct exchange_pos_undo *d, int dir)
{
  struct pos *p0, *p1;
  if (dir >=0) {
    p0 = &d->p0;
    p1 = &d->p1;
  } else {
    p0 = &d->p1;
    p1 = &d->p0;
  }
  exchange_pos (p0, p1, d->prepare, d->invert_dir);
  register_changed_pos (p0, -1);
  register_changed_pos (p1, -1);
}

/*************/
/* ROOM UNDO */
/*************/

void
register_room_undo (struct undo *u, int room, struct con c[FLOORS][PLACES],
                    char *desc)
{
  if (! memcmp (&level.con[room], c, FLOORS * PLACES * sizeof (struct con)))
    return;

  struct room_undo *d = xmalloc (sizeof (struct room_undo));
  d->room = room;
  memcpy (&d->b, &level.con[room], sizeof (d->b));
  memcpy (&d->f, c, sizeof (d->f));
  register_undo (u, d, (undo_f) room_undo, desc);
  room_undo (d, +1);
}

void
room_undo (struct room_undo *d, int dir)
{
  destroy_room (d->room);
  memcpy (&level.con[d->room], (dir >= 0)
          ? &d->f : &d->b, FLOORS * PLACES * sizeof (struct con));
  register_room (d->room);
  prepare_room (d->room);
  register_changed_room (d->room);
}

/*********/
/* EVENT */
/*********/

void
register_event_undo (struct undo *u, int e, struct pos *p, bool next,
                     char *desc)
{
  if (peq (&level.event[e].p, p)
      && level.event[e].next == next) return;

  struct event_undo *d = xmalloc (sizeof (struct event_undo));
  d->e = e;
  d->b = level.event[e];
  d->f.p = *p;
  d->f.next = next;
  register_undo (u, d, (undo_f) event_undo, desc);
  event_undo (d, +1);
}

void
event_undo (struct event_undo *d, int dir)
{
  level.event[d->e] = (dir >= 0) ? d->f : d->b;
}

/********************************/
/* HORIZONTAL ROOM CON EXCHANGE */
/********************************/

void
register_h_room_con_exchange_undo (struct undo *u, int _room, char *desc)
{
  int *room = xmalloc (sizeof (* room));
  *room = _room;
  register_undo (u, room, (undo_f) h_room_con_exchange_undo, desc);
  h_room_con_exchange_undo (room, +1);
}

void
h_room_con_exchange_undo (int *room, int dir)
{
  struct pos p0, p1;
  p0.room = *room;
  for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
    for (p0.place = 0; p0.place < PLACES / 2; p0.place++) {
      reflect_pos_h (&p0, &p1);
      exchange_pos (&p0, &p1, false, true);
    }
  prepare_room (p0.room);
  register_changed_room (*room);
}

/********************************/
/* VERTICAL ROOM CON EXCHANGE */
/********************************/

void
register_v_room_con_exchange_undo (struct undo *u, int _room, char *desc)
{
  int *room = xmalloc (sizeof (* room));
  *room = _room;
  register_undo (u, room, (undo_f) v_room_con_exchange_undo, desc);
  v_room_con_exchange_undo (room, +1);
}

void
v_room_con_exchange_undo (int *room, int dir)
{
  struct pos p0, p1;
  p0.room = *room;
  for (p0.floor = 0; p0.floor < FLOORS / 2; p0.floor++)
    for (p0.place = 0; p0.place < PLACES; p0.place++) {
      reflect_pos_v (&p0, &p1);
      exchange_pos (&p0, &p1, false, false);
    }
  prepare_room (p0.room);
  register_changed_room (*room);
}

/****************************/
/* RANDOM ROOM CON EXCHANGE */
/****************************/

void
register_random_room_con_exchange_undo (struct undo *u, int _room,
                                        bool prepare, bool invert_dir,
                                        char *desc)
{
  struct random_room_con_exchange_undo *d = xmalloc (sizeof (* d));
  d->room = _room;
  d->prepare = prepare;
  d->invert_dir = invert_dir;

  struct pos p;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      random_pos (&d->p[p.floor][p.place]);

  register_undo (u, d, (undo_f) random_room_con_exchange_undo, desc);

  random_room_con_exchange_undo (d, +1);
}

void
random_room_con_exchange_undo (struct random_room_con_exchange_undo *d, int dir)
{
  struct pos p0, p1;
  p0.room = d->room;

  if (dir >= 0)
    for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
      for (p0.place = 0; p0.place < PLACES; p0.place++) {
        p1 = d->p[p0.floor][p0.place];
        p1.room = d->room;
        exchange_pos (&p0, &p1, d->prepare, d->invert_dir);
      }
  else
    for (p0.floor = FLOORS - 1; p0.floor >= 0; p0.floor--)
      for (p0.place = PLACES - 1; p0.place >= 0; p0.place--) {
        p1 = d->p[p0.floor][p0.place];
        p1.room = d->room;
        exchange_pos (&p1, &p0, d->prepare, d->invert_dir);
      }

  prepare_room (d->room);
  register_changed_room (d->room);
}

/********/
/* LINK */
/********/

void
register_link_undo (struct undo *u, struct room_linking l[ROOMS],
                    char *desc)
{
  if (! memcmp (l, &level.link, sizeof (level.link))) return;

  struct link_undo *d = xmalloc (sizeof (struct link_undo));
  memcpy (&d->b, l, sizeof (d->b));
  memcpy (&d->f, &level.link, sizeof (d->f));
  register_undo (u, d, (undo_f) link_undo, desc);
  link_undo (d, +1);
}

void
link_undo (struct link_undo *d, int dir)
{
  memcpy (&level.link, (dir >= 0) ? &d->f : &d->b, sizeof (d->f));
  prepare_view ();
}

/******************/
/* START POSITION */
/******************/

void
register_start_pos_undo (struct undo *u, struct pos *p, char *desc)
{
  if (peq (p, &level.start_pos)) return;

  struct start_pos_undo *d = xmalloc (sizeof (* d));
  d->b = level.start_pos;
  npos (p, &d->f);
  register_undo (u, d, (undo_f) start_pos_undo, desc);
  start_pos_undo (d, +1);
}

void
start_pos_undo (struct start_pos_undo *d, int dir)
{
  level.start_pos = (dir >= 0) ? d->f : d->b;
}

/**************************/
/* TOGGLE START DIRECTION */
/**************************/

void
register_toggle_start_dir_undo (struct undo *u, char *desc)
{
  register_undo (u, NULL, (undo_f) toggle_start_dir_undo, desc);
  toggle_start_dir_undo (NULL, +1);
}

void
toggle_start_dir_undo (struct start_pos_undo *d, int dir)
{
  level.start_dir = (level.start_dir == LEFT) ? RIGHT : LEFT;
}

/********************/
/* TOGGLE HAS SWORD */
/********************/

void
register_toggle_has_sword_undo (struct undo *u, char *desc)
{
  register_undo (u, NULL, (undo_f) toggle_has_sword_undo, desc);
  toggle_has_sword_undo (NULL, +1);
}

void
toggle_has_sword_undo (struct start_pos_undo *d, int dir)
{
  level.has_sword = ! level.has_sword;
}

/************************/
/* GUARD START POSITION */
/************************/

void
register_guard_start_pos_undo (struct undo *u, int i, struct pos *p, char *desc)
{
  struct guard *g = &level.guard[i];
  if (peq (p, &g->p)) return;

  struct guard_start_pos_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b = g->p;
  npos (p, &d->f);
  register_undo (u, d, (undo_f) guard_start_pos_undo, desc);
  guard_start_pos_undo (d, +1);
}

void
guard_start_pos_undo (struct guard_start_pos_undo *d, int dir)
{
  level.guard[d->i].p = (dir >= 0) ? d->f : d->b;
}

/********************************/
/* TOGGLE GUARD START DIRECTION */
/********************************/

void
register_toggle_guard_start_dir_undo (struct undo *u, int i, char *desc)
{
  int *d = xmalloc (sizeof (* d));
  *d = i;
  register_undo (u, d, (undo_f) toggle_guard_start_dir_undo, desc);
  toggle_guard_start_dir_undo (d, +1);
}

void
toggle_guard_start_dir_undo (int *d, int dir)
{
  level.guard[*d].dir = (level.guard[*d].dir == LEFT) ? RIGHT : LEFT;
}

/***************/
/* GUARD SKILL */
/***************/

void
register_guard_skill_undo (struct undo *u, int i, struct skill *s, char *desc)
{
  struct guard *g = &level.guard[i];
  if (! memcmp (s, &g->skill, sizeof (* s))) return;

  struct guard_skill_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b_skill = *s;
  d->f_skill = g->skill;
  register_undo (u, d, (undo_f) guard_skill_undo, desc);
  guard_skill_undo (d, +1);
}

void
guard_skill_undo (struct guard_skill_undo *d, int dir)
{
  struct guard *g = &level.guard[d->i];
  memcpy (&g->skill, (dir >= 0) ? &d->f_skill : &d->b_skill, sizeof (g->skill));
}

/***************/
/* GUARD LIVES */
/***************/

void
register_guard_lives_undo (struct undo *u, int i, int l, char *desc)
{
  struct guard *g = &level.guard[i];
  if (g->total_lives == l) return;

  struct indexed_int_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b = l;
  d->f = g->total_lives;
  register_undo (u, d, (undo_f) guard_lives_undo, desc);
  guard_lives_undo (d, +1);
}

void
guard_lives_undo (struct indexed_int_undo *d, int dir)
{
  struct guard *g = &level.guard[d->i];
  g->total_lives = (dir >= 0) ? d->f : d->b;
}

/**************/
/* GUARD TYPE */
/**************/

void
register_guard_type_undo (struct undo *u, int i, enum anim_type t, char *desc)
{
  struct guard *g = &level.guard[i];
  if (g->type == t) return;

  struct indexed_int_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b = t;
  d->f = g->type;
  register_undo (u, d, (undo_f) guard_type_undo, desc);
  guard_type_undo (d, +1);
}

void
guard_type_undo (struct indexed_int_undo *d, int dir)
{
  struct guard *g = &level.guard[d->i];
  g->type = (dir >= 0) ? d->f : d->b;
}

/***************/
/* GUARD STYLE */
/***************/

void
register_guard_style_undo (struct undo *u, int i, int s, char *desc)
{
  struct guard *g = &level.guard[i];
  if (g->style == s) return;

  struct indexed_int_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b = s;
  d->f = g->style;
  register_undo (u, d, (undo_f) guard_style_undo, desc);
  guard_style_undo (d, +1);
}

void
guard_style_undo (struct indexed_int_undo *d, int dir)
{
  struct guard *g = &level.guard[d->i];
  g->style = (dir >= 0) ? d->f : d->b;
}

/************/
/* INT UNDO */
/************/

void
register_int_undo (struct undo *u, int *f, int b, undo_f func, char *desc)
{
  if (*f == b) return;

  struct int_undo *d = xmalloc (sizeof (* d));
  d->i = f;
  d->b = b;
  d->f = *f;
  register_undo (u, d, (undo_f) func, desc);
  func (d, +1);
}

void
int_undo (struct int_undo *d, int dir)
{
  *d->i = (dir >= 0) ? d->f : d->b;
}

/**************************/
/* LEVEL ENVIRONMENT UNDO */
/**************************/

void
level_environment_undo (struct int_undo *d, int dir)
{
  int_undo (d, dir);
  em = level.em;
}

/******************/
/* LEVEL HUE UNDO */
/******************/

void
level_hue_undo (struct int_undo *d, int dir)
{
  int_undo (d, dir);
  hue = level.hue;
}
