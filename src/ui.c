/*
  ui.c -- user interface module;

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

#if MENU_FEATURE
ALLEGRO_MENU *main_menu;
ALLEGRO_MENU *current_menu;
#endif

#if MENU_FEATURE
ALLEGRO_MENU_INFO main_menu_info[] = {
  ALLEGRO_START_OF_MENU ("&Game", GAME_MID),
    ALLEGRO_START_OF_MENU ("&Load", LOAD_MID),
    { "&Game... (Ctrl+L)", LOAD_GAME_MID, 0, NULL },
    { "&Configuration... (Ctrl+L)", LOAD_CONFIG_MID, 0, NULL },
    { "&Level file...", LOAD_LEVEL_FILE_MID,
      ALLEGRO_MENU_ITEM_DISABLED, NULL },
    ALLEGRO_END_OF_MENU,
    ALLEGRO_START_OF_MENU ("&Save", SAVE_MID),
    { "&Game... (Ctrl+G)", SAVE_GAME_MID, 0, NULL },
    { "&Configuration...", SAVE_CONFIG_MID,
      ALLEGRO_MENU_ITEM_DISABLED, NULL },
    { "&Level file...", SAVE_LEVEL_FILE_MID,
      ALLEGRO_MENU_ITEM_DISABLED, NULL },
    ALLEGRO_END_OF_MENU,
    ALLEGRO_MENU_SEPARATOR,
    { "&Restart (Ctrl+R)", RESTART_GAME_MID, 0, NULL },
    { "&Quit (Ctrl+Q)", QUIT_GAME_MID, 0, NULL },
  ALLEGRO_END_OF_MENU,

  ALLEGRO_START_OF_MENU ("&View", VIEW_MID),
    { "&Full screen (F)", FULL_SCREEN_MID, 0, NULL },
    { "&Screenshot... (Ctrl+P)", SCREENSHOT_MID, 0, NULL },
  ALLEGRO_END_OF_MENU,

  ALLEGRO_START_OF_MENU ("&Replay", REPLAY_MID),
    { "&Play... (F7)", PLAY_REPLAY_MID, 0, NULL },
    { "&Record... (ALT+F7)", RECORD_REPLAY_MID, 0, NULL },
  ALLEGRO_END_OF_MENU,

  ALLEGRO_START_OF_MENU ("&Help", HELP_MID),
    { "&About", ABOUT_MID, 0, NULL },
  ALLEGRO_END_OF_MENU,

  ALLEGRO_END_OF_MENU
};
#endif

ALLEGRO_BITMAP *
load_menu_icon (char *filename)
{
  if (MINGW_BUILD) return load_scaled_memory_bitmap (filename, 13, 13);
  else return load_scaled_memory_bitmap (filename, 16, 16);
}

void
create_main_menu (void)
{
#if MENU_FEATURE
  main_menu = al_build_menu (main_menu_info);

  /* open */
  al_set_menu_item_icon
    (main_menu, LOAD_MID, load_menu_icon (OPEN_ICON));

  /* save */
  al_set_menu_item_icon
    (main_menu, SAVE_MID, load_menu_icon (SAVE_ICON));

  /* restart game */
  al_set_menu_item_icon
    (main_menu, RESTART_GAME_MID, load_menu_icon (RELOAD_ICON));

  /* quit game */
  al_set_menu_item_icon
    (main_menu, QUIT_GAME_MID, load_menu_icon (QUIT_ICON));

  /* full screen */
  update_main_menu_full_screen_item ();

  /* screenshot */
  al_set_menu_item_icon
    (main_menu, SCREENSHOT_MID, load_menu_icon (CAMERA_ICON));

  /* replay */
  update_main_menu_replay_item ();

  /* about */
  al_set_menu_item_icon
    (main_menu, ABOUT_MID, load_menu_icon (SMALL_LOGO_ICON));
#endif
}

void
update_main_menu_full_screen_item (void)
{
#if MENU_FEATURE
  if (is_fullscreen ()) {
    al_set_menu_item_caption
      (main_menu, FULL_SCREEN_MID, "&Windowed (F)");
    al_set_menu_item_icon
      (main_menu, FULL_SCREEN_MID, load_menu_icon (WINDOWS_ICON));
  } else {
    al_set_menu_item_caption
      (main_menu, FULL_SCREEN_MID, "&Full screen (F)");
    al_set_menu_item_icon
      (main_menu, FULL_SCREEN_MID, load_menu_icon (FULL_SCREEN_ICON));
  }
#endif
}

void
update_main_menu_replay_item (void)
{
  if (command_line_replay) {
    int flags = al_get_menu_item_flags (main_menu, REPLAY_MID);
    al_set_menu_item_flags (main_menu, REPLAY_MID,
                            flags | ALLEGRO_MENU_ITEM_DISABLED);
  } else {
    /* play replay */
    al_set_menu_item_icon
      (main_menu, PLAY_REPLAY_MID, load_menu_icon (PLAY_ICON));

    /* record replay */
    al_set_menu_item_icon
      (main_menu, RECORD_REPLAY_MID, load_menu_icon (RECORD_ICON));
  }
}

void
select_main_menu (void)
{
  current_menu = main_menu;
}





void
toggle_menu_visibility (void)
{
  if (is_showing_menu ()) hide_menu ();
  else show_menu ();
}

bool
is_showing_menu (void)
{
#if MENU_FEATURE
  return al_get_display_menu (display) ? true : false;
#else
  return false;
#endif
}

void
show_menu (void)
{
#if MENU_FEATURE
  if (display_mode < 0) {
    al_set_display_menu (display, current_menu);
    show_mouse_cursor ();
  }
#endif
}

void
hide_menu (void)
{
#if MENU_FEATURE
  if (is_fullscreen ()) {
    al_remove_display_menu (display);
    hide_mouse_cursor ();
  }
#endif
}

#if MENU_FEATURE
void
enable_menu (bool enable)
{
  int i = 0;
  while (al_get_menu_item_flags (current_menu, i--) != -1) {
    int flags;
    if (enable)
      flags &= ~ALLEGRO_MENU_ITEM_DISABLED;
    else flags |= ALLEGRO_MENU_ITEM_DISABLED;
    al_set_menu_item_flags (current_menu, i + 1, flags);
  }
}
#endif






void
ui_load_game (void)
{
  load_config_dialog_thread =
    create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_load_config (void)
{
  load_config_dialog_thread =
    create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_screenshot (void)
{
  save_picture_dialog_thread =
    create_thread (dialog_thread, &save_picture_dialog);
  al_start_thread (save_picture_dialog_thread);
  pause_animation (true);
}

void
ui_restart_game (void)
{
  quit_anim = RESTART_GAME;
}

void
ui_quit_game (void)
{
  quit_anim = QUIT_GAME;
}

void
ui_full_screen (void)
{
  char *text;
  if (display_mode < 0) {
    force_full_redraw = true;
    char *boolean;
    if (is_fullscreen ()) {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, false);
      boolean = "OFF";
      show_mouse_cursor ();
      show_menu ();
    } else {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, true);
      boolean = "ON";
      hide_mouse_cursor ();
      hide_menu ();
    }
    update_main_menu_full_screen_item ();
    xasprintf (&text, "FULL SCREEN MODE %s", boolean);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  } else draw_bottom_text (NULL, "NON-DESKTOP MODE IS FULL SCREEN ONLY", 0);
}

void
ui_play_replay (void)
{
  if (! title_demo && replay_mode == PLAY_REPLAY) {
    print_replay_chain_aborted ();
    stop_replaying (2);
  } else create_load_replay_thread ();
}

void
ui_record_replay (void)
{
  if (replay_mode == RECORD_REPLAY) create_save_replay_thread ();
  else if (recording_replay_countdown < 0)
    prepare_for_recording_replay ();
  else {
    draw_bottom_text (NULL, "REPLAY RECORDING ABORTED", 2);
    recording_replay_countdown = -1;
  }
}
