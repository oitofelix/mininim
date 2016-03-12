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
  LM_FREE_SPOT3_DUNGEON_BLUE_LINE_PALACE = 0xFF
};

enum lm_spike {
  LM_SPIKE_NORMAL, LM_SPIKE_BARELY_OUT_1, LM_SPIKE_HALF_OUT_1,
  LM_SPIKE_FULLY_OUT_1, LM_SPIKE_FULLY_OUT_2, LM_SPIKE_OUT_1,
  LM_SPIKE_OUT_2, LM_SPIKE_HALF_OUT_2, LM_SPIKE_BARELY_OUT_2,
  LM_SPIKE_DISABLED,
  LM_SPIKE_WEIRD_1 = 0x20,
  LM_SPIKE_WEIRD_2 = 0x40,
  LM_SPIKE_WEIRD_3 = 0x60
};

enum lm_gate {
  LM_GATE_CLOSED, LM_GATE_OPEN,
};

enum lm_potion {
  LM_POTION_EMPTY, LM_POTION_HEALTH_POINT, LM_POTION_LIFE,
  LM_POTION_FEATHER_FALL, LM_POTION_INVERT, LM_POTION_POISON,
  LM_POTION_OPEN
};

enum lm_tapest {
  LM_TAPEST_WITH_LATTICE, LM_TAPEST_ALTERNATIVE_DESIGN,
  LM_TAPEST_NORMAL, LM_TAPEST_BLACK,
  LM_TAPEST_WEIRD_1 = 0x20,
  LM_TAPEST_WEIRD_2 = 0x40,
  LM_TAPEST_WEIRD_3 = 0x60,
  LM_TAPEST_WEIRD_4 = 0x80,
  LM_TAPEST_WEIRD_5 = 0xA0,
  LM_TAPEST_WEIRD_6 = 0xC0,
  LM_TAPEST_WEIRD_7 = 0xE0
};

enum lm_ttop {
  LM_TTOP_WITH_LATTICE, LM_TTOP_ALTERNATIVE_DESIGN,
  LM_TTOP_NORMAL, LM_TTOP_BLACK_01, LM_TTOP_BLACK_02,
  LM_TTOP_WITH_ALTERNATIVE_DESIGN_AND_BOTTOM,
  LM_TTOP_WITH_BOTTOM, LM_TTOP_WITH_WINDOW,
  LM_TTOP_WEIRD_1 = 0x20,
  LM_TTOP_WEIRD_2 = 0x40,
  LM_TTOP_WEIRD_3 = 0x60,
  LM_TTOP_WEIRD_4 = 0x80,
  LM_TTOP_WEIRD_5 = 0xA0,
  LM_TTOP_WEIRD_6 = 0xC0,
  LM_TTOP_WEIRD_7 = 0xE0
};

enum lm_chomp {
  LM_CHOMP_NORMAL, LM_CHOMP_HALF_OPEN, LM_CHOMP_CLOSED,
  LM_CHOMP_PARTIALLY_OPEN, LM_CHOMP_EXTRA_OPEN, LM_STUCK_OPEN
};

enum lm_wall {
  LM_WALL_MARK, LM_WALL_NO_MARK
};

enum lm_exit {
  LM_EXIT_HALF_OPEN = 0x20,
  LM_EXIT_MORE_OPEN_1 = 0x40,
  LM_EXIT_MORE_OPEN_2 = 0x60,
  LM_EXIT_MORE_OPEN_3 = 0x80,
  LM_EXIT_MORE_OPEN_4 = 0xA0,
  LM_EXIT_MORE_OPEN_5 = 0xC0,
  LM_EXIT_MORE_OPEN_6 = 0xE0,
  LM_EXIT_EVEN_MORE_OPEN = 0xFD,
  LM_EXIT_MOST_OPEN = 0xFF,
};

/* functions */
void legacy_level_start (void);
void legacy_level_special_events (void);
void legacy_level_end (struct pos *p);
void next_legacy_level (int number);
void play_legacy_level (int number);
void interpret_legacy_level (int number);
struct skill *get_legacy_skill (int i, struct skill *skill);

/* variables */
extern struct legacy_level lv;
extern struct level legacy_level;

#endif	/* MININIM_LEGACY_LEVEL_H */
