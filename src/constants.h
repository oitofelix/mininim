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

#define LEVELS 14
#define ROOMS 25
#define FLOORS 3
#define PLACES 10
#define EVENTS 256
#define GUARDS 25

#define LEVENTS 256
#define LROOMS 24
#define LFLOORS 3
#define LPLACES 10

#define PLACE_WIDTH 32
#define PLACE_HEIGHT 63

#define CUTSCENE_WIDTH 320
#define CUTSCENE_HEIGHT 200

#define ORIGINAL_WIDTH (PLACE_WIDTH * PLACES)
#define ORIGINAL_HEIGHT (PLACE_HEIGHT * FLOORS + 11)
#define DISPLAY_WIDTH (ORIGINAL_WIDTH * 2)
#define DISPLAY_HEIGHT (ORIGINAL_HEIGHT * 2)

#define ROOM_HEIGHT (PLACE_HEIGHT * FLOORS)

#define LOOSE_FLOOR_STEPS 2
#define LOOSE_FLOOR_FASES 1
#define LOOSE_FLOOR_TOTAL (LOOSE_FLOOR_STEP * LOOSE_FLOOR_FASES)

#define SPIKES_STEPS 10
#define SPIKES_FASES 1
#define SPIKES_TOTAL (SPIKES_STEP * SPIKES_FASES)

#define DOOR_STEPS 48
#define DOOR_FASES 1
#define DOOR_TOTAL (DOOR_STEPS * DOOR_FASES)

#define LEVEL_DOOR_STEPS 44
#define LEVEL_DOOR_FASES 2
#define LEVEL_DOOR_TOTAL (LEVEL_DOOR_STEPS * LEVEL_DOOR_FASES)

#define CHOPPER_STEPS 6
#define CHOPPER_FASES 2
#define CHOPPER_TOTAL (CHOPPER_STEPS * CHOPPER_FASES)

#define KID_INITIAL_TOTAL_LIVES 3
#define KID_INITIAL_CURRENT_LIVES 3
#define KID_MINIMUM_LIVES_TO_BLINK 1
#define BOTTOM_TEXT_DURATION (3 * anim_freq)

#define TIME_LIMIT (3600 * SCRIPT_HZ_DEFAULT)
#define START_TIME 0

#define INITIAL_KCA -1
#define INITIAL_KCD -1

#define SCRIPT_HZ_DEFAULT 12
#define SEC2CYC(x) (anim_freq > 0 ? (x) * anim_freq : (x) / (-anim_freq + 2))
#define CYC2SEC(x) (anim_freq > 0 ? (x) / anim_freq : (x) * (-anim_freq + 2))
#define EFFECT_HZ 30
#define SEC2EFF(x) ((x) * EFFECT_HZ)

#define SELECT_CYCLES (SEC2CYC (3))

#define CHOPPER_WAIT 10

#define DOOR_WAIT SEC2CYC (5)
#define DOOR_CLIMB_LIMIT 40
#define DOOR_WAIT_LOOK 4

#define INVERSION_RANGE 22
#define HIT_RANGE 50
#define ATTACK_RANGE (HIT_RANGE + 10)
#define FIGHT_RANGE (ATTACK_RANGE + 10)
#define FOLLOW_RANGE (2 * ORIGINAL_WIDTH)

#define FIRE_RANDOM_SEED_0 234423479
#define FIRE_RANDOM_SEED_1 501477214

#define LOOSE_FLOOR_RESISTENCE 0

#define SPIKES_WAIT SEC2CYC (1)

#define STARS_RANDOM_SEED 78234782
#define STARS 7

#define ESCAPE_KEY 27
#define BACKSPACE_KEY 8

#define ENEMY_REFRACTION_TIME 36

#define RESERVED_AUDIO_SAMPLES 16

#define DOOR_GRID_TIP_THRESHOLD 10

#define SHOCKWAVE_RADIUS PLACES

#endif	/* MININIM_CONSTANTS_H */
