/*
  kid.c -- kid module;

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

ALLEGRO_SAMPLE *step_sample,
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

void
load_samples (void)
{
  step_sample = load_sample (STEP_SAMPLE);
  hit_ground_sample = load_sample (HIT_GROUND_SAMPLE);
  hit_ground_harm_sample = load_sample (HIT_GROUND_HARM_SAMPLE);
  hit_ground_fatal_sample = load_sample (HIT_GROUND_FATAL_SAMPLE);
  hit_wall_sample = load_sample (HIT_WALL_SAMPLE);
  hang_on_fall_sample = load_sample (HANG_ON_FALL_SAMPLE);
  drink_sample = load_sample (DRINK_SAMPLE);
  glory_sample = load_sample (GLORY_SAMPLE);
  take_sword_sample = load_sample (TAKE_SWORD_SAMPLE);
  sword_attack_sample = load_sample (SWORD_ATTACK_SAMPLE);
  harm_sample = load_sample (HARM_SAMPLE);
  small_life_potion_sample = load_sample (SMALL_LIFE_POTION_SAMPLE);
  big_life_potion_sample = load_sample (BIG_LIFE_POTION_SAMPLE);
  scream_sample = load_sample (SCREAM_SAMPLE);
  spiked_sample = load_sample (SPIKED_SAMPLE);
  chopped_sample = load_sample (CHOPPED_SAMPLE);
  floating_sample = load_sample (FLOATING_SAMPLE);
  death_sample = load_sample (DEATH_SAMPLE);
  fight_death_sample = load_sample (FIGHT_DEATH_SAMPLE);
  press_key_sample = load_sample (PRESS_KEY_SAMPLE);
  mirror_sample = load_sample (MIRROR_SAMPLE);
  suspense_sample = load_sample (SUSPENSE_SAMPLE);
  success_sample = load_sample (SUCCESS_SAMPLE);
  success_suspense_sample = load_sample (SUCCESS_SUSPENSE_SAMPLE);
  sword_defense_sample = load_sample (SWORD_DEFENSE_SAMPLE);
  chopper_sample = load_sample (CHOPPER_SAMPLE);
  door_open_sample = load_sample (DOOR_OPEN_SAMPLE);
  door_close_sample = load_sample (DOOR_CLOSE_SAMPLE);
  door_end_sample = load_sample (DOOR_END_SAMPLE);
  door_abruptly_close_sample = load_sample (DOOR_ABRUPTLY_CLOSE_SAMPLE);
  guard_hit_sample = load_sample (GUARD_HIT_SAMPLE);
  skeleton_sample = load_sample (SKELETON_SAMPLE);
  meet_vizier_sample = load_sample (MEET_VIZIER_SAMPLE);
  vizier_death_sample = load_sample (VIZIER_DEATH_SAMPLE);
  level_door_open_sample = load_sample (LEVEL_DOOR_OPEN_SAMPLE);
  level_door_close_sample = load_sample (LEVEL_DOOR_CLOSE_SAMPLE);
  closer_floor_sample = load_sample (CLOSER_FLOOR_SAMPLE);
  loose_floor_00_sample = load_sample (LOOSE_FLOOR_00_SAMPLE);
  loose_floor_01_sample = load_sample (LOOSE_FLOOR_01_SAMPLE);
  loose_floor_02_sample = load_sample (LOOSE_FLOOR_02_SAMPLE);
  broken_floor_sample = load_sample (BROKEN_FLOOR_SAMPLE);
  spikes_sample = load_sample (SPIKES_SAMPLE);
  opener_floor_sample = load_sample (OPENER_FLOOR_SAMPLE);
  main_theme_sample = load_sample (MAIN_THEME_SAMPLE);
  in_the_absence_sample = load_sample (IN_THE_ABSENCE_SAMPLE);
  princess_waiting_sample = load_sample (PRINCESS_WAITING_SAMPLE);
  vizier_and_princess_sample = load_sample (VIZIER_AND_PRINCESS_SAMPLE);
  marry_jaffar_sample = load_sample (MARRY_JAFFAR_SAMPLE);
  creaking_door_sample = load_sample (CREAKING_DOOR_SAMPLE);
  cutscene_01_03_05_11_sample = load_sample (CUTSCENE_01_03_05_11_SAMPLE);
  cutscene_07_08_sample = load_sample (CUTSCENE_07_08_SAMPLE);
  cutscene_14_sample = load_sample (CUTSCENE_14_SAMPLE);
  happy_end_sample = load_sample (HAPPY_END_SAMPLE);
  cutscene_11_little_time_left_sample = load_sample (CUTSCENE_11_LITTLE_TIME_LEFT_SAMPLE);
  cutscene_out_of_time_sample = load_sample (CUTSCENE_OUT_OF_TIME_SAMPLE);
}

void
unload_samples (void)
{
  al_destroy_sample (step_sample);
  al_destroy_sample (hit_ground_sample);
  al_destroy_sample (hit_ground_harm_sample);
  al_destroy_sample (hit_ground_fatal_sample);
  al_destroy_sample (hit_wall_sample);
  al_destroy_sample (hang_on_fall_sample);
  al_destroy_sample (drink_sample);
  al_destroy_sample (glory_sample);
  al_destroy_sample (take_sword_sample);
  al_destroy_sample (sword_attack_sample);
  al_destroy_sample (harm_sample);
  al_destroy_sample (small_life_potion_sample);
  al_destroy_sample (big_life_potion_sample);
  al_destroy_sample (scream_sample);
  al_destroy_sample (spiked_sample);
  al_destroy_sample (chopped_sample);
  al_destroy_sample (floating_sample);
  al_destroy_sample (death_sample);
  al_destroy_sample (fight_death_sample);
  al_destroy_sample (press_key_sample);
  al_destroy_sample (mirror_sample);
  al_destroy_sample (suspense_sample);
  al_destroy_sample (success_sample);
  al_destroy_sample (success_suspense_sample);
  al_destroy_sample (sword_defense_sample);
  al_destroy_sample (chopper_sample);
  al_destroy_sample (door_open_sample);
  al_destroy_sample (door_close_sample);
  al_destroy_sample (door_end_sample);
  al_destroy_sample (door_abruptly_close_sample);
  al_destroy_sample (guard_hit_sample);
  al_destroy_sample (skeleton_sample);
  al_destroy_sample (meet_vizier_sample);
  al_destroy_sample (vizier_death_sample);
  al_destroy_sample (level_door_open_sample);
  al_destroy_sample (level_door_close_sample);
  al_destroy_sample (closer_floor_sample);
  al_destroy_sample (loose_floor_00_sample);
  al_destroy_sample (loose_floor_01_sample);
  al_destroy_sample (loose_floor_02_sample);
  al_destroy_sample (broken_floor_sample);
  al_destroy_sample (spikes_sample);
  al_destroy_sample (opener_floor_sample);
  al_destroy_sample (main_theme_sample);
  al_destroy_sample (in_the_absence_sample);
  al_destroy_sample (princess_waiting_sample);
  al_destroy_sample (vizier_and_princess_sample);
  al_destroy_sample (marry_jaffar_sample);
  al_destroy_sample (creaking_door_sample);
  al_destroy_sample (cutscene_01_03_05_11_sample);
  al_destroy_sample (cutscene_07_08_sample);
  al_destroy_sample (cutscene_14_sample);
  al_destroy_sample (happy_end_sample);
  al_destroy_sample (cutscene_11_little_time_left_sample);
  al_destroy_sample (cutscene_out_of_time_sample);
}
