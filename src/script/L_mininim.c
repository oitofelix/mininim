/*
  L_mininim.c -- mininim script module;

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

int mininim_lua_ref = LUA_NOREF;

static DEFUN (__eq);
static DEFUN (__index);
static DEFUN (__newindex);
static DEFUN (__tostring);

static DEFUN (quit);
static DEFUN (rest);

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

  /* lua table */
  lua_newtable (L);
  L_set_registry_by_ref (L, &mininim_lua_ref);

  /* global shortcuts */
  lua_register (L, "_quit", quit);

  /* mininim.level */
  define_L_mininim_level (L);

  /* mininim.actor */
  define_L_mininim_actor (L);

  /* mininim.audio */
  define_L_mininim_audio (L);

  /* mininim.video */
  define_L_mininim_video (L);

  /* mininim.settings */
  define_L_mininim_settings (L);

  /* mininim.mouse */
  define_L_mininim_mouse (L);

  /* mininim.multiroom */
  define_L_mininim_multiroom (L);

  /* mininim.profiler */
  define_L_mininim_profiler (L);

  /* mininim.debugger */
  define_L_mininim_debugger (L);

  /* mininim.math */
  define_L_mininim_math (L);
}

void
error_lua_invalid (const char *template, ...)
{
  va_list ap;
  va_start (ap, template);
  char *name = xvasprintf (template, ap);
  warning ("'MININIM.lua.%s' invalid",
	   name);
  al_free (name);
  va_end (ap);
}

bool
call_lua_function (lua_State *L, const char *name, int nargs, int nresults)
{
  L_get_registry_by_ref (L, mininim_lua_ref);

  lua_pushstring (L, name);
  lua_rawget (L, -2);
  lua_remove (L, -2);

  if (! lua_isfunction (L, -1)) {
    error_lua_invalid ("%s", name);
    lua_pop (L, 1);
    return false;
  }

  return L_call (L, nargs, nresults);
}

bool
run_lua_hook (lua_State *L, const char *name)
{
  /* lock_lua (); */
  L_get_registry_by_ref (L, mininim_lua_ref);
  lua_pushstring (L, name);
  lua_rawget (L, -2);
  lua_remove (L, -2);
  bool error = L_run_hook (L);
  /* unlock_lua (); */
  return error;
}

DEFUN (__eq)
{
  lua_pushboolean (L, true);
  return 1;
}

DEFUN (__index)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "cycle")) {
      lua_pushnumber (L, anim_cycle);
      return 1;
    } else if (! strcasecmp (key, "tick")) {
      lua_pushnumber (L, anim_tick);
      return 1;
    } else if (! strcasecmp (key, "place_width")) {
      lua_pushnumber (L, PLACE_WIDTH);
      return 1;
    } else if (! strcasecmp (key, "place_height")) {
      lua_pushnumber (L, PLACE_HEIGHT);
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
    } else if (! strcasecmp (key, "settings")) {
      L_push_interface (L, L_MININIM_SETTINGS);
      return 1;
    } else if (! strcasecmp (key, "mouse")) {
      L_push_interface (L, L_MININIM_MOUSE);
      return 1;
    } else if (! strcasecmp (key, "multiroom")) {
      L_push_interface (L, L_MININIM_MULTIROOM);
      return 1;
    } else if (! strcasecmp (key, "profiler")) {
      L_push_interface (L, L_MININIM_PROFILER);
      return 1;
    } else if (! strcasecmp (key, "debugger")) {
      L_push_interface (L, L_MININIM_DEBUGGER);
      return 1;
    } else if (! strcasecmp (key, "math")) {
      L_push_interface (L, L_MININIM_MATH);
      return 1;
    } else if (! strcasecmp (key, "quit")) {
      lua_pushcfunction (L, quit);
      return 1;
    } else if (! strcasecmp (key, "lua")) {
      L_get_registry_by_ref (L, mininim_lua_ref);
      return 1;
    } else if (! strcasecmp (key, "editing")) {
      lua_pushboolean (L, edit != EDIT_NONE);
      return 1;
    } else if (! strcasecmp (key, "paused")) {
      lua_pushboolean (L, is_game_paused ());
      return 1;
    } else if (! strcasecmp (key, "clipboard")) {
      if (al_clipboard_has_text (display)) {
        char *text = al_get_clipboard_text (display);
        al_free (text);
        text = al_get_clipboard_text (display);
        lua_pushstring (L, text);
        al_free (text);
      } else lua_pushnil (L);
      return 1;
    } else if (! strcasecmp (key, "rest")) {
      lua_pushcfunction (L, rest);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}

DEFUN (__newindex)
{
  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "clipboard")) {
      if (lua_isstring (L, 3)) {
        const char *text = lua_tostring (L, 3);
        al_set_clipboard_text (display, text);
      }
      return 0;
    } else if (! strcasecmp (key, "paused")) {
      bool pause = lua_toboolean (L, 3);
      pause_game (pause);
      return 0;
    } else break;
  default: break;
  }

  return 0;
}

DEFUN (__tostring)
{
  lua_pushfstring (L, "MININIM %s INTERFACE", VERSION);
  return 1;
}

DEFUN (quit)
{
  quit_anim = QUIT_GAME;
  return 0;
}

DEFUN (rest)
{
  double seconds = lua_tonumber (L, 1);
  unlock_lua ();
  al_rest (seconds);
  lock_lua ();
  return 0;
}
