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

static bool tile_undo_ignore_state;
static bool should_undo_update_event_tree (struct undo *u, int dir);

void
register_undo (struct undo *u, void *data, undo_f f,
               destroy_undo_f df, char *desc)
{
  if (! u->pass || ! u->count || u->current == -1) {
    u->pass = NULL;
    u->count = 0;
    u->current = -1;
  }

  free_undo_tail (u);

  u->count++;
  u->pass = xrealloc (u->pass, u->count * sizeof (* u->pass));
  u->current++;

  u->pass[u->current].data = data;
  u->pass[u->current].f = f;
  u->pass[u->current].df = df;
  if (desc) u->pass[u->current].desc = xasprintf ("%s", desc);
  else u->pass[u->current].desc = NULL;

  if (desc) {
    struct undo_update uc;
    memset (&uc, 0, sizeof (uc));
    should_undo_update (&uc, u, -1);
    undo_updates (&uc);
  }

  if (editor_register && desc) editor_msg (desc, editor_register);
}

void
free_undo_tail (struct undo *u)
{
  size_t i;
  for (i = u->current + 1; i < u->count; i++) {
    if (u->pass[i].df) u->pass[i].df (u->pass[i].data);
    al_free (u->pass[i].data);
    al_free (u->pass[i].desc);
  }

  u->count = u->current + 1;
  u->pass = xrealloc (u->pass, u->count * sizeof (* u->pass));
}

void
free_undo (struct undo *u)
{
  size_t i;
  for (i = 0; i < u->count; i++) {
    if (u->pass[i].df) u->pass[i].df (u->pass[i].data);
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
  if (! u->count) return;
  if (desc) {
    u->pass[u->current].desc = xasprintf ("%s", desc);
    if (editor_register) editor_msg (desc, editor_register);
  } else u->pass[u->current].desc = NULL;
}

int
current_macro_undo (struct undo *u)
{
  int current = -1;
  for (int i = 0; i <= u->current; i++)
    if (u->pass[i].desc) current++;
  return current;
}


/*************/
/* TILE UNDO */
/*************/

void
register_tile_undo (struct undo *u, struct pos *p,
                    int f, int b, int e, int ff,
                    union tile_state *cs,
                    intptr_t ignore_id,
                    char *desc)
{
  struct tile c;

  c.fg = (f != MIGNORE) ? fg_val (f) : fg (p);
  c.bg = (b != MIGNORE) ? bg_val (b) : bg (p);
  c.ext = (e != MIGNORE) ? ext_val (p->l, c.fg, e) : ext (p);

  if (ff != MIGNORE && ff != NO_FAKE) c.fake = fg_val (ff);
  else if (ff == NO_FAKE) c.fake = ff;
  else c.fake = tile (p)->fake;

  if (c.fg == c.fake) c.fake = -1;

  struct tile_undo *prev_data = u->count
    ? (struct tile_undo *) u->pass[u->current].data
    : NULL;

  if (ignore_id
      && prev_data
      && u->pass[u->current].f == (undo_f) tile_undo
      && prev_data->ignore_id == ignore_id
      && peq (&prev_data->p, p)
      && u->pass[u->current].desc
      && desc) {
    tile_undo_ignore_state = true;
    undo_pass (u, -1, NULL);
    free_undo_tail (u);
    tile_undo_ignore_state = false;
  }

  if (! memcmp (tile (p), &c, sizeof (c))) return;

  struct tile_undo *d = xmalloc (sizeof (struct tile_undo));
  d->p = *p;
  d->b = *tile (p);
  d->f = c;
  d->ignore_id = ignore_id;

  copy_to_tile_state (&d->bs, p);

  *tile (&d->p) = d->f;

  /* if (should_init (&d->b, &d->f)) */
  init_tile_at_pos (&d->p);

  if (cs) {
    d->fs = *cs;
    copy_from_tile_state (p, cs);
  } else copy_to_tile_state (&d->fs, p);

  register_undo (u, d, (undo_f) tile_undo, NULL, desc);

  register_changed_pos (p);
}

void
tile_undo (struct tile_undo *d, int dir)
{
  /* copy_to_tile_state ((dir >= 0) ? &d->bs : &d->fs, &d->p); */

  *tile (&d->p) = (dir >= 0) ? d->f : d->b;

  /* if (should_init (&d->b, &d->f)) */
  init_tile_at_pos (&d->p);

  if (! tile_undo_ignore_state)
    copy_from_tile_state (&d->p, (dir >= 0) ? &d->fs : &d->bs);

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
  register_undo (u, d, (undo_f) mirror_pos_undo, NULL, desc);
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

  struct level_undo *d = xmalloc (sizeof (*d));
  memset (d, 0, sizeof (*d));
  copy_level (&d->b, &global_level);
  copy_level (&d->f, l);
  d->f.n = global_level.n;
  d->f.nominal_n = global_level.nominal_n;
  register_undo (u, d, (undo_f) level_undo, NULL, desc);
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
  register_undo (u, d, (undo_f) level_exchange_undo, NULL, desc);
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
register_event_undo (struct undo *u, struct level_event *event,
                     size_t event_nmemb, char *desc)
{
  if (event_nmemb == global_level.event_nmemb
      && ! memcmp (event, global_level.event, event_nmemb
                   * sizeof (*event))) return;

  struct event_undo *d = xmalloc (sizeof (struct event_undo));
  d->f_event = copy_array (event, event_nmemb, &d->f_event_nmemb,
                           sizeof (*d->f_event));
  d->b_event = copy_array (global_level.event, global_level.event_nmemb,
                           &d->b_event_nmemb, sizeof (*d->b_event));
  register_undo (u, d, (undo_f) event_undo,
                 (destroy_undo_f) destroy_event_undo, desc);
  event_undo (d, +1);
}

void
event_undo (struct event_undo *d, int dir)
{
  destroy_array ((void **) &global_level.event, &global_level.event_nmemb);

  struct level_event *event = (dir >= 0) ? d->f_event : d->b_event;
  size_t event_nmemb = (dir >= 0) ? d->f_event_nmemb : d->b_event_nmemb;

  global_level.event =
    copy_array (event, event_nmemb, &global_level.event_nmemb,
                sizeof (*global_level.event));
}

void
destroy_event_undo (struct event_undo *d)
{
  destroy_array ((void **) &d->b_event, &d->b_event_nmemb);
  destroy_array ((void **) &d->f_event, &d->f_event_nmemb);
}

/*******************************/
/* HORIZONTAL ROOM CONS MIRROR */
/*******************************/

void
register_h_room_mirror_tile_undo (struct undo *u, int _room, char *desc)
{
  int *room = xmalloc (sizeof (* room));
  *room = _room;
  register_undo (u, room, (undo_f) h_room_mirror_tile_undo, NULL, desc);
  h_room_mirror_tile_undo (room, +1);
}

void
h_room_mirror_tile_undo (int *room, int dir)
{
  mirror_room_h (&global_level, *room);
}

/*****************************/
/* VERTICAL ROOM CONS MIRROR */
/*****************************/

void
register_v_room_mirror_tile_undo (struct undo *u, int _room, char *desc)
{
  int *room = xmalloc (sizeof (* room));
  *room = _room;
  register_undo (u, room, (undo_f) v_room_mirror_tile_undo, NULL, desc);
  v_room_mirror_tile_undo (room, +1);
}

void
v_room_mirror_tile_undo (int *room, int dir)
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
register_random_room_mirror_tile_undo (struct undo *u, int _room,
                                      bool invert_dir,
                                      char *desc)
{
  struct random_room_mirror_tile_undo *d = xmalloc (sizeof (* d));
  d->room = _room;
  d->invert_dir = invert_dir;

  struct pos p;
  for (p.floor = 0; p.floor < FLOORS; p.floor++)
    for (p.place = 0; p.place < PLACES; p.place++)
      random_pos (&global_level, &d->p[p.floor][p.place]);

  register_undo (u, d, (undo_f) random_room_mirror_tile_undo, NULL, desc);

  random_room_mirror_tile_undo (d, +1);
}

void
random_room_mirror_tile_undo (struct random_room_mirror_tile_undo *d, int dir)
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
register_link_undo (struct undo *u, struct room_linking *rlink, char *desc)
{
  if (! memcmp
      (rlink, global_level.link, global_level.room_nmemb
       * sizeof (*global_level.link))) return;

  struct link_undo *d = xmalloc (sizeof (*d));
  d->f = copy_array (rlink, global_level.room_nmemb, NULL,
                     sizeof (*d->f));
  d->b = copy_array (global_level.link, global_level.room_nmemb, NULL,
                     sizeof (*d->b));
  register_undo (u, d, (undo_f) link_undo, (destroy_undo_f) destroy_link_undo,
                 desc);
  link_undo (d, +1);
}

void
link_undo (struct link_undo *d, int dir)
{
  destroy_array ((void **) &global_level.link, NULL);
  global_level.link = copy_array
    ((dir >= 0) ? d->f : d->b, global_level.room_nmemb,
     NULL, sizeof (*global_level.link));
}

void
destroy_link_undo (struct link_undo *d)
{
  destroy_array ((void **) &d->b, NULL);
  destroy_array ((void **) &d->f, NULL);
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
  register_undo (u, d, (undo_f) start_pos_undo, NULL, desc);
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
  register_undo (u, NULL, (undo_f) toggle_start_dir_undo, NULL, desc);
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
  register_undo (u, NULL, (undo_f) toggle_has_sword_undo, NULL, desc);
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
  register_undo (u, d, (undo_f) guard_start_pos_undo, NULL, desc);
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
  register_undo (u, d, (undo_f) toggle_guard_start_dir_undo, NULL, desc);
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
  register_undo (u, d, (undo_f) guard_skill_undo, NULL, desc);
  guard_skill_undo (d, +1);
}

void
guard_skill_undo (struct guard_skill_undo *d, int dir)
{
  struct guard *g = guard (&global_level, d->i);
  g->skill = (dir >= 0) ? d->f_skill : d->b_skill;
  struct actor *a = get_guard_actor_by_level_id (d->i);
  if (a) a->skill = g->skill;
}

/************/
/* GUARD HP */
/************/

void
register_guard_hp_undo (struct undo *u, int i, int l, char *desc)
{
  struct guard *g = guard (&global_level, i);
  if (g->total_hp == l) return;

  struct indexed_int_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b = l;
  d->f = g->total_hp;
  register_undo (u, d, (undo_f) guard_hp_undo, NULL, desc);
  guard_hp_undo (d, +1);
}

void
guard_hp_undo (struct indexed_int_undo *d, int dir)
{
  struct guard *g = guard (&global_level, d->i);
  g->total_hp = (dir >= 0) ? d->f : d->b;
}

/**************/
/* GUARD TYPE */
/**************/

void
register_guard_type_undo (struct undo *u, int i, enum actor_type t,
                          char *desc)
{
  struct guard *g = guard (&global_level, i);
  if (g->type == t) return;

  struct indexed_int_undo *d = xmalloc (sizeof (* d));
  d->i = i;
  d->b = t;
  d->f = g->type;
  register_undo (u, d, (undo_f) guard_type_undo, NULL, desc);
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
  register_undo (u, d, (undo_f) guard_style_undo, NULL, desc);
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
  register_undo (u, d, (undo_f) func, NULL, desc);
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



/***********************/
/* GUI controls update */
/***********************/

struct undo_update *
should_undo_update (struct undo_update *uc,
                    struct undo *u, int dir)
{
  uc->event_tree =
    uc->event_tree || should_undo_update_event_tree (u, dir);
  return uc;
}

void
undo_updates (struct undo_update *uc)
{
  if (uc->event_tree) {
    Ihandle *events_control =
      IupGetDialogChild (gui_editor_dialog, "EVENTS_CONTROL");
    gui_run_callback_IFn ("_UPDATE_TREE_CB", events_control);
  }
}

bool
should_undo_update_event_tree (struct undo *u, int dir)
{
  if (! can_undo (u, dir)) return false;

  struct undo_pass *pass =
    &u->pass[u->current + (dir >= 0 ? 1 : 0)];

  if (pass->f == (undo_f) event_undo) return true;
  else if (pass->f == (undo_f) tile_undo) {
    struct tile_undo *d = pass->data;
    return event_fg_cs (d->f.fg) || event_fg_cs (d->b.fg)
      || target_event (&d->p) > 0;
  }

  return false;
}
