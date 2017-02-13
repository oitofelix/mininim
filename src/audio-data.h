/*
  audio-data.h -- audio data module;

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

#ifndef MININIM_AUDIO_DATA_H
#define MININIM_AUDIO_DATA_H

/* audio data */
#define BIG_LIFE_POTION_AUDIO "data/audio/big-life-potion.ogg"
#define STEP_AUDIO "data/audio/step.ogg"
#define HIT_GROUND_AUDIO "data/audio/hit-ground.ogg"
#define HIT_GROUND_HARM_AUDIO "data/audio/hit-ground-harm.ogg"
#define HIT_GROUND_FATAL_AUDIO "data/audio/hit-ground-fatal.ogg"
#define HIT_WALL_AUDIO "data/audio/hit-wall.ogg"
#define HANG_ON_FALL_AUDIO "data/audio/hang-on-fall.ogg"
#define DRINK_AUDIO "data/audio/drink.ogg"
#define TAKE_SWORD_AUDIO "data/audio/take-sword.ogg"
#define SWORD_ATTACK_AUDIO "data/audio/sword-attack.ogg"
#define HARM_AUDIO "data/audio/harm.ogg"
#define SCREAM_AUDIO "data/audio/scream.ogg"
#define SPIKED_AUDIO "data/audio/spiked.ogg"
#define CHOPPED_AUDIO "data/audio/chopped.ogg"
#define PRESS_KEY_AUDIO "data/audio/press-key.ogg"
#define MIRROR_AUDIO "data/audio/mirror.ogg"
#define SWORD_DEFENSE_AUDIO "data/audio/sword-defense.ogg"
#define CHOPPER_AUDIO "data/audio/chopper.ogg"
#define DOOR_OPEN_AUDIO "data/audio/door-open.ogg"
#define DOOR_CLOSE_AUDIO "data/audio/door-close.ogg"
#define DOOR_END_AUDIO "data/audio/door-end.ogg"
#define DOOR_ABRUPTLY_CLOSE_AUDIO "data/audio/door-abruptly-close.ogg"
#define GUARD_HIT_AUDIO "data/audio/guard-hit.ogg"
#define SKELETON_AUDIO "data/audio/skeleton.ogg"
#define LEVEL_DOOR_OPEN_AUDIO "data/audio/level-door-open.ogg"
#define LEVEL_DOOR_CLOSE_AUDIO "data/audio/level-door-close.ogg"
#define CLOSER_FLOOR_AUDIO "data/audio/closer-floor.ogg"
#define LOOSE_FLOOR_00_AUDIO "data/audio/loose-floor-00.ogg"
#define LOOSE_FLOOR_01_AUDIO "data/audio/loose-floor-01.ogg"
#define LOOSE_FLOOR_02_AUDIO "data/audio/loose-floor-02.ogg"
#define BROKEN_FLOOR_AUDIO "data/audio/broken-floor.ogg"
#define SPIKES_AUDIO "data/audio/spikes.ogg"
#define OPENER_FLOOR_AUDIO "data/audio/opener-floor.ogg"
#define CREAKING_DOOR_AUDIO "data/audio/creaking-door.ogg"
#define DEATH_AUDIO "data/audio/death.ogg"
#define MAIN_THEME_AUDIO "data/audio/main-theme.ogg"
#define CUTSCENE_01_03_05_11_AUDIO "data/audio/cutscene-01-03-05-11.ogg"
#define CUTSCENE_07_08_AUDIO "data/audio/cutscene-07-08.ogg"
#define CUTSCENE_14_AUDIO "data/audio/cutscene-14.ogg"
#define CUTSCENE_11_LITTLE_TIME_LEFT_AUDIO "data/audio/cutscene-11-little-time-left.ogg"
#define CUTSCENE_OUT_OF_TIME_AUDIO "data/audio/cutscene-out-of-time.ogg"
#define FIGHT_DEATH_AUDIO "data/audio/fight-death.ogg"
#define VIZIER_DEATH_AUDIO "data/audio/vizier-death.ogg"
#define FLOATING_AUDIO "data/audio/floating.ogg"
#define GLORY_AUDIO "data/audio/glory.ogg"
#define HAPPY_END_AUDIO "data/audio/happy-end.ogg"
#define IN_THE_ABSENCE_AUDIO "data/audio/in-the-absence.ogg"
#define MARRY_JAFFAR_AUDIO "data/audio/marry-jaffar.ogg"
#define MEET_VIZIER_AUDIO "data/audio/meet-vizier.ogg"
#define PRINCESS_WAITING_AUDIO "data/audio/princess-waiting.ogg"
#define VIZIER_AND_PRINCESS_AUDIO "data/audio/vizier-and-princess.ogg"
#define SMALL_LIFE_POTION_AUDIO "data/audio/small-life-potion.ogg"
#define SUCCESS_AUDIO "data/audio/success.ogg"
#define SUCCESS_SUSPENSE_AUDIO "data/audio/success-suspense.ogg"
#define SUSPENSE_AUDIO "data/audio/suspense.ogg"

/* functions */
void load_audio_data (void);
void unload_audio_data (void);

/* audio variables */
extern struct audio_source
  big_life_potion_audio,
  step_audio,
  hit_ground_audio,
  hit_ground_harm_audio,
  hit_ground_fatal_audio,
  hit_wall_audio,
  hang_on_fall_audio,
  drink_audio,
  take_sword_audio,
  sword_attack_audio,
  harm_audio,
  scream_audio,
  spiked_audio,
  chopped_audio,
  press_key_audio,
  mirror_audio,
  sword_defense_audio,
  chopper_audio,
  door_open_audio,
  door_close_audio,
  door_end_audio,
  door_abruptly_close_audio,
  guard_hit_audio,
  skeleton_audio,
  level_door_open_audio,
  level_door_close_audio,
  closer_floor_audio,
  loose_floor_00_audio,
  loose_floor_01_audio,
  loose_floor_02_audio,
  broken_floor_audio,
  spikes_audio,
  opener_floor_audio,
  death_audio,
  creaking_door_audio,
  main_theme_audio,
  cutscene_01_03_05_11_audio,
  cutscene_07_08_audio,
  cutscene_14_audio,
  cutscene_11_little_time_left_audio,
  cutscene_out_of_time_audio,
  fight_death_audio,
  vizier_death_audio,
  floating_audio,
  glory_audio,
  happy_end_audio,
  in_the_absence_audio,
  marry_jaffar_audio,
  meet_vizier_audio,
  princess_waiting_audio,
  small_life_potion_audio,
  success_audio,
  success_suspense_audio,
  vizier_and_princess_audio;

#endif	/* MININIM_AUDIO_DATA_H */
