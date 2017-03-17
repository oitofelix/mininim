/*
  L_mininim.actor.h -- mininim.actor script module;

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

#ifndef MININIM_L_MININIM_ACTOR_H
#define MININIM_L_MININIM_ACTOR_H

#define L_MININIM_ACTOR L_MININIM ".actor"

void define_L_mininim_actor (lua_State *L);
void L_pushactor (lua_State *L, int id);

const char *actor_type_string (enum actor_type type);
enum actor_type actor_type_enum (const char *type);
const char *actor_action_string (ACTION action);
ACTION actor_action_enum (const char *action, enum actor_type type);

bool select_actor_frame (struct actor *a, const char *type,
                         const char *action, int i);
bool select_actor_xframe (struct actor *a, const char *type,
                          const char *xtype, int j);

ALLEGRO_BITMAP *actor_bitmap (struct actor *a, const char *type,
                              const char *action, int index);
struct coord *actor_coord (struct coord *c_ret, struct actor *a,
                           const char *type, const char *action,
                           int index);
int actor_bitmap_dx (struct actor *a, const char *type, const char *action,
                     int index);
int actor_bitmap_dy (struct actor *a, const char *type, const char *action,
                     int index);
void place_actor (struct actor *a, struct pos *p, int dx, int dy,
                  const char *type, const char *action, int index);


DECLARE_LUA (L_mininim_actor);

#endif	/* MININIM_L_MININIM_ACTOR_H */
