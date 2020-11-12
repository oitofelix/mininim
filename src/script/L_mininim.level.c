/*
  L_mininim.level.c -- mininim.level script module;

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

#include "mininim.h"

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

void
define_L_mininim_level (lua_State *L)
{
  luaL_newmetatable (L, L_MININIM_LEVEL);

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

  /* mininim.level.position */
  define_L_mininim_level_position (L);
}

const char *
env_mode_string (enum em em)
{
  switch (em) {
  case DUNGEON: return "DUNGEON"; break;
  case PALACE: return "PALACE"; break;
  default: assert (false); return NULL; break;
  }
}

const char *
hue_mode_string (enum hue hue)
{
  switch (hue) {
  case HUE_NONE: return "NONE"; break;
  case HUE_GREEN: return "GREEN"; break;
  case HUE_GRAY: return "GRAY"; break;
  case HUE_YELLOW: return "YELLOW"; break;
  case HUE_BLUE: return "BLUE"; break;
  default: assert (false); return NULL; break;
  }
}

BEGIN_LUA (__eq)
{
  lua_pushboolean (L, true);
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "number")) {
      lua_pushnumber (L, global_level.n);
      return 1;
    } else if (! strcasecmp (key, "env_mode")) {
      lua_pushstring (L, env_mode_string (global_level.em));
      return 1;
    } else if (! strcasecmp (key, "hue_mode")) {
      lua_pushstring (L, hue_mode_string (global_level.hue));
      return 1;
    } else if (! strcasecmp (key, "position")) {
      lua_pushcfunction (L, L_mininim_level_position);
      return 1;
    } else if (! strcasecmp (key, "retry")) {
      lua_pushboolean (L, retry_level == global_level.n
                       && replay_mode == NO_REPLAY);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "number")) {
      int n = lua_tonumber (L, 3);
      ui_jump_to_level (n);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushfstring (L, "MININIM LEVEL %d INTERFACE", global_level.n);
  return 1;
}
END_LUA
