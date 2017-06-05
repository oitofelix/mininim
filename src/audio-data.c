/*
  audio-data.c -- audio data module;

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

/* audio */
struct audio_source big_hp_potion_audio,
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
  chomped_audio,
  press_key_audio,
  mirror_audio,
  sword_defense_audio,
  chomper_audio,
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
  creaking_door_audio,
  main_theme_audio,
  cutscene_01_03_05_11_audio,
  cutscene_07_08_audio,
  cutscene_14_audio,
  cutscene_11_little_time_left_audio,
  cutscene_out_of_time_audio,
  death_audio,
  fight_death_audio,
  vizier_death_audio,
  floating_audio,
  glory_audio,
  happy_end_audio,
  in_the_absence_audio,
  marry_jaffar_audio,
  meet_vizier_audio,
  princess_waiting_audio,
  small_hp_potion_audio,
  success_audio,
  success_suspense_audio,
  vizier_and_princess_audio;

void
load_audio_data (void)
{
  load_audio (&big_hp_potion_audio, AUDIO_SAMPLE, BIG_HP_POTION_AUDIO);
  load_audio (&step_audio, AUDIO_SAMPLE, STEP_AUDIO);
  load_audio (&hit_ground_audio, AUDIO_SAMPLE, HIT_GROUND_AUDIO);
  load_audio (&hit_ground_harm_audio, AUDIO_SAMPLE, HIT_GROUND_HARM_AUDIO);
  load_audio (&hit_ground_fatal_audio, AUDIO_SAMPLE, HIT_GROUND_FATAL_AUDIO);
  load_audio (&hit_wall_audio, AUDIO_SAMPLE, HIT_WALL_AUDIO);
  load_audio (&hang_on_fall_audio, AUDIO_SAMPLE, HANG_ON_FALL_AUDIO);
  load_audio (&drink_audio, AUDIO_SAMPLE, DRINK_AUDIO);
  load_audio (&take_sword_audio, AUDIO_SAMPLE, TAKE_SWORD_AUDIO);
  load_audio (&sword_attack_audio, AUDIO_SAMPLE, SWORD_ATTACK_AUDIO);
  load_audio (&harm_audio, AUDIO_SAMPLE, HARM_AUDIO);
  load_audio (&scream_audio, AUDIO_SAMPLE, SCREAM_AUDIO);
  load_audio (&spiked_audio, AUDIO_SAMPLE, SPIKED_AUDIO);
  load_audio (&chomped_audio, AUDIO_SAMPLE, CHOMPED_AUDIO);
  load_audio (&press_key_audio, AUDIO_SAMPLE, PRESS_KEY_AUDIO);
  load_audio (&mirror_audio, AUDIO_SAMPLE, MIRROR_AUDIO);
  load_audio (&sword_defense_audio, AUDIO_SAMPLE, SWORD_DEFENSE_AUDIO);
  load_audio (&chomper_audio, AUDIO_SAMPLE, CHOMPER_AUDIO);
  load_audio (&door_open_audio, AUDIO_SAMPLE, DOOR_OPEN_AUDIO);
  load_audio (&door_close_audio, AUDIO_SAMPLE, DOOR_CLOSE_AUDIO);
  load_audio (&door_end_audio, AUDIO_SAMPLE, DOOR_END_AUDIO);
  load_audio (&door_abruptly_close_audio, AUDIO_SAMPLE, DOOR_ABRUPTLY_CLOSE_AUDIO);
  load_audio (&guard_hit_audio, AUDIO_SAMPLE, GUARD_HIT_AUDIO);
  load_audio (&skeleton_audio, AUDIO_SAMPLE, SKELETON_AUDIO);
  load_audio (&level_door_open_audio, AUDIO_SAMPLE, LEVEL_DOOR_OPEN_AUDIO);
  load_audio (&level_door_close_audio, AUDIO_SAMPLE, LEVEL_DOOR_CLOSE_AUDIO);
  load_audio (&closer_floor_audio, AUDIO_SAMPLE, CLOSER_FLOOR_AUDIO);
  load_audio (&loose_floor_00_audio, AUDIO_SAMPLE, LOOSE_FLOOR_00_AUDIO);
  load_audio (&loose_floor_01_audio, AUDIO_SAMPLE, LOOSE_FLOOR_01_AUDIO);
  load_audio (&loose_floor_02_audio, AUDIO_SAMPLE, LOOSE_FLOOR_02_AUDIO);
  load_audio (&broken_floor_audio, AUDIO_SAMPLE, BROKEN_FLOOR_AUDIO);
  load_audio (&spikes_audio, AUDIO_SAMPLE, SPIKES_AUDIO);
  load_audio (&opener_floor_audio, AUDIO_SAMPLE, OPENER_FLOOR_AUDIO);
  load_audio (&creaking_door_audio, AUDIO_SAMPLE, CREAKING_DOOR_AUDIO);
  load_audio (&main_theme_audio, AUDIO_STREAM, MAIN_THEME_AUDIO);
  load_audio (&cutscene_01_03_05_11_audio, AUDIO_STREAM, CUTSCENE_01_03_05_11_AUDIO);
  load_audio (&cutscene_07_08_audio, AUDIO_STREAM, CUTSCENE_07_08_AUDIO);
  load_audio (&cutscene_14_audio, AUDIO_STREAM, CUTSCENE_14_AUDIO);
  load_audio (&cutscene_11_little_time_left_audio, AUDIO_STREAM,
              CUTSCENE_11_LITTLE_TIME_LEFT_AUDIO);
  load_audio (&cutscene_out_of_time_audio, AUDIO_STREAM, CUTSCENE_OUT_OF_TIME_AUDIO);
  load_audio (&death_audio, AUDIO_SAMPLE, DEATH_AUDIO);
  load_audio (&fight_death_audio, AUDIO_SAMPLE, FIGHT_DEATH_AUDIO);
  load_audio (&vizier_death_audio, AUDIO_SAMPLE, VIZIER_DEATH_AUDIO);
  load_audio (&floating_audio, AUDIO_STREAM, FLOATING_AUDIO);
  load_audio (&glory_audio, AUDIO_SAMPLE, GLORY_AUDIO);
  load_audio (&happy_end_audio, AUDIO_STREAM, HAPPY_END_AUDIO);
  load_audio (&in_the_absence_audio, AUDIO_STREAM, IN_THE_ABSENCE_AUDIO);
  load_audio (&marry_jaffar_audio, AUDIO_STREAM, MARRY_JAFFAR_AUDIO);
  load_audio (&meet_vizier_audio, AUDIO_SAMPLE, MEET_VIZIER_AUDIO);
  load_audio (&princess_waiting_audio, AUDIO_STREAM, PRINCESS_WAITING_AUDIO);
  load_audio (&small_hp_potion_audio, AUDIO_SAMPLE, SMALL_HP_POTION_AUDIO);
  load_audio (&success_audio, AUDIO_STREAM, SUCCESS_AUDIO);
  load_audio (&success_suspense_audio, AUDIO_STREAM, SUCCESS_SUSPENSE_AUDIO);
  load_audio (&vizier_and_princess_audio, AUDIO_STREAM, VIZIER_AND_PRINCESS_AUDIO);
}

void
unload_audio_data (void)
{
  destroy_audio (&big_hp_potion_audio);
  destroy_audio (&step_audio);
  destroy_audio (&hit_ground_audio);
  destroy_audio (&hit_ground_harm_audio);
  destroy_audio (&hit_ground_fatal_audio);
  destroy_audio (&hit_wall_audio);
  destroy_audio (&hang_on_fall_audio);
  destroy_audio (&drink_audio);
  destroy_audio (&take_sword_audio);
  destroy_audio (&sword_attack_audio);
  destroy_audio (&harm_audio);
  destroy_audio (&scream_audio);
  destroy_audio (&spiked_audio);
  destroy_audio (&chomped_audio);
  destroy_audio (&press_key_audio);
  destroy_audio (&mirror_audio);
  destroy_audio (&sword_defense_audio);
  destroy_audio (&chomper_audio);
  destroy_audio (&door_open_audio);
  destroy_audio (&door_close_audio);
  destroy_audio (&door_end_audio);
  destroy_audio (&door_abruptly_close_audio);
  destroy_audio (&guard_hit_audio);
  destroy_audio (&skeleton_audio);
  destroy_audio (&level_door_open_audio);
  destroy_audio (&level_door_close_audio);
  destroy_audio (&closer_floor_audio);
  destroy_audio (&loose_floor_00_audio);
  destroy_audio (&loose_floor_01_audio);
  destroy_audio (&loose_floor_02_audio);
  destroy_audio (&broken_floor_audio);
  destroy_audio (&spikes_audio);
  destroy_audio (&opener_floor_audio);
  destroy_audio (&creaking_door_audio);
  destroy_audio (&main_theme_audio);
  destroy_audio (&cutscene_01_03_05_11_audio);
  destroy_audio (&cutscene_07_08_audio);
  destroy_audio (&cutscene_14_audio);
  destroy_audio (&cutscene_11_little_time_left_audio);
  destroy_audio (&cutscene_out_of_time_audio);
  destroy_audio (&death_audio);
  destroy_audio (&fight_death_audio);
  destroy_audio (&vizier_death_audio);
  destroy_audio (&floating_audio);
  destroy_audio (&glory_audio);
  destroy_audio (&happy_end_audio);
  destroy_audio (&in_the_absence_audio);
  destroy_audio (&marry_jaffar_audio);
  destroy_audio (&meet_vizier_audio);
  destroy_audio (&princess_waiting_audio);
  destroy_audio (&small_hp_potion_audio);
  destroy_audio (&success_audio);
  destroy_audio (&success_suspense_audio);
  destroy_audio (&vizier_and_princess_audio);
}
