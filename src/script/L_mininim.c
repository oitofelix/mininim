/*
  L_mininim.c -- mininim script module;

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

static int load_hook_ref = LUA_NOREF;

static int __eq (lua_State *L);
static int __index (lua_State *L);
static int __newindex (lua_State *L);
static int __tostring (lua_State *L);

void
define_L_mininim (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM);

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

  /* mininim.level */
  define_L_mininim_level (L);

  /* mininim.actor */
  define_L_mininim_actor (L);

  /* mininim.audio */
  define_L_mininim_audio (L);

  /* mininim.video */
  define_L_mininim_video (L);

  /* mininim.setting */
  define_L_mininim_setting (L);
}

void
run_load_hook (lua_State *L)
{
  L_get_registry (L, load_hook_ref);
  L_run_hook (L);
}

int
__eq (lua_State *L)
{
  lua_pushboolean (L, true);
  return 1;
}

int
__index (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "cycle")) {
      lua_pushnumber (L, anim_cycle);
      return 1;
    } else if (! strcasecmp (key, "place_width")) {
      lua_pushnumber (L, PLACE_WIDTH);
      return 1;
    } else if (! strcasecmp (key, "place_height")) {
      lua_pushnumber (L, PLACE_HEIGHT);
      return 1;
    } else if (! strcasecmp (key, "load_hook")) {
      L_get_registry (L, load_hook_ref);
      return 1;
    } else if (! strcasecmp (key, "level")) {
      L_push_interface (L, L_MININIM_LEVEL);
      return 1;
    } else if (! strcasecmp (key, "actor")) {
      lua_pushcfunction (L, L_mininim_actor);
      return 1;
    } else if (! strcasecmp (key, "audio")) {
      L_push_interface (L, L_MININIM_AUDIO);
      return 1;
    } else if (! strcasecmp (key, "video")) {
      L_push_interface (L, L_MININIM_VIDEO);
      return 1;
    } else if (! strcasecmp (key, "setting")) {
      L_push_interface (L, L_MININIM_SETTING);
      return 1;
    } else if (! strcasecmp (key, "clipboard")) {
      if (al_clipboard_has_text (display)) {
        char *text = al_get_clipboard_text (display);
        if (text) al_free (text);
        text = al_get_clipboard_text (display);
        lua_pushstring (L, text);
        if (text) al_free (text);
      } else lua_pushnil (L);
      return 1;
    } else break;
  default: break;
  }

  lua_pushnil (L);
  return 1;
}

int
__newindex (lua_State *L)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "load_hook")) {
      L_set_registry (L, &load_hook_ref);
      return 0;
    } else if (! strcasecmp (key, "clipboard")) {
      if (lua_isstring (L, 3)) {
        const char *text = lua_tostring (L, 3);
        al_set_clipboard_text (display, text);
      }
      return 0;
    } else break;
  default: break;
  }

  return 0;
}

int
__tostring (lua_State *L)
{
  lua_pushfstring (L, "MININIM %s INTERFACE", VERSION);
  return 1;
}
