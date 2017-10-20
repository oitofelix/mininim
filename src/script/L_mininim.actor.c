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

static DECLARE_LUA (__eq);
static DECLARE_LUA (__index);
static DECLARE_LUA (__newindex);
static DECLARE_LUA (__tostring);

static DECLARE_LUA (draw);
static DECLARE_LUA (position);

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
  case ABOVE: return "ABOVE";
  case BELOW: return "BELOW";
  default: assert (0); return NULL;
  }
}

enum dir
direction_enum (const char *dir)
{
  if (! strcasecmp (dir, "LEFT")) return LEFT;
  else if (! strcasecmp (dir, "RIGHT")) return RIGHT;
  else if (! strcasecmp (dir, "ABOVE")) return ABOVE;
  else if (! strcasecmp (dir, "BELOW")) return BELOW;
  else return INVALID;
}

const char *
actor_type_string (enum actor_type type)
{
  switch (type) {
  case NO_ACTOR: return "NONE";
  case KID: return "KID";
  case SHADOW: return "SHADOW";
  case GUARD: return "GUARD";
  case FAT: return "FAT";
  case SKELETON: return "SKELETON";
  case VIZIER: return "VIZIER";
  case PRINCESS: return "PRINCESS";
  case MOUSE: return "MOUSE";
  default: assert (0); return NULL;
  }
}

enum actor_type
actor_type_enum (const char *type)
{
  if (! strcasecmp (type, "NONE")) return NO_ACTOR;
  else if (! strcasecmp (type, "KID")) return KID;
  else if (! strcasecmp (type, "SHADOW")) return SHADOW;
  else if (! strcasecmp (type, "GUARD")) return GUARD;
  else if (! strcasecmp (type, "FAT")) return FAT;
  else if (! strcasecmp (type, "SKELETON")) return SKELETON;
  else if (! strcasecmp (type, "VIZIER")) return VIZIER;
  else if (! strcasecmp (type, "PRINCESS")) return PRINCESS;
  else if (! strcasecmp (type, "MOUSE")) return MOUSE;
  else return INVALID;
}

const char *
actor_action_string (ACTION action)
{
  if (action == kid_turn) return "TURN";
  else if (action == kid_turn_run) return "TURN_RUN";
  else if (action == princess_turn) return "TURN";
  else if (action == kid_run) return "RUN";
  else if (action == mouse_run) return "RUN";
  else if (action == kid_start_run) return "START_RUN";
  else if (action == kid_stop_run) return "STOP_RUN";
  else if (action == kid_run_jump) return "RUN_JUMP";
  else if (action == kid_stabilize) return "STABILIZE";
  else if (action == kid_jump) return "JUMP";
  else if (action == kid_misstep) return "MISSTEP";
  else if (action == kid_crouch) return "CROUCH";
  else if (action == kid_climb) return "CLIMB";
  else if (action == kid_unclimb) return "UNCLIMB";
  else if (action == kid_hang) return "HANG";
  else if (action == kid_hang_free) return "HANG_FREE";
  else if (action == kid_hang_non_free) return "HANG_NON_FREE";
  else if (action == kid_fall) return "FALL";
  else if (action == kid_normal) return "NORMAL";
  else if (action == kid_vjump) return "VJUMP";
  else if (action == kid_walk) return "WALK";
  else if (action == kid_die) return "DIE";
  else if (action == kid_die_suddenly) return "DIE_SUDDENLY";
  else if (action == kid_die_spiked) return "DIE_SPIKED";
  else if (action == kid_die_chomped) return "DIE_CHOMPED";
  else if (action == kid_drink) return "DRINK";
  else if (action == kid_stairs) return "STAIRS";
  else if (action == kid_raise_sword) return "RAISE_SWORD";
  else if (action == kid_keep_sword) return "KEEP_SWORD";
  else if (action == kid_take_sword) return "TAKE_SWORD";
  else if (action == kid_sword_normal) return "SWORD_NORMAL";
  else if (action == kid_sword_walkf) return "SWORD_WALKF";
  else if (action == kid_sword_walkb) return "SWORD_WALKB";
  else if (action == kid_sword_hit) return "SWORD_HIT";
  else if (action == kid_sword_defense) return "SWORD_DEFENSE";
  else if (action == kid_sword_attack) return "SWORD_ATTACK";
  else if (action == guard_normal) return "NORMAL";
  else if (action == guard_sword_normal) return "SWORD_NORMAL";
  else if (action == guard_sword_walkf) return "SWORD_WALKF";
  else if (action == guard_sword_walkb) return "SWORD_WALKB";
  else if (action == guard_sword_attack) return "SWORD_ATTACK";
  else if (action == guard_sword_defense) return "SWORD_DEFENSE";
  else if (action == guard_sword_hit) return "SWORD_HIT";
  else if (action == guard_fall) return "FALL";
  else if (action == guard_die) return "DIE";
  else if (action == guard_die_suddenly) return "DIE_SUDDENLY";
  else if (action == guard_die_spiked) return "DIE_SPIKED";
  else if (action == guard_die_chomped) return "DIE_CHOMPED";
  else return "UNKNOWN";
}

ACTION
actor_action_enum (const char *action, enum actor_type type)
{
  if (! strcasecmp (action, "TURN")) {
    switch (type) {
    case KID: return kid_turn;
    case PRINCESS: return princess_turn;
    default: return NULL;
    }
  } if (! strcasecmp (action, "TURN_RUN")) {
    switch (type) {
    case KID: return kid_turn_run;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "RUN")) {
    switch (type) {
    case KID: return kid_run;
    case MOUSE: return mouse_run;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "START_RUN")) {
    switch (type) {
    case KID: return kid_start_run;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "STOP_RUN")) {
    switch (type) {
    case KID: return kid_stop_run;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "RUN_JUMP")) {
    switch (type) {
    case KID: return kid_run_jump;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "STABILIZE")) {
    switch (type) {
    case KID: return kid_stabilize;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "JUMP")) {
    switch (type) {
    case KID: return kid_jump;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "MISSTEP")) {
    switch (type) {
    case KID: return kid_misstep;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "CROUCH")) {
    switch (type) {
    case KID: return kid_crouch;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "CLIMB")) {
    switch (type) {
    case KID: return kid_climb;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "UNCLIMB")) {
    switch (type) {
    case KID: return kid_unclimb;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "HANG")) {
    switch (type) {
    case KID: return kid_hang;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "HANG_FREE")) {
    switch (type) {
    case KID: return kid_hang_free;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "HANG_NON_FREE")) {
    switch (type) {
    case KID: return kid_hang_non_free;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "FALL")) {
    switch (type) {
    case KID: return kid_fall;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_fall;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "NORMAL")) {
    switch (type) {
    case KID: return kid_normal;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_normal;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "VJUMP")) {
    switch (type) {
    case KID: return kid_vjump;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "WALK")) {
    switch (type) {
    case KID: return kid_walk;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "DIE")) {
    switch (type) {
    case KID: return kid_die;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_die;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "DIE_SUDDENLY")) {
    switch (type) {
    case KID: return kid_die_suddenly;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_die_suddenly;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "DIE_SPIKED")) {
    switch (type) {
    case KID: return kid_die_spiked;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_die_spiked;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "DIE_CHOMPED")) {
    switch (type) {
    case KID: return kid_die_chomped;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_die_chomped;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "DRINK")) {
    switch (type) {
    case KID: return kid_drink;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "STAIRS")) {
    switch (type) {
    case KID: return kid_stairs;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "RAISE_SWORD")) {
    switch (type) {
    case KID: return kid_raise_sword;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "KEEP_SWORD")) {
    switch (type) {
    case KID: return kid_keep_sword;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "TAKE_SWORD")) {
    switch (type) {
    case KID: return kid_take_sword;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "SWORD_NORMAL")) {
    switch (type) {
    case KID: return kid_sword_normal;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_sword_normal;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "SWORD_WALKF")) {
    switch (type) {
    case KID: return kid_sword_walkf;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_sword_walkf;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "SWORD_WALKB")) {
    switch (type) {
    case KID: return kid_sword_walkb;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_sword_walkb;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "SWORD_HIT")) {
    switch (type) {
    case KID: return kid_sword_hit;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_sword_hit;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "SWORD_DEFENSE")) {
    switch (type) {
    case KID: return kid_sword_defense;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_sword_defense;
    default: return NULL;
    }
  } else if (! strcasecmp (action, "SWORD_ATTACK")) {
    switch (type) {
    case KID: return kid_sword_attack;
    case GUARD: case FAT: case VIZIER:
    case SKELETON: case SHADOW: return guard_sword_attack;
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
  else return 0;

  struct actor *a = get_actor_by_id (id);
  if (! a) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "id")) {
      lua_pushnumber (L, id);
      return 1;
    } else if (! strcasecmp (key, "room")) {
      lua_pushnumber (L, a->f.c.room);
      return 1;
    } else if (! strcasecmp (key, "previous_index")) {
      lua_pushnumber (L, a->oi + 1);
      return 1;
    } else if (! strcasecmp (key, "index")) {
      lua_pushnumber (L, a->i + 1);
      return 1;
    } else if (! strcasecmp (key, "xindex")) {
      lua_pushnumber (L, a->j + 1);
      return 1;
    } else if (! strcasecmp (key, "direction")) {
      lua_pushstring (L, direction_string (a->f.dir));
      return 1;
    } else if (! strcasecmp (key, "type")) {
      lua_pushstring (L, actor_type_string (a->type));
      return 1;
    } else if (! strcasecmp (key, "action")) {
      lua_pushstring (L, actor_action_string (a->action));
      return 1;
    } else if (! strcasecmp (key, "previous_action")) {
      lua_pushstring (L, actor_action_string (a->oaction));
      return 1;
    } else if (! strcasecmp (key, "current_hp")) {
      lua_pushnumber (L, a->current_hp);
      return 1;
    } else if (! strcasecmp (key, "total_hp")) {
      lua_pushnumber (L, a->total_hp);
      return 1;
    } else if (! strcasecmp (key, "should_draw")) {
      lua_pushboolean (L, ! a->invisible);
      return 1;
    } else if (! strcasecmp (key, "should_draw_hp")) {
      lua_pushboolean (L, ! a->dont_draw_hp);
      return 1;
    } else if (! strcasecmp (key, "should_draw_splash")) {
      lua_pushboolean (L, a->splash);
      return 1;
    } else if (! strcasecmp (key, "style")) {
      lua_pushnumber (L, a->style);
      return 1;
    } else if (! strcasecmp (key, "shadow_of")) {
      if (a->shadow_of < 0) return 0;
      L_pushactor (L, a->shadow_of);
      return 1;
    } else if (! strcasecmp (key, "is_shadow")) {
      lua_pushboolean (L, a->shadow || a->shadow_of >= 0);
      return 1;
    } else if (! strcasecmp (key, "delayed_stand_up")) {
      lua_pushboolean (L, a->uncrouch_slowly);
      return 1;
    } else if (! strcasecmp (key, "is_guard")) {
      lua_pushboolean (L, is_guard (a));
      return 1;
    } else if (! strcasecmp (key, "draw")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, draw, 1);
      return 1;
    } else if (! strcasecmp (key, "top_left")) {
      struct rect r; coord2rect (&a->f.c, &r);
      L_pushrectangle (L, &r);
      return 1;
    } else if (! strcasecmp (key, "bitmap")) {
      L_pushbitmap (L, a->f.b);
      return 1;
    } else if (! strcasecmp (key, "float_timer")) {
      lua_pushnumber (L, a->float_timer);
      return 1;
    } else if (! strcasecmp (key, "has_collided")) {
      lua_pushboolean (L, a->collision);
      return 1;
    } else if (! strcasecmp (key, "hang_position")) {
      L_pushposition (L, &a->hang_pos);
      return 1;
    } else if (! strcasecmp (key, "will_hang")) {
      lua_pushboolean (L, a->hang);
      return 1;
    } else if (! strcasecmp (key, "is_reversed")) {
      lua_pushboolean (L, a->reverse);
      return 1;
    } else if (! strcasecmp (key, "position")) {
      lua_pushvalue (L, 1);
      lua_pushcclosure (L, position, 1);
      return 1;
    } else break;
  default: break;
  }

  return 0;
}
END_LUA

BEGIN_LUA (__newindex)
{
  int *id_ptr = luaL_checkudata (L, 1, L_MININIM_ACTOR);

  int id;
  if (id_ptr) id = *id_ptr;
  else return 0;

  struct actor *a = get_actor_by_id (id);
  if (! a) return 0;

  const char *key;
  int type = lua_type (L, 2);
  switch (type) {
  case LUA_TSTRING:
    key = lua_tostring (L, 2);
    if (! strcasecmp (key, "direction")) {
      if (! lua_isstring (L, 3)) return 0;
      enum dir dir = direction_enum (lua_tostring (L, 3));
      if (invalid (dir)) return 0;
      else if (dir != a->f.dir) invert_frame_dir (&a->f, &a->f);
      return 0;
    } else if (! strcasecmp (key, "type")) {
      if (! lua_isstring (L, 3)) return 0;
      enum actor_type type = actor_type_enum (lua_tostring (L, 3));
      if (invalid (type)) return 0;
      if (!id) return 0;
      a->type = type;
      return 0;
    } else if (! strcasecmp (key, "action")) {
      if (! lua_isstring (L, 3)) return 0;
      ACTION action = actor_action_enum (lua_tostring (L, 3), a->type);
      if (! action) return 0;
      a->next_action = action;
      return 0;
    } else if (! strcasecmp (key, "delayed_stand_up")) {
      a->uncrouch_slowly = lua_toboolean (L, 3);
      return 0;
    } else if (! strcasecmp (key, "current_hp")) {
      int hp = lua_tonumber (L, 3);
      a->current_hp = max_int (0, hp);
      a->total_hp = max_int (a->current_hp, a->total_hp);
      return 0;
    } else if (! strcasecmp (key, "total_hp")) {
      int hp = lua_tonumber (L, 3);
      a->total_hp = max_int (0, hp);
      a->current_hp = min_int (a->current_hp, a->total_hp);
      return 0;
    } else if (! strcasecmp (key, "should_draw")) {
      a->invisible = ! lua_toboolean (L, 3);
      return 0;
    } else if (! strcasecmp (key, "should_draw_hp")) {
      a->dont_draw_hp = ! lua_toboolean (L, 3);
      return 0;
    } else if (! strcasecmp (key, "should_draw_splash")) {
      a->splash = lua_toboolean (L, 3);
      return 0;
    } else if (! strcasecmp (key, "style")) {
      int style = lua_tonumber (L, 3);
      style = max_int (0, style);
      a->style = min_int (7, style);
      return 0;
    } else if (! strcasecmp (key, "top_left")) {
      struct rect *r = luaL_checkudata (L, 3, L_MININIM_VIDEO_RECTANGLE);
      if (! r) return 0;
      a->f.c = r->c;
      return 0;
    } else if (! strcasecmp (key, "bitmap")) {
      ALLEGRO_BITMAP **b = luaL_checkudata (L, 3, L_MININIM_VIDEO_BITMAP);
      if (! b) return 0;
      a->f.b = *b;
      return 0;
    } else if (! strcasecmp (key, "xbitmap")) {
      ALLEGRO_BITMAP **b = luaL_checkudata (L, 3, L_MININIM_VIDEO_BITMAP);
      if (! b) return 0;
      a->xf.b = *b;
      return 0;
    } else if (! strcasecmp (key, "is_shadow")) {
      a->shadow = lua_toboolean (L, 3);
      return 0;
    } else if (! strcasecmp (key, "float_timer")) {
      lua_Number float_timer = lua_tonumber (L, 3);
      a->float_timer = fmax (0, float_timer);
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

BEGIN_LUA (draw)
{
  int *id_ptr = luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_ACTOR);

  int id;
  if (id_ptr) id = *id_ptr;
  else return 0;

  struct actor *a = get_actor_by_id (id);
  if (! a) return 0;

  struct frame f = a->f;
  struct frame_offset xf = a->xf;

  ALLEGRO_BITMAP **bitmap = luaL_checkudata (L, 1, L_MININIM_VIDEO_BITMAP);
  if (bitmap) f.b = *bitmap;

  ALLEGRO_BITMAP **xbitmap = luaL_checkudata (L, 2, L_MININIM_VIDEO_BITMAP);
  if (xbitmap) xf.b = *xbitmap;

  ALLEGRO_BITMAP **splash = luaL_checkudata (L, 3, L_MININIM_VIDEO_BITMAP);

  ALLEGRO_SHADER **s =
    luaL_checkudata (L, lua_gettop (L), L_MININIM_VIDEO_SHADER);

  if (s) {
    al_set_target_bitmap (L_target_bitmap);
    al_use_shader (*s);
  }

  draw_xframe (L_target_bitmap, &f, &xf);
  draw_frame (L_target_bitmap, &f);

  if (splash) {
    struct coord c;
    draw_bitmapc (*splash, L_target_bitmap,
                  splash_coord (*splash, &f, &c), f.flip);
    a->splash_bitmap = *splash;
  }

  if (s) {
    al_set_target_bitmap (L_target_bitmap);
    al_use_shader (NULL);
  }

  return 0;
}
END_LUA

BEGIN_LUA (position)
{
  int *id_ptr = luaL_checkudata (L, lua_upvalueindex (1), L_MININIM_ACTOR);

  int id;
  if (id_ptr) id = *id_ptr;
  else return 0;

  struct actor *a = get_actor_by_id (id);
  if (! a) return 0;

  const char *name = lua_tostring (L, 1);
  coord_f cf = str2coord_f (name);

  cf = cf ? cf : _m;

  struct pos p;
  survey (cf, pos, &a->f, NULL, NULL, &p);
  L_pushposition (L, &p);

  return 1;
}
END_LUA


bool
select_actor_frame (struct actor *a, const char *type, const char *action,
                    int i)
{
  ALLEGRO_BITMAP *b = actor_bitmap (a, type, action, i);

  if (! b) return false;

  struct rect r, *r_ret;
  r_ret = actor_rect (&r, a, type, action, i);

  if (! r_ret) return false;

  a->fo.b = b;
  a->fo.dx = -r.c.x;
  a->fo.dy = -r.c.y;
  a->i = i;
  return true;
}

bool
select_actor_xframe (struct actor *a, const char *type, const char *xtype,
                     int j)
{
  ALLEGRO_BITMAP *b = actor_bitmap (a, type, xtype, j);

  if (! b) return false;

  struct rect r, *r_ret;
  r_ret = actor_rect (&r, a, type, xtype, j);

  if (! r_ret) return false;

  a->xf.b = b;
  a->xf.dx = -r.c.x;
  a->xf.dy = -r.c.y;
  return true;
}

ALLEGRO_BITMAP *
actor_bitmap (struct actor *a, const char *type, const char *action,
              int index)
{
  if (! type) type = actor_type_string (a->type);
  if (! action) action = actor_action_string (a->action);
  ALLEGRO_BITMAP *b = bitmap_object_index_part (type, action, index + 1);
  if (! b && a) b = a->f.b;
  return b;
}

struct rect *
_actor_rect (struct rect *r_ret, struct actor *a, const char *type,
             const char *action, uintptr_t index)
{
  if (! type) type = actor_type_string (a->type);
  if (! action) action = actor_action_string (a->action);
  index += index <= VIDEO_INDEX_MAX ? +1 : 0;
  return _rect_object_index_part
    (r_ret, type, (uintptr_t) action, index, NULL, a->id);
}

lua_Number
_actor_dx (struct actor *a, const char *type, const char *action,
           uintptr_t index)
{
  struct rect r, *r_ret;
  r_ret = actor_rect (&r, a, type, action, index);
  if (r_ret) return -r.c.x;
  else return 0;
}

lua_Number
_actor_dy (struct actor *a, const char *type, const char *action,
           uintptr_t index)
{
  struct rect r, *r_ret;
  r_ret = actor_rect (&r, a, type, action, index);
  if (r_ret) return -r.c.y;
  else return 0;
}

void
place_actor (struct actor *a, struct pos *p, lua_Number dx, lua_Number dy,
             const char *type, const char *action, int index)
{
  ALLEGRO_BITMAP *b = actor_bitmap (a, type, action, index);
  place_frame (&a->f, &a->f, b, p, dx, dy);
}

void
draw_actor_part (ALLEGRO_BITMAP *bitmap, const char *type,
                 const char *part, struct actor *a)
{
  if (! type) type = actor_type_string (a->type);
  video (bitmap, 0, "DRAW", type, 0, (uintptr_t) (part),
         NULL, a->id, -1);
}
