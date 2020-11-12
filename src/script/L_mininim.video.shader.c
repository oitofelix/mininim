/*
  L_mininim.video.shader.c -- mininim.video.shader script module;

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

static DECLARE_LUA (__gc);
static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

void
define_L_mininim_video_shader (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_SHADER);

  lua_pushstring (L, "__gc");
  lua_pushcfunction (L, __gc);
  lua_rawset (L, -3);

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
L_pushshader (lua_State *L, ALLEGRO_SHADER *s)
{
  if (s) {
    ALLEGRO_SHADER **s_new = lua_newuserdata (L, sizeof (*s_new));
    luaL_getmetatable (L, L_MININIM_VIDEO_SHADER);
    lua_setmetatable (L, -2);
    *s_new = s;
  } else lua_pushnil (L);
}

BEGIN_LUA (L_mininim_video_shader)
{
  ALLEGRO_SHADER *s = al_create_shader (ALLEGRO_SHADER_AUTO);

  if (! s) {
    warning ("cannot create shader");
    return 0;
  }

  int i;
  for (i = 1; i <= lua_gettop (L); i++) {
    if (lua_type (L, i) != LUA_TSTRING) {
      al_destroy_shader (s);
      return luaL_argerror (L, 1, "invalid argument, expecting (filename)");
    }
    const char *filename = lua_tostring (L, i);
    if (! attach_shader_source_file (s, filename)) {
      const char *log = al_get_shader_log (s);
      if (strcmp (log, ""))
        warning ("cannot attach shader source file '%s'\n%s",
                 filename,
		 log);
      else
	warning ("cannot find shader source file '%s'",
		 filename);
      al_destroy_shader (s);
      return 0;
    }
  }

  if (! al_build_shader (s)) {
    warning ("cannot build shader\n%s",
	     al_get_shader_log (s));
    al_destroy_shader (s);
    return 0;
  }

  L_pushshader (L, s);

  return 1;
}
END_LUA

BEGIN_LUA (__gc)
{
  ALLEGRO_SHADER **s = luaL_testudata (L, 1, L_MININIM_VIDEO_SHADER);
  al_destroy_shader (*s);
  return 0;
}
END_LUA

BEGIN_LUA (__eq)
{
  ALLEGRO_SHADER **s0 = luaL_testudata (L, 1, L_MININIM_VIDEO_SHADER);
  ALLEGRO_SHADER **s1 = luaL_testudata (L, 2, L_MININIM_VIDEO_SHADER);
  if (s0 && s1) lua_pushboolean (L, *s0 == *s1);
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  ALLEGRO_SHADER **s_ptr = luaL_testudata (L, 1, L_MININIM_VIDEO_SHADER);

  ALLEGRO_SHADER *s;
  if (s_ptr) s = *s_ptr;
  else return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "log")) {
      const char *log = al_get_shader_log (s);
      lua_pushstring (L, log);
      return 1;
    } else if (! strcasecmp (key, "platform")) {
      lua_pushstring (L, get_shader_platform (s));
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__newindex)
{
  ALLEGRO_SHADER **s_ptr = luaL_testudata (L, 1, L_MININIM_VIDEO_SHADER);

  ALLEGRO_SHADER *s;
  if (s_ptr) s = *s_ptr;
  else return 0;

  al_set_target_backbuffer (display);
  al_use_shader (s);

  const char *key;
  int key_type = lua_type (L, 2);
  switch (key_type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    int value_type = lua_type (L, 3);
    switch (value_type) {
    case LUA_TUSERDATA: {
      ALLEGRO_BITMAP **value_ptr = luaL_testudata (L, 3, L_MININIM_VIDEO_BITMAP);
      /* QUESTION: Is assuming a fixed unit of 1 appropriate or
         enough? */
      if (value_ptr) al_set_shader_sampler (key, *value_ptr, 1);
      break;
    }
    case LUA_TSTRING: {
      int value = lua_tonumber (L, 3);
      al_set_shader_int (key, value);
      break;
    }
    case LUA_TNUMBER: {
      float value = lua_tonumber (L, 3);
      al_set_shader_float (key, value);
      break;
    }
    case LUA_TTABLE: {
      /* TODO: implement matrix uniform setting using
         al_set_shader_matrix in case Allegro transforms get exposed
         to the Lua world */
      int num_elems = lua_getn (L, 3);
      lua_rawgeti (L, 3, 1);
      int type = lua_type (L, -1);
      if (type == LUA_TTABLE) {
        int num_components = lua_getn (L, -1);
        if (num_components < 1 || num_components > 4) break;
        lua_rawgeti (L, -1, 1);
        int type = lua_type (L, -1);
        lua_pop (L, 2);
        void *v = xcalloc (num_elems * num_components, (type == LUA_TSTRING)
                           ? sizeof (int) : sizeof (float));
        for (int i = 0; i < num_elems; i++) {
          lua_rawgeti (L, 3, i + 1);
          for (int j = 0; j < num_components; j++)
            if (type == LUA_TSTRING)
              ((int *) v)[i * num_components + j] =
                L_rawgeti_tonumber (L, -1, j + 1);
            else
              ((float *) v)[i * num_components + j] =
                L_rawgeti_tonumber (L, -1, j + 1);
          lua_pop (L, 1);
        }
        if (type == LUA_TSTRING)
          al_set_shader_int_vector
            (key, num_components, (int *) v, num_elems);
        else
          al_set_shader_float_vector
            (key, num_components, (float *) v, num_elems);
        al_free (v);
      } else {
        lua_pop (L, 1);
        void *v = xcalloc (num_elems, (type == LUA_TSTRING)
                           ? sizeof (int) : sizeof (float));
        for (int i = 0; i < num_elems; i++) {
          if (type == LUA_TSTRING)
            ((int *) v)[i] = L_rawgeti_tonumber (L, 3, i + 1);
          else ((float *) v)[i] = L_rawgeti_tonumber (L, 3, i + 1);
        }
        if (type == LUA_TSTRING)
          al_set_shader_int_vector (key, 1, (int *) v, num_elems);
        else
          al_set_shader_float_vector (key, 1, (float *) v, num_elems);
        al_free (v);
      }
      break;
    }
    default: {
      bool value = lua_toboolean (L, 3);
      al_set_shader_bool (key, value);
      break;
    }
    }
  default: break;
  }

  al_use_shader (NULL);
  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  ALLEGRO_SHADER **s = luaL_testudata (L, 1, L_MININIM_VIDEO_SHADER);
  lua_pushfstring (L, L_MININIM_VIDEO_SHADER " (%s)",
                   get_shader_platform (*s));
  return 1;
}
END_LUA
