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
  *desc = u->pass[u->current].desc;
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
                   char *desc)
{
  struct con c;

  c.fg = (fg != IGNORE) ? fg : con (p)->fg;
  c.bg = (bg != IGNORE) ? bg : con (p)->bg;
  c.ext = (ext != IGNORE) ? (union conext) ext : con (p)->ext;

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
}

void
con_undo (struct con_undo *d, int dir)
{
  destroy_con_at_pos (&d->p);
  *con (&d->p) = (dir >= 0) ? d->f : d->b;
  register_con_at_pos (&d->p);
  prepare_con_at_pos (&d->p);
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
  prepare_room (room_view);
}
