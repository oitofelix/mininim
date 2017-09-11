/*
  ui-command.h -- user interface command module;

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

#ifndef MININIM_UI_COMMAND_H
#define MININIM_UI_COMMAND_H

void mirror_mode (bool mirror);
bool in_mirror_mode (void);

bool display_remaining_time (int priority);
void print_game_paused (int priority);

bool ui_save_setting (char *section, char *key, char *value);
char *ui_get_setting (char *section, char *key);
bool ui_msg (int priority, const char *template, ...)
  __attribute__ ((format (printf, 2, 3)));
bool ui_msg_clear (int priority);

void ui_load_replay_favorites (void);
void ui_go_to_replay_favorite (int i);

void ui_load_game (void);
void ui_screenshot (void);
void ui_record_replay (void);
void ui_play_replay (void);
void ui_restart_game (void);
void ui_quit_game (void);
void ui_change_anim_freq (int f);
void ui_about_screen (bool value);
void ui_editor (void);
void ui_version (void);
void ui_audio_volume (float volume);
void ui_full_screen (void);
void ui_flip_screen (int flags, bool correct_mouse, bool save_only);
void ui_flip_gamepad (bool v, bool h, bool save_only);
void ui_gamepad_mode (enum gpm new_gpm);
void ui_hue_mode (enum hue new_hue);
void ui_em (enum em new_em);
void ui_vm (char *new_vm);
void ui_next_display_mode (void);
void ui_jump_to_level_rel (int d);
void ui_jump_to_level_menu (int i, int lower);
void ui_save_game (void);
void ui_gm (enum gm new_gm);
void ui_undo_pass (struct undo *u, int dir, char *prefix);
void ui_zoom_fit (struct mr *mr, enum mr_fit_mode fit);
bool ui_mr_set_dim (struct mr *mr, int dw, int dh, bool correct_mouse);
void ui_show_coordinates (void);
void ui_show_indirect_coordinates (void);
void ui_room_drawing (bool draw);
void ui_kill_enemy (void);
void ui_resurrect (void);
void ui_immortal (bool immortal);
void ui_float (void);
void ui_fill_hp (void);
void ui_add_hp (void);
void ui_time (void);
void ui_skills (void);
void ui_change_time (int m);
void ui_change_kca (int d);
void ui_change_kcd (int d);
void ui_home (void);

void ui_load_config (void);
void ui_mirror_mode (bool mirror);
void ui_start_game (void);

void ui_inhibit_screensaver (bool inhibit);

void ui_save_replay_favorites (void);
void ui_add_replay_favorite (void);
void ui_replace_replay_favorite (int i);
void ui_remove_replay_favorite (int i);
void ui_jump_to_level (int n);
void ui_decrease_time_frequency (void);
void ui_increase_time_frequency (void);
void ui_toggle_time_frequency_constraint (void);
void ui_toggle_pause_game (void);
void ui_next_frame (void);

void ui_move_locked_place_selection (struct mr *mr, enum dir d);

#endif	/* MININIM_UI_COMMAND_H */
