/*
  L_mininim.video.c -- mininim.video script module;

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

int REAL_WIDTH = ORIGINAL_WIDTH;
int REAL_HEIGHT = ORIGINAL_HEIGHT;
bool changing_vm;

static DEFUN ( __eq);
static DEFUN (__index);
static DEFUN (__tostring);

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
  L_get_registry_by_ref (L, lua_table_ref);
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

DEFUN (__tostring)
{
  lua_pushstring (L, "MININIM VIDEO INTERFACE");
  return 1;
}

/* Push MININIM.lua.video_mode.  Ensure it's a table.  Return 1. */

int
push_video_mode_table (lua_State *L)
{
  /* Lua state must be not NULL! */
  assert (L);
  /* Acquire Lua lock. */
  lock_lua (L);
  /* Ensure stack size. */
  luaL_checkstack (L, 1, "cannot push video mode table");
  /* Push MININIM.lua.video_mode */
  push_lua_table_field (L, "video_mode");
  /* Ensure it is a table. */
  if (! lua_istable (L, -1))
    return luaL_error (L, "MININIM.lua.video_mode is not a table");
  /* Release Lua lock. */
  unlock_lua (L);
  /* One value has been put on stack. */
  return 1;
}

/* Push MININIM.lua.video_mode[vm].  Ensure it's a function.  VM
   defaults to VIDEO_MODE.  Return 1. */

int
push_video_routine (lua_State *L, const char *vm)
{
  /* Ensure stack size. */
  luaL_checkstack (L, 2, "cannot push video routine");
  /* VM defaults to VIDEO_MODE. */
  vm = vm ? vm : video_mode;
  /* Push MININIM.lua.video_mode. */
  push_video_mode_table (L);
  /* Push MININIM.lua.video_mode[vm]. */
  lua_getfield (L, -1, vm);
  /* Ensure it is a function. */
  if (! lua_isfunction (L, -1))
    return luaL_error (L, "MININIM.lua.video_mode['%s'] is not a function",
		       vm);
  /* Remove MININIM.lua.video_mode. */
  lua_remove (L, -2);
  /* Put one value on stack. */
  return 1;
}

/* MININIM.lua.video_mode[VM] = nil.  VM defaults to VIDEO_MODE.
   Return 0. */

int
remove_video_mode (lua_State *L, const char *vm)
{
  /* Ensure stack size. */
  luaL_checkstack (L, 1, "cannot remove video mode");
  /* VM defaults to VIDEO_MODE */
  vm = vm ? vm : video_mode;
  /* VM must not be NULL! */
  assert (vm);
  /* To be safe, make own copy of VM, so this function won't be
     affected by any other sub-routine that might free it. */
  char vm_copy[strlen (vm)];
  vm = strcpy (vm_copy, vm);
  /* Presume should not setup video mode after removal. */
  bool should_setup_vm = false;
  /* Is VM the current video mode? */
  if (! strcmp (vm, video_mode))
    {
      /* The current video mode will change, therefore must setup
	 video mode after removal. */
      should_setup_vm = true;
      /* Change the current video mode to the next one available. */
      set_string_var (&video_mode, next_video_mode (L, NULL));
      /* Is VM still the current video mode? */
      if (! strcmp (vm, video_mode))
	/* There is no remaining video mode.  Failure! */
	failure ("Removing last video mode available.  "
		 "MININIM.lua.video_mode is empty!  Exiting.");
    }
  /* Push MININIM.lua.video_mode. */
  push_video_mode_table (L);
  /* MININIM.lua.video_mode[vm] = nil */
  remove_table_field (L, -1, vm);
  /* Pop MININIM.lua.video_mode. */
  lua_pop (L, 1);
  /* Should setup new current video mode? */
  if (should_setup_vm)
    setup_video_mode (L, NULL);	/* Set it up! */
  /* No value has been put on stack. */
  return 0;
}

/* Return video mode name next to CURRENT_VM as a newly allocated
   string.  If CURRENT_VM is NULL or the name of the last video mode
   available, get the first one.  It's the responsibility of the
   caller to free the returned string. */

char *
next_video_mode (lua_State *L, const char *current_vm)
{
  /* Ensure stack size. */
  luaL_checkstack (L, 3, "cannot query next video mode");
  /* CURRENT_VM defaults to VIDEO_MODE. */
  current_vm = current_vm ? current_vm : video_mode;
  /* Push MININIM.lua.video_mode. */
  push_video_mode_table (L);
  /* Ensure there is at least one video mode. */
  if (is_table_empty (L, -1))
    failure ("No video mode available.  MININIM.lua.video_mode is empty.");
  /* Is CURRENT_VM the last video mode? */
  if (is_last_table_key (L, -1, current_vm))
    {
      /* CURRENT_VM is not NULL!  Otherwise there would be no video
	 mode. */
      assert (current_vm);
      /* Wrap around to get the first video mode. */
      lua_pushnil (L);
    }
  else
    /* Get video mode next to CURRENT_VM.  If CURRENT_VM is NULL, get
       the first one. */
    lua_pushstring (L, current_vm);
  /* There is a next video mode!  Get it. */
  assert (lua_next (L, -2));
  /* Make a copy of the video mode name. */
  char *vm = xasprintf ("%s", lua_tostring (L, -2));
  /* Pop MININIM.lua.video_mode, the next video mode name and its
     routine function. */
  lua_pop (L, 3);
  /* Return the newly allocated video mode name. */
  return vm;
}

bool
video (lua_State *L,
       ALLEGRO_BITMAP *bitmap,
       int nret,
       const char *command,
       const char *object,
       uintptr_t index,
       uintptr_t part,
       struct pos *p,
       int actor_id,
       lua_Number width)
{
  if (! is_video_rendering ())
    return false;

  push_video_routine (L, NULL);

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
  if (! is_video_rendering ())
    return;

  char *object_str = NULL;
  if (object)
    object_str = xasprintf (" %s", object);

  char *index_str = NULL;
  if (index)
    {
      if (index <= VIDEO_INDEX_MAX)
	index_str = xasprintf (" %zu", index);
      else index_str = xasprintf (" %s", (char *) index);
    }

  char *part_str = NULL;
  if (part)
    {
      if (part <= VIDEO_INDEX_MAX)
	part_str = xasprintf (" %zu", part);
      else part_str = xasprintf (" %s", (char *) part);
    }

  warning ("video command '%s%s%s%s' failed",
	   command,
	   object_str,
	   index_str,
	   part_str);

  al_free (object_str);
  al_free (index_str);
  al_free (part_str);

  remove_video_mode (main_L, NULL);
}

struct rect *
_rect_object_index_part (struct rect *r_ret, const char *object,
                         uintptr_t index, uintptr_t part,
                         struct pos *p, int actor_id)
{
  lua_State *L = main_L;

  bool r = video (L, NULL, 1, "RECTANGLE", object, index, part, p,
                  actor_id, -1);

  struct rect *r_ptr =
    luaL_testudata (L, -1, L_MININIM_VIDEO_RECTANGLE);

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

  bool r = video (L, NULL, 1, "BITMAP", object, index, part, NULL, -1, -1);

  ALLEGRO_BITMAP **b_ptr =
    luaL_testudata (L, -1, L_MININIM_VIDEO_BITMAP);

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

  bool r = video (L, NULL, 1, "VALUE", object, 0, 0, NULL, -1, -1);

  if (! r || ! lua_isnumber (L, -1))
    video_command_error ("VALUE", object, 0, 0);

  lua_Number n = lua_tonumber (L, -1);
  lua_pop (L, 1);

  return n;
}

void
setup_video_mode (lua_State *L, char *requested_vm)
{
  if (! is_video_rendering ())
    return;

  if (changing_vm) return;
  changing_vm = true;
  lock_lua (L);

  if (requested_vm)
    set_string_var (&video_mode, requested_vm);
  else if (! video_mode)
    set_string_var (&video_mode, next_video_mode (L, NULL));

  /* VIDEO_MODE is defined! */
  assert (video_mode);

  eprintf ("Loading video mode '%s'... ", video_mode);
  load_callback = process_display_events;
  if (! video (L, NULL, 0, "LOAD", NULL, 0, 0, NULL, -1, -1))
    {
      eprintf ("Error!\n");
      warning ("Video mode '%s' failed to load.  Removing it!", video_mode);
      load_callback = NULL;
      changing_vm = false;
      remove_video_mode (L, NULL);
      unlock_lua (L);
      return;
    }
  load_callback = NULL;
  eprintf ("Done!\n");

  int w = video_mode_value ("WIDTH");
  int h = video_mode_value ("HEIGHT");

  if (! w || ! h) {
    REAL_WIDTH = ORIGINAL_WIDTH;
    REAL_HEIGHT = ORIGINAL_HEIGHT;
    set_string_var (&video_mode, NULL);
    video_rendering (false);
    changing_vm = false;
    unlock_lua (L);
    return;
  }

  struct {
    ALLEGRO_BITMAP *f;
    ALLEGRO_BITMAP *of;
    ALLEGRO_BITMAP *fo;
  } *ba = NULL;
  size_t ba_nmemb = 0;
  if (w != REAL_WIDTH && h != REAL_HEIGHT) {
    ba_nmemb = actor_nmemb;
    scalloc (ba_nmemb, ba);

    for (size_t i = 0; i < actor_nmemb; i++) {
      struct actor *a = &actor[i];

      if (a->f.b) {
        int fw = (w * get_bitmap_width (a->f.b)) / REAL_WIDTH;
        int fh = (h * get_bitmap_height (a->f.b)) / REAL_HEIGHT;
        a->f.b = ba[i].f = create_bitmap (fw, fh);
      }

      if (a->of.b) {
        int ofw = (w * get_bitmap_width (a->of.b)) / REAL_WIDTH;
        int ofh = (h * get_bitmap_height (a->of.b)) / REAL_HEIGHT;
        a->of.b = ba[i].of = create_bitmap (ofw, ofh);
      }

      if (a->fo.b) {
        int fow = (w * get_bitmap_width (a->fo.b)) / REAL_WIDTH;
        int foh = (h * get_bitmap_height (a->fo.b)) / REAL_HEIGHT;
        a->fo.b = ba[i].fo = create_bitmap (fow, foh);
      }

      if (w != REAL_WIDTH) {
        a->f.c.x = round (a->f.c.x);
        a->of.c.x = round (a->of.c.x);
        a->fo.dx = round (a->fo.dx);
      }
      if (h != REAL_HEIGHT) {
        a->f.c.y = round (a->f.c.y);
        a->of.c.y = round (a->of.c.y);
        a->fo.dy = round (a->fo.dy);
      }
    }
  }

  REAL_WIDTH = w;
  REAL_HEIGHT = h;

  al_destroy_bitmap (uscreen);
  uscreen = create_bitmap (OW (CUTSCENE_WIDTH), OH (CUTSCENE_HEIGHT));
  clear_bitmap (uscreen, TRANSPARENT_COLOR);

  mr_set_dim (&global_mr, global_mr.w, global_mr.h);

  update_room0_cache ();
  update_cache (&global_mr);

  if (is_game_paused ()) step_cycle = 1;
  if (anim_compute_callback) anim_compute_callback ();
  if (anim_draw_callback) anim_draw_callback ();
  if (anim_cleanup_callback) anim_cleanup_callback ();

  if (ba && ba_nmemb)
    for (size_t i = 0; i < ba_nmemb; i++) {
      if (ba[i].f) al_destroy_bitmap (ba[i].f);
      if (ba[i].of) al_destroy_bitmap (ba[i].of);
      if (ba[i].fo) al_destroy_bitmap (ba[i].fo);
    }
  al_free (ba);

  eprintf ("Changed to video mode '%s'.\n", video_mode);
  changing_vm = false;
  unlock_lua (L);
}
