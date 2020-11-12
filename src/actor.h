/*
  actor.h -- actor module;

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

#ifndef MININIM_ACTOR_H
#define MININIM_ACTOR_H

extern struct actor *actor;
extern size_t actor_nmemb;

int create_actor (struct actor *a0, enum actor_type t,
                 struct pos *p, enum dir dir);
void destroy_actor (struct actor *a);
void destroy_actors (void);
struct actor *get_next_controllable (struct actor *k);
void select_controllable_by_id (int id);
struct actor *get_reciprocal_enemy (struct actor *k);
struct actor *get_actor_by_id (int id);
struct actor *get_actor_dead_at_pos (struct pos *p);
struct actor *get_guard_actor_by_level_id (int id);
void draw_actor_frame (ALLEGRO_BITMAP *bitmap, struct actor *a);
void draw_actors (ALLEGRO_BITMAP *bitmap);
int compare_actors (const void *a0, const void *a1);
void draw_actor_if_at_pos (ALLEGRO_BITMAP *bitmap, struct actor *a,
                          struct pos *p);
void clear_actors_keyboard_state (void);
bool is_actor_dead (struct actor *a);
bool is_actor_chomped (struct actor *a);
bool is_actor_fall (struct actor *a);
bool is_fightable_actor (struct actor *a);
void actor_die_suddenly (struct actor *a);
void actor_die_spiked (struct actor *a);
void actor_die_chomped (struct actor *a);
void actor_die (struct actor *a);
void actor_sword_hit (struct actor *a);
void actor_fall (struct actor *a);
void actor_walkf (struct actor *a);
void actor_walkb (struct actor *a);
struct coord *splash_coord (ALLEGRO_BITMAP *splash, struct frame *f,
                            struct coord *c);
ALLEGRO_COLOR start_actor_palette (ALLEGRO_COLOR c, void *data);


bool uncollide_static_neutral (struct actor *a);
bool uncollide_static_kid_normal (struct actor *a);
bool uncollide_static_fall (struct actor *a);
bool uncollide_static_die (struct actor *a);
bool uncollide_static_fight (struct actor *a);
bool is_colliding_front_fight (struct actor *a);
bool is_colliding_back_fight (struct actor *a);
bool uncollide_front_fight (struct actor *a);
bool uncollide_back_fight (struct actor *a);

#endif	/* MININIM_ACTOR_H */
