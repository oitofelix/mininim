/*
  native-level.c -- native level module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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
save_native_level (struct level *l, char *filename)
{
  ALLEGRO_CONFIG *c = create_config ();
  char *s, *v;

  /* MININIM LEVEL FILE */
  al_add_config_comment (c, NULL, "MININIM LEVEL FILE");

  /* CONSTRUCTIONS */
  struct pos p;
  for (p.room = 0; p.room < ROOMS; p.room++)
    for (p.floor = 0; p.floor < FLOORS; p.floor++)
      for (p.place = 0; p.place < PLACES; p.place++) {

        /* [CON r f p] */
        xasprintf (&s, "CON %i %i %i", p.room, p.floor, p.place);

        /* FG= */
        v = get_confg_str (l, &p);
        if (v) al_set_config_value (c, s, "FG", v);

        /* BG= */
        v = get_conbg_str (l, &p);
        if (v) al_set_config_value (c, s, "BG", v);

        /* EXT= */
        v = get_conext_str (l, &p);
        if (v) {
         al_set_config_value (c, s, "EXT", v);
         al_free (v);
        }

        al_free (s);
      }

  save_config_file (filename, c);
  al_destroy_config (c);
}

char *
get_confg_str (struct level *l, struct pos *p)
{
  switch (xcon (l, p)->fg) {
  case NO_FLOOR: return "NO_FLOOR";
  case FLOOR: return "FLOOR";
  case BROKEN_FLOOR: return "BROKEN_FLOOR";
  case SKELETON_FLOOR: return "SKELETON_FLOOR";
  case LOOSE_FLOOR: return "LOOSE_FLOOR";
  case SPIKES_FLOOR: return "SPIKES_FLOOR";
  case OPENER_FLOOR: return "OPENER_FLOOR";
  case CLOSER_FLOOR: return "CLOSER_FLOOR";
  case STUCK_FLOOR: return "STUCK_FLOOR";
  case HIDDEN_FLOOR: return "HIDDEN_FLOOR";
  case PILLAR: return "PILLAR";
  case BIG_PILLAR_BOTTOM: return "BIG_PILLAR_BOTTOM";
  case BIG_PILLAR_TOP: return "BIG_PILLAR_TOP";
  case WALL: return "WALL";
  case DOOR: return "DOOR";
  case LEVEL_DOOR: return "LEVEL_DOOR";
  case CHOPPER: return "CHOPPER";
  case MIRROR: return "MIRROR";
  case CARPET: return "CARPET";
  case TCARPET: return "TCARPET";
  case ARCH_BOTTOM: return "ARCH_BOTTOM";
  case ARCH_TOP_LEFT: return "ARCH_TOP_LEFT";
  case ARCH_TOP_RIGHT: return "ARCH_TOP_RIGHT";
  case ARCH_TOP_MID: return "ARCH_TOP_MID";
  case ARCH_TOP_SMALL: return "ARCH_TOP_SMALL";
  default: return NULL;
  }
}

char *
get_conbg_str (struct level *l, struct pos *p)
{
  switch (xcon (l, p)->bg) {
  case NO_BG: return "NO_BG";
  case BRICKS_00: return "BRICKS_00";
  case BRICKS_01: return "BRICKS_01";
  case BRICKS_02: return "BRICKS_02";
  case BRICKS_03: return "BRICKS_03";
  case NO_BRICKS: return "NO_BRICKS";
  case TORCH: return "TORCH";
  case WINDOW: return "WINDOW";
  case BALCONY: return "BALCONY";
  default: return NULL;
  }
}

char *
get_conext_str (struct level *l, struct pos *p)
{
  char *s = NULL;

  switch (xcon (l, p)->fg) {
  case FLOOR:
    switch (xcon (l, p)->ext.item) {
    case NO_ITEM: xasprintf (&s, "NO_ITEM"); break;
    case EMPTY_POTION: xasprintf (&s, "EMPTY_POTION"); break;
    case SMALL_LIFE_POTION: xasprintf (&s, "SMALL_LIFE_POTION"); break;
    case BIG_LIFE_POTION: xasprintf (&s, "BIG_LIFE_POTION"); break;
    case SMALL_POISON_POTION: xasprintf (&s, "SMALL_POISON_POTION"); break;
    case BIG_POISON_POTION: xasprintf (&s, "BIG_POISON_POTION"); break;
    case FLOAT_POTION: xasprintf (&s, "FLOAT_POTION"); break;
    case FLIP_POTION: xasprintf (&s, "FLIP_POTION"); break;
    case ACTIVATION_POTION: xasprintf (&s, "ACTIVATION_POTION"); break;
    case SWORD: xasprintf (&s, "SWORD"); break;
    }
    break;
  case LOOSE_FLOOR:
    xasprintf (&s, "%s", xcon (l, p)->ext.cant_fall ? "CANT_FALL" : "FALL");
    break;
  case SPIKES_FLOOR: case DOOR: case LEVEL_DOOR: case CHOPPER:
    xasprintf (&s, "%i", xcon (l, p)->ext.step);
    break;
  case OPENER_FLOOR: case CLOSER_FLOOR:
    xasprintf (&s, "%i", xcon (l, p)->ext.event);
    break;
  case CARPET:
    switch (xcon (l, p)->ext.design) {
    case CARPET_00: s = "CARPET_00"; break;
    case CARPET_01: s = "CARPET_01"; break;
    case ARCH_CARPET_LEFT: s = "ARCH_CARPET_LEFT"; break;
    }
    if (s) xasprintf (&s, "%s", s);
    break;
  case TCARPET:
    switch (xcon (l, p)->ext.design) {
    case CARPET_00: s = "CARPET_00"; break;
    case CARPET_01: s = "CARPET_01"; break;
    case ARCH_CARPET_RIGHT_00: s = "ARCH_CARPET_RIGHT_00"; break;
    case ARCH_CARPET_RIGHT_01: s = "ARCH_CARPET_RIGHT_01"; break;
    case ARCH_CARPET_LEFT: s = "ARCH_CARPET_LEFT"; break;
    }
    if (s) xasprintf (&s, "%s", s);
    break;
  default: break;
  }

  return s;
}
