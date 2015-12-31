/*
  legacy-level.h -- legacy level module;

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

#ifndef MININIM_LEGACY_LEVEL_H
#define MININIM_LEGACY_LEVEL_H

#define LEVENTS 256
#define LROOMS 24

enum ltile {
  LT_EMPTY, LT_FLOOR, LT_SPIKES, LT_PILLAR, LT_GATE, LT_STUCK_BUTTON,
  LT_DROP_BUTTON, LT_TAPESTRY, LT_BOTTOM_BIG_PILLAR, LT_TOP_BIG_PILLAR,
  LT_POTION, LT_LOOSE_BOARD, LT_TAPESTRY_TOP, LT_MIRROR, LT_DEBRIS,
  LT_RAISE_BUTTON, LT_EXIT_LEFT, LT_EXIT_RIGHT, LT_CHOPPER, LT_TORCH,
  LT_WALL, LT_SKELETON, LT_SWORD, LT_BALCONY_LEFT, LT_BALCONY_RIGHT,
  LT_LATTICE_PILLAR, LT_LATTICE_SUPPORT, LT_SMALL_LATTICE, LT_LATTICE_LEFT,
  LT_LATTICE_RIGHT, LT_TORCH_WITH_DEBRIS, LT_NULL
};

enum ldir {
  LD_LEFT, LD_RIGHT, LD_ABOVE, LD_BELOW
};

enum lgroup {
  LG_NONE, LG_FREE, LG_SPIKE, LG_GATE,
  LG_TAPEST, LG_POTION, LG_TTOP, LG_CHOMP,
  LG_WALL, LG_EXIT, LG_EVENT
};

enum lm_free {
  LM_FREE_NOTHING_DUNGEON_BLUE_LINE_PALACE,
  LM_FREE_SPOT1_DUNGEON_NO_BLUE_LINE_PALACE,
  LM_FREE_SPOT2_DUNGEON_BLUE_LINE2_PALACE,
  LM_FREE_WINDOW,
  LM_FREE_SPOT3_DUNGEON_BLUE_LINE_PALACE = 0xFF,
};


enum lm_potion {
  LM_POTION_EMPTY, LM_POTION_HEALTH_POINT, LM_POTION_LIFE,
  LM_POTION_FEATHER_FALL, LM_POTION_INVERT, LM_POTION_POISON,
  LM_POTION_OPEN
};

enum lm_tapest {
  LM_TAPEST_WITH_LATTICE, LM_TAPEST_ALTERNATIVE_DESIGN,
  LM_TAPEST_NORMAL, LM_TAPEST_BLACK
};

enum lm_ttop {
  LM_TTOP_WITH_LATTICE, LM_TTOP_ALTERNATIVE_DESIGN,
  LM_TTOP_NORMAL, LM_TTOP_BLACK_01, LM_TTOP_BLACK_02,
  LM_TTOP_WITH_ALTERNATIVE_DESIGN_AND_BOTTOM,
  LM_TTOP_WITH_BOTTOM, LM_TTOP_WITH_WINDOW
};

void play_legacy_level (void);

#endif	/* MININIM_LEGACY_LEVEL_H */
