/*
  debug.c -- debug module;

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

void
save_guard_bitmap (enum actor_type type, int style)
{
  char *filename = NULL, *prefix = NULL;
  switch (type) {
  case GUARD: prefix = "guard"; break;
  case FAT: prefix = "fat"; break;
  case VIZIER: prefix = "vizier"; break;
  case SKELETON: prefix = "skeleton"; break;
  case SHADOW: prefix = "shadow"; break;
  default: prefix = "invalid"; break;

  }
  filename = xasprintf ("%s-%i.png", prefix, style);

  const char *type_str = actor_type_string (type);
  ALLEGRO_BITMAP *b = actor_bitmap (NULL, type_str, "NORMAL", 0);
  b = apply_guard_palette (b, type, style);
  save_bitmap (filename, b);
  al_free (filename);
}

void
save_guard_bitmaps (void)
{
  int type, style;
  for (type = SHADOW; type <= VIZIER; type++)
    for (style = 0; style <= 7; style++)
      save_guard_bitmap (type, style);
}

void
debug_pos (struct pos *p)
{
  printf ("(%i,%i,%i)\n", p->room, p->floor, p->place);
}

void
debug_random_seed (void)
{
  fprintf (stderr, "%ju 0x%X\n", anim_cycle, random_seed);
}

void
debug_actor (struct actor *a)
{
  char *hl = hline ('=');
  fprintf (stderr, "%s\n"
           "action: %s\n"
           "previous_action: %s\n"
           "index: %i\n"
           "direction: %s\n"
           "dx: %f\n"
           "dy: %f\n",
           hl,
           actor_action_string (a->action),
           actor_action_string (a->oaction),
           a->i + 1,
           direction_string (a->f.dir),
           -a->fo.dx,
           -a->fo.dy);
  al_free (hl);
}

void
L_dump_stack (lua_State *L)
{
  int i;
  for (i = 1; i <= lua_gettop (L); i++) {
    int type = lua_type (L, i);
    switch (type) {
    case LUA_TNIL: fprintf (stderr, "%i: nil\n", i); break;
    case LUA_TNUMBER:
      fprintf (stderr, "%i: %f\n", i, lua_tonumber (L, i));
      break;
    case LUA_TBOOLEAN:
      fprintf (stderr, "%i: %s\n", i,
               lua_toboolean (L, i) ? "true" : "false");
      break;
    case LUA_TSTRING:
      fprintf (stderr, "%i: \"%s\"\n", i, lua_tostring (L, i));
      break;
    case LUA_TTABLE:
      fprintf (stderr, "%i: [table]\n", i);
      break;
    case LUA_TFUNCTION:
      fprintf (stderr, "%i: [function]\n", i);
      break;
    case LUA_TUSERDATA:
      fprintf (stderr, "%i: [userdata]\n", i);
      break;
    case LUA_TTHREAD:
      fprintf (stderr, "%i: [thread]\n", i);
      break;
    case LUA_TLIGHTUSERDATA:
      fprintf (stderr, "%i: [lightuserdata]\n", i);
      break;
    default:
      fprintf (stderr, "%i: [unknown]\n", i);
      break;
    }
  }
}

void
gdb (void)
{
  /* Putting "break gdb" inside yours GDB init script you can
     programatically invoke GDB by using this function. */
  return;
}
