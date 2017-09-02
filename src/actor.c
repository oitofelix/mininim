/*
  actor.c -- actor module;

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

struct actor *actor;
size_t actor_nmemb;

int
create_actor (struct actor *a0, enum actor_type t, struct pos *p, enum dir dir)
{
  struct actor a;
  int i = actor_nmemb;
  memset (&a, 0, sizeof (a));

  if (a0) a = *a0;
  else {
    a.type = t;
    a.original_type = t;
    a.f.dir = dir;
    new_coord (&a.f.c, p->l, p->room, -1, -1);
    a.controllable = false;
    invalid_pos (&a.enemy_pos);
  }

  a.id = i;
  a.f.parent_id = i;
  a.level_id = -1;

  switch (a.type) {
  case NO_ACTOR: default: break;
  case KID: create_kid (a0, &a, p, dir); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    create_guard (a0, &a, p, dir); break;
  case MOUSE: create_mouse (a0, &a, p, dir); break;
  }

  actor = add_to_array (&a, 1, actor, &actor_nmemb, i, sizeof (a));
  return i;
}

void
destroy_actor (struct actor *a)
{
  forget_enemy (a);

  if (a->type == KID) destroy_kid (a);

  size_t i =  a - actor;
  actor = remove_from_array (actor, &actor_nmemb, i, 1, sizeof (*a));
}

void
destroy_actors (void)
{
  while (actor_nmemb) destroy_actor (&actor[0]);
  actor = NULL;
  actor_nmemb = 0;
}

struct actor *
get_next_controllable (struct actor *k)
{
  do {
    k = &actor[(k - actor + 1) % actor_nmemb];
  } while (k->type != KID || ! k->controllable);
  return k;
}

void
select_controllable_by_id (int id)
{
  struct actor *old_k = get_actor_by_id (current_kid_id);
  struct actor *k = get_actor_by_id (id);
  current_kid_id = id;
  if (id) {
    k->death_timer = 0;
    last_fellow_shadow_id = id;
  }
  if (! is_frame_visible (&k->f))
    mr_restore_origin (&k->mr_origin);
  if (k->f.c.room != old_k->f.c.room)
    mr_focus_room (k->f.c.room);
  k->selection_cycle = anim_cycle;
}

struct actor *
get_actor_by_id (int id)
{
  int i;
  if (id < 0) return NULL;
  for (i = 0; i < actor_nmemb; i++)
    if (actor[i].id == id) return &actor[i];
  return NULL;
}

struct actor *
get_reciprocal_enemy (struct actor *k)
{
  if (! k) return NULL;
  else if (k->enemy_id == -1) return NULL;
  else if (k->enemy_id != -1 && k->enemy_refraction >= 0)
    return NULL;
  else if (k->enemy_id != -1) {
    struct actor *ke = get_actor_by_id (k->enemy_id);
    if (! ke) return NULL;
    else if (ke->enemy_id == -1) return NULL;
    else if (ke->enemy_id != -1 && ke->enemy_refraction >= 0)
      return NULL;
    else if (ke->enemy_id == k->id) return ke;
    else return NULL;
  } else return NULL;
}

struct actor *
get_actor_dead_at_pos (struct pos *p)
{
  int i;
  for (i = 0; i < actor_nmemb; i++)
    if (actor[i].current_hp <= 0
        && peq (&actor[i].p, p))
      return &actor[i];
  return NULL;
}

struct actor *
get_guard_actor_by_level_id (int id)
{
  int i;
  if (id < 0) return NULL;
  for (i = 0; i < actor_nmemb; i++)
    if (is_guard (&actor[i]) && actor[i].level_id == id)
      return &actor[i];
  return NULL;
}

void
draw_actor_frame (ALLEGRO_BITMAP *bitmap, struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: draw_kid_frame (bitmap, a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW: draw_guard_frame (bitmap, a); break;
  case MOUSE: draw_mouse_frame (bitmap, a); break;
  }
}

void
draw_actors (ALLEGRO_BITMAP *bitmap)
{
  struct actor *a;

  /* coord_wa = true; */

  qsort (actor, actor_nmemb, sizeof (*a), compare_actors);

  size_t i;
  for (i = 0; i < actor_nmemb; i++) {
    a = &actor[i];
    if (a->invisible || ! a->f.b) continue;
    draw_actor_frame (bitmap, a);
    draw_room_actor_front (bitmap, a);
  }

  /* coord_wa = false; */
}

int
compare_actors (const void *_a0, const void *_a1)
{
  struct coord tr0, tr1;

  struct actor *a0 = (struct actor *) _a0;
  struct actor *a1 = (struct actor *) _a1;

  if (a0->action == kid_stairs && a1->action != kid_stairs)
    return -1;
  if (a0->action != kid_stairs && a1->action == kid_stairs)
    return 1;

  _tr (&a0->f, &tr0);
  _tr (&a1->f, &tr1);

  coord2room (&tr1, tr0.room, &tr1);

  if (tr0.room < tr1.room) return -1;
  if (tr0.room > tr1.room) return 1;

  if (is_near (a0, a1)) {
    if (tr0.x < tr1.x) return -1;
    if (tr0.x > tr1.x) return 1;
    if (a0->id < a1->id) return -1;
    if (a0->id > a1->id) return 1;
  } else {
    struct coord o = {tr0.l, tr0.room, 0, ORIGINAL_HEIGHT};

    lua_Number d0 = dist_coord (&o, &tr0);
    lua_Number d1 = dist_coord (&o, &tr1);

    if (d0 < d1) return -1;
    if (d0 > d1) return 1;
  }

  return 0;
}

void
draw_actor_if_at_pos (ALLEGRO_BITMAP *bitmap, struct actor *a, struct pos *p)
{
  struct coord nc;
  struct pos np, pbl, pmbo, pbr, pml, pm, pmr, ptl, pmt, ptr;
  survey (_bl, pos, &a->f, &nc, &pbl, &np);
  survey (_mbo, pos, &a->f, &nc, &pmbo, &np);
  survey (_br, pos, &a->f, &nc, &pbr, &np);
  survey (_ml, pos, &a->f, &nc, &pml, &np);
  survey (_m, pos, &a->f, &nc, &pm, &np);
  survey (_mr, pos, &a->f, &nc, &pmr, &np);
  survey (_tl, pos, &a->f, &nc, &ptl, &np);
  survey (_mt, pos, &a->f, &nc, &pmt, &np);
  survey (_tr, pos, &a->f, &nc, &ptr, &np);

  if (! peq (&pbl, p)
      && ! peq (&pmbo, p)
      && ! peq (&pbr, p)
      && ! peq (&pml, p)
      && ! peq (&pm, p)
      && ! peq (&pmr, p)
      && ! peq (&ptl, p)
      && ! peq (&pmt, p)
      && ! peq (&ptr, p)) return;

  draw_actor_frame (bitmap, a);
}

void
clear_actors_keyboard_state (void)
{
  int i;
  for (i = 0; i < actor_nmemb; i++)
    memset (&actor[i].key, 0, sizeof (actor[i].key));
}

bool
is_actor_dead (struct actor *a)
{
  return is_kid_dead (a)
    || is_guard_dead (a);
}

bool
is_actor_chomped (struct actor *a)
{
  return a->action == kid_die_chomped
    || a->action == guard_die_chomped;
}

bool
is_actor_fall (struct actor *a)
{
  return a->action == kid_fall
    || a->action == guard_fall;
}

bool
is_fightable_actor (struct actor *a)
{
  return a->type == KID || a->type == SHADOW
    || a->type == GUARD || a->type == FAT
    || a->type == SKELETON || a->type == VIZIER;
}

void
actor_die_suddenly (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_die_suddenly (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_suddenly (a); break;
  }
}

void
actor_die_spiked (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_die_spiked (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_spiked (a); break;
  }
}

void
actor_die_chomped (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_die_chomped (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die_chomped (a); break;
  }
}

void
actor_die (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_die (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_die (a); break;
  }
}

void
actor_sword_hit (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_sword_hit (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_sword_hit (a); break;
  }
}

void
actor_fall (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_fall (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_fall (a); break;
  }
}

void
actor_walkf (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_sword_walkf (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_sword_walkf (a); break;
  }
}

void
actor_walkb (struct actor *a)
{
  switch (a->type) {
  case NO_ACTOR: default: break;
  case KID: kid_sword_walkb (a); break;
  case GUARD:
  case FAT:
  case VIZIER:
  case SKELETON:
  case SHADOW:
    guard_sword_walkb (a); break;
  }
}

struct coord *
splash_coord (ALLEGRO_BITMAP *splash, struct frame *f, struct coord *c)
{
  lua_Number w = IW (get_bitmap_width (splash));
  lua_Number h = IH (get_bitmap_height (splash));
  lua_Number fw = IW (get_bitmap_width (f->b));
  lua_Number fh = IH (get_bitmap_height (f->b));
  return
    new_coord (c, f->c.l, f->c.room,
               f->c.x + (fw / 2) - (w / 2),
               f->c.y + (fh / 2) - (h / 2));
}

ALLEGRO_COLOR
start_actor_palette (ALLEGRO_COLOR c, void *data)
{
  unsigned char r, g, b, a;
  al_unmap_rgba (c, &r, &g, &b, &a);
  if (a == 0) return c;
  r = add_char (r, -64);
  g = add_char (g, -64);
  b = add_char (b, -64);
  return al_map_rgba (r, g, b, 100);
}




bool
uncollide_static_neutral (struct actor *a)
{
  return uncollide_static (&a->f, &a->fo, _bf, 0, 0, _bb, 0, 0,
                           &a->fo);
}

bool
uncollide_static_kid_normal (struct actor *a)
{
  return uncollide_static (&a->f, &a->fo, _bf,
                           COLLISION_FRONT_LEFT_NORMAL,
                           COLLISION_FRONT_RIGHT_NORMAL,
                           _bb,
                           COLLISION_BACK_LEFT_NORMAL,
                           COLLISION_BACK_RIGHT_NORMAL,
                           &a->fo);
}

bool
uncollide_static_fight (struct actor *a)
{
  return uncollide_static (&a->f, &a->fo, _bf,
                           COLLISION_FRONT_LEFT_FIGHT,
                           COLLISION_FRONT_RIGHT_FIGHT,
                           _bb,
                           COLLISION_BACK_LEFT_FIGHT,
                           COLLISION_BACK_RIGHT_FIGHT,
                           &a->fo);
}

bool
is_colliding_front_fight (struct actor *a)
{
  return uncollide (&a->f, &a->fo, _bf,
                    COLLISION_FRONT_LEFT_FIGHT,
                    COLLISION_FRONT_RIGHT_FIGHT,
                    NULL, &a->ci);
}

bool
is_colliding_back_fight (struct actor *a)
{
  return uncollide (&a->f, &a->fo, _bb,
                    COLLISION_BACK_LEFT_FIGHT,
                    COLLISION_BACK_RIGHT_FIGHT,
                    NULL, &a->ci);
}

bool
uncollide_front_fight (struct actor *a)
{
  bool uf = uncollide (&a->f, &a->fo, _bf,
                       COLLISION_FRONT_LEFT_FIGHT,
                       COLLISION_FRONT_RIGHT_FIGHT,
                       &a->fo, NULL);

  bool ub = uncollide (&a->f, &a->fo, _bb,
                       COLLISION_BACK_LEFT_FIGHT,
                       COLLISION_BACK_RIGHT_FIGHT,
                       &a->fo, NULL);
  return uf || ub;
}

bool
uncollide_back_fight (struct actor *a)
{
  bool ub = uncollide (&a->f, &a->fo, _bb,
                       COLLISION_BACK_LEFT_FIGHT,
                       COLLISION_BACK_RIGHT_FIGHT,
                       &a->fo, NULL);

  bool uf = uncollide (&a->f, &a->fo, _bf,
                       COLLISION_FRONT_LEFT_FIGHT,
                       COLLISION_FRONT_RIGHT_FIGHT,
                       &a->fo, NULL);
  return ub || uf;
}
