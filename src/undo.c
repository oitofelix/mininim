/*
  undo.c -- undo module;

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

void
register_undo (struct undo *u, void *data, undo_f f, char *desc)
{
  if (! u->pass || ! u->count || u->current == -1) {
    u->pass = NULL;
    u->count = 0;
    u->current = -1;
  }

  size_t i;
  for (i = u->current + 1; i < u->count; i++) {
    al_free (u->pass[i].data);
    al_free (u->pass[i].desc);
  }

  u->count = u->current + 2;
  u->pass = xrealloc (u->pass, u->count * sizeof (* u->pass));
  u->current++;

  u->pass[u->current].data = data;
  u->pass[u->current].f = f;
  if (desc) xasprintf (&u->pass[u->current].desc, "%s", desc);
  else u->pass[u->current].desc = NULL;

  if (editor_register && desc) editor_msg (desc, editor_register);
}

void
free_undo (struct undo *u)
{
  size_t i;
  for (i = 0; i < u->count; i++) {
    al_free (u->pass[i].data);
    al_free (u->pass[i].desc);
  }

  u->pass = NULL;
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

  if (dir >= 0) {
    do {
      u->current++;
      u->pass[u->current].f (u->pass[u->current].data, dir);
    } while (! u->pass[u->current].desc);
    if (desc) *desc = u->pass[u->current].desc;
  } else {
    if (desc) *desc = u->pass[u->current].desc;
    do {
      u->pass[u->current].f (u->pass[u->current].data, dir);
      u->current--;
    } while (! u->pass[u->current].desc);
  }

  return true;
}

void
end_undo_set (struct undo *u, char *desc)
{
  if (desc) {
    xasprintf (&u->pass[u->current].desc, "%s", desc);
    if (editor_register) editor_msg (desc, editor_register);
  } else u->pass[u->current].desc = NULL;
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
    editor_msg (undo_msg, EDITOR_CYCLES_3);
    return;
  }

  undo_pass (u, dir, &text);

  if (prefix) xasprintf (&undo_msg, "%s %s: %s", prefix, dir_str, text);
  else xasprintf (&undo_msg, "%s: %s", dir_str, text);
  editor_msg (undo_msg, EDITOR_CYCLES_3);
}

/*********************/
/* CONSTRUCTION UNDO */
/*********************/

void
register_con_undo (struct undo *u, struct pos *p,
                   int f, int b, int e, int ff,
                   union con_state *cs,
                   bool ignore_intermediate,
                   char *desc)
{
  struct con c;

  c.fg = (f != MIGNORE) ? fg_val (f) : fg (p);
  c.bg = (b != MIGNORE) ? bg_val (b) : bg (p);
  c.ext = (e != MIGNORE) ? ext_val (c.fg, e) : ext (p);

  if (ff != MIGNORE && ff != NO_FAKE) c.fake = fg_val (ff);
  else if (ff == NO_FAKE) c.fake = ff;
  else c.fake = con (p)->fake;

  if (c.fg == c.fake) c.fake = -1;

  struct con_undo *prev_data = u->count
    ? (struct con_undo *) u->pass[u->current].data
    : NULL;

  enum con_diff cd = con_diff (con (p), &c);

  if (ignore_intermediate
      && prev_data
      && u->pass[u->current].f == (undo_f) con_undo
      && peq (&prev_data->p, p)
      && ((cd == con_diff (&prev_data->b, &prev_data->f)
           && cd != CON_DIFF_MIXED
           && cd != CON_DIFF_NO_DIFF)
          || ! memcmp (&prev_data->b, &c, sizeof (c))))
    undo_pass (u, -1, NULL);

  if (! memcmp (con (p), &c, sizeof (c))) return;

  struct con_undo *d = xmalloc (sizeof (struct con_undo));
  d->p = *p;
  d->b = *con (p);
  d->f = c;

  copy_to_con_state (&d->bs, p);

  *con (&d->p) = d->f;

  if (should_init (&d->b, &d->f))
    init_con_at_pos (&d->p);

  if (cs) {
    d->fs = *cs;
    copy_from_con_state (p, cs);
  } else copy_to_con_state (&d->fs, p);

  register_undo (u, d, (undo_f) con_undo, desc);

  register_changed_pos (p);
}

void
con_undo (struct con_undo *d, int dir)
{
  /* copy_to_con_state ((dir >= 0) ? &d->bs : &d->fs, &d->p); */

  *con (&d->p) = (dir >= 0) ? d->f : d->b;

  if (should_init (&d->b, &d->f))
    init_con_at_pos (&d->p);

  copy_from_con_state (&d->p, (dir >= 0) ? &d->fs : &d->bs);

  register_changed_pos (&d->p);
}

/*******************/
/* MIRROR POS UNDO */
/*******************/

void
register_mirror_pos_undo (struct undo *u, struct pos *p0, struct pos *p1,
                          bool invert_dir, char *desc)
{
  if (peq (p0, p1)) return;

  struct mirror_pos_undo *d = xmalloc (sizeof (struct mirror_pos_undo));
  d->p0 = *p0;
  d->p1 = *p1;
  d->invert_dir = invert_dir;
  register_undo (u, d, (undo_f) mirror_pos_undo, desc);
  mirror_pos_undo (d, +1);
}

void
mirror_pos_undo (struct mirror_pos_undo *d, int dir)
{
  struct pos *p0, *p1;
  if (dir >=0) {
    p0 = &d->p0;
    p1 = &d->p1;
  } else {
    p0 = &d->p1;
    p1 = &d->p0;
  }
  mirror_pos (p0, p1, d->invert_dir);
}

/**************/
/* LEVEL UNDO */
/**************/

void
register_level_undo (struct undo *u, struct level *l, char *desc)
{
  if (level_eq (&global_level, l)) return;

  struct level_undo *d = xmalloc (sizeof (struct level_undo));
  copy_level (&d->b, &global_level);
  copy_level (&d->f, l);
  d->f.n = global_level.n;
  d->f.nominal_n = global_level.nominal_n;
  register_undo (u, d, (undo_f) level_undo, desc);
  level_undo (d, +1);
}

void
level_undo (struct level_undo *d, int dir)
{
  replace_playing_level ((dir >= 0) ? &d->f : &d->b);
}

/***********************/
/* LEVEL EXCHANGE UNDO */
/***********************/

void
register_level_exchange_undo (struct undo *u, int n, char *desc)
{
  if (global_level.n == n) return;

  int *d = xmalloc (sizeof (* d));
  *d = n;
  register_undo (u, d, (undo_f) level_exchange_undo, desc);
  level_exchange_undo (d, +1);
}

void
level_exchange_undo (int *d, int dir)
{
  global_level.next_level (&vanilla_level, *d);

  int n = vanilla_level.n;
  int nominal_n = vanilla_level.nominal_n;

  vanilla_level.n = global_level.n;
  vanilla_level.nominal_n = global_level.nominal_n;
  global_level.n = n;
  global_level.nominal_n = nominal_n;

  save_level (&global_level);
  save_level (&vanilla_level);

  replace_playing_level (&vanilla_level);
}


/*********/
/* EVENT */
/*********/

void
register_event_undo (struct undo *u, int e, struct pos *p, bool next,
                     char *desc)
{
  struct level_event *le = event (p->l, e);
  if (peq (&le->p, p) && le->next == next) return;

  struct event_undo *d = xmalloc (sizeof (struct event_undo));
  d->e = e;
  d->b = *event (p->l, e);
  d->f.p = *p;
  d->f.next = next;
  register_undo (u, d, (undo_f) event_undo, desc);
  event_undo (d, +1);
}

void
event_undo (struct event_undo *d, int dir)
{
  *event (d->f.p.l, d->e) = (dir >= 0) ? d->f : d->b;
}

/*******************************/
/* HORIZONTAL ROOM CONS MIRROR */
/*******************************/

void
register_h_room_mirror_con_undo (struct undo *u, int _room, char *desc)
{
  int *room = xmalloc (sizeof (* room));
  *room = _room;
  register_undo (u, room, (undo_f) h_room_mirror_con_undo, desc);
  h_room_mirror_con_undo (room, +1);
}

void
h_room_mirror_con_undo (int *room, int dir)
{
  mirror_room_h (&global_level, *room);
}

/*****************************/
/* VERTICAL ROOM CONS MIRROR */
/*****************************/

void
register_v_room_mirror_con_undo (struct undo *u, int _room, char *desc)
{
  int *room = xmalloc (sizeof (* room));
  *room = _room;
  register_undo (u, room, (undo_f) v_room_mirror_con_undo, desc);
  v_room_mirror_con_undo (room, +1);
}

void
v_room_mirror_con_undo (int *room, int dir)
{
  struct pos p0, p1;
  new_pos (&p0, &global_level, *room, -1, -1);
  for (p0.floor = 0; p0.floor < FLOORS / 2; p0.floor++)
    for (p0.place = 0; p0.place < PLACES; p0.place++) {
      reflect_pos_v (&p0, &p1);
      mirror_pos (&p0, &p1, false);
    }
  register_changed_room (*room);
}

/***************************/
/* RANDOM ROOM CONS MIRROR */
/***************************/

void
register_random_room_mirror_con_undo (struct undo *u, int _room,
                                      bool invert_dir,
                                      char *desc)
{
  struct random_room_mirror_con_undo *d = xmalloc (sizeof (* d));
  d->room = _room;
  d->invert_dir = invert_dir;

  struct pos p;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      random_pos (&global_level, &d->p[p.floor][p.place]);

  register_undo (u, d, (undo_f) random_room_mirror_con_undo, desc);

  random_room_mirror_con_undo (d, +1);
}

void
random_room_mirror_con_undo (struct random_room_mirror_con_undo *d, int dir)
{
  struct pos p0, p1;
  new_pos (&p0, &global_level, d->room, -1, -1);

  if (dir >= 0)
    for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
      for (p0.place = 0; p0.place < PLACES; p0.place++) {
        p1 = d->p[p0.floor][p0.place];
        p1.room = d->room;
        mirror_pos (&p0, &p1, d->invert_dir);
      }
  else
    for (p0.floor = FLOORS - 1; p0.floor >= 0; p0.floor--)
      for (p0.place = PLACES - 1; p0.place >= 0; p0.place--) {
        p1 = d->p[p0.floor][p0.place];
        p1.room = d->room;
        mirror_pos (&p1, &p0, d->invert_dir);
      }

  register_changed_room (d->room);
}

/********/
/* LINK */
/********/

void
register_link_undo (struct undo *u, struct room_linking l[ROOMS],
                    char *desc)
{
  if (! memcmp (l, &global_level.link, sizeof (global_level.link))) return;

  struct link_undo *d = xmalloc (sizeof (struct link_undo));
  memcpy (&d->b, l, sizeof (d->b));
  memcpy (&d->f, &global_level.link, sizeof (d->f));
  register_undo (u, d, (undo_f) link_undo, desc);
  link_undo (d, +1);
}

void
link_undo (struct link_undo *d, int dir)
{
  memcpy (&global_level.link, (dir >= 0) ? &d->f : &d->b, sizeof (d->f));
}

/******************/
/* START POSITION */
/******************/

void
register_start_pos_undo (struct undo *u, struct pos *p, char *desc)
{
  if (peq (p, &p->l->start_pos)) return;

  struct start_pos_undo *d = xmalloc (sizeof (* d));
  d->b = p->l->start_pos;
  npos (p, &d->f);
  register_undo (u, d, (undo_f) start_pos_undo, desc);
  start_pos_undo (d, +1);
}

void
start_pos_undo (struct start_pos_undo *d, int dir)
{
  struct pos p = (dir >= 0) ? d->f : d->b;
  p.l->start_pos = p;
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
  global_level.start_dir = (global_level.start_dir == LEFT) ? RIGHT : LEFT;
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
  global_level.has_sword = ! global_level.has_sword;
}

/************************/
/* GUARD START POSITION */
/************************/

void
register_guard_start_pos_undo (struct undo *u, int i, struct pos *p, char *desc)
{
  struct guard *g = guard (p->l, i);
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
  struct pos p = (dir >= 0) ? d->f : d->b;
  guard (p.l, d->i)->p = p;
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
  guard (&global_level, *d)->dir =
    (guard (&global_level, *d)->dir == LEFT) ? RIGHT : LEFT;
}

/***************/
/* GUARD SKILL */
/***************/

void
register_guard_skill_undo (struct undo *u, int i, struct skill *s, char *desc)
{
  struct guard *g = guard (&global_level, i);
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
  struct guard *g = guard (&global_level, d->i);
  g->skill = (dir >= 0) ? d->f_skill : d->b_skill;
  struct anim *a = get_guard_anim_by_level_id (d->i);
  if (a) a->skill = g->skill;
}

/***************/
/* GUARD LIVES */
/***************/

void
register_guard_lives_undo (struct undo *u, int i, int l, char *desc)
{
  struct guard *g = guard (&global_level, i);
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
  struct guard *g = guard (&global_level, d->i);
  g->total_lives = (dir >= 0) ? d->f : d->b;
}

/**************/
/* GUARD TYPE */
/**************/

void
register_guard_type_undo (struct undo *u, int i, enum anim_type t, char *desc)
{
  struct guard *g = guard (&global_level, i);
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
  struct guard *g = guard (&global_level, d->i);
  g->type = (dir >= 0) ? d->f : d->b;
}

/***************/
/* GUARD STYLE */
/***************/

void
register_guard_style_undo (struct undo *u, int i, int s, char *desc)
{
  struct guard *g = guard (&global_level, i);
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
  struct guard *g = guard (&global_level, d->i);
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
  em = global_level.em;
}

/******************/
/* LEVEL HUE UNDO */
/******************/

void
level_hue_undo (struct int_undo *d, int dir)
{
  int_undo (d, dir);
  hue = global_level.hue;
}
