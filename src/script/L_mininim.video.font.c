/*
  L_mininim.video.font.c -- mininim.video.font script module;

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

static DECLARE_LUA (__gc);
static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__tostring);

static DECLARE_LUA (draw);

void
define_L_mininim_video_font (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_FONT);

  lua_pushstring (L, "__gc");
  lua_pushcfunction (L, __gc);
  lua_rawset (L, -3);

  lua_pushstring (L, "__eq");
  lua_pushcfunction (L, __eq);
  lua_rawset (L, -3);

  lua_pushstring (L, "__index");
  lua_pushcfunction (L, __index);
  lua_rawset (L, -3);

  lua_pushstring (L, "__tostring");
  lua_pushcfunction (L, __tostring);
  lua_rawset (L, -3);

  lua_pop (L, 1);
}

void
L_pushfont (lua_State *L, ALLEGRO_FONT *f)
{
  if (f) {
    L_get_weak_registry_by_ptr (L, f);
    if (lua_isnil (L, -1)) {
      lua_pop (L, 1);
      ALLEGRO_FONT **f_new = lua_newuserdata (L, sizeof (*f_new));
      luaL_getmetatable (L, L_MININIM_VIDEO_FONT);
      lua_setmetatable (L, -2);
      *f_new = f;

      /* put font into weak registry for GC tracking */
      lua_pushvalue (L, -1);
      L_set_weak_registry_by_ptr (L, f);
    }
  } else lua_pushnil (L);
}

BEGIN_LUA (L_mininim_video_font)
{
  ALLEGRO_FONT *f = NULL;
  ALLEGRO_BITMAP **b = luaL_testudata (L, 1, L_MININIM_VIDEO_BITMAP);

  if (b) {
    int *ranges = NULL;
    size_t ranges_nmemb = 0;

    int i;
    for (i = 2; i <= lua_gettop (L); i++) {
      int r = lua_tonumber (L, i);
      ranges = add_to_array (&r, 1, ranges, &ranges_nmemb,
                             ranges_nmemb, sizeof (r));
    }
    int ranges_n = ranges_nmemb / 2;
    f = al_grab_font_from_bitmap (*b, ranges_n, ranges);
    al_free (ranges);
    if (! f)
      fprintf (stderr, "error: cannot load font from bitmap"
               " (incorrect ranges?)\n");
  } else if (lua_type (L, 1) == LUA_TSTRING) {
    const char *filename = lua_tostring (L, 1);
    f = load_font (filename);
  } else if (! lua_gettop (L)) f = builtin_font;
    else return luaL_argerror (L, 1, "invalid argument, expecting (filename)"
                               " or (bitmap, range_start, range_end, ...)");

  L_pushfont (L, f);
  return 1;
}
END_LUA

BEGIN_LUA (__gc)
{
  ALLEGRO_FONT **f = luaL_testudata (L, 1, L_MININIM_VIDEO_FONT);
  al_destroy_font (*f);
  return 0;
}
END_LUA

BEGIN_LUA (__eq)
{
  ALLEGRO_FONT **f0 = luaL_testudata (L, 1, L_MININIM_VIDEO_FONT);
  ALLEGRO_FONT **f1 = luaL_testudata (L, 2, L_MININIM_VIDEO_FONT);
  if (f0 && f1) lua_pushboolean (L, *f0 == *f1);
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  ALLEGRO_FONT **f_ptr = luaL_testudata (L, 1, L_MININIM_VIDEO_FONT);

  ALLEGRO_FONT *f;
  if (f_ptr) f = *f_ptr;
  else return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "height")) {
      int h = al_get_font_line_height (f);
      lua_pushnumber (L, h);
      return 1;
    } else if (! strcasecmp (key, "draw")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, draw, 1);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  ALLEGRO_FONT **f = luaL_testudata (L, 1, L_MININIM_VIDEO_FONT);
  lua_pushfstring (L, L_MININIM_VIDEO_FONT " (%d ranges)",
                   f ? al_get_font_ranges (*f, 0, NULL) : 0);
  return 1;
}
END_LUA

BEGIN_LUA (draw)
{
  ALLEGRO_FONT **f =
    luaL_testudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_FONT);

  if (! f) return 0;

  const char *text = luaL_checkstring (L, 1);

  ALLEGRO_COLOR c = WHITE;
  float x = NAN, y = NAN;
  int flags = ALLEGRO_ALIGN_CENTRE;
  ALLEGRO_BITMAP *target_bitmap = L_target_bitmap;

  int i;
  for (i = 2; i <= lua_gettop (L); i++) {
    ALLEGRO_BITMAP **b_ptr = luaL_testudata (L, i, L_MININIM_VIDEO_BITMAP);
    ALLEGRO_COLOR *c_ptr = luaL_testudata (L, i, L_MININIM_VIDEO_COLOR);
    if (b_ptr) target_bitmap = *b_ptr;
    else if (c_ptr) c = *c_ptr;
    else if (lua_isnumber (L, i)) {
      if (isnan (x)) x = lua_tonumber (L, i);
      else if (isnan (y)) y = lua_tonumber (L, i);
    } else if (lua_isstring (L, i)) {
      const char *flag_str = lua_tostring (L, i);
      if (! strcasecmp (flag_str, "LEFT"))
        flags = ALLEGRO_ALIGN_LEFT;
      else if (! strcasecmp (flag_str, "CENTER"))
        flags = ALLEGRO_ALIGN_CENTRE;
      else if (! strcasecmp (flag_str, "RIGHT"))
        flags = ALLEGRO_ALIGN_RIGHT;
    }
  }

  if (isnan (x)) x = REAL_WIDTH / 2.0;
  if (isnan (y)) y = REAL_HEIGHT - al_get_font_line_height (*f) + 1;

  draw_text (target_bitmap, *f, c, x, y, flags, text);

  return 0;
}
END_LUA
