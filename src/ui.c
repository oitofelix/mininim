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
static ALLEGRO_MENU *append_menu;
static int append_menu_index;

static void start_menu (ALLEGRO_MENU *parent, uint16_t id);
static bool menu_item (char const *title, uint16_t id, int flags,
                       ALLEGRO_BITMAP *icon, bool has_submenu);
static void menu_separator (void);
static void end_menu (void);
#endif

#if MENU_FEATURE
ALLEGRO_MENU *main_menu;

static void menu_widget_pause (void);
static void menu_widget_speed (void);
#endif

ALLEGRO_BITMAP *small_logo_icon,
  *open_icon, *save_icon, *reload_icon, *quit_icon,
  *full_screen_icon, *windows_icon, *camera_icon, *play_icon,
  *record_icon, *stop_icon, *eject_icon, *backward_icon,
  *forward_icon, *pause_icon, *previous_icon, *next_icon,
  *screen_icon, *right_icon, *card_icon, *dungeon_icon,
  *palace_icon;




ALLEGRO_BITMAP *
load_icon (char *filename)
{
  if (WINDOWS_PORT) return load_scaled_memory_bitmap (filename, 13, 13, 0);
  else return load_scaled_memory_bitmap (filename, 16, 16, 0);
}

ALLEGRO_BITMAP *
micon (ALLEGRO_BITMAP *bitmap)
{
  return clone_memory_bitmap (bitmap);
}

ALLEGRO_BITMAP *
micon_flags (ALLEGRO_BITMAP *bitmap, int flags)
{
  return clone_scaled_memory_bitmap
    (bitmap, al_get_bitmap_width (bitmap),
     al_get_bitmap_height (bitmap), flags);
}

void
load_icons (void)
{
  small_logo_icon = load_icon (SMALL_LOGO_ICON);
  open_icon = load_icon (OPEN_ICON);
  save_icon = load_icon (SAVE_ICON);
  reload_icon = load_icon (RELOAD_ICON);
  quit_icon = load_icon (QUIT_ICON);
  full_screen_icon = load_icon (FULL_SCREEN_ICON);
  windows_icon = load_icon (WINDOWS_ICON);
  camera_icon = load_icon (CAMERA_ICON);
  play_icon = load_icon (PLAY_ICON);
  record_icon = load_icon (RECORD_ICON);
  stop_icon = load_icon (STOP_ICON);
  eject_icon = load_icon (EJECT_ICON);
  backward_icon = load_icon (BACKWARD_ICON);
  forward_icon = load_icon (FORWARD_ICON);
  pause_icon = load_icon (PAUSE_ICON);
  previous_icon = load_icon (PREVIOUS_ICON);
  next_icon = load_icon (NEXT_ICON);
  screen_icon = load_icon (SCREEN_ICON);
  right_icon = load_icon (RIGHT_ICON);
  card_icon = load_icon (CARD_ICON);
  dungeon_icon = load_icon (DUNGEON_ICON);
  palace_icon = load_icon (PALACE_ICON);
}

void
unload_icons (void)
{
  destroy_bitmap (small_logo_icon);
  destroy_bitmap (open_icon);
  destroy_bitmap (save_icon);
  destroy_bitmap (reload_icon);
  destroy_bitmap (quit_icon);
  destroy_bitmap (full_screen_icon);
  destroy_bitmap (windows_icon);
  destroy_bitmap (camera_icon);
  destroy_bitmap (play_icon);
  destroy_bitmap (record_icon);
  destroy_bitmap (stop_icon);
  destroy_bitmap (eject_icon);
  destroy_bitmap (backward_icon);
  destroy_bitmap (forward_icon);
  destroy_bitmap (pause_icon);
  destroy_bitmap (previous_icon);
  destroy_bitmap (next_icon);
  destroy_bitmap (screen_icon);
  destroy_bitmap (right_icon);
  destroy_bitmap (card_icon);
  destroy_bitmap (dungeon_icon);
  destroy_bitmap (palace_icon);
}





#if MENU_FEATURE
void
start_menu (ALLEGRO_MENU *parent, uint16_t id)
{
  if (id > 0) append_menu = al_find_menu (parent, id);
  else append_menu = parent;
  assert (append_menu);
  append_menu_index = 0;
}
#endif

#if MENU_FEATURE
bool
menu_item (char const *title, uint16_t id, int flags,
           ALLEGRO_BITMAP *icon, bool has_submenu)
{
  ALLEGRO_MENU *submenu = al_find_menu (append_menu, id);;

  if (has_submenu && ! submenu) {
    submenu = al_create_menu ();
    goto replace;
  } else if (! has_submenu && submenu) {
    submenu = NULL;
    goto replace;
  }

  ALLEGRO_MENU *f_menu;
  int f_index;

  if (al_find_menu_item (append_menu, id, &f_menu, &f_index)) {
    if (f_menu != append_menu || f_index != append_menu_index)
      goto replace;

    al_set_menu_item_caption (append_menu, -append_menu_index, title);
    al_set_menu_item_flags (append_menu, -append_menu_index, flags);
    al_set_menu_item_icon (append_menu, -append_menu_index, icon);

    append_menu_index++;

    return true;
  }

 replace:
  al_remove_menu_item (append_menu, -append_menu_index);
  return al_insert_menu_item (append_menu, -(append_menu_index++), title,
                              id, flags, icon, submenu);
}
#endif

#if MENU_FEATURE
void
menu_separator (void)
{
  if (WINDOWS_PORT)
    menu_item (NULL, NO_MID, 0, NULL, false);
  else
    menu_item ("", NO_MID, ALLEGRO_MENU_ITEM_DISABLED, NULL, false);
}
#endif

#if MENU_FEATURE
void
end_menu (void)
{
  while (al_remove_menu_item (append_menu, -append_menu_index));
}
#endif





void
show_menu (void)
{
#if MENU_FEATURE
  if (display_mode < 0) {
    al_set_display_menu (display, main_menu);
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
enable_menu (bool enable)
{
#if MENU_FEATURE
  int i = 0;
  int flags;
  while ((flags = al_get_menu_item_flags (main_menu, i--)) != -1) {
    if (enable) flags &= ~ALLEGRO_MENU_ITEM_DISABLED;
    else flags |= ALLEGRO_MENU_ITEM_DISABLED;
    al_set_menu_item_flags (main_menu, i + 1, flags);
  }
#endif
}





void
create_main_menu (void)
{
#if MENU_FEATURE
  if (! main_menu) main_menu = al_create_menu ();
  start_menu (main_menu, 0);
  menu_item ("&Game", GAME_MID, 0, NULL, true);
  menu_item ("&View", VIEW_MID, 0, NULL, true);
  menu_item ("&Replay", REPLAY_MID, 0, NULL, true);
  menu_item ("&Help", HELP_MID, 0, NULL, true);
  end_menu ();

  game_menu ();
  view_menu ();
  replay_menu ();
  help_menu ();
#endif
}

void
game_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAME_MID);
  menu_item ("&Load", LOAD_MID, 0, micon (open_icon), true);
  menu_item ("&Save", SAVE_MID, 0, micon (save_icon), true);
  menu_separator ();
  menu_item (cutscene ? "Sta&rt (Enter)" : "&Restart (Ctrl+R)",
             cutscene ? START_GAME_MID : RESTART_GAME_MID, 0,
             cutscene ? micon (right_icon) : micon (reload_icon), false);
  menu_item ("&Quit (Ctrl+Q)", QUIT_GAME_MID, 0,
             micon (quit_icon), false);
  end_menu ();

  load_menu ();
  save_menu ();
#endif
}

void
load_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, LOAD_MID);
  menu_item ("&Game... (Ctrl+L)", LOAD_GAME_MID, 0, NULL, false);
  menu_item ("&Configuration... (Ctrl+L)", LOAD_CONFIG_MID,
             0, NULL, false);
  menu_item ("&Level file...", LOAD_LEVEL_FILE_MID,
             ALLEGRO_MENU_ITEM_DISABLED, NULL, false);
  end_menu ();
#endif
}

void
save_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, SAVE_MID);
  menu_item ("&Game... (Ctrl+G)", SAVE_GAME_MID, 0, NULL, false);
  menu_item ("&Configuration...", SAVE_CONFIG_MID,
             ALLEGRO_MENU_ITEM_DISABLED, NULL, false);
  menu_item ("&Level file...", SAVE_LEVEL_FILE_MID,
             ALLEGRO_MENU_ITEM_DISABLED, NULL, false);
  end_menu ();
#endif
}

void
view_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VIEW_MID);

  menu_item (is_fullscreen () ? "&Windowed (F)" : "&Full screen (F)",
             FULL_SCREEN_MID, 0,
             is_fullscreen ()
             ? micon (windows_icon)
             : micon (full_screen_icon), false);

  menu_item ("&Environment mode (F11)", ENVIRONMENT_MODE_MID, 0,
             micon (em == DUNGEON ? dungeon_icon : palace_icon), true);

  menu_item ("&Video mode (F12)", VIDEO_MODE_MID, 0,
             micon (card_icon), true);

  menu_item ("Fli&p screen (Shift+I)", FLIP_SCREEN_MID, 0,
             micon_flags (screen_icon, screen_flags), true);

  menu_item ("&Screenshot... (Ctrl+P)", SCREENSHOT_MID, 0,
             micon (camera_icon), false);

  end_menu ();

  environment_mode_menu ();
  video_mode_menu ();
  screen_flip_menu ();
#endif
}

void
environment_mode_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, ENVIRONMENT_MODE_MID);

  menu_item ("&ORIGINAL", ORIGINAL_ENV_MID,
             force_em ? ALLEGRO_MENU_ITEM_CHECKBOX
             : ALLEGRO_MENU_ITEM_CHECKED,
             NULL, false);

  menu_item ("&DUNGEON", DUNGEON_MID,
             (force_em && em == DUNGEON) ? ALLEGRO_MENU_ITEM_CHECKED
             : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  menu_item ("&PALACE", PALACE_MID,
             (force_em && em == PALACE) ? ALLEGRO_MENU_ITEM_CHECKED
             : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  end_menu ();
#endif
}

void
video_mode_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VIDEO_MODE_MID);

  menu_item ("&VGA", VGA_MID,
             vm == VGA ? ALLEGRO_MENU_ITEM_CHECKED
             : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  menu_item ("&EGA", EGA_MID,
             vm == EGA ? ALLEGRO_MENU_ITEM_CHECKED
             : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  menu_item ("&CGA", CGA_MID,
             (vm == CGA && ! hgc) ? ALLEGRO_MENU_ITEM_CHECKED
             : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  menu_item ("&HGC", HGC_MID,
             (vm == CGA && hgc) ? ALLEGRO_MENU_ITEM_CHECKED
             : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  end_menu ();
#endif
}

void
screen_flip_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, FLIP_SCREEN_MID);

  menu_item ("&Vertical", FLIP_SCREEN_VERTICAL_MID,
             screen_flags & ALLEGRO_FLIP_VERTICAL
             ? ALLEGRO_MENU_ITEM_CHECKED : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  menu_item ("&Horizontal", FLIP_SCREEN_HORIZONTAL_MID,
             screen_flags & ALLEGRO_FLIP_HORIZONTAL
             ? ALLEGRO_MENU_ITEM_CHECKED : ALLEGRO_MENU_ITEM_CHECKBOX,
             NULL, false);

  end_menu ();
#endif
}

void
replay_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, REPLAY_MID);

  char *text;

  if (recording_replay_countdown >= 0
      || level_start_replay_mode == RECORD_REPLAY)
    goto record_replay;

  if (title_demo) goto no_replay;

  switch (replay_mode) {
  case PLAY_REPLAY:
    xasprintf (&text, "REPLAYING (%i/%i)", replay_index + 1,
               replay_chain_nmemb);
    menu_item
      (text, NO_MID, ALLEGRO_MENU_ITEM_DISABLED,
       micon (play_icon), false);
    al_free (text);

    menu_separator ();

    menu_item
      ("&Stop (F7)", PLAY_REPLAY_MID, 0, micon (stop_icon), false);

    menu_item
      ("Pre&vious (Shift+M)", PREVIOUS_REPLAY_MID,
       replay_index > 0
       ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
       micon (previous_icon), false);

    menu_item
      ("&Next (Shift+L)", NEXT_REPLAY_MID,
       replay_index + 1 < replay_chain_nmemb
       ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
       micon (next_icon), false);

    menu_widget_pause ();

    menu_widget_speed ();

    break;
  case RECORD_REPLAY: record_replay:

    menu_item ("RECORDING", NO_MID,
               ALLEGRO_MENU_ITEM_DISABLED,
               micon (record_icon), false);

    menu_separator ();

    menu_item
      (recording_replay_countdown >= 0
       ? "&Abort recording (F7)" : "&Stop... (F7)",
       RECORD_REPLAY_MID, 0,
       replay_mode == RECORD_REPLAY
       ? micon (stop_icon) : micon (eject_icon), false);

    menu_widget_pause ();

    break;
  default: assert (false);
  case NO_REPLAY: no_replay:

    menu_item
      ("&Play... (F7)", PLAY_REPLAY_MID, 0,
       micon (play_icon), false);

    menu_item
      ("&Record... (Alt+F7)", RECORD_REPLAY_MID, 0,
       micon (record_icon), false);

    menu_widget_pause ();

    menu_widget_speed ();

    break;
  }

  end_menu ();
#endif
}

void
help_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, HELP_MID);
  menu_item ("&About", ABOUT_MID, 0, micon (small_logo_icon), false);
  end_menu ();
#endif
}

#if MENU_FEATURE
void
menu_widget_pause (void)
{
  menu_separator ();

  menu_item
    (is_game_paused ()
     ? "&Continue" : "&Pause (Esc)", TOGGLE_PAUSE_GAME_MID,
     (cutscene || title_demo || recording_replay_countdown >= 0)
     ? ALLEGRO_MENU_ITEM_DISABLED : 0,
     is_game_paused ()
     ? micon (play_icon)
     : micon (pause_icon), false);

  menu_item
    ("Next &frame (Esc)", NEXT_FRAME_MID,
     (! is_game_paused () || cutscene || title_demo
      || recording_replay_countdown >= 0)
     ? ALLEGRO_MENU_ITEM_DISABLED : 0,
     micon (forward_icon), false);
}
#endif

#if MENU_FEATURE
void
menu_widget_speed (void)
{
  menu_separator ();

  int flags = anim_freq > 0
    ? ALLEGRO_MENU_ITEM_CHECKED
    : ALLEGRO_MENU_ITEM_CHECKBOX;
  flags |= (cutscene || title_demo) ? ALLEGRO_MENU_ITEM_DISABLED : 0;

  menu_item
    ("Speed cons&traint", TOGGLE_TIME_FREQUENCY_CONSTRAINT_MID,
     flags, NULL, false);

  menu_item
    ("&Decrease speed [(]", DECREASE_TIME_FREQUENCY_MID,
     (anim_freq <= 2 || cutscene || title_demo)
     ? ALLEGRO_MENU_ITEM_DISABLED : 0,
     micon (backward_icon), false);

  menu_item
    ("&Increase speed [)]", INCREASE_TIME_FREQUENCY_MID,
     (anim_freq == 0 || cutscene || title_demo)
     ? ALLEGRO_MENU_ITEM_DISABLED: 0,
     micon (forward_icon), false);
}
#endif






void
menu_mid (intptr_t mid)
{
  /********/
  /* MENU */
  /********/

  switch (mid) {

    /* GAME */
  case LOAD_GAME_MID:
    ui_load_game ();
    break;
  case LOAD_CONFIG_MID:
    ui_load_config ();
    break;
  case RESTART_GAME_MID:
    ui_restart_game ();
    break;
  case START_GAME_MID:
    ui_start_game ();
    break;
  case QUIT_GAME_MID:
    ui_quit_game ();
    break;


    /* REPLAY */
  case PLAY_REPLAY_MID:
    ui_play_replay ();
    break;
  case PREVIOUS_REPLAY_MID:
    ui_previous_replay ();
    break;
  case NEXT_REPLAY_MID:
    ui_next_replay ();
    break;
  case RECORD_REPLAY_MID:
    ui_record_replay ();
    break;
  case TOGGLE_PAUSE_GAME_MID:
    ui_toggle_pause_game ();
    break;
  case NEXT_FRAME_MID:
    ui_next_frame ();
    break;
  case DECREASE_TIME_FREQUENCY_MID:
    ui_decrease_time_frequency ();
    break;
  case INCREASE_TIME_FREQUENCY_MID:
    ui_increase_time_frequency ();
    break;
  case TOGGLE_TIME_FREQUENCY_CONSTRAINT_MID:
    ui_toggle_time_frequency_constraint ();
    break;


    /* VIEW */
  case FULL_SCREEN_MID:
    ui_full_screen ();
    break;
  case ORIGINAL_ENV_MID:
    ui_environment_mode (ORIGINAL_ENV);
    break;
  case DUNGEON_MID:
    ui_environment_mode (DUNGEON);
    break;
  case PALACE_MID:
    ui_environment_mode (PALACE);
    break;
  case VGA_MID:
    ui_video_mode (VGA);
    break;
  case EGA_MID:
    ui_video_mode (EGA);
    break;
  case CGA_MID:
    ui_video_mode (CGA);
    break;
  case HGC_MID:
    ui_video_mode (HGC);
    break;
  case FLIP_SCREEN_VERTICAL_MID:
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_VERTICAL, false);
    break;
  case FLIP_SCREEN_HORIZONTAL_MID:
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_HORIZONTAL, false);
    break;
  case SCREENSHOT_MID:
    ui_screenshot ();
    break;

    /* HELP */
  case ABOUT_MID:
    ui_version ();
    break;
  default: break;
  }
}






void
anim_key_bindings (void)
{
  /****************/
  /* KEY BINDINGS */
  /****************/


  /* CTRL+L: load game */
  if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_L)
      && ! load_config_dialog_thread)
    ui_load_game ();

  /* CTRL+P: screenshot */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_P)
           && ! save_picture_dialog_thread)
    ui_screenshot ();

  /* ALT+F7: start/stop replay recording */
  else if (! command_line_replay
           && (((title_demo || replay_mode != PLAY_REPLAY)
                && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7))
               || ((replay_mode == RECORD_REPLAY
                    || recording_replay_countdown > 0)
                   && was_key_pressed (0, ALLEGRO_KEY_F7))))
    ui_record_replay ();

  /* F7: load replay/stop replaying */
  else if (! command_line_replay
           && ((replay_mode != RECORD_REPLAY
                && was_key_pressed (0, ALLEGRO_KEY_F7))
               || (replay_mode == PLAY_REPLAY
                   && was_key_pressed
                   (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F7))))
    ui_play_replay ();

  /* CTRL+R: restart game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_R))
    ui_restart_game ();

  /* CTRL+Q: quit game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Q))
    ui_quit_game ();

  /* (: decrease time frequency */
  else if (was_char_pressed ('('))
    ui_change_anim_freq (anim_freq - 1);

  /* ): increase time frenquency */
  else if (was_char_pressed (')'))
    ui_change_anim_freq (anim_freq + 1);

  /* F8: enable/disable level editor */
  else if (was_key_pressed (0, ALLEGRO_KEY_F8))
    ui_editor ();

  /* CTRL+V: show engine name and version */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_V))
    ui_version ();

  /* CTRL+S: enable/disable sound */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_S)) {
    char *text;
    audio_enabled = ! audio_enabled;
    enable_audio (audio_enabled);
    xasprintf (&text, "SOUND %s", audio_enabled ? "ON" : "OFF");
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* D: change display mode */
  else if (! active_menu && was_key_pressed (0, ALLEGRO_KEY_D)) {
    if (display_mode < 0)
      draw_bottom_text (NULL, "DISPLAY MODE: DESKTOP", 0);
    else {
      int n = al_get_num_display_modes ();
      if (n) {
        int display_mode_bkp = display_mode;
      next_display_mode:
        display_mode = (display_mode + 1) % n;
        int w = al_get_display_width (display);
        int h = al_get_display_height (display);
        ALLEGRO_DISPLAY_MODE d;
        if (al_get_display_mode (display_mode, &d)) {
          if (d.width == w && d.height == h
              && display_mode != display_mode_bkp)
            goto next_display_mode;
          char *text;
          al_resize_display (display, d.width, d.height);
          xasprintf (&text, "DISPLAY MODE: %ix%i", d.width, d.height);
          draw_bottom_text (NULL, text, 0);
          al_free (text);
        } else draw_bottom_text (NULL, "DISPLAY MODES QUERYING FAILED", 0);
      } else draw_bottom_text (NULL, "NO DISPLAY MODE AVAILABLE", 0);
    }
  }

  /* F: enable/disable fullscreen mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_F))
           || was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_ENTER))
    ui_full_screen ();

  /* SHIFT+I: flip screen */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_I)) {
    switch (screen_flags) {
    case 0: ui_flip_screen (ALLEGRO_FLIP_VERTICAL, true); break;
    case ALLEGRO_FLIP_VERTICAL:
      ui_flip_screen (ALLEGRO_FLIP_HORIZONTAL, true); break;
    case ALLEGRO_FLIP_HORIZONTAL:
      ui_flip_screen (ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL,
                      true);
      break;
    case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    default: ui_flip_screen (0, true); break;
    }
  }

  /* SHIFT+K: flip gamepad */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_K)) {
    char *flip = "NONE";
    if (! flip_gamepad_vertical && ! flip_gamepad_horizontal) {
      flip_gamepad_vertical = true;
      flip = "VERTICAL";
    } else if (flip_gamepad_vertical && ! flip_gamepad_horizontal) {
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = true;
      flip = "HORIZONTAL";
    } else if (! flip_gamepad_vertical && flip_gamepad_horizontal) {
      flip_gamepad_vertical = true;
      flip = "VERTICAL + HORIZONTAL";
    } else if (flip_gamepad_vertical && flip_gamepad_horizontal) {
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = false;
    }
    char *text;
    xasprintf (&text, "GAMEPAD FLIP: %s", flip);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* CTRL+J: joystick mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_J)) {
    char *text;
    char *joystick_str = joystick ? "CALIBRATED" : "MODE";
    calibrate_joystick ();
    gamepad_rumble (1.0, 0.6);
    xasprintf (&text, "JOYSTICK %s", joystick
               ? joystick_str : "NOT FOUND");
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* CTRL+K: keyboard mode (disables joystick) */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_K)) {
    disable_joystick ();
    draw_bottom_text (NULL, "KEYBOARD MODE", 0);
  }

  /* F9: change hue palette */
  else if (was_key_pressed (0, ALLEGRO_KEY_F9)) {
    char *em_str = NULL;

    if (force_hue) {
      switch (hue) {
      case HUE_NONE: hue = HUE_GREEN; em_str = "GREEN"; break;
      case HUE_GREEN: hue = HUE_GRAY; em_str = "GRAY"; break;
      case HUE_GRAY: hue = HUE_YELLOW; em_str = "YELLOW"; break;
      case HUE_YELLOW: hue = HUE_BLUE; em_str = "BLUE"; break;
      case HUE_BLUE: hue = global_level.hue;
        em_str = "ORIGINAL"; force_hue = false; break;
      }
    } else {
      hue = HUE_NONE; em_str = "NONE"; force_hue = true;
    }

    char *text;
    xasprintf (&text, "HUE MODE: %s", em_str);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }

  /* F11: change environment mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F11)) {
    if (force_em) {
      switch (em) {
      case DUNGEON: ui_environment_mode (PALACE); break;
      case PALACE: ui_environment_mode (ORIGINAL_ENV); break;
      }
    } else ui_environment_mode (DUNGEON);
  }

  /* F12: change video mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F12))
    switch (vm) {
    case VGA: ui_video_mode (EGA); break;
    case EGA: ui_video_mode (CGA); break;
    case CGA: ui_video_mode (hgc ? VGA : HGC); break;
    }
}






void
ui_editor (void)
{
  if (edit == EDIT_NONE) {
    if (cutscene) toggle_menu_visibility ();
    else {
      if (replay_mode == NO_REPLAY ) {
        enter_editor ();
        show_menu ();
      } else  {
        toggle_menu_visibility ();
        print_replay_mode (0);
      }
    }
  } else {
    exit_editor (0);
    hide_menu ();
  }
}

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
ui_start_game (void)
{
  quit_anim = CUTSCENE_KEY_PRESS;
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
      if (edit == EDIT_NONE) hide_menu ();
    }
    view_menu ();
    xasprintf (&text, "FULL SCREEN MODE %s", boolean);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
    ui_save_setting ("FULLSCREEN", boolean);
  } else draw_bottom_text (NULL, "NON-DESKTOP MODE IS FULL SCREEN ONLY", 0);
}

void
ui_environment_mode (enum em new_em)
{
  char *em_str = NULL;

  switch (new_em) {
  default: /* ORIGINAL_ENV */
    force_em = false;
    em = global_level.em;
    em_str = "ORIGINAL";
    break;
  case DUNGEON:
    force_em = true;
    em = DUNGEON;
    em_str = "DUNGEON";
    break;
  case PALACE:
    force_em = true;
    em = PALACE;
    em_str = "PALACE";
    break;
  }

  char *text;
  xasprintf (&text, "ENVIRONMENT MODE: %s", em_str);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting ("ENVIRONMENT MODE", em_str);
  view_menu ();
}

void
ui_video_mode (enum vm new_vm)
{
  if (new_vm == HGC) {
    vm = CGA; hgc = true;
  } else {
    vm = new_vm; hgc = false;
  }

  char *vm_str;

  switch (vm) {
  case VGA: vm_str = "VGA"; break;
  case EGA: vm_str = "EGA"; break;
  case CGA: vm_str = hgc ? "HGC" : "CGA"; break;
  }

  video_mode_menu ();

  char *text;
  xasprintf (&text, "VIDEO MODE: %s", vm_str);
  draw_bottom_text (NULL, text, 0);
  al_free (text);

  ui_save_setting ("VIDEO MODE", vm_str);
}

void
ui_flip_screen (int flags, bool correct_mouse)
{
  if (correct_mouse) {
    int w = al_get_display_width (display);
    int h = al_get_display_height (display);

    ALLEGRO_MOUSE_STATE m;

    if ((screen_flags & ALLEGRO_FLIP_VERTICAL
         && ! (flags & ALLEGRO_FLIP_VERTICAL))
        || (! (screen_flags & ALLEGRO_FLIP_VERTICAL)
            && flags & ALLEGRO_FLIP_VERTICAL)) {
      al_get_mouse_state (&m);
      al_set_mouse_xy (display, m.x, h - m.y);
    }

    if ((screen_flags & ALLEGRO_FLIP_HORIZONTAL
         && ! (flags & ALLEGRO_FLIP_HORIZONTAL))
        || (! (screen_flags & ALLEGRO_FLIP_HORIZONTAL)
            && flags & ALLEGRO_FLIP_HORIZONTAL)) {
      al_get_mouse_state (&m);
      al_set_mouse_xy (display, w - m.x, m.y);
    }
  }

  char *text, *flip_str;

  potion_flags = 0;
  screen_flags = flags;
  switch (screen_flags) {
  case 0: flip_str = "NONE"; break;
  case ALLEGRO_FLIP_VERTICAL: flip_str = "VERTICAL"; break;
  case ALLEGRO_FLIP_HORIZONTAL: flip_str = "HORIZONTAL"; break;
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    flip_str = "VERTICAL-HORIZONTAL"; break;
  default: assert (false); break;
  }
  xasprintf (&text, "SCREEN FLIP: %s", flip_str);
  draw_bottom_text (NULL, text, 1);
  al_free (text);

  view_menu ();
  ui_save_setting ("DISPLAY FLIP MODE", flip_str);
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
ui_previous_replay (void)
{
  quit_anim = REPLAY_PREVIOUS;
}

void
ui_next_replay (void)
{
  quit_anim = REPLAY_NEXT;
}

void
ui_record_replay (void)
{
  if (replay_mode == RECORD_REPLAY) create_save_replay_thread ();
  else if (recording_replay_countdown < 0)
    prepare_for_recording_replay ();
  else {
    draw_bottom_text (NULL, "RECORDING ABORTED", 2);
    recording_replay_countdown = -1;
    replay_menu ();
  }
}

void
ui_increase_time_frequency (void)
{
  if (anim_freq >= DEFAULT_HZ)
    ui_change_anim_freq (DEFAULT_HZ * ((anim_freq / DEFAULT_HZ) + 1));
  else ui_change_anim_freq (2 * ((anim_freq / 2) + 1));
}

void
ui_decrease_time_frequency (void)
{
  if (anim_freq > DEFAULT_HZ)
    ui_change_anim_freq (DEFAULT_HZ * (anim_freq / DEFAULT_HZ - 1));
  else if (anim_freq > 2)
    ui_change_anim_freq (2 * ((anim_freq / 2) - 1));
}

void
ui_toggle_time_frequency_constraint (void)
{
  static int f = DEFAULT_HZ;

  if (anim_freq > 0) {
    f = anim_freq;
    ui_change_anim_freq (0);
  } else ui_change_anim_freq (f);
}

void
ui_change_anim_freq (int f)
{
  char *text;
  f = f < 0 ? 0 : f;
  f = f > UNLIMITED_HZ ? UNLIMITED_HZ : f;
  anim_freq = f;
  al_set_timer_speed (timer, f > 0 ? 1.0 / f : 1.0 / UNLIMITED_HZ);
  if (f > 0) {
    xasprintf (&text, "TIME FREQ: %iHz", f);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  }
  replay_menu ();
}

void
ui_toggle_pause_game (void)
{
  if (is_game_paused ()) pause_game (false);
  else pause_game (true);
  replay_menu ();
}

void
ui_next_frame (void)
{
  step_cycle = 1;
}

void
print_game_paused (int priority)
{
  char *text;

  switch (replay_mode) {
  case RECORD_REPLAY: text = "RECORDING PAUSED"; break;
  case PLAY_REPLAY: text = "REPLAY PAUSED"; break;
  case NO_REPLAY: default: text = "GAME PAUSED"; break;
  }

  draw_bottom_text (NULL, text, priority);
}

void
ui_version (void)
{
  char *text;
  xasprintf (&text, "MININIM %s", VERSION);
  draw_bottom_text (NULL, text, 0);
  al_free (text);
}





error_t
ui_save_setting (char *key, char *value)
{
  ALLEGRO_CONFIG *config = NULL;

  if (al_filename_exists (config_filename))
    config = al_load_config_file (config_filename);
  else {
    if (! al_make_directory (user_settings_dir))
      return al_get_errno ();
    config = al_create_config ();
  }

  if (! config) return al_get_errno ();

  ALLEGRO_CONFIG_ENTRY *iterator;
  char const *entry = al_get_first_config_entry (config, NULL, &iterator);

  while (entry) {
    if (! strcasecmp (entry, key)) {
      al_remove_config_key (config, NULL, entry);
      entry = al_get_first_config_entry (config, NULL, &iterator);
    } else entry = al_get_next_config_entry (&iterator);
  }

  al_set_config_value (config, NULL, key, value);

  if (! al_save_config_file (config_filename, config)) {
    al_destroy_config (config);
    return al_get_errno ();
  }

  al_destroy_config (config);

  return 0;
}
