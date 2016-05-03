/*
  constants.h -- constant definitions;

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

#ifndef MININIM_CONSTANTS_H
#define MININIM_CONSTANTS_H

#define WINDOW_TITLE "MININIM: The Advanced Prince of Persia Engine (a childhood dream)"
#define ICON "data/icons/mininim.png"

#define MIGNORE (INT_MIN)

#define ROOMS 25
#define FLOORS 3
#define PLACES 10
#define EVENTS 256
#define GUARDS 25

#define LEVENTS 256
#define LROOMS 24

#define DISPLAY_WIDTH 640
#define DISPLAY_HEIGHT 400
#define ORIGINAL_WIDTH 320
#define ORIGINAL_HEIGHT 200

#define PLACE_WIDTH (ORIGINAL_WIDTH / PLACES)
#define PLACE_HEIGHT ((ORIGINAL_HEIGHT - 11) / FLOORS)

#define ROOM_HEIGHT (PLACE_HEIGHT * FLOORS)

#define KID_INITIAL_TOTAL_LIVES 3
#define KID_INITIAL_CURRENT_LIVES 3
#define KID_MINIMUM_LIVES_TO_BLINK 1
#define BOTTOM_TEXT_DURATION (3 * SCRIPT_HZ)

#define TIME_LIMIT 3600
#define START_TIME 0

#define INITIAL_KCA -1
#define INITIAL_KCD -1

#define EFFECT_HZ 30
#define SECS_TO_VCYCLES(x) ((x) * EFFECT_HZ)

#define SCRIPT_HZ 12
#define CYCLE_TO_EFFECT_DURATION(x) ((x) * (EFFECT_HZ / SCRIPT_HZ))
#define SECS_TO_SCYCLES(x) ((x) * SCRIPT_HZ)

#define SELECT_CYCLES (3 * SCRIPT_HZ)

#define CHOPPER_WAIT 10
#define CHOPPER_MAX_STEP 5

#define DOOR_MAX_STEP 47
#define DOOR_WAIT SECS_TO_SCYCLES (5)
#define DOOR_CLIMB_LIMIT 40
#define DOOR_WAIT_LOOK 4

#define INVERSION_RANGE 18
#define ATTACK_RANGE 61
#define FIGHT_RANGE 75
#define FOLLOW_RANGE (2 * ORIGINAL_WIDTH)
#define HIT_RANGE 50

#define FIRE_RANDOM_SEED_0 234423479
#define FIRE_RANDOM_SEED_1 501477214

#define LEVEL_DOOR_MAX_STEP 43

#define LOOSE_FLOOR_RESISTENCE 0

#define SPIKES_WAIT SECS_TO_SCYCLES (1)
#define SPIKES_FLOOR_MAX_STEP 9

#define STARS_RANDOM_SEED 78234782
#define STARS 7

#define ESCAPE_KEY 27
#define BACKSPACE_KEY 8

#endif	/* MININIM_CONSTANTS_H */
