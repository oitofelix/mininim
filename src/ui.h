/*
  ui.h -- user interface module;

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

#ifndef MININIM_UI_H
#define MININIM_UI_H

#define OPEN_ICON "data/icons/open.png"
#define SAVE_ICON "data/icons/save.png"
#define RELOAD_ICON "data/icons/reload.png"
#define QUIT_ICON "data/icons/quit.png"
#define FULL_SCREEN_ICON "data/icons/full-screen.png"
#define WINDOWS_ICON "data/icons/windows.png"
#define CAMERA_ICON "data/icons/camera.png"
#define PLAY_ICON "data/icons/play.png"
#define RECORD_ICON "data/icons/record.png"
#define STOP_ICON "data/icons/stop.png"
#define EJECT_ICON "data/icons/eject.png"
#define BACKWARD_ICON "data/icons/backward.png"
#define FORWARD_ICON "data/icons/forward.png"
#define PAUSE_ICON "data/icons/pause.png"
#define PREVIOUS_ICON "data/icons/previous.png"
#define NEXT_ICON "data/icons/next.png"
#define SCREEN_ICON "data/icons/screen.png"
#define RIGHT_ICON "data/icons/right.png"
#define DUNGEON_ICON "data/icons/dungeon.png"
#define PALACE_ICON "data/icons/palace.png"
#define GREEN_ICON "data/icons/green.png"
#define GRAY_ICON "data/icons/gray.png"
#define YELLOW_ICON "data/icons/yellow.png"
#define BLUE_ICON "data/icons/blue.png"
#define BLACK_ICON "data/icons/black.png"
#define VGA_ICON "data/icons/vga.png"
#define EGA_ICON "data/icons/ega.png"
#define CGA_ICON "data/icons/cga.png"
#define HGC_ICON "data/icons/hgc.png"
#define VERTICAL_ICON "data/icons/vertical.png"
#define HORIZONTAL_ICON "data/icons/horizontal.png"
#define KEYBOARD_ICON "data/icons/keyboard.png"
#define JOYSTICK_ICON "data/icons/joystick.png"
#define CANCEL_ICON "data/icons/cancel.png"
#define CLOCK_ICON "data/icons/clock.png"
#define EDIT_ICON "data/icons/edit.png"
#define JOYSTICK2_ICON "data/icons/joystick2.png"
#define UNDO_ICON "data/icons/undo.png"
#define REDO_ICON "data/icons/redo.png"
#define SCREENSAVER_ICON "data/icons/screensaver.png"
#define JOYSTICK3_ICON "data/icons/joystick3.png"
#define VOLUME_OFF_ICON "data/icons/volume-off.png"
#define VOLUME_LOW_ICON "data/icons/volume-low.png"
#define VOLUME_MEDIUM_ICON "data/icons/volume-medium.png"
#define VOLUME_HIGH_ICON "data/icons/volume-high.png"
#define GAME_ICON "data/icons/game.png"
#define SETTINGS_ICON "data/icons/settings.png"
#define ZOOM_NONE_ICON "data/icons/zoom-none.png"
#define ZOOM_STRETCH_ICON "data/icons/zoom-stretch.png"
#define ZOOM_RATIO_ICON "data/icons/zoom-ratio.png"
#define VERTICAL_HORIZONTAL_ICON "data/icons/vertical-horizontal.png"
#define ZOOM_OUT_ICON "data/icons/zoom-out.png"
#define ZOOM_IN_ICON "data/icons/zoom-in.png"
#define HEADING_ICON "data/icons/heading.png"
#define ZOOM_ICON "data/icons/zoom.png"
#define NAVIGATION_ICON "data/icons/navigation.png"
#define NAV_SELECT_ICON "data/icons/nav-select.png"
#define NAV_CELL_ICON "data/icons/nav-cell.png"
#define NAV_PAGE_ICON "data/icons/nav-page.png"
#define NAV_LEFT_ICON "data/icons/nav-left.png"
#define NAV_RIGHT_ICON "data/icons/nav-right.png"
#define NAV_ABOVE_ICON "data/icons/nav-above.png"
#define NAV_BELOW_ICON "data/icons/nav-below.png"
#define NAV_HOME_ICON "data/icons/nav-home.png"
#define REPEAT_ICON "data/icons/repeat.png"
#define COMPASS_ICON "data/icons/compass.png"
#define COMPASS2_ICON "data/icons/compass2.png"
#define DRAWING_ICON "data/icons/drawing.png"
#define FIRST_ICON "data/icons/first.png"
#define LAST_ICON "data/icons/last.png"
#define JUMP_ICON "data/icons/jump.png"
#define ORIGINAL_ICON "data/icons/original.png"
#define GUARD_ICON "data/icons/guard.png"
#define FAT_GUARD_ICON "data/icons/fat-guard.png"
#define VIZIER_ICON "data/icons/vizier.png"
#define SKELETON_ICON "data/icons/skeleton.png"
#define SHADOW_ICON "data/icons/shadow.png"
#define RESURRECT_ICON "data/icons/resurrect.png"
#define DEATH_ICON "data/icons/death.png"
#define FEATHER_ICON "data/icons/feather.png"
#define ANGEL_ICON "data/icons/angel.png"
#define LIFE_EMPTY_ICON "data/icons/life-empty.png"
#define LIFE_FULL_ICON "data/icons/life-full.png"
#define SKILLS_ICON "data/icons/skills.png"
#define TIME_ICON "data/icons/time.png"
#define TIME_ADD_ICON "data/icons/time-add.png"
#define TIME_SUB_ICON "data/icons/time-sub.png"
#define ATTACK_ICON "data/icons/attack.png"
#define ATTACK_ADD_ICON "data/icons/attack-add.png"
#define ATTACK_SUB_ICON "data/icons/attack-sub.png"
#define DEFENSE_ICON "data/icons/defense.png"
#define DEFENSE_ADD_ICON "data/icons/defense-add.png"
#define DEFENSE_SUB_ICON "data/icons/defense-sub.png"
#define COUNTER_ATTACK_ICON "data/icons/counter-attack.png"
#define COUNTER_ATTACK_ADD_ICON "data/icons/counter-attack-add.png"
#define COUNTER_ATTACK_SUB_ICON "data/icons/counter-attack-sub.png"
#define COUNTER_DEFENSE_ICON "data/icons/counter-defense.png"
#define COUNTER_DEFENSE_ADD_ICON "data/icons/counter-defense-add.png"
#define COUNTER_DEFENSE_SUB_ICON "data/icons/counter-defense-sub.png"
#define SHADOW_FACE_ICON "data/icons/shadow-face.png"

/* MID arrays */
#define SEP_MID_NMEMB 1000
#define JUMP_TO_LEVEL_MID_NMEMB 21

enum main_menu {
  BASE_MID = 0,

  /* real menu items follow */
  GAME_MID,
  LOAD_MID,
  LOAD_GAME_MID,
  LOAD_CONFIG_MID,
  LOAD_LEVEL_FILE_MID,
  SAVE_MID,
  SAVE_GAME_MID,
  SAVE_CONFIG_MID,
  SAVE_LEVEL_FILE_MID,
  MIRROR_MODE_MID,
  VOLUME_MID,
  VOLUME_OFF_MID,
  VOLUME_LOW_MID,
  VOLUME_MEDIUM_MID,
  VOLUME_HIGH_MID,
  RESTART_GAME_MID,
  START_GAME_MID,
  QUIT_GAME_MID,

  VIEW_MID,
  ZOOM_MID,
  ZOOM_FIT_MID,
  ZOOM_NONE_MID,
  ZOOM_STRETCH_MID,
  ZOOM_RATIO_MID,
  ZOOM_OUT_MID,
  ZOOM_OUT_BOTH_MID,
  ZOOM_OUT_VERTICAL_MID,
  ZOOM_OUT_HORIZONTAL_MID,
  ZOOM_IN_MID,
  ZOOM_IN_BOTH_MID,
  ZOOM_IN_VERTICAL_MID,
  ZOOM_IN_HORIZONTAL_MID,
  ZOOM_RESET_MID,
  NAV_MID,
  NAV_CURRENT_ROOM_SELECT_MID,
  NAV_SELECT_MID,
  NAV_SELECT_LEFT_MID,
  NAV_SELECT_RIGHT_MID,
  NAV_SELECT_ABOVE_MID,
  NAV_SELECT_BELOW_MID,
  NAV_CELL_MID,
  NAV_CELL_LEFT_MID,
  NAV_CELL_RIGHT_MID,
  NAV_CELL_ABOVE_MID,
  NAV_CELL_BELOW_MID,
  NAV_PAGE_MID,
  NAV_PAGE_LEFT_MID,
  NAV_PAGE_RIGHT_MID,
  NAV_PAGE_ABOVE_MID,
  NAV_PAGE_BELOW_MID,
  NAV_HOME_MID,
  NAV_CENTER_MID,
  COORDINATES_MID,
  INDIRECT_COORDINATES_MID,
  HUE_MODE_MID,
  HUE_ORIGINAL_MID,
  HUE_NONE_MID,
  HUE_GREEN_MID,
  HUE_GRAY_MID,
  HUE_YELLOW_MID,
  HUE_BLUE_MID,
  GM_MID,
  ORIGINAL_GM_MID,
  GUARD_GM_MID,
  FAT_GUARD_GM_MID,
  VIZIER_GM_MID,
  SKELETON_GM_MID,
  SHADOW_GM_MID,
  EM_MID,
  ORIGINAL_EM_MID,
  DUNGEON_MID,
  PALACE_MID,
  VM_MID,
  VGA_MID,
  EGA_MID,
  CGA_MID,
  HGC_MID,
  FULL_SCREEN_MID,
  FLIP_SCREEN_MID,
  FLIP_SCREEN_VERTICAL_MID,
  FLIP_SCREEN_HORIZONTAL_MID,
  INHIBIT_SCREENSAVER_MID,
  ROOM_DRAWING_MID,
  SCREENSHOT_MID,

  GAMEPAD_MID,
  GAMEPAD_DEVICE_MID,
  KEYBOARD_MODE_MID,
  JOYSTICK_MODE_MID,
  GAMEPAD_CALIBRATE_MID,
  FLIP_GAMEPAD_MID,
  FLIP_GAMEPAD_VERTICAL_MID,
  FLIP_GAMEPAD_HORIZONTAL_MID,

  PLAY_MID,
  REPLAYING_HEADER_MID,
  PLAY_REPLAY_MID,
  RECORDING_HEADER_MID,
  RECORD_REPLAY_MID,
  JUMP_TO_LEVEL_MID,
  RESTART_LEVEL_MID,
  PREVIOUS_LEVEL_MID,
  NEXT_LEVEL_MID,
  TIME_MID,
  SKILLS_MID,
  CYCLE_HEADER_MID,
  TOGGLE_PAUSE_GAME_MID,
  NEXT_FRAME_MID,
  RESET_TIME_FREQ_MID,
  INCREASE_TIME_FREQ_MID,
  DECREASE_TIME_FREQ_MID,
  TOGGLE_TIME_FREQ_CONSTRAINT_MID,

  EDITOR_MID,
  EDIT_MODE_MID,
  UNDO_MID,
  REDO_MID,

  CHEAT_MID,
  RESURRECT_MID,
  KILL_ENEMY_MID,
  FLOAT_MID,
  IMMORTAL_MID,
  FILL_LIFE_MID,
  ADD_LIFE_MID,
  FELLOW_SHADOW_MID,
  TIME_CHANGE_MID,
  TIME_ADD_MID,
  TIME_SUB_MID,
  KCA_CHANGE_MID,
  KCA_ADD_MID,
  KCA_SUB_MID,
  KCD_CHANGE_MID,
  KCD_ADD_MID,
  KCD_SUB_MID,
  HELP_MID,
  ABOUT_MID,

  /* range used for separators */
  SEP_MID,

  /* range used for jump to level submenu */
  JUMP_TO_LEVEL_1_MID = SEP_MID + SEP_MID_NMEMB,
};

/* functions */
void load_icons (void);
void unload_icons (void);

void main_menu (void);

void ui_about_screen (bool value);
void ui_editor (void);

void menu_mid (ALLEGRO_EVENT *event);
void anim_key_bindings (void);
void level_key_bindings (void);

void show_menu (void);
void hide_menu (void);
bool is_showing_menu (void);
void toggle_menu_visibility (void);

void mirror_mode (bool mirror);
bool in_mirror_mode (void);

bool display_remaining_time (int priority);
void print_game_paused (int priority);

void ui_save_setting (char *key, char *value);
bool ui_msg (int priority, const char *template, ...)
  __attribute__ ((format (printf, 2, 3)));
bool ui_msg_clear (int priority);


/* variables */
bool main_menu_enabled;

#endif	/* MININIM_UI_H */
