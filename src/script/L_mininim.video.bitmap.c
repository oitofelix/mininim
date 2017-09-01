/*
  L_mininim.video.bitmap.c -- mininim.video.bitmap script module;

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

ALLEGRO_BITMAP *L_target_bitmap;

static DECLARE_LUA (__gc);
static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__tostring);

static DECLARE_LUA (L_apply_palette);
static DECLARE_LUA (draw);
static DECLARE_LUA (set_pixel);
static DECLARE_LUA (get_pixel);
static DECLARE_LUA (sub);

void
define_L_mininim_video_bitmap (lua_State *L)
{
  luaL_newmetatable(L, L_MININIM_VIDEO_BITMAP);

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
L_pushbitmap (lua_State *L, ALLEGRO_BITMAP *b)
{
  if (b) {
    L_get_weak_registry_by_ptr (L, b);
    if (lua_isnil (L, -1)) {
      lua_pop (L, 1);
      ALLEGRO_BITMAP **b_new = lua_newuserdata (L, sizeof (*b_new));
      luaL_getmetatable (L, L_MININIM_VIDEO_BITMAP);
      lua_setmetatable (L, -2);
      *b_new = b;

      /* put bitmap into weak registry for GC tracking */
      lua_pushvalue (L, -1);
      L_set_weak_registry_by_ptr (L, b);
    }
  } else lua_pushnil (L);
}

BEGIN_LUA (L_mininim_video_bitmap)
{
  ALLEGRO_BITMAP *b = NULL;

  if (lua_type (L, 1) == LUA_TSTRING) {
    const char *filename = lua_tostring (L, 1);
    b = load_bitmap (filename);
  } else if (lua_isnumber (L, 1) && lua_isnumber (L, 2)) {
    int width = lua_tonumber (L, 1);
    int height = luaL_checknumber (L, 2);
    b = create_bitmap (width, height);
    clear_bitmap (b, al_map_rgba (0, 0, 0, 0));
    validate_bitmap_for_mingw (b);
  } else return luaL_argerror (L, 1, "invalid argument, expecting (filename)"
                               " or (width, height)");
  L_pushbitmap (L, b);
  return 1;
}
END_LUA

BEGIN_LUA (__gc)
{
  ALLEGRO_BITMAP **b = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  al_destroy_bitmap (*b);
  return 0;
}
END_LUA

BEGIN_LUA (__eq)
{
  ALLEGRO_BITMAP **b0 = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  ALLEGRO_BITMAP **b1 = luaL_checkudata (L, 2, L_MININIM_VIDEO_BITMAP);
  if (b0 && b1) lua_pushboolean (L, *b0 == *b1);
  else lua_pushboolean (L, lua_rawequal (L, 1, 2));
  return 1;
}
END_LUA

BEGIN_LUA (__index)
{
  ALLEGRO_BITMAP **b_ptr = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);

  ALLEGRO_BITMAP *b;
  if (b_ptr) b = *b_ptr;
  else return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "width")) {
      int width = get_bitmap_width (b);
      lua_pushnumber (L, width);
      return 1;
    } else if (! strcasecmp (key, "height")) {
      int height = get_bitmap_height (b);
      lua_pushnumber (L, height);
      return 1;
    } else if (! strcasecmp (key, "apply_palette")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, L_apply_palette, 1);
      return 1;
    } else if (! strcasecmp (key, "draw")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, draw, 1);
      return 1;
    } else if (! strcasecmp (key, "set_pixel")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, set_pixel, 1);
      return 1;
    } else if (! strcasecmp (key, "get_pixel")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, get_pixel, 1);
      return 1;
    } else if (! strcasecmp (key, "sub")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, sub, 1);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  ALLEGRO_BITMAP **b = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  lua_pushfstring (L, L_MININIM_VIDEO_BITMAP " %dx%d",
                   b ? get_bitmap_width (*b) : 0,
                   b ? get_bitmap_height (*b) : 0);
  return 1;
}
END_LUA

static lua_State *state_L;

ALLEGRO_COLOR
L_palette (ALLEGRO_COLOR c)
{
  lua_State *L = state_L;
  lua_pushvalue (L, -1);
  L_pushcolor (L, c);
  ALLEGRO_COLOR c_ret;
  L_call (L, 1, 1);
  ALLEGRO_COLOR *c_ptr = luaL_checkudata (L, -1, L_MININIM_VIDEO_COLOR);
  if (c_ptr) c_ret = *c_ptr;
  else c_ret = c;
  lua_pop (L, 1);
  return c_ret;
}

BEGIN_LUA (L_apply_palette)
{
  ALLEGRO_BITMAP **b =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_BITMAP);

  if (! b) return 0;

  /* apply palette */
  luaL_checktype (L, 1, LUA_TFUNCTION);

  void *cache_key;
  if (lua_isboolean (L, 2) && lua_toboolean (L, 2))
    cache_key = (void *) lua_topointer (L, 1);
  else cache_key = (void *) lua_topointer (L, 2);

  lua_settop (L, 1);

  state_L = L;
  ALLEGRO_BITMAP *r = apply_palette_k (*b, L_palette, cache_key);
  lua_pop (L, 1);

  /* push result */
  L_pushbitmap (L, r);
  return 1;
}
END_LUA

BEGIN_LUA (draw)
{
  ALLEGRO_BITMAP **b =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_BITMAP);

  if (! b) return 0;

  int arg_offset = 0;

  ALLEGRO_BITMAP *target_bitmap = NULL;

  ALLEGRO_BITMAP **target_bitmap_ptr =
    luaL_checkudata (L, arg_offset + 1, L_MININIM_VIDEO_BITMAP);

  if (target_bitmap_ptr) {
    target_bitmap = *target_bitmap_ptr;
    arg_offset++;
  } else if (L_target_bitmap) target_bitmap = L_target_bitmap;
  else return luaL_error (L, "cannot draw");

  struct rect *r =
    luaL_checkudata (L, arg_offset + 1, L_MININIM_VIDEO_RECTANGLE);

  int dx, dy;
  if (r) arg_offset++;
  else if (lua_isnumber (L, arg_offset + 1)
           && lua_isnumber (L, arg_offset + 2)) {
    dx = lua_tonumber (L, arg_offset + 1);
    dy = lua_tonumber (L, arg_offset + 2);
    arg_offset += 2;
  } else return luaL_error (L, "bad argument #%d: %s or %s expected",
                            arg_offset + 1, L_MININIM_VIDEO_RECTANGLE,
                            "(dx, dy)");

  float sx = lua_tonumber (L, arg_offset + 1);
  float sy = lua_tonumber (L, arg_offset + 2);

  float sw;
  if (lua_isnumber (L, arg_offset + 3))
    sw = lua_tonumber (L, arg_offset + 3);
  else if (r && r->w > 0) sw = r->w;
  else sw = get_bitmap_width (*b);

  float sh;
  if (lua_isnumber (L, arg_offset + 4))
    sh = lua_tonumber (L, arg_offset + 4);
  else if (r && r->h > 0) sh = r->h;
  else sh = get_bitmap_height (*b);

  ALLEGRO_SHADER **s =
    luaL_checkudata (L, lua_gettop (L), L_MININIM_VIDEO_SHADER);

  if (s) {
    al_set_target_bitmap (target_bitmap);
    al_use_shader (*s);
  }

  if (r) draw_bitmap_regionc (*b, target_bitmap, sx, sy, sw, sh, &r->c, 0);
  else draw_bitmap_region (*b, target_bitmap, sx, sy, sw, sh, dx, dy, 0);

  if (s) {
    al_set_target_bitmap (target_bitmap);
    al_use_shader (NULL);
  }

  return 0;
}
END_LUA

BEGIN_LUA (set_pixel)
{
  int x = luaL_checknumber (L, 1);
  int y = luaL_checknumber (L, 2);
  ALLEGRO_COLOR *c = luaL_checkudata (L, 3, L_MININIM_VIDEO_COLOR);
  luaL_argcheck (L, c, 3, "expecting " L_MININIM_VIDEO_COLOR);

  ALLEGRO_BITMAP **b =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_BITMAP);

  if (! b) return 0;

  al_set_target_bitmap (*b);
  /* al_lock_bitmap (*b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE); */
  al_put_pixel (x, y, *c);

  return 0;
}
END_LUA

BEGIN_LUA (get_pixel)
{
  int x = luaL_checknumber (L, 1);
  int y = luaL_checknumber (L, 2);

  ALLEGRO_BITMAP **b =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_BITMAP);

  if (! b) return 0;

  /* al_lock_bitmap (*b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY); */
  ALLEGRO_COLOR c = al_get_pixel (*b, x, y);
  L_pushcolor (L, c);
  return 1;
}
END_LUA

BEGIN_LUA (sub)
{
  ALLEGRO_BITMAP **b =
    luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_VIDEO_BITMAP);

  if (! b) return 0;

  int x = lua_tonumber (L, 1);
  int y = lua_tonumber (L, 2);

  int w = lua_isnumber (L, 3)
    ? lua_tonumber (L, 3) : get_bitmap_width (*b) - x;
  int h = lua_isnumber (L, 4)
    ? lua_tonumber (L, 4) : get_bitmap_height (*b) - y;

  ALLEGRO_BITMAP *sb = al_create_sub_bitmap (*b, x, y, w, h);
  L_pushbitmap (L, sb);
  return 1;
}
END_LUA
