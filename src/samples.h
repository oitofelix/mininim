/*
  samples.h -- samples module;

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

#ifndef MININIM_SAMPLES_H
#define MININIM_SAMPLES_H

#define STEP_SAMPLE "data/sample/step.ogg"
#define HIT_GROUND_SAMPLE "data/sample/hit-ground.ogg"
#define HIT_GROUND_HARM_SAMPLE "data/sample/hit-ground-harm.ogg"
#define HIT_GROUND_FATAL_SAMPLE "data/sample/hit-ground-fatal.ogg"
#define HIT_WALL_SAMPLE "data/sample/hit-wall.ogg"
#define HANG_ON_FALL_SAMPLE "data/sample/hang-on-fall.ogg"
#define DRINK_SAMPLE "data/sample/drink.ogg"
#define GLORY_SAMPLE "data/sample/glory.ogg"
#define TAKE_SWORD_SAMPLE "data/sample/take-sword.ogg"
#define SWORD_ATTACK_SAMPLE "data/sample/sword-attack.ogg"
#define HARM_SAMPLE "data/sample/harm.ogg"
#define SMALL_LIFE_POTION_SAMPLE "data/sample/small-life-potion.ogg"
#define BIG_LIFE_POTION_SAMPLE "data/sample/big-life-potion.ogg"
#define SCREAM_SAMPLE "data/sample/scream.ogg"
#define SPIKED_SAMPLE "data/sample/spiked.ogg"
#define CHOPPED_SAMPLE "data/sample/chopped.ogg"
#define FLOATING_SAMPLE "data/sample/floating.ogg"
#define DEATH_SAMPLE "data/sample/death.ogg"
#define FIGHT_DEATH_SAMPLE "data/sample/fight-death.ogg"
#define PRESS_KEY_SAMPLE "data/sample/press-key.ogg"
#define MIRROR_SAMPLE "data/sample/mirror.ogg"
#define SUSPENSE_SAMPLE "data/sample/suspense.ogg"
#define SUCCESS_SAMPLE "data/sample/success.ogg"
#define SUCCESS_SUSPENSE_SAMPLE "data/sample/success-suspense.ogg"
#define SWORD_DEFENSE_SAMPLE "data/sample/sword-defense.ogg"
#define CHOPPER_SAMPLE "data/sample/chopper.ogg"
#define DOOR_OPEN_SAMPLE "data/sample/door-open.ogg"
#define DOOR_CLOSE_SAMPLE "data/sample/door-close.ogg"
#define DOOR_END_SAMPLE "data/sample/door-end.ogg"
#define DOOR_ABRUPTLY_CLOSE_SAMPLE "data/sample/door-abruptly-close.ogg"
#define GUARD_HIT_SAMPLE "data/sample/guard-hit.ogg"
#define SKELETON_SAMPLE "data/sample/skeleton.ogg"
#define MEET_VIZIER_SAMPLE "data/sample/meet-vizier.ogg"
#define VIZIER_DEATH_SAMPLE "data/sample/vizier-death.ogg"
#define LEVEL_DOOR_OPEN_SAMPLE "data/sample/level-door-open.ogg"
#define LEVEL_DOOR_CLOSE_SAMPLE "data/sample/level-door-close.ogg"
#define CLOSER_FLOOR_SAMPLE "data/sample/closer-floor.ogg"
#define LOOSE_FLOOR_00_SAMPLE "data/sample/loose-floor-00.ogg"
#define LOOSE_FLOOR_01_SAMPLE "data/sample/loose-floor-01.ogg"
#define LOOSE_FLOOR_02_SAMPLE "data/sample/loose-floor-02.ogg"
#define BROKEN_FLOOR_SAMPLE "data/sample/broken-floor.ogg"
#define SPIKES_SAMPLE "data/sample/spikes.ogg"
#define OPENER_FLOOR_SAMPLE "data/sample/opener-floor.ogg"
#define MAIN_THEME_SAMPLE "data/sample/main-theme.ogg"
#define IN_THE_ABSENCE_SAMPLE "data/sample/in-the-absence.ogg"
#define PRINCESS_WAITING_SAMPLE "data/sample/princess-waiting.ogg"
#define VIZIER_AND_PRINCESS_SAMPLE "data/sample/vizier-and-princess.ogg"
#define MARRY_JAFFAR_SAMPLE "data/sample/marry-jaffar.ogg"
#define CREAKING_DOOR_SAMPLE "data/sample/creaking-door.ogg"
#define CUTSCENE_01_03_05_11_SAMPLE "data/sample/cutscene-01-03-05-11.ogg"
#define CUTSCENE_07_08_SAMPLE "data/sample/cutscene-07-08.ogg"
#define CUTSCENE_14_SAMPLE "data/sample/cutscene-14.ogg"
#define HAPPY_END_SAMPLE "data/sample/happy-end.ogg"
#define CUTSCENE_11_LITTLE_TIME_LEFT_SAMPLE "data/sample/cutscene-11-little-time-left.ogg"
#define CUTSCENE_OUT_OF_TIME_SAMPLE "data/sample/cutscene-out-of-time.ogg"

void load_samples (void);
void unload_samples (void);

extern ALLEGRO_SAMPLE *step_sample,
  *hit_ground_sample,
  *hit_ground_harm_sample,
  *hit_ground_fatal_sample,
  *hit_wall_sample,
  *hang_on_fall_sample,
  *drink_sample,
  *glory_sample,
  *take_sword_sample,
  *sword_attack_sample,
  *harm_sample,
  *small_life_potion_sample,
  *big_life_potion_sample,
  *scream_sample,
  *spiked_sample,
  *chopped_sample,
  *floating_sample,
  *death_sample,
  *fight_death_sample,
  *press_key_sample,
  *mirror_sample,
  *suspense_sample,
  *success_sample,
  *success_suspense_sample,
  *sword_defense_sample,
  *chopper_sample,
  *door_open_sample,
  *door_close_sample,
  *door_end_sample,
  *door_abruptly_close_sample,
  *guard_hit_sample,
  *skeleton_sample,
  *meet_vizier_sample,
  *vizier_death_sample,
  *level_door_open_sample,
  *level_door_close_sample,
  *closer_floor_sample,
  *loose_floor_00_sample,
  *loose_floor_01_sample,
  *loose_floor_02_sample,
  *broken_floor_sample,
  *spikes_sample,
  *opener_floor_sample,
  *main_theme_sample,
  *in_the_absence_sample,
  *princess_waiting_sample,
  *vizier_and_princess_sample,
  *marry_jaffar_sample,
  *creaking_door_sample,
  *cutscene_01_03_05_11_sample,
  *cutscene_07_08_sample,
  *cutscene_14_sample,
  *happy_end_sample,
  *cutscene_11_little_time_left_sample,
  *cutscene_out_of_time_sample;

#endif	/* MININIM_SAMPLES_H */
