/*
  L_mininim.video.c -- mininim.video script module;

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

int REAL_WIDTH = ORIGINAL_WIDTH;
int REAL_HEIGHT = ORIGINAL_HEIGHT;

static DECLARE_LUA ( __eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__tostring);

static bool L_push_video_routine (lua_State *L);
static void video_command_error (const char *command, const char *object,
                                 uintptr_t index, uintptr_t part);

void
define_L_mininim_video (lua_State *L)
{
  /* mininim.video */
  luaL_newmetatable(L, L_MININIM_VIDEO);

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

  /* create video mode table */
  L_get_registry_by_ref (L, mininim_lua_ref);
  lua_pushstring (L, "video_mode");
  lua_newtable (L);
  lua_rawset (L, -3);
  lua_pop (L, 1);

  /* mininim.video.bitmap */
  define_L_mininim_video_bitmap (L);

  /* mininim.video.font */
  define_L_mininim_video_font (L);

  /* mininim.video.color */
  define_L_mininim_video_color (L);

  /* mininim.video.rectangle */
  define_L_mininim_video_rectangle (L);

  /* mininim.video.shader */
  define_L_mininim_video_shader (L);
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
    if (! strcasecmp (key, "bitmap")) {
      lua_pushcfunction (L, L_mininim_video_bitmap);
      return 1;
    } else if (! strcasecmp (key, "font")) {
      lua_pushcfunction (L, L_mininim_video_font);
      return 1;
    } else if (! strcasecmp (key, "color")) {
      lua_pushcfunction (L, L_mininim_video_color);
      return 1;
    } else if (! strcasecmp (key, "rectangle")) {
      lua_pushcfunction (L, L_mininim_video_rectangle);
      return 1;
    } else if (! strcasecmp (key, "shader")) {
      lua_pushcfunction (L, L_mininim_video_shader);
      return 1;
    } else if (! strcasecmp (key, "shader_platform")) {
      ALLEGRO_SHADER *s = al_create_shader (ALLEGRO_SHADER_AUTO);
      lua_pushstring (L, get_shader_platform (s));
      al_destroy_shader (s);
      return 1;
    } else if (! strcasecmp (key, "default_vertex_shader_source")) {
      const char *s = al_get_default_shader_source
        (ALLEGRO_SHADER_AUTO, ALLEGRO_VERTEX_SHADER);
      lua_pushstring (L, s);
      return 1;
    } else if (! strcasecmp (key, "default_fragment_shader_source")) {
      const char *s = al_get_default_shader_source
        (ALLEGRO_SHADER_AUTO, ALLEGRO_PIXEL_SHADER);
      lua_pushstring (L, s);
      return 1;
    } else if (! strcasecmp (key, "mode")) {
      lua_pushstring (L, video_mode);
      return 1;
    } else if (! strcasecmp (key, "original_width")) {
      lua_pushnumber (L, ORIGINAL_WIDTH);
      return 1;
    } else if (! strcasecmp (key, "original_height")) {
      lua_pushnumber (L, ORIGINAL_HEIGHT);
      return 1;
    } else if (! strcasecmp (key, "env_mode")) {
      if (! strcasecmp (env_mode, "ORIGINAL"))
        lua_pushstring (L, env_mode_string (global_level.em));
      else lua_pushstring (L, env_mode);
      return 1;
    } else if (! strcasecmp (key, "hue_mode")) {
      if (! strcasecmp (hue_mode, "ORIGINAL"))
        lua_pushstring (L, hue_mode_string (global_level.hue));
      else lua_pushstring (L, hue_mode);
      return 1;
    } else if (! strcasecmp (key, "palette_cache_size_limit")) {
      lua_pushnumber (L, palette_cache_size_limit);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__tostring)
{
  lua_pushstring (L, "MININIM VIDEO INTERFACE");
  return 1;
}
END_LUA

bool
L_push_video_mode_table (lua_State *L)
{
  L_get_registry_by_ref (L, mininim_lua_ref);

  lua_pushstring (L, "video_mode");
  lua_rawget (L, -2);
  lua_remove (L, -2);

  if (! lua_istable (L, -1)) {
    error_lua_invalid ("video_mode");
    lua_pop (L, 1);
    return false;
  }

  return true;
}

bool
L_push_video_routine (lua_State *L)
{
  if (! L_push_video_mode_table (L)) return false;

  lua_pushstring (L, video_mode);
  lua_rawget (L, -2);
  lua_remove (L, -2);

  if (! lua_isfunction (L, -1)) {
    error_lua_invalid ("%s.%s", "video_mode", video_mode);
    lua_pop (L, 1);
    return false;
  }

  return true;
}

bool
video (ALLEGRO_BITMAP *bitmap, int nret, const char *command,
       const char *object, uintptr_t index, uintptr_t part,
       struct pos *p, int actor_id, lua_Number width)
{
  lua_State *L = main_L;
  if (! L_push_video_routine (L)) {
    int i;
    for (i = 0; i < nret; i++) lua_pushnil (L);
    return false;
  }

  int nargs = 0;

  if (command) {
    lua_pushstring (L, command);
    nargs++;
  }

  if (object) {
    lua_pushstring (L, object);
    nargs++;
  }

  if (index) {
    if (index <= VIDEO_INDEX_MAX) lua_pushnumber (L, index);
    else lua_pushstring (L, (char *) index);
    nargs++;
  }

  if (part) {
    if (part <= VIDEO_INDEX_MAX) lua_pushnumber (L, part);
    else lua_pushstring (L, (char *) part);
    nargs++;
  }

  if (p) {
    L_pushposition (L, p);
    nargs++;
  }

  if (actor_id >= 0) {
    L_pushactor (L, actor_id);
    nargs++;
  }

  if (width >= 0) {
    lua_pushnumber (L, width);
    nargs++;
  }

  L_target_bitmap = bitmap;
  bool r = L_call (L, nargs, nret);
  L_target_bitmap = NULL;

  return r;
}

void
video_command_error (const char *command, const char *object,
                     uintptr_t index, uintptr_t part)
{
  char *object_str;
  if (object) object_str = xasprintf (" %s", object);
  else object_str = xasprintf ("%s", "");

  char *index_str;
  if (index) {
    if (index <= VIDEO_INDEX_MAX)
      index_str = xasprintf (" %ju", index);
    else index_str = xasprintf (" %s", (char *) index);
  } else index_str = xasprintf ("%s", "");

  char *part_str;
  if (part) {
    if (part <= VIDEO_INDEX_MAX)
      part_str = xasprintf (" %ju", part);
    else part_str = xasprintf (" %s", (char *) part);
  } else part_str = xasprintf ("%s", "");

  fprintf (stderr, "error: video command \"%s%s%s%s\" failed\n",
           command, object_str, index_str, part_str);

  al_free (object_str);
  al_free (index_str);
  al_free (part_str);
}

struct rect *
_rect_object_index_part (struct rect *r_ret, const char *object,
                         uintptr_t index, uintptr_t part,
                         struct pos *p, int actor_id)
{
  lua_State *L = main_L;

  bool r = video (NULL, 1, "RECTANGLE", object, index, part, p,
                  actor_id, -1);

  struct rect *r_ptr =
    luaL_checkudata (L, -1, L_MININIM_VIDEO_RECTANGLE);

  lua_pop (L, 1);

  if (r && r_ptr) {
    *r_ret = *r_ptr;
    return r_ret;
  } else {
    new_rect (r_ret, 0, 0, 0, 0, 0);
    video_command_error ("RECTANGLE", object, index, part);
    return NULL;
  }
}

ALLEGRO_BITMAP *
_bitmap_object_index_part (const char *object, uintptr_t index,
                           uintptr_t part)
{
  lua_State *L = main_L;

  bool r = video (NULL, 1, "BITMAP", object, index, part, NULL, -1, -1);

  ALLEGRO_BITMAP **b_ptr =
    luaL_checkudata (L, -1, L_MININIM_VIDEO_BITMAP);

  lua_pop (L, 1);

  if (r && b_ptr) return *b_ptr;
  else {
    video_command_error ("BITMAP", object, index, part);
    return NULL;
  }
}

lua_Number
video_mode_value (const char *object)
{
  lua_State *L = main_L;

  bool r = video (NULL, 1, "VALUE", object, 0, 0, NULL, -1, -1);

  if (! r || ! lua_isnumber (L, -1))
    video_command_error ("VALUE", object, 0, 0);

  lua_Number n = lua_tonumber (L, -1);
  lua_pop (L, 1);

  return n;
}

bool
is_valid_video_mode (char *vm)
{
  lua_State *L = main_L;

  if (! vm) return false;

  if (! L_push_video_mode_table (L)) return false;

  lua_pushstring (L, vm);
  lua_rawget (L, -2);

  if (lua_isfunction (L, -1)) {
    lua_pop (L, 2);
    return true;
  } else {
    lua_pop (L, 2);
    return false;
  }
}

char *
next_video_mode (char *current_vm)
{
  lua_State *L = main_L;

  int first_time = true;

  if (! L_push_video_mode_table (L)) return NULL;

  if (current_vm) {
    lua_pushstring (L, current_vm);
    lua_rawget (L, -2);

    if (! lua_isnil (L, -1)) {
      lua_pop (L, 1);
      lua_pushstring (L, current_vm);
    }
  } else lua_pushnil (L);

 retry:
  while (lua_next (L, -2)) {
    if (lua_isstring (L, -2) && lua_isfunction (L, -1)) {
      char *vm = xasprintf ("%s", lua_tostring (L, -2));
      lua_pop (L, 3);
      return vm;
    } else lua_pop (L, 1);
  }

  if (first_time) {
    first_time = false;
    lua_pushnil (L);
    goto retry;
  }

  lua_pop (L, 1);

  return NULL;
}

void
setup_video_mode (char *requested_vm)
{
  if (is_valid_video_mode (requested_vm))
    set_string_var (&video_mode, requested_vm);
  else if (! is_valid_video_mode (video_mode))
    set_string_var (&video_mode, next_video_mode (NULL));

  if (video_mode) video_rendering (true);
  else {
    REAL_WIDTH = ORIGINAL_WIDTH;
    REAL_HEIGHT = ORIGINAL_HEIGHT;
    video_rendering (false);
    return;
  }

  int w = video_mode_value ("WIDTH");
  int h = video_mode_value ("HEIGHT");

  if (! w || ! h) {
    REAL_WIDTH = ORIGINAL_WIDTH;
    REAL_HEIGHT = ORIGINAL_HEIGHT;
    set_string_var (&video_mode, NULL);
    video_rendering (false);
    return;
  }

  ALLEGRO_BITMAP **ba = NULL;
  size_t ba_nmemb = 0;
  if (w != REAL_WIDTH && h != REAL_HEIGHT) {
    ba_nmemb = actor_nmemb;
    ba = xcalloc (ba_nmemb, sizeof (*ba));

    for (size_t i = 0; i < actor_nmemb; i++) {
      struct actor *a = &actor[i];

      int bw = (w * get_bitmap_width (a->f.b)) / REAL_WIDTH;
      int bh = (h * get_bitmap_height (a->f.b)) / REAL_HEIGHT;

      a->f.b = ba[i] = create_bitmap (bw, bh);
      if (w != REAL_WIDTH) a->f.c.x = round (a->f.c.x);
      if (h != REAL_HEIGHT) a->f.c.y = round (a->f.c.y);
    }
  }

  REAL_WIDTH = w;
  REAL_HEIGHT = h;

  al_destroy_bitmap (uscreen);
  uscreen = create_bitmap (OW (CUTSCENE_WIDTH), OH (CUTSCENE_HEIGHT));
  clear_bitmap (uscreen, TRANSPARENT_COLOR);

  set_multi_room (mr.w, mr.h);

  update_room0_cache ();
  update_cache ();

  if (is_game_paused ()) step_cycle = 1;
  if (anim_compute_callback) anim_compute_callback ();
  if (anim_draw_callback) anim_draw_callback ();
  if (anim_cleanup_callback) anim_cleanup_callback ();

  if (ba && ba_nmemb)
    for (size_t i = 0; i < ba_nmemb; i++) al_destroy_bitmap (ba[i]);
  al_free (ba);
}
