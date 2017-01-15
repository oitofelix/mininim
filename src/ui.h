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

enum main_menu {
  BASE_MID = 0,

  /* real menu itens follow */
  GAME_MID,

  LOAD_MID,
  LOAD_GAME_MID,
  LOAD_CONFIG_MID,
  LOAD_LEVEL_FILE_MID,

  SAVE_MID,
  SAVE_GAME_MID,
  SAVE_CONFIG_MID,
  SAVE_LEVEL_FILE_MID,

  RESTART_GAME_MID,
  QUIT_GAME_MID,

  REPLAY_MID,
  PLAY_REPLAY_MID,
  RECORD_REPLAY_MID,

  VIEW_MID,
  FULL_SCREEN_MID,
  SCREENSHOT_MID,

  HELP_MID,
  ABOUT_MID,
};

ALLEGRO_BITMAP *load_menu_icon (char *filename);

void create_main_menu (void);
void update_main_menu_full_screen_item (void);
void update_main_menu_replay_item (void);
void select_main_menu (void);

void show_menu (void);
void hide_menu (void);
bool is_showing_menu (void);
void toggle_menu_visibility (void);
void enable_menu (bool enable);

void ui_load_game (void);
void ui_load_config (void);
void ui_restart_game (void);
void ui_quit_game (void);

void ui_full_screen (void);
void ui_screenshot (void);

void ui_play_replay (void);
void ui_record_replay (void);

extern ALLEGRO_MENU_INFO main_menu_info[];

#endif	/* MININIM_UI_H */
