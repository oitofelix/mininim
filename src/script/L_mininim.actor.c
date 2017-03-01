/*
  L_mininim.actor.c -- mininim.actor script module;

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

static const char *direction_string (enum dir d);
static enum dir direction_value (const char *d);

static const char *type_string (enum anim_type type);
static enum anim_type type_value (const char *type);

static const char *action_string (ACTION action);
static ACTION action_value (const char *action, enum anim_type type);

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

void
define_L_mininim_actor (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_ACTOR);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

void
L_pushactor (lua_State *L, int id)
{
  int *id_new = lua_newuserdata (L, sizeof (id_new));
  luaL_getmetatable (L, L_MININIM_ACTOR);
  lua_setmetatable (L, -2);
  *id_new = id;
}

BEGIN_LUA (L_mininim_actor)
{
  int id = luaL_checknumber (L, 1);
  L_pushactor (L, id);
  return 1;
}
END_LUA

const char *
direction_string (enum dir dir)
{
  switch (dir) {
  case LEFT: return "LEFT";
  case RIGHT: return "RIGHT";
  default: assert (0); return NULL;
  }
}

enum dir
direction_value (const char *dir)
{
  if (! strcasecmp (dir, "LEFT")) return LEFT;
  else if (! strcasecmp (dir, "RIGHT")) return RIGHT;
  else return INVALID;
}

const char *
type_string (enum anim_type type)
{
  switch (type) {
  case NO_ANIM: return "NONE";
  case KID: return "KID";
  case SHADOW: return "SHADOW";
  case GUARD: return "GUARD";
  case FAT_GUARD: return "FAT GUARD";
  case SKELETON: return "SKELETON";
  case VIZIER: return "VIZIER";
  case PRINCESS: return "PRINCESS";
  case MOUSE: return "MOUSE";
  default: assert (0); return NULL;
  }
}

enum anim_type
type_value (const char *type)
{
  if (! strcasecmp (type, "NONE")) return NO_ANIM;
  else if (! strcasecmp (type, "KID")) return KID;
  else if (! strcasecmp (type, "SHADOW")) return SHADOW;
  else if (! strcasecmp (type, "GUARD")) return GUARD;
  else if (! strcasecmp (type, "FAT GUARD")) return FAT_GUARD;
  else if (! strcasecmp (type, "SKELETON")) return SKELETON;
  else if (! strcasecmp (type, "VIZIER")) return VIZIER;
  else if (! strcasecmp (type, "PRINCESS")) return PRINCESS;
  else if (! strcasecmp (type, "MOUSE")) return MOUSE;
  else return INVALID;
}

const char *
action_string (ACTION action)
{
  if (action == kid_turn) return "TURN";
  else if (action == princess_turn) return "TURN";
  else if (action == kid_run) return "RUN";
  else if (action == mouse_run) return "RUN";
  else return "UNKNOWN";
}

ACTION
action_value (const char *action, enum anim_type type)
{
  if (! strcasecmp (action, "TURN")) {
    switch (type) {
    case KID: return kid_turn;
    case PRINCESS: return princess_turn;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "RUN")) {
    switch (type) {
    case KID: return kid_run;
    case MOUSE: return mouse_run;
    default: return NULL;
    }
  } else return NULL;
}

BEGIN_LUA (__eq)
{
  int *id0 = luaL_checkudata (L, 1, L_MININIM_ACTOR);
  int *id1 = luaL_checkudata (L, 2, L_MININIM_ACTOR);
  if (id0 && id1) lua_pushboolean (L, *id0 == *id1);
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  int *id_ptr = luaL_checkudata (L, 1, L_MININIM_ACTOR);

  int id;
  if (id_ptr) id = *id_ptr;
  else {
    lua_pushnil (L);
    return 1;
  }

  struct anim *a = get_anim_by_id (id);
  if (! a) {
    lua_pushnil (L);
    return 1;
  }

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "id")) {
      lua_pushnumber (L, id);
      return 1;
    } else if (! strcasecmp (key, "direction")) {
      lua_pushstring (L, direction_string (a->f.dir));
      return 1;
    } else if (! strcasecmp (key, "type")) {
      lua_pushstring (L, type_string (a->type));
      return 1;
    } else if (! strcasecmp (key, "action")) {
      lua_pushstring (L, action_string (a->action));
      return 1;
    } else if (! strcasecmp (key, "delayed_stand_up")) {
      lua_pushboolean (L, a->uncouch_slowly);
      return 1;
    } else break;
  default: break;
  }

  lua_pushnil (L);
  return 1;
}
END_LUA

BEGIN_LUA (__newindex)
{
  int *id_ptr = luaL_checkudata (L, 1, L_MININIM_ACTOR);

  int id;
  if (id_ptr) id = *id_ptr;
  else return 0;

  struct anim *a = get_anim_by_id (id);
  if (! a) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "direction")) {
      if (! lua_isstring (L, 3)) return 0;
      enum dir dir = direction_value (lua_tostring (L, 3));
      if (invalid (dir)) return 0;
      else if (dir != a->f.dir) invert_frame_dir (&a->f, &a->f);
      return 0;
    } else if (! strcasecmp (key, "type")) {
      if (! lua_isstring (L, 3)) return 0;
      enum anim_type type = type_value (lua_tostring (L, 3));
      if (invalid (type)) return 0;
      if (!id) return 0;
      a->type = type;
      return 0;
    } else if (! strcasecmp (key, "action")) {
      if (! lua_isstring (L, 3)) return 0;
      ACTION action = action_value (lua_tostring (L, 3), a->type);
      if (! action) return 0;
      a->next_action = action;
      return 0;
    } else if (! strcasecmp (key, "delayed_stand_up")) {
      a->uncouch_slowly = lua_toboolean (L, 3);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  int *id = luaL_checkudata (L, 1, L_MININIM_ACTOR);
  lua_pushfstring (L, L_MININIM_ACTOR " (%d)", id ? *id : -1);
  return 1;
}
END_LUA
