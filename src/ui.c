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
  *screen_icon, *right_icon;




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

  menu_item ("Fli&p screen (Shift+I)", FLIP_SCREEN_MID, 0,
             micon_flags (screen_icon, ALLEGRO_FLIP_VERTICAL), true);

  menu_item ("&Screenshot... (Ctrl+P)", SCREENSHOT_MID, 0,
             micon (camera_icon), false);

  end_menu ();

  screen_flip_menu ();
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
      if (edit == EDIT_NONE) {
        hide_mouse_cursor ();
        hide_menu ();
      }
    }
    view_menu ();
    xasprintf (&text, "FULL SCREEN MODE %s", boolean);
    draw_bottom_text (NULL, text, 0);
    al_free (text);
  } else draw_bottom_text (NULL, "NON-DESKTOP MODE IS FULL SCREEN ONLY", 0);
}

void
ui_flip_screen (int flags, bool correct_mouse)
{
  char *text, *flip;

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

  potion_flags = 0;
  screen_flags = flags;
  switch (screen_flags) {
  case 0: flip = "NONE"; break;
  case ALLEGRO_FLIP_VERTICAL: flip = "VERTICAL"; break;
  case ALLEGRO_FLIP_HORIZONTAL: flip = "HORIZONTAL"; break;
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    flip = "VERTICAL + HORIZONTAL"; break;
  default: assert (false); break;
  }
  xasprintf (&text, "SCREEN FLIP: %s", flip);
  draw_bottom_text (NULL, text, 1);
  al_free (text);
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
