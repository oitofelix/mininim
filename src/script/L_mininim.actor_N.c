/*
  L_mininim.actor_N.c -- mininim.actor[?] script module;

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

static int __index (lua_State *L);
static int __newindex (lua_State *L);

static const char *direction_string (enum dir d);
static enum dir direction_value (const char *d);
static int direction_error (lua_State *L, int id);

static const char *type_string (enum anim_type type);
static enum anim_type type_value (const char *type);
static int type_error (lua_State *L, int id);

static const char *action_string (ACTION action);
static ACTION action_value (const char *action, enum anim_type type);
static int action_error (lua_State *L, int id, enum anim_type type);

void
define_L_mininim_actor_N (lua_State *L)
{
  luaL_newmetatable(L, "mininim.actor[?]");

  lua_pushstring (L, "__tostring");
  lua_pushstring (L, "mininim.actor[?]");
  lua_pushcclosure (L, L__tostring, 1);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__newindex");
  lua_pushcfunction (L, __newindex);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

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

int
direction_error (lua_State *L, int id)
{
  return
    luaL_error (L, "mininim.actor[%d].direction must be either "
                "\"%s\" or \"%s\"", id,
                direction_string (LEFT),
                direction_string (RIGHT));
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

int
type_error (lua_State *L, int id)
{
  return
    luaL_error (L, "mininim.actor[%d].type must be "
                "\"%s\", \"%s\", \"%s\", \"%s\", \"%s\","
                " \"%s\", \"%s\", \"%s\", or \"%s\" ", id,
                type_string (NO_ANIM),
                type_string (KID),
                type_string (SHADOW),
                type_string (GUARD),
                type_string (FAT_GUARD),
                type_string (SKELETON),
                type_string (VIZIER),
                type_string (PRINCESS),
                type_string (MOUSE));
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

int
action_error (lua_State *L, int id, enum anim_type type)
{
  switch (type) {
  case KID:
    return luaL_error (L, "mininim.actor[%d].action must be "
                       "\"%s\" or \"%s\"", id,
                       action_string (kid_turn),
                       action_string (kid_run));
  case PRINCESS:
    return luaL_error (L, "mininim.actor[%d].action must be "
                       "\"%s\"", id, action_string (princess_turn));
  case MOUSE:
    return luaL_error (L, "mininim.actor[%d].action must be "
                       "\"%s\"", id, action_string (mouse_run));
  default:
    return luaL_error (L, "mininim.actor[%d].action read-only", id);
  }
}

int
__index (lua_State *L)
{
  int *id = L_check_type (L, 1, "mininim.actor[?]");

  struct anim *a = get_anim_by_id (*id);
  if (! a) return luaL_error (L, "mininim.actor[%d] does not exist", *id);

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "id")) {
      lua_pushnumber (L, *id);
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
    } else return L_error_invalid_key_string (L, key, "mininim.actor[?]");
  default: return L_error_invalid_key_type (L, type, "mininim.actor[?]");
  }
}

int
__newindex (lua_State *L)
{
  int *id = L_check_type (L, 1, "mininim.actor[?]");

  struct anim *a = get_anim_by_id (*id);
  if (! a) return luaL_error (L, "mininim.actor[%d] does not exist", *id);

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "id")) {
      return luaL_error (L, "mininim.actor[%d].id is read-only", *id);
    } else if (! strcasecmp (key, "direction")) {
      if (! lua_isstring (L, 3)) return direction_error (L, *id);
      enum dir dir = direction_value (lua_tostring (L, 3));
      if (invalid (dir)) return direction_error (L, *id);
      else if (dir != a->f.dir) invert_frame_dir (&a->f, &a->f);
      return 0;
    } else if (! strcasecmp (key, "type")) {
      if (! lua_isstring (L, 3)) return type_error (L, *id);
      enum anim_type type = type_value (lua_tostring (L, 3));
      if (invalid (type)) return type_error (L, *id);
      if (!*id) return luaL_error (L, "mininim.actor[%d].id is read-only", *id);
      a->type = type;
      return 0;
    } else if (! strcasecmp (key, "action")) {
      if (! lua_isstring (L, 3)) return action_error (L, *id, a->type);
      ACTION action = action_value (lua_tostring (L, 3), a->type);
      if (! action) return action_error (L, *id, a->type);
      a->next_action = action;
      return 0;
    } else if (! strcasecmp (key, "delayed_stand_up")) {
      a->uncouch_slowly = lua_toboolean (L, 3);
      return 0;
    } else return L_error_invalid_key_string (L, key, "mininim.actor[?]");
  default: return L_error_invalid_key_type (L, type, "mininim.actor[?]");
  }
}
