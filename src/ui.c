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
static bool menu_hitem (char const *title, uint16_t id, bool enabled);
static bool menu_sitem (char const *title, uint16_t id, bool enabled,
                        ALLEGRO_BITMAP *icon);
static bool menu_ditem (bool first, char const *title0, char const *title1,
                        uint16_t id0, uint16_t id1, bool enabled,
                        ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1);
static bool menu_sub (char const *title, uint16_t id, bool enabled,
                      ALLEGRO_BITMAP *icon);
static bool menu_citem (char const *title, uint16_t id, bool enabled,
                        bool checked, ALLEGRO_BITMAP *icon);
static void menu_sep (void);
static void end_menu (void);
#endif

#if MENU_FEATURE
ALLEGRO_MENU *main_menu;
#endif

ALLEGRO_BITMAP *small_logo_icon,
  *open_icon, *save_icon, *reload_icon, *quit_icon,
  *full_screen_icon, *windows_icon, *camera_icon, *play_icon,
  *record_icon, *stop_icon, *eject_icon, *backward_icon,
  *forward_icon, *pause_icon, *previous_icon, *next_icon,
  *screen_icon, *right_icon, *dungeon_icon,
  *palace_icon, *green_icon, *gray_icon, *yellow_icon,
  *blue_icon, *black_icon, *vga_icon, *ega_icon, *cga_icon,
  *hgc_icon, *vertical_icon, *horizontal_icon,
  *keyboard_icon, *joystick_icon, *cancel_icon, *clock_icon,
  *edit_icon, *joystick2_icon, *undo_icon, *redo_icon,
  *screensaver_icon, *joystick3_icon, *volume_off_icon,
  *volume_low_icon, *volume_medium_icon, *volume_high_icon,
  *game_icon, *settings_icon, *zoom_none_icon, *zoom_stretch_icon,
  *zoom_ratio_icon, *vertical_horizontal_icon, *zoom_out_icon,
  *zoom_in_icon, *heading_icon, *zoom_icon,
  *navigation_icon, *nav_select_icon, *nav_cell_icon, *nav_page_icon,
  *nav_left_icon, *nav_right_icon, *nav_above_icon, *nav_below_icon,
  *nav_home_icon, *nav_center_icon, *compass_icon, *compass2_icon,
  *drawing_icon, *first_icon, *last_icon, *jump_icon, *original_icon,
  *guard_icon, *fat_guard_icon, *vizier_icon, *skeleton_icon, *shadow_icon,
  *resurrect_icon, *death_icon, *feather_icon, *angel_icon, *life_empty_icon,
  *life_full_icon;




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
  if (! bitmap) return NULL;
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
  dungeon_icon = load_icon (DUNGEON_ICON);
  palace_icon = load_icon (PALACE_ICON);
  green_icon = load_icon (GREEN_ICON);
  gray_icon = load_icon (GRAY_ICON);
  yellow_icon = load_icon (YELLOW_ICON);
  blue_icon = load_icon (BLUE_ICON);
  black_icon = load_icon (BLACK_ICON);
  vga_icon = load_icon (VGA_ICON);
  ega_icon = load_icon (EGA_ICON);
  cga_icon = load_icon (CGA_ICON);
  hgc_icon = load_icon (HGC_ICON);
  vertical_icon = load_icon (VERTICAL_ICON);
  horizontal_icon = load_icon (HORIZONTAL_ICON);
  keyboard_icon = load_icon (KEYBOARD_ICON);
  joystick_icon = load_icon (JOYSTICK_ICON);
  cancel_icon = load_icon (CANCEL_ICON);
  clock_icon = load_icon (CLOCK_ICON);
  edit_icon = load_icon (EDIT_ICON);
  joystick2_icon = load_icon (JOYSTICK2_ICON);
  undo_icon = load_icon (UNDO_ICON);
  redo_icon = load_icon (REDO_ICON);
  screensaver_icon = load_icon (SCREENSAVER_ICON);
  joystick3_icon = load_icon (JOYSTICK3_ICON);
  volume_off_icon = load_icon (VOLUME_OFF_ICON);
  volume_low_icon = load_icon (VOLUME_LOW_ICON);
  volume_medium_icon = load_icon (VOLUME_MEDIUM_ICON);
  volume_high_icon = load_icon (VOLUME_HIGH_ICON);
  game_icon = load_icon (GAME_ICON);
  settings_icon = load_icon (SETTINGS_ICON);
  zoom_none_icon = load_icon (ZOOM_NONE_ICON);
  zoom_stretch_icon = load_icon (ZOOM_STRETCH_ICON);
  zoom_ratio_icon = load_icon (ZOOM_RATIO_ICON);
  vertical_horizontal_icon = load_icon (VERTICAL_HORIZONTAL_ICON);
  zoom_out_icon = load_icon (ZOOM_OUT_ICON);
  zoom_in_icon = load_icon (ZOOM_IN_ICON);
  heading_icon = load_icon (HEADING_ICON);
  zoom_icon = load_icon (ZOOM_ICON);
  navigation_icon = load_icon (NAVIGATION_ICON);
  nav_select_icon = load_icon (NAV_SELECT_ICON);
  nav_cell_icon = load_icon (NAV_CELL_ICON);
  nav_page_icon = load_icon (NAV_PAGE_ICON);
  nav_left_icon = load_icon (NAV_LEFT_ICON);
  nav_right_icon = load_icon (NAV_RIGHT_ICON);
  nav_above_icon = load_icon (NAV_ABOVE_ICON);
  nav_below_icon = load_icon (NAV_BELOW_ICON);
  nav_home_icon = load_icon (NAV_HOME_ICON);
  nav_center_icon = load_icon (NAV_CENTER_ICON);
  compass_icon = load_icon (COMPASS_ICON);
  compass2_icon = load_icon (COMPASS2_ICON);
  drawing_icon = load_icon (DRAWING_ICON);
  first_icon = load_icon (FIRST_ICON);
  last_icon = load_icon (LAST_ICON);
  jump_icon = load_icon (JUMP_ICON);
  original_icon = load_icon (ORIGINAL_ICON);
  guard_icon = load_icon (GUARD_ICON);
  fat_guard_icon = load_icon (FAT_GUARD_ICON);
  vizier_icon = load_icon (VIZIER_ICON);
  skeleton_icon = load_icon (SKELETON_ICON);
  shadow_icon = load_icon (SHADOW_ICON);
  resurrect_icon = load_icon (RESURRECT_ICON);
  death_icon = load_icon (DEATH_ICON);
  feather_icon = load_icon (FEATHER_ICON);
  angel_icon = load_icon (ANGEL_ICON);
  life_empty_icon = load_icon (LIFE_EMPTY_ICON);
  life_full_icon = load_icon (LIFE_FULL_ICON);
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
  destroy_bitmap (dungeon_icon);
  destroy_bitmap (palace_icon);
  destroy_bitmap (green_icon);
  destroy_bitmap (gray_icon);
  destroy_bitmap (yellow_icon);
  destroy_bitmap (blue_icon);
  destroy_bitmap (black_icon);
  destroy_bitmap (vga_icon);
  destroy_bitmap (ega_icon);
  destroy_bitmap (cga_icon);
  destroy_bitmap (hgc_icon);
  destroy_bitmap (vertical_icon);
  destroy_bitmap (horizontal_icon);
  destroy_bitmap (keyboard_icon);
  destroy_bitmap (joystick_icon);
  destroy_bitmap (cancel_icon);
  destroy_bitmap (clock_icon);
  destroy_bitmap (edit_icon);
  destroy_bitmap (joystick2_icon);
  destroy_bitmap (undo_icon);
  destroy_bitmap (redo_icon);
  destroy_bitmap (screensaver_icon);
  destroy_bitmap (joystick3_icon);
  destroy_bitmap (volume_off_icon);
  destroy_bitmap (volume_low_icon);
  destroy_bitmap (volume_medium_icon);
  destroy_bitmap (volume_high_icon);
  destroy_bitmap (game_icon);
  destroy_bitmap (settings_icon);
  destroy_bitmap (zoom_none_icon);
  destroy_bitmap (zoom_stretch_icon);
  destroy_bitmap (zoom_ratio_icon);
  destroy_bitmap (vertical_horizontal_icon);
  destroy_bitmap (zoom_out_icon);
  destroy_bitmap (zoom_in_icon);
  destroy_bitmap (heading_icon);
  destroy_bitmap (zoom_icon);
  destroy_bitmap (navigation_icon);
  destroy_bitmap (nav_select_icon);
  destroy_bitmap (nav_cell_icon);
  destroy_bitmap (nav_page_icon);
  destroy_bitmap (nav_left_icon);
  destroy_bitmap (nav_right_icon);
  destroy_bitmap (nav_above_icon);
  destroy_bitmap (nav_below_icon);
  destroy_bitmap (nav_home_icon);
  destroy_bitmap (nav_center_icon);
  destroy_bitmap (compass_icon);
  destroy_bitmap (compass2_icon);
  destroy_bitmap (drawing_icon);
  destroy_bitmap (first_icon);
  destroy_bitmap (last_icon);
  destroy_bitmap (jump_icon);
  destroy_bitmap (original_icon);
  destroy_bitmap (guard_icon);
  destroy_bitmap (fat_guard_icon);
  destroy_bitmap (vizier_icon);
  destroy_bitmap (skeleton_icon);
  destroy_bitmap (shadow_icon);
  destroy_bitmap (resurrect_icon);
  destroy_bitmap (death_icon);
  destroy_bitmap (feather_icon);
  destroy_bitmap (angel_icon);
  destroy_bitmap (life_empty_icon);
  destroy_bitmap (life_full_icon);
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

    int f_flags = al_get_menu_item_flags (append_menu, -append_menu_index);
    if ((f_flags & ~ALLEGRO_MENU_ITEM_DISABLED)
        != (flags & ~ALLEGRO_MENU_ITEM_DISABLED)) goto replace;

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
bool
menu_hitem (char const *title, uint16_t id, bool enabled)
{
  return menu_item (title, id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    micon (heading_icon), false);
}
#endif

#if MENU_FEATURE
bool
menu_sitem (char const *title, uint16_t id, bool enabled,
            ALLEGRO_BITMAP *icon)
{
  return menu_item (title, id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    icon, false);
}
#endif

#if MENU_FEATURE
bool
menu_ditem (bool first, char const *title0, char const *title1,
            uint16_t id0, uint16_t id1, bool enabled,
            ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1)
{
  return menu_item (first ? title0 : title1,
                    first ? id0 : id1,
                    enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    first ? icon0 : icon1, false);
}
#endif

#if MENU_FEATURE
bool
menu_sub (char const *title, uint16_t id, bool enabled, ALLEGRO_BITMAP *icon)
{
  return menu_item (title, id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                    icon, true);
}
#endif

#if MENU_FEATURE
bool
menu_citem (char const *title, uint16_t id, bool enabled, bool checked,
            ALLEGRO_BITMAP *icon)
{
  int flags = enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED;
  flags |= checked ? ALLEGRO_MENU_ITEM_CHECKED : 0;
  bool r = menu_item (title, id, flags, checked ? NULL : icon, false);
  if (checked) destroy_bitmap (icon);
  return r;
}
#endif

#if MENU_FEATURE
void
menu_sep (void)
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

  menu_sub ("&Game", GAME_MID, true, NULL);

  menu_sub ("&View", VIEW_MID, true, NULL);

  menu_sub ("&Input", GAMEPAD_MID, true, NULL);

  menu_sub ("&Replay", REPLAY_MID, true, NULL);

  menu_sub ("&Editor", EDITOR_MID, can_edit (), NULL);

  menu_sub ("&Cheat", CHEAT_MID, can_edit (), NULL);

  menu_sub ("Hel&p", HELP_MID, true, NULL);

  end_menu ();

  game_menu ();
  view_menu ();
  gamepad_menu ();
  replay_menu ();
  editor_menu ();
  cheat_menu ();
  help_menu ();
#endif
}

ALLEGRO_BITMAP *
volume_icon (float volume)
{
  if (volume == VOLUME_RANGE_MIN) return micon (volume_off_icon);
  else if (volume < VOLUME_RANGE_LOW) return micon (volume_low_icon);
  else if (volume < VOLUME_RANGE_MEDIUM) return micon (volume_medium_icon);
  else if (volume <= VOLUME_RANGE_MAX) return micon (volume_high_icon);
  else assert (false);
  return NULL;
}

void
game_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAME_MID);

  menu_sub ("&Load", LOAD_MID, true, micon (open_icon));

  menu_sub ("&Save", SAVE_MID, true, micon (save_icon));

  if (cutscene || title_demo) menu_sep ();
  else {
    char *title;
    xasprintf (&title, "LEVEL %i", global_level.n);
    menu_hitem (title, NO_MID, false);
    al_free (title);
  }

  menu_sitem ("Restart le&vel (Ctrl+A)", RESTART_LEVEL_MID,
              ! cutscene && ! title_demo,
              micon (jump_icon));

  menu_sitem ("&Next level (Shift+L)", NEXT_LEVEL_MID,
              ! cutscene && ! title_demo
              && global_level.n < 14,
              micon (last_icon));

  menu_sitem ("&Previous level (Shift+M)", PREVIOUS_LEVEL_MID,
              ! cutscene && ! title_demo
              && global_level.n > 1,
              micon (first_icon));

  menu_sep ();

  menu_sub ("&Volume (Ctrl+S)", VOLUME_MID, true,
            volume_icon (audio_volume));

  menu_citem ("&Mirror", MIRROR_MODE_MID, true,
              in_mirror_mode (), micon (horizontal_icon));

  menu_sep ();

  menu_ditem (cutscene || title_demo,
              "Sta&rt (Enter)", "&Restart (Ctrl+R)",
              START_GAME_MID, RESTART_GAME_MID, true,
              micon (right_icon), micon (reload_icon));

  menu_sitem ("&Quit (Ctrl+Q)", QUIT_GAME_MID, true, micon (quit_icon));

  end_menu ();

  load_menu ();
  save_menu ();
  volume_menu ();
#endif
}

void
load_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, LOAD_MID);

  menu_sitem ("&Game... (Ctrl+L)", LOAD_GAME_MID, true, micon (game_icon));

  menu_sitem ("&Configuration... (Ctrl+L)", LOAD_CONFIG_MID, true,
              micon (settings_icon));

  menu_sitem ("&Level file...", LOAD_LEVEL_FILE_MID, false, NULL);

  end_menu ();
#endif
}

void
save_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, SAVE_MID);

  menu_sitem ("&Game... (Ctrl+G)", SAVE_GAME_MID, true, micon (game_icon));

  menu_sitem ("&Configuration...", SAVE_CONFIG_MID, false,
              micon (settings_icon));

  menu_sitem ("&Level file...", SAVE_LEVEL_FILE_MID, false, NULL);

  end_menu ();
#endif
}

void
volume_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VOLUME_MID);

  menu_citem ("&Off", VOLUME_OFF_MID, true,
              audio_volume == VOLUME_RANGE_MIN,
              micon (volume_off_icon));

  menu_citem ("&Low", VOLUME_LOW_MID, true,
              audio_volume > VOLUME_RANGE_MIN
              && audio_volume < VOLUME_RANGE_LOW,
              micon (volume_low_icon));

  menu_citem ("&Medium", VOLUME_MEDIUM_MID, true,
              audio_volume >= VOLUME_RANGE_LOW
              && audio_volume < VOLUME_RANGE_MEDIUM,
              micon (volume_medium_icon));

  menu_citem ("&High", VOLUME_HIGH_MID, true,
              audio_volume >= VOLUME_RANGE_MEDIUM
              && audio_volume <= VOLUME_RANGE_MAX,
              micon (volume_high_icon));

  end_menu ();
#endif
}

void
view_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VIEW_MID);

  menu_ditem (is_fullscreen (), "&Windowed (F)", "&Fullscreen (F)",
              FULL_SCREEN_MID, FULL_SCREEN_MID, true,
              micon (windows_icon), micon (full_screen_icon));

  menu_sub ("&Zoom", ZOOM_MID, ! cutscene && ! title_demo,
            micon (zoom_icon));

  menu_sub ("&Navigation", NAV_MID, ! cutscene && ! title_demo,
            micon (navigation_icon));

  menu_sep ();

  menu_sub ("&Video (F12)", VM_MID, true, vm_icon (vm));

  menu_sub ("&Environment (F11)", EM_MID,
            ! cutscene && ! title_demo,
            force_em ? em_icon (em) : micon (original_icon));

  menu_sub ("&Hue (Alt+F11)", HUE_MODE_MID, ! cutscene && ! title_demo,
            force_hue ? hue_icon (hue) : micon (original_icon));

  menu_sub ("&Guard (Shift+F11)", GM_MID, ! cutscene && ! title_demo,
            gm_icon (gm));

  menu_sub ("&Flip (Shift+I)", FLIP_SCREEN_MID, true,
            flip_screen_icon (screen_flags));

  menu_sep ();

  menu_citem ("Room &drawing (Shift+B)", ROOM_DRAWING_MID,
              ! cutscene && ! title_demo,
              ! no_room_drawing, micon (drawing_icon));

  menu_citem ("&Inhibit screensaver", INHIBIT_SCREENSAVER_MID, true,
              inhibit_screensaver, micon (screensaver_icon));

  menu_sep ();

  menu_sitem ("&Screenshot... (Ctrl+P)", SCREENSHOT_MID, true,
              micon (camera_icon));

  end_menu ();

  zoom_menu ();
  navigation_menu ();
  hue_mode_menu ();
  gm_menu ();
  em_menu ();
  vm_menu ();
  screen_flip_menu ();
#endif
}

void
zoom_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, ZOOM_MID);

  char *text;
  xasprintf (&text, "MULTI-ROOM %ix%i", mr.w, mr.h);
  menu_hitem (text, ZOOM_RESET_MID, ! cutscene && ! title_demo
              && (mr.w != 2 || mr.h != 2
                  || mr.fit_mode != MR_FIT_NONE));
  al_free (text);

  menu_sub ("&Fit (M)", ZOOM_FIT_MID, ! cutscene && ! title_demo,
            zoom_fit_icon (mr.fit_mode));

  menu_sub ("&In", ZOOM_IN_MID,
            ! cutscene && ! title_demo && (mr.w > 1 || mr.h > 1),
            micon (zoom_in_icon));

  menu_sub ("&Out", ZOOM_OUT_MID, ! cutscene && ! title_demo,
            micon (zoom_out_icon));

  end_menu ();

  zoom_fit_menu ();
  zoom_out_menu ();
  zoom_in_menu ();
#endif
}

ALLEGRO_BITMAP *
zoom_fit_icon (enum mr_fit_mode fit)
{
  switch (fit) {
  case MR_FIT_NONE: return micon (zoom_none_icon); break;
  case MR_FIT_STRETCH: return micon (zoom_stretch_icon); break;
  case MR_FIT_RATIO: return micon (zoom_ratio_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
zoom_fit_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, ZOOM_FIT_MID);

  menu_citem ("&None", ZOOM_NONE_MID, true,
              mr.fit_mode == MR_FIT_NONE, micon (zoom_none_icon));

  menu_citem ("&Stretch", ZOOM_STRETCH_MID, true,
              mr.fit_mode == MR_FIT_STRETCH, micon (zoom_stretch_icon));

  menu_citem ("&Ratio", ZOOM_RATIO_MID, true,
              mr.fit_mode == MR_FIT_RATIO, micon (zoom_ratio_icon));

  end_menu ();
#endif
}

void
zoom_out_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, ZOOM_OUT_MID);

  menu_sitem ("&Both (])", ZOOM_OUT_BOTH_MID, true,
              micon (vertical_horizontal_icon));

  menu_sitem ("&Vertical (Alt+])", ZOOM_OUT_VERTICAL_MID, true,
              micon (vertical_icon));

  menu_sitem ("&Horizontal (Ctrl+])", ZOOM_OUT_HORIZONTAL_MID, true,
              micon (horizontal_icon));

  end_menu ();
#endif
}

void
zoom_in_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, ZOOM_IN_MID);

  menu_sitem ("&Both ([)", ZOOM_IN_BOTH_MID, mr.w > 1 && mr.h > 1,
              micon (vertical_horizontal_icon));

  menu_sitem ("&Vertical (Alt+[)", ZOOM_IN_VERTICAL_MID, mr.h > 1,
              micon (vertical_icon));

  menu_sitem ("&Horizontal (Ctrl+[)", ZOOM_IN_HORIZONTAL_MID, mr.w > 1,
              micon (horizontal_icon));

  end_menu ();
#endif
}

void
navigation_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, NAV_MID);

  char *text;
  xasprintf (&text, "ROOM %i", mr.room);
  menu_hitem (text, NAV_CURRENT_ROOM_SELECT_MID, true);
  al_free (text);

  menu_sub ("&Selection", NAV_SELECT_MID, true,
            micon (nav_select_icon));

  menu_sub ("Scroll &row", NAV_CELL_MID, true,
            micon (nav_cell_icon));

  menu_sub ("Scroll &page", NAV_PAGE_MID, true,
            micon (nav_page_icon));

  struct anim *k = get_anim_by_id (current_kid_id);
  menu_sitem ("&Home (Home)", NAV_HOME_MID,
              k && k->f.c.room != mr.room,
              micon (nav_home_icon));

  menu_sitem ("Cen&ter (Shift+Home)", NAV_CENTER_MID, true,
              micon (nav_home_icon));

  menu_sitem ("C&oordinates (C)", COORDINATES_MID, true,
              micon (compass_icon));

  menu_sitem ("&Indirect coordinates (Shift+C)",
              INDIRECT_COORDINATES_MID, true,
              micon (compass2_icon));

  end_menu ();

  nav_select_menu ();
  nav_cell_menu ();
  nav_page_menu ();
#endif
}

void
nav_select_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, NAV_SELECT_MID);

  menu_sitem ("&Left (H)", NAV_SELECT_LEFT_MID,
              roomd (&global_level, mr.room, LEFT),
              micon (nav_left_icon));

  menu_sitem ("&Above (U)", NAV_SELECT_ABOVE_MID,
              roomd (&global_level, mr.room, ABOVE),
              micon (nav_above_icon));

  menu_sitem ("&Right (J)", NAV_SELECT_RIGHT_MID,
              roomd (&global_level, mr.room, RIGHT),
              micon (nav_right_icon));

  menu_sitem ("&Below (N)", NAV_SELECT_BELOW_MID,
              roomd (&global_level, mr.room, BELOW),
              micon (nav_below_icon));

  end_menu ();
#endif
}

void
nav_cell_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, NAV_CELL_MID);

  menu_sitem ("&Left (Shift+H)", NAV_CELL_LEFT_MID, true,
              micon (nav_left_icon));

  menu_sitem ("&Above (Shift+U)", NAV_CELL_ABOVE_MID, true,
              micon (nav_above_icon));

  menu_sitem ("&Right (Shift+J)", NAV_CELL_RIGHT_MID, true,
              micon (nav_right_icon));

  menu_sitem ("&Below (Shift+N)", NAV_CELL_BELOW_MID, true,
              micon (nav_below_icon));

  end_menu ();
#endif
}

void
nav_page_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, NAV_PAGE_MID);

  menu_sitem ("&Left (Alt+H)", NAV_PAGE_LEFT_MID, true,
              micon (nav_left_icon));

  menu_sitem ("&Above (Alt+U)", NAV_PAGE_ABOVE_MID, true,
              micon (nav_above_icon));

  menu_sitem ("&Right (Alt+J)", NAV_PAGE_RIGHT_MID, true,
              micon (nav_right_icon));

  menu_sitem ("&Below (Alt+N)", NAV_PAGE_BELOW_MID, true,
              micon (nav_below_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
hue_icon (enum hue hue)
{
  switch (hue) {
  case HUE_NONE: return micon (black_icon); break;
  case HUE_GREEN: return micon (green_icon); break;
  case HUE_GRAY: return micon (gray_icon); break;
  case HUE_YELLOW: return micon (yellow_icon); break;
  case HUE_BLUE: return micon (blue_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
hue_mode_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, HUE_MODE_MID);

  menu_citem ("&Original", HUE_ORIGINAL_MID, true, ! force_hue,
              micon (original_icon));

  menu_citem ("&None", HUE_NONE_MID, true, force_hue && hue == HUE_NONE,
              micon (black_icon));

  menu_citem ("&Green", HUE_GREEN_MID, true, force_hue && hue == HUE_GREEN,
              micon (green_icon));

  menu_citem ("G&ray", HUE_GRAY_MID, true, force_hue && hue == HUE_GRAY,
              micon (gray_icon));

  menu_citem ("&Yellow", HUE_YELLOW_MID, true, force_hue && hue == HUE_YELLOW,
              micon (yellow_icon));

  menu_citem ("&Blue", HUE_BLUE_MID, true, force_hue && hue == HUE_BLUE,
              micon (blue_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
gm_icon (enum gm gm)
{
  switch (gm) {
  case ORIGINAL_GM: return micon (original_icon); break;
  case GUARD_GM: return micon (guard_icon); break;
  case FAT_GUARD_GM: return micon (fat_guard_icon); break;
  case VIZIER_GM: return micon (vizier_icon); break;
  case SKELETON_GM: return micon (skeleton_icon); break;
  case SHADOW_GM: return micon (shadow_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
gm_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GM_MID);

  menu_citem ("&Original", ORIGINAL_GM_MID, true,
              gm == ORIGINAL_GM, gm_icon (ORIGINAL_GM));

  menu_citem ("&Guard", GUARD_GM_MID, true,
              gm == GUARD_GM, gm_icon (GUARD_GM));

  menu_citem ("&Fat guard", FAT_GUARD_GM_MID, true,
              gm == FAT_GUARD_GM, gm_icon (FAT_GUARD_GM));

  menu_citem ("&Vizier", VIZIER_GM_MID, true,
              gm == VIZIER_GM, gm_icon (VIZIER_GM));

  menu_citem ("&Skeleton", SKELETON_GM_MID, true,
              gm == SKELETON_GM, gm_icon (SKELETON_GM));

  menu_citem ("S&hadow", SHADOW_GM_MID, true,
              gm == SHADOW_GM, gm_icon (SHADOW_GM));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
em_icon (enum em em)
{
  switch (em) {
  case DUNGEON: return micon (dungeon_icon); break;
  case PALACE: return micon (palace_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
em_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, EM_MID);

  menu_citem ("&Original", ORIGINAL_EM_MID, true, ! force_em,
              micon (original_icon));

  menu_citem ("&Dungeon", DUNGEON_MID, true, force_em && em == DUNGEON,
              micon (dungeon_icon));

  menu_citem ("&Palace", PALACE_MID, true, force_em && em == PALACE,
              micon (palace_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
vm_icon (enum vm vm)
{
  switch (vm) {
  case VGA: return micon (vga_icon); break;
  case EGA: return micon (ega_icon); break;
  case CGA: return micon (hgc ? hgc_icon : cga_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
vm_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, VM_MID);

  menu_citem ("&VGA", VGA_MID, true, vm == VGA, micon (vga_icon));

  menu_citem ("&EGA", EGA_MID, true, vm == EGA, micon (ega_icon));

  menu_citem ("&CGA", CGA_MID, true, vm == CGA && ! hgc, micon (cga_icon));

  menu_citem ("&HGC", HGC_MID, true, vm == CGA && hgc, micon (hgc_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
flip_screen_icon (int flags)
{
  switch (flags) {
  case 0: return micon (screen_icon);
  case ALLEGRO_FLIP_VERTICAL: return micon (vertical_icon);
  case ALLEGRO_FLIP_HORIZONTAL: return micon (horizontal_icon);
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    return micon (vertical_horizontal_icon);
  default: assert (false); return NULL;
  }
}

void
screen_flip_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, FLIP_SCREEN_MID);

  menu_citem ("&Vertical", FLIP_SCREEN_VERTICAL_MID, true,
              screen_flags & ALLEGRO_FLIP_VERTICAL,
              micon (vertical_icon));

  menu_citem ("&Horizontal", FLIP_SCREEN_HORIZONTAL_MID, true,
              screen_flags & ALLEGRO_FLIP_HORIZONTAL,
              micon (horizontal_icon));

  end_menu ();
#endif
}

void
gamepad_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAMEPAD_MID);

  menu_sub ("&Device", GAMEPAD_DEVICE_MID, true, gamepad_device_icon (gpm));

  menu_sitem ("&Calibrate (Ctrl+J)", GAMEPAD_CALIBRATE_MID,
              gpm == JOYSTICK, micon (clock_icon));

  menu_sep ();

  menu_sub ("&Flip (Shift+K)", FLIP_GAMEPAD_MID, true,
            flip_gamepad_icon (flip_gamepad_vertical,
                               flip_gamepad_horizontal));

  end_menu ();

  gamepad_device_menu ();
  flip_gamepad_menu ();
#endif
}

ALLEGRO_BITMAP *
gamepad_device_icon (enum gpm gpm)
{
  switch (gpm) {
  case KEYBOARD: return micon (keyboard_icon); break;
  case JOYSTICK: return micon (joystick_icon); break;
  default: assert (false); return NULL; break;
  }
}

void
gamepad_device_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, GAMEPAD_DEVICE_MID);

  menu_citem ("&Keyboard (Ctrl+K)", KEYBOARD_MODE_MID, true,
              gpm == KEYBOARD, micon (keyboard_icon));

  menu_citem ("&Joystick (Ctrl+J)", JOYSTICK_MODE_MID, joystick,
              gpm == JOYSTICK, micon (joystick_icon));

  end_menu ();
#endif
}

ALLEGRO_BITMAP *
flip_gamepad_icon (bool v, bool h)
{
  if (! v && ! h) return micon (joystick3_icon);
  else if (v && ! h) return micon (vertical_icon);
  else if (! v && h) return micon (horizontal_icon);
  else if (v && h) return micon (vertical_horizontal_icon);
  else assert (false);
  return NULL;
}

void
flip_gamepad_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, FLIP_GAMEPAD_MID);

  menu_citem ("&Vertical", FLIP_GAMEPAD_VERTICAL_MID, true,
              flip_gamepad_vertical, micon (vertical_icon));

  menu_citem ("&Horizontal", FLIP_GAMEPAD_HORIZONTAL_MID, true,
              flip_gamepad_horizontal, micon (horizontal_icon));

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
    xasprintf (&text, "REPLAYING (%zu/%zu)", replay_index + 1,
               replay_chain_nmemb);
    menu_hitem (text, RESTART_LEVEL_MID, true);
    al_free (text);

    menu_sitem
      ("&Stop (F7)", PLAY_REPLAY_MID, true, micon (stop_icon));

    menu_sitem
      ("Pre&vious (Shift+M)", PREVIOUS_LEVEL_MID,
       replay_index > 0, micon (previous_icon));

    menu_sitem
      ("&Next (Shift+L)", NEXT_LEVEL_MID,
       replay_index + 1 < replay_chain_nmemb, micon (next_icon));

    pause_menu_widget ();

    speed_menu_widget ();

    break;
  case RECORD_REPLAY: record_replay:

    menu_hitem ("RECORDING", NO_MID, false);

    menu_sitem
      (recording_replay_countdown >= 0
       ? "&Abort recording (F7)" : "&Stop... (F7)",
       RECORD_REPLAY_MID, true,
       replay_mode == RECORD_REPLAY
       ? micon (stop_icon) : micon (eject_icon));

    pause_menu_widget ();

    break;
  default: assert (false);
  case NO_REPLAY: no_replay:

    menu_sitem
      ("&Play... (F7)", PLAY_REPLAY_MID, true, micon (play_icon));

    menu_sitem
      ("&Record... (Alt+F7)", RECORD_REPLAY_MID, true,
       micon (record_icon));

    pause_menu_widget ();

    speed_menu_widget ();

    break;
  }

  end_menu ();
#endif
}

void
editor_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, EDITOR_MID);

  menu_ditem (edit == EDIT_NONE, "&Edit (F8)", "&Play (F8)",
              EDIT_MODE_MID, EDIT_MODE_MID,
              true, micon (edit_icon), micon (joystick2_icon));

  menu_sitem ("&Undo (Ctrl+Z)", UNDO_MID,
              can_undo (&undo, -1), micon (undo_icon));

  menu_sitem ("&Redo (Ctrl+Y)", REDO_MID,
              can_undo (&undo, +1), micon (redo_icon));

  end_menu ();
#endif
}

void
cheat_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, CHEAT_MID);

  struct anim *k = get_anim_by_id (current_kid_id);

  menu_sitem ("&Kill enemy (K)", KILL_ENEMY_MID,
              k && k->enemy_id > 0, micon (death_icon));

  menu_sitem ("&Resurrect (R)", RESURRECT_MID,
              k && k->current_lives <= 0,
              micon (resurrect_icon));

  menu_citem ("&Immortal (I)", IMMORTAL_MID, true, immortal_mode,
              micon (angel_icon));

  menu_sitem ("&Float (Shift+W)", FLOAT_MID,
              k && (k->current_lives > 0 || is_kid_fall (&k->f))
              && (k->float_timer == 0
                  || k->float_timer >= REFLOAT_MENU_THRESHOLD),
              micon (feather_icon));

  menu_sitem ("Fill &single container (Shift+S)", FILL_LIFE_MID,
              k && k->current_lives > 0
              && k->current_lives < k->total_lives,
              micon (life_empty_icon));

  menu_sitem ((! k || k->total_lives < MAX_LIVES)
              ? "&Add container (Shift+T)"
              : "Fill &all containers (Shift+T)", ADD_LIFE_MID,
              k && k->current_lives > 0
              && (k->current_lives < k->total_lives
                  || k->total_lives < MAX_LIVES),
              micon (life_full_icon));

  end_menu ();
#endif
}

void
help_menu (void)
{
#if MENU_FEATURE
  start_menu (main_menu, HELP_MID);

  menu_sitem ("&About", ABOUT_MID, true, micon (small_logo_icon));

  end_menu ();
#endif
}

#if MENU_FEATURE
void
pause_menu_widget (void)
{
  if (is_game_paused ()) {
    char *title;
    xasprintf (&title, "CYCLE: %ju", anim_cycle);
    menu_hitem (title, NO_MID, false);
    al_free (title);
  } else menu_sep ();

  menu_sitem
    (is_game_paused ()
     ? "&Continue" : "Pau&se (Esc)", TOGGLE_PAUSE_GAME_MID,
     ! cutscene && ! title_demo && recording_replay_countdown < 0,
     is_game_paused () ? micon (play_icon) : micon (pause_icon));

  menu_sitem
    ("Next &frame (Esc)", NEXT_FRAME_MID,
     is_game_paused () && ! cutscene && ! title_demo
     && recording_replay_countdown < 0,
     micon (forward_icon));
}
#endif

#if MENU_FEATURE
void
speed_menu_widget (void)
{
  if (anim_freq > 0) {
    char *title;
    xasprintf (&title, "FREQ: %iHz", anim_freq);
    menu_hitem (title, RESET_TIME_FREQ_MID, anim_freq != DEFAULT_HZ);
    al_free (title);
  } else menu_sep ();

  menu_citem
    ("Speed cons&traint", TOGGLE_TIME_FREQ_CONSTRAINT_MID,
     ! cutscene && ! title_demo, anim_freq > 0, micon (cancel_icon));

  menu_sitem
    ("&Decrease speed [(]", DECREASE_TIME_FREQ_MID,
     anim_freq > 2 && ! cutscene && ! title_demo,
     micon (backward_icon));

  menu_sitem
    ("&Increase speed [)]", INCREASE_TIME_FREQ_MID,
     anim_freq != 0 && ! cutscene && ! title_demo,
     micon (forward_icon));
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
  case SAVE_GAME_MID:
    ui_save_game ();
    break;
  case RESTART_LEVEL_MID:
    ui_restart_level ();
    break;
  case PREVIOUS_LEVEL_MID:
    ui_previous_level ();
    break;
  case NEXT_LEVEL_MID:
    ui_next_level ();
    break;
  case MIRROR_MODE_MID:
    ui_mirror_mode (! in_mirror_mode ());
    break;
  case VOLUME_OFF_MID:
    ui_audio_volume (VOLUME_OFF);
    break;
  case VOLUME_LOW_MID:
    ui_audio_volume (VOLUME_LOW);
    break;
  case VOLUME_MEDIUM_MID:
    ui_audio_volume (VOLUME_MEDIUM);
    break;
  case VOLUME_HIGH_MID:
    ui_audio_volume (VOLUME_HIGH);
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


    /* VIEW */
  case FULL_SCREEN_MID:
    ui_full_screen ();
    break;
  case ZOOM_NONE_MID:
    ui_zoom_fit (MR_FIT_NONE);
    break;
  case ZOOM_STRETCH_MID:
    ui_zoom_fit (MR_FIT_STRETCH);
    break;
  case ZOOM_RATIO_MID:
    ui_zoom_fit (MR_FIT_RATIO);
    break;
  case ZOOM_OUT_BOTH_MID:
    ui_set_multi_room (+1, +1, false);
    break;
  case ZOOM_OUT_VERTICAL_MID:
    ui_set_multi_room (+0, +1, false);
    break;
  case ZOOM_OUT_HORIZONTAL_MID:
    ui_set_multi_room (+1, +0, false);
    break;
  case ZOOM_IN_BOTH_MID:
    ui_set_multi_room (-1, -1, false);
    break;
  case ZOOM_IN_VERTICAL_MID:
    ui_set_multi_room (+0, -1, false);
    break;
  case ZOOM_IN_HORIZONTAL_MID:
    ui_set_multi_room (-1, +0, false);
    break;
  case ZOOM_RESET_MID:
    ui_zoom_fit (MR_FIT_NONE);
    ui_set_multi_room (2 - mr.w, 2 - mr.h, false);
    break;
  case NAV_CURRENT_ROOM_SELECT_MID:
    mr.select_cycles = SELECT_CYCLES;
    break;
  case NAV_SELECT_LEFT_MID:
    mr_select_trans (LEFT);
    break;
  case NAV_SELECT_RIGHT_MID:
    mr_select_trans (RIGHT);
    break;
  case NAV_SELECT_ABOVE_MID:
    mr_select_trans (ABOVE);
    break;
  case NAV_SELECT_BELOW_MID:
    mr_select_trans (BELOW);
    break;
  case NAV_CELL_LEFT_MID:
    mr_view_trans (LEFT);
    break;
  case NAV_CELL_RIGHT_MID:
    mr_view_trans (RIGHT);
    break;
  case NAV_CELL_ABOVE_MID:
    mr_view_trans (ABOVE);
    break;
  case NAV_CELL_BELOW_MID:
    mr_view_trans (BELOW);
    break;
  case NAV_PAGE_LEFT_MID:
    mr_view_page_trans (LEFT);
    break;
  case NAV_PAGE_RIGHT_MID:
    mr_view_page_trans (RIGHT);
    break;
  case NAV_PAGE_ABOVE_MID:
    mr_view_page_trans (ABOVE);
    break;
  case NAV_PAGE_BELOW_MID:
    mr_view_page_trans (BELOW);
    break;
  case NAV_HOME_MID:
    mr_focus_room (get_anim_by_id (current_kid_id)->f.c.room);
    break;
  case NAV_CENTER_MID:
    mr_center_room (mr.room);
    break;
  case COORDINATES_MID:
    ui_show_coordinates ();
    break;
  case INDIRECT_COORDINATES_MID:
    ui_show_indirect_coordinates ();
    break;
  case HUE_ORIGINAL_MID:
    ui_hue_mode (HUE_ORIGINAL);
    break;
  case HUE_NONE_MID:
    ui_hue_mode (HUE_NONE);
    break;
  case HUE_GREEN_MID:
    ui_hue_mode (HUE_GREEN);
    break;
  case HUE_GRAY_MID:
    ui_hue_mode (HUE_GRAY);
    break;
  case HUE_YELLOW_MID:
    ui_hue_mode (HUE_YELLOW);
    break;
  case HUE_BLUE_MID:
    ui_hue_mode (HUE_BLUE);
    break;
  case ORIGINAL_GM_MID:
    ui_gm (ORIGINAL_GM);
    break;
  case GUARD_GM_MID:
    ui_gm (GUARD_GM);
    break;
  case FAT_GUARD_GM_MID:
    ui_gm (FAT_GUARD_GM);
    break;
  case VIZIER_GM_MID:
    ui_gm (VIZIER_GM);
    break;
  case SKELETON_GM_MID:
    ui_gm (SKELETON_GM);
    break;
  case SHADOW_GM_MID:
    ui_gm (SHADOW_GM);
    break;
  case ORIGINAL_EM_MID:
    ui_em (ORIGINAL_EM);
    break;
  case DUNGEON_MID:
    ui_em (DUNGEON);
    break;
  case PALACE_MID:
    ui_em (PALACE);
    break;
  case VGA_MID:
    ui_vm (VGA);
    break;
  case EGA_MID:
    ui_vm (EGA);
    break;
  case CGA_MID:
    ui_vm (CGA);
    break;
  case HGC_MID:
    ui_vm (HGC);
    break;
  case FLIP_SCREEN_VERTICAL_MID:
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_VERTICAL, false, false);
    break;
  case FLIP_SCREEN_HORIZONTAL_MID:
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_HORIZONTAL, false, false);
    break;
  case INHIBIT_SCREENSAVER_MID:
    ui_inhibit_screensaver (! inhibit_screensaver);
    break;
  case ROOM_DRAWING_MID:
    ui_room_drawing (no_room_drawing);
    break;
  case SCREENSHOT_MID:
    ui_screenshot ();
    break;


    /* GAMEPAD */
  case KEYBOARD_MODE_MID:
    ui_gamepad_mode (KEYBOARD);
    break;
  case JOYSTICK_MODE_MID:
    ui_gamepad_mode (JOYSTICK);
    break;
  case GAMEPAD_CALIBRATE_MID:
    ui_gamepad_mode (JOYSTICK);
    break;
  case FLIP_GAMEPAD_VERTICAL_MID:
    ui_flip_gamepad (! flip_gamepad_vertical, flip_gamepad_horizontal, false);
    break;
  case FLIP_GAMEPAD_HORIZONTAL_MID:
    ui_flip_gamepad (flip_gamepad_vertical, ! flip_gamepad_horizontal, false);
    break;


    /* REPLAY */
  case PLAY_REPLAY_MID:
    ui_play_replay ();
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
  case RESET_TIME_FREQ_MID:
    ui_change_anim_freq (DEFAULT_HZ);
    break;
  case DECREASE_TIME_FREQ_MID:
    ui_decrease_time_frequency ();
    break;
  case INCREASE_TIME_FREQ_MID:
    ui_increase_time_frequency ();
    break;
  case TOGGLE_TIME_FREQ_CONSTRAINT_MID:
    ui_toggle_time_frequency_constraint ();
    break;


    /* EDITOR */
  case EDIT_MODE_MID:
    ui_editor ();
    break;
  case UNDO_MID:
    ui_undo_pass (&undo, -1, NULL);
    break;
  case REDO_MID:
    ui_undo_pass (&undo, +1, NULL);
    break;


    /* CHEAT */
  case RESURRECT_MID:
    ui_resurrect ();
    break;
  case KILL_ENEMY_MID:
    ui_kill_enemy ();
    break;
  case FLOAT_MID:
    ui_float ();
    break;
  case IMMORTAL_MID:
    ui_immortal (! immortal_mode);
    break;
  case FILL_LIFE_MID:
    ui_fill_life ();
    break;
  case ADD_LIFE_MID:
    ui_add_life ();
    break;


    /* HELP */
  case ABOUT_MID:
    ui_about_screen (true);
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
    if (audio_volume == VOLUME_RANGE_MIN) ui_audio_volume (VOLUME_LOW);
    else if (audio_volume < VOLUME_RANGE_LOW) ui_audio_volume (VOLUME_MEDIUM);
    else if (audio_volume < VOLUME_RANGE_MEDIUM) ui_audio_volume (VOLUME_HIGH);
    else if (audio_volume <= VOLUME_RANGE_MAX) ui_audio_volume (VOLUME_OFF);
  }

  /* F: enable/disable fullscreen mode */
  else if ((! active_menu && was_key_pressed (0, ALLEGRO_KEY_F))
           || was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_ENTER))
    ui_full_screen ();

  /* SHIFT+I: flip screen */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_I)) {
    switch (screen_flags) {
    case 0: ui_flip_screen (ALLEGRO_FLIP_VERTICAL, true, false); break;
    case ALLEGRO_FLIP_VERTICAL:
      ui_flip_screen (ALLEGRO_FLIP_HORIZONTAL, true, false); break;
    case ALLEGRO_FLIP_HORIZONTAL:
      ui_flip_screen (ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL,
                      true, false);
      break;
    case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    default: ui_flip_screen (0, true, false); break;
    }
  }

  /* SHIFT+K: flip gamepad */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_K)) {
    if (! flip_gamepad_vertical && ! flip_gamepad_horizontal)
      ui_flip_gamepad (true, false, false);
    else if (flip_gamepad_vertical && ! flip_gamepad_horizontal)
      ui_flip_gamepad (false, true, false);
    else if (! flip_gamepad_vertical && flip_gamepad_horizontal)
      ui_flip_gamepad (true, true, false);
    else if (flip_gamepad_vertical && flip_gamepad_horizontal)
      ui_flip_gamepad (false, false, false);
  }

  /* CTRL+K: keyboard mode (disables joystick) */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_K))
    ui_gamepad_mode (KEYBOARD);

  /* CTRL+J: joystick mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_J))
    ui_gamepad_mode (JOYSTICK);

  /* ALT+F11: change hue palette */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_F11)) {
    if (force_hue)
      switch (hue) {
      case HUE_NONE: ui_hue_mode (HUE_GREEN); break;
      case HUE_GREEN: ui_hue_mode (HUE_GRAY); break;
      case HUE_GRAY: ui_hue_mode (HUE_YELLOW); break;
      case HUE_YELLOW: ui_hue_mode (HUE_BLUE); break;
      case HUE_BLUE: ui_hue_mode (HUE_ORIGINAL); break;
      }
    else ui_hue_mode (HUE_NONE);
  }

  /* F11: change environment mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F11)) {
    if (force_em) {
      switch (em) {
      case DUNGEON: ui_em (PALACE); break;
      case PALACE: ui_em (ORIGINAL_EM); break;
      }
    } else ui_em (DUNGEON);
  }

  /* F12: change video mode */
  else if (was_key_pressed (0, ALLEGRO_KEY_F12))
    switch (vm) {
    case VGA: ui_vm (EGA); break;
    case EGA: ui_vm (CGA); break;
    case CGA: ui_vm (hgc ? VGA : HGC); break;
    }

  /* D: change display mode */
  else if (! active_menu && was_key_pressed (0, ALLEGRO_KEY_D))
    ui_next_display_mode ();
}






void
level_key_bindings (void)
{
  if (title_demo) return;

  /* CTRL+A: restart level */
  if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_A))
    ui_restart_level ();

  /* SHIFT+L: warp to next level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_L))
    ui_next_level ();

  /* SHIFT+M: warp to previous level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_M))
    ui_previous_level ();

  /* CTRL+G: save game */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_G)
      && ! save_game_dialog_thread)
    ui_save_game ();

  /* SHIFT+F11: change guard mode */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_F11))
      switch (gm) {
      case ORIGINAL_GM: ui_gm (GUARD_GM); break;
      case GUARD_GM: ui_gm (FAT_GUARD_GM); break;
      case FAT_GUARD_GM: ui_gm (VIZIER_GM); break;
      case VIZIER_GM: ui_gm (SKELETON_GM); break;
      case SKELETON_GM: ui_gm (SHADOW_GM); break;
      case SHADOW_GM: ui_gm (ORIGINAL_GM); break;
      }

  /* CTRL+Z: undo */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Z))
    ui_undo_pass (&undo, -1, NULL);

  /* CTRL+Y: redo */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_Y))
    ui_undo_pass (&undo, +1, NULL);

  /* M: change multi-room fit mode */
  else if (! active_menu &&
           was_key_pressed (0, ALLEGRO_KEY_M))
    switch (mr.fit_mode) {
    case MR_FIT_NONE: ui_zoom_fit (MR_FIT_STRETCH); break;
    case MR_FIT_STRETCH: ui_zoom_fit (MR_FIT_RATIO); break;
    case MR_FIT_RATIO: ui_zoom_fit (MR_FIT_NONE); break;
    default: assert (false); break;
    }

  /* [: decrease multi-room resolution */
  else if (was_key_pressed (0, ALLEGRO_KEY_OPENBRACE)
           && ! cutscene)
    ui_set_multi_room (-1, -1, true);

  /* ]: increase multi-room resolution */
  else if (was_key_pressed (0, ALLEGRO_KEY_CLOSEBRACE)
           && ! cutscene)
    ui_set_multi_room (+1, +1, true);

  /* CTRL+[: decrease multi-room width resolution */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_OPENBRACE)
           && ! cutscene)
    ui_set_multi_room (-1, +0, true);

  /* CTRL+]: increase multi-room width resolution */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_CLOSEBRACE)
           && ! cutscene)
    ui_set_multi_room (+1, +0, true);

  /* ALT+[: decrease multi-room height resolution */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_OPENBRACE)
           && ! cutscene)
    ui_set_multi_room (+0, -1, true);

  /* ALT+]: increase multi-room height resolution */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_CLOSEBRACE)
           && ! cutscene)
    ui_set_multi_room (+0, +1, true);

  /* H: select room at left (J if flipped horizontally) */
  else if (! active_menu
           && ((! flip_gamepad_horizontal
                && was_key_pressed (0, ALLEGRO_KEY_H))
               || (flip_gamepad_horizontal
                   && was_key_pressed (0, ALLEGRO_KEY_J))))
    mr_select_trans (LEFT);

  /* J: select room at right (H if flipped horizontally) */
  else if (! active_menu
           && ((! flip_gamepad_horizontal
                && was_key_pressed (0, ALLEGRO_KEY_J))
               || (flip_gamepad_horizontal
                   && was_key_pressed (0, ALLEGRO_KEY_H))))
    mr_select_trans (RIGHT);

  /* U: select room above (N if flipped vertically) */
  else if (! active_menu
           && ((! flip_gamepad_vertical
                && was_key_pressed (0, ALLEGRO_KEY_U))
               || (flip_gamepad_vertical
                   && was_key_pressed (0, ALLEGRO_KEY_N))))
    mr_select_trans (ABOVE);

  /* N: select room below (U if flipped vertically) */
  else if (! active_menu
           && ((! flip_gamepad_vertical
                && was_key_pressed (0, ALLEGRO_KEY_N))
               || (flip_gamepad_vertical
                   && was_key_pressed (0, ALLEGRO_KEY_U))))
    mr_select_trans (BELOW);

  /* SHIFT+H: multi-room view to left (J if flipped horizontally) */
  else if (! active_menu
           && ((! flip_gamepad_horizontal
                && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_H))
               || (flip_gamepad_horizontal
                   && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_J))))
    mr_view_trans (LEFT);

  /* SHIFT+J: multi-room view to right (H if flipped horizontally) */
  else if (! active_menu
           && ((! flip_gamepad_horizontal
                && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_J))
               || (flip_gamepad_horizontal
                   && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_H))))
    mr_view_trans (RIGHT);

  /* SHIFT+U: multi-room view above (N if flipped vertically) */
  else if (! active_menu
           && ((! flip_gamepad_vertical
                && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_U))
               || (flip_gamepad_vertical
                   && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_N))))
    mr_view_trans (ABOVE);

  /* SHIFT+N: multi-room view below (U if flipped vertically) */
  else if (! active_menu
           && ((! flip_gamepad_vertical
                && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_N))
               || (flip_gamepad_vertical
                   && was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_U))))
    mr_view_trans (BELOW);

  /* ALT+H: multi-room page view to left (J if flipped horizontally) */
  else if (! active_menu
           && ((! flip_gamepad_horizontal
                && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_H))
               || (flip_gamepad_horizontal
                   && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_J))))
    mr_view_page_trans (LEFT);

  /* ALT+J: multi-room page view to right (H if flipped horizontally) */
  else if (! active_menu
           && ((! flip_gamepad_horizontal
                && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_J))
               || (flip_gamepad_horizontal
                   && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_H))))
    mr_view_page_trans (RIGHT);

  /* ALT+U: multi-room page view above (N if flipped vertically) */
  else if (! active_menu
           && ((! flip_gamepad_vertical
                && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_U))
               || (flip_gamepad_vertical
                   && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_N))))
    mr_view_page_trans (ABOVE);

  /* ALT+N: multi-room page view below (U if flipped vertically) */
  else if (! active_menu
           && ((! flip_gamepad_vertical
                && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_N))
               || (flip_gamepad_vertical
                   && was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_U))))
    mr_view_page_trans (BELOW);

  /* HOME: focus multi-room view on kid */
  else if (was_key_pressed (0, ALLEGRO_KEY_HOME)) {
    struct anim *k = get_anim_by_id (current_kid_id);
    mr_focus_room (k->f.c.room);
  }

  /* SHIFT+HOME: center multi-room view */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_HOME))
    mr_center_room (mr.room);

  /* C: show direct coordinates */
  else if (! active_menu
           && was_key_pressed (0, ALLEGRO_KEY_C))
    ui_show_coordinates ();

  /* SHIFT+C: show indirect coordinates */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_C))
    ui_show_indirect_coordinates ();

  /* SHIFT+B: enable/disable room drawing */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_B))
    ui_room_drawing (no_room_drawing);

  /* K: kill enemy */
  else if (! active_menu
           && was_key_pressed (0, ALLEGRO_KEY_K))
    ui_kill_enemy ();

  /* R: resurrect kid */
  else if (! active_menu
           && was_key_pressed (0, ALLEGRO_KEY_R))
    ui_resurrect ();

  /* I: enable/disable immortal mode */
  else if (! active_menu
           && was_key_pressed (0, ALLEGRO_KEY_I))
    ui_immortal (! immortal_mode);

  /* SHIFT+W: float kid */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_W))
    ui_float ();

  /* SHIFT+S: incremet kid current lives */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_S))
    ui_fill_life ();

  /* SHIFT+T: incremet kid total lives */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_T))
    ui_add_life ();

  /* SPACE: display remaining time */
  else if (! active_menu
           && (was_key_pressed (0, ALLEGRO_KEY_SPACE)
               || was_joystick_button_pressed (joystick_time_button)))
    display_remaining_time (0);

  /* +: increment time limit and display remaining time */
  else if (! active_menu
           && (was_key_pressed (0, ALLEGRO_KEY_EQUALS)
               || was_key_pressed (ALLEGRO_KEYMOD_SHIFT,
                                   ALLEGRO_KEY_EQUALS))) {
    if (replay_mode == NO_REPLAY) {
      int t = time_limit - play_time;
      int d = t > (60 * DEFAULT_HZ) ? (+60 * DEFAULT_HZ) : (+1 * DEFAULT_HZ);
      time_limit += d;
      display_remaining_time (0);
    } else print_replay_mode (0);
  }

  /* -: decrement time limit and display remaining time */
  else if (! active_menu
           && (was_key_pressed (0, ALLEGRO_KEY_MINUS)
               || was_char_pressed ('_'))) {
    if (replay_mode == NO_REPLAY) {
      int t = time_limit - play_time;
      int d = t > (60 * DEFAULT_HZ) ? (-60 * DEFAULT_HZ) : (-1 * DEFAULT_HZ);
      time_limit += d;
      display_remaining_time (0);
    } else print_replay_mode (0);
  }

  /* TAB: display skill */
  else if (was_key_pressed (0, ALLEGRO_KEY_TAB)) {
    struct anim *k = get_anim_by_id (current_kid_id);
    display_skill (k);
  }

  /* CTRL+=: increment counter attack skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_EQUALS)) {
    if (replay_mode == NO_REPLAY) {
      struct anim *k = get_anim_by_id (current_kid_id);
      if (k->skill.counter_attack_prob < 99)
        k->skill.counter_attack_prob++;
      display_skill (k);
    } else print_replay_mode (0);
  }

  /* CTRL+-: decrement counter attack skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_MINUS)) {
    if (replay_mode == NO_REPLAY) {
      struct anim *k = get_anim_by_id (current_kid_id);
      if (k->skill.counter_attack_prob > -1)
        k->skill.counter_attack_prob--;
      display_skill (k);
    } else print_replay_mode (0);
  }

  /* ALT+=: increment counter defense skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_EQUALS)) {
    if (replay_mode == NO_REPLAY) {
      struct anim *k = get_anim_by_id (current_kid_id);
      if (k->skill.counter_defense_prob < 99)
        k->skill.counter_defense_prob++;
      display_skill (k);
    } else print_replay_mode (0);
  }

  /* ALT+-: decrement counter defense skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_MINUS)) {
    if (replay_mode == NO_REPLAY) {
      struct anim *k = get_anim_by_id (current_kid_id);
      if (k->skill.counter_defense_prob > -1)
        k->skill.counter_defense_prob--;
      display_skill (k);
    } else print_replay_mode (0);
  }

  /* A: alternate between kid and its shadows */
  else if (! active_menu
           && was_key_pressed (0, ALLEGRO_KEY_A)) {
    struct anim *k;
    do {
      k = &anima[(k - anima + 1) % anima_nmemb];
    } while (k->type != KID || ! k->controllable);
    current_kid_id = k->id;
    mr_focus_room (k->f.c.room);
  }

  /* ESC: pause game */
  if (step_cycle > 0) {
    game_paused = false;
    step_cycle--;
  } else if (step_cycle == 0) {
    game_paused = true;
    step_cycle = -1;
    replay_menu ();
  }

  if (was_key_pressed (0, ALLEGRO_KEY_ESCAPE)
      || was_joystick_button_pressed (joystick_pause_button)) {
    if (is_game_paused ()) {
      step_cycle = 0;
      game_paused = false;
    } else pause_game (true);
  } else if (is_game_paused ()
             && (! active_menu || ! was_menu_key_pressed ())
             && was_any_key_pressed ())
    pause_game (false);
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

  editor_menu ();
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
ui_save_game (void)
{
  save_game_dialog_thread =
    create_thread (dialog_thread, &save_game_dialog);
  al_start_thread (save_game_dialog_thread);
  pause_animation (true);
}

void
ui_restart_level (void)
{
  if (replay_mode == NO_REPLAY)
    quit_anim = RESTART_LEVEL;
  else if (replay_mode == PLAY_REPLAY)
    quit_anim = REPLAY_RESTART_LEVEL;
  else print_replay_mode (0);
}

void
ui_previous_level (void)
{
  if (replay_mode == NO_REPLAY) {
    ignore_level_cutscene = true;
    next_level_number = global_level.n - 1;
    quit_anim = NEXT_LEVEL;
  } else if (replay_mode == PLAY_REPLAY) {
    if (replay_index > 0) quit_anim = REPLAY_PREVIOUS;
    else ui_msg (0, "NO PREVIOUS REPLAY");
  } else print_replay_mode (0);
}

void
ui_next_level (void)
{
  if (replay_mode == NO_REPLAY) {
    ignore_level_cutscene = true;
    next_level_number = global_level.n + 1;
    quit_anim = NEXT_LEVEL;
  } else if (replay_mode == PLAY_REPLAY) {
    if (replay_index + 1 < replay_chain_nmemb)
      quit_anim = REPLAY_NEXT;
    else ui_msg (0, "NO NEXT REPLAY");
  } else print_replay_mode (0);
}

void
ui_audio_volume (float volume)
{
  char *key = "SOUND GAIN";
  char *value;
  xasprintf (&value, "%.1f", volume);

  set_audio_volume (volume);

  char *status;
  if (volume == VOLUME_RANGE_MIN) status = "OFF";
  else if (volume < VOLUME_RANGE_LOW) status = "LOW";
  else if (volume < VOLUME_RANGE_MEDIUM) status = "MEDIUM";
  else if (volume <= VOLUME_RANGE_MAX) status = "HIGH";

  ui_msg (0, "SOUND %s", status);

  ui_save_setting (key, value);
  al_free (value);
  game_menu ();
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
  if (display_mode < 0) {
    char *key = "FULLSCREEN";
    char *value;
    force_full_redraw = true;
    if (is_fullscreen ()) {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, false);
      value = "OFF";
      show_mouse_cursor ();
      show_menu ();
    } else {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, true);
      value = "ON";
      if (edit == EDIT_NONE) hide_menu ();
    }

    ui_msg (0, "%s: %s", key, value);

    ui_save_setting (key, value);
    view_menu ();
  } else ui_msg (0, "NON-DESKTOP MODE IS FULLSCREEN ONLY");
}

void
ui_zoom_fit (enum mr_fit_mode fit)
{
  char *key = "MULTI ROOM FIT MODE";
  char *value;

  switch (fit) {
  case MR_FIT_NONE:
    mr.fit_mode = MR_FIT_NONE;
    value = "NONE";
    break;
  case MR_FIT_STRETCH:
    mr.fit_mode = MR_FIT_STRETCH;
    value = "STRETCH";
    break;
  case MR_FIT_RATIO:
    mr.fit_mode = MR_FIT_RATIO;
    value = "RATIO";
    break;
  default:
    assert (false);
    return;
  }

  apply_mr_fit_mode ();

  ui_msg (0, "ZOOM FIT: %s", value);

  ui_save_setting (key, value);
  view_menu ();
}

bool
ui_set_multi_room (int dw, int dh, bool correct_mouse)
{
  char *key = "MULTI ROOM";
  char *value;

  if (mr.w + dw < 1 || mr.h + dh < 1) {
    ui_msg (0, "MULTI-ROOM %ix%i", mr.w, mr.h);
    return false;
  }

  struct mouse_coord m;
  get_mouse_coord (&m);

  if (mr.w + dw != mr.w || mr.h + dh != mr.h)
    set_multi_room (mr.w + dw, mr.h + dh);

  mr_center_room (mr.room);

  if (mr_coord (m.c.room, -1, NULL, NULL) && correct_mouse)
    set_mouse_coord (&m);

  ui_msg (0, "MULTI-ROOM %ix%i", mr.w, mr.h);

  xasprintf (&value, "%ix%i", mr.w, mr.h);
  ui_save_setting (key, value);
  al_free (value);

  view_menu ();

  return true;
}

void
ui_show_coordinates (void)
{
  int s = mr.room;
  int l = roomd (&global_level, s, LEFT);
  int r = roomd (&global_level, s, RIGHT);
  int a = roomd (&global_level, s, ABOVE);
  int b = roomd (&global_level, s, BELOW);

  mr.select_cycles = SELECT_CYCLES;

  ui_msg (0, "S%i L%i R%i A%i B%i", s, l, r, a, b);
}

void
ui_show_indirect_coordinates (void)
{
  int a = roomd (&global_level, mr.room, ABOVE);
  int b = roomd (&global_level, mr.room, BELOW);
  int al = roomd (&global_level, a, LEFT);
  int ar = roomd (&global_level, a, RIGHT);
  int bl = roomd (&global_level, b, LEFT);
  int br = roomd (&global_level, b, RIGHT);

  mr.select_cycles = SELECT_CYCLES;

  ui_msg (0, "LV%i AL%i AR%i BL%i BR%i",
          global_level.n, al, ar, bl, br);
}

void
ui_hue_mode (enum hue new_hue)
{
  char *key = "HUE MODE";
  char *value = NULL;

  switch (new_hue) {
  default: /* HUE_ORIGINAL */
    force_hue = false;
    hue = global_level.hue;
    value = "ORIGINAL";
    break;
  case HUE_NONE:
    force_hue = true;
    hue = HUE_NONE;
    value = "NONE";
    break;
  case HUE_GREEN:
    force_hue = true;
    hue = HUE_GREEN;
    value = "GREEN";
    break;
  case HUE_GRAY:
    force_hue = true;
    hue = HUE_GRAY;
    value = "GRAY";
    break;
  case HUE_YELLOW:
    force_hue = true;
    hue = HUE_YELLOW;
    value = "YELLOW";
    break;
  case HUE_BLUE:
    force_hue = true;
    hue = HUE_BLUE;
    value = "BLUE";
    break;
  }

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_gm (enum gm new_gm)
{
  if (replay_mode != NO_REPLAY) {
    print_replay_mode (0);
    return;
  }

  char *key = "GUARD MODE";
  char *value;

  switch (new_gm) {
  case ORIGINAL_GM:
    gm = ORIGINAL_GM;
    value = "ORIGINAL";
    break;
  case GUARD_GM:
    gm = GUARD_GM;
    value = "GUARD";
    break;
  case FAT_GUARD_GM:
    gm = FAT_GUARD_GM;
    value = "FAT-GUARD";
    break;
  case VIZIER_GM:
    gm = VIZIER_GM;
    value = "VIZIER";
    break;
  case SKELETON_GM:
    gm = SKELETON_GM;
    value = "SKELETON";
    break;
  case SHADOW_GM:
    gm = SHADOW_GM;
    value = "SHADOW";
    break;
  }

  /* apply next guard mode */
  int i;
  for (i = 0; i < anima_nmemb; i++) apply_guard_mode (&anima[i], gm);

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_em (enum em new_em)
{
  char *key = "ENVIRONMENT MODE";
  char *value = NULL;

  switch (new_em) {
  default: /* ORIGINAL_EM */
    force_em = false;
    em = global_level.em;
    value = "ORIGINAL";
    break;
  case DUNGEON:
    force_em = true;
    em = DUNGEON;
    value = "DUNGEON";
    break;
  case PALACE:
    force_em = true;
    em = PALACE;
    value = "PALACE";
    break;
  }

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_vm (enum vm new_vm)
{
  char *key = "VIDEO MODE";
  char *value;

  if (new_vm == HGC) {
    vm = CGA; hgc = true;
  } else {
    vm = new_vm; hgc = false;
  }

  switch (vm) {
  case VGA: value = "VGA"; break;
  case EGA: value = "EGA"; break;
  case CGA: value = hgc ? "HGC" : "CGA"; break;
  }

  vm_menu ();

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_flip_screen (int flags, bool correct_mouse, bool save_only)
{
  char *key = "DISPLAY FLIP MODE";
  char *value;

  switch (flags) {
  case 0: value = "NONE"; break;
  case ALLEGRO_FLIP_VERTICAL: value = "VERTICAL"; break;
  case ALLEGRO_FLIP_HORIZONTAL: value = "HORIZONTAL"; break;
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    value = "VERTICAL-HORIZONTAL"; break;
  default: assert (false); break;
  }

  ui_save_setting (key, value);

  if (save_only) return;

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

  ui_msg (0, "%s: %s", key, value);

  ui_flip_gamepad (flip_gamepad_vertical, flip_gamepad_horizontal, true);
  ui_save_setting ("MIRROR MODE", NULL);
  game_menu ();
  view_menu ();
}

void
ui_inhibit_screensaver (bool inhibit)
{
  char *key = "INHIBIT SCREENSAVER";
  char *value = inhibit ? "ON" : "OFF";

  inhibit_screensaver = inhibit;
  al_inhibit_screensaver (inhibit_screensaver);

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (key, value);
  view_menu ();
}

void
ui_room_drawing (bool draw)
{
  char *key = "ROOM DRAWING";
  char *value = draw ? "ON" : "OFF";

  no_room_drawing = ! draw;
  force_full_redraw = true;

  ui_msg (0, "%s: %s", key, value);

  view_menu ();
}

void
ui_gamepad_mode (enum gpm new_gpm)
{
  char *key = "GAMEPAD MODE";
  char *value;

  switch (new_gpm) {
  case KEYBOARD:
    gpm = KEYBOARD;
    ui_msg (0, "KEYBOARD MODE");
    value = "KEYBOARD";
    ui_save_setting (key, value);
    break;
  case JOYSTICK:
    if (calibrate_joystick ()) {
      gpm = JOYSTICK;
      value = "JOYSTICK";
      ui_save_setting (key, value);
      gamepad_rumble (1.0, 0.6);
      ui_msg (0, "JOYSTICK %s",
              gpm == JOYSTICK ? "CALIBRATED" : "MODE");
    } else ui_msg (0, "JOYSTICK NOT FOUND");
    break;
  default: assert (false);
  }

  gamepad_menu ();
}

void
ui_flip_gamepad (bool v, bool h, bool save_only)
{
  char *key = "GAMEPAD FLIP MODE";
  char *value;

  if (! v && ! h) value = "NONE";
  else if (v && ! h) value = "VERTICAL";
  else if (! v && h) value = "HORIZONTAL";
  else if (v && h) value = "VERTICAL-HORIZONTAL";

  ui_save_setting (key, value);

  if (save_only) return;

  flip_gamepad_vertical = v;
  flip_gamepad_horizontal = h;

  ui_msg (0, "%s: %s", key, value);

  ui_flip_screen (screen_flags, false, true);
  ui_save_setting ("MIRROR MODE", NULL);
  game_menu ();
  gamepad_menu ();
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
    ui_msg (2, "RECORDING ABORTED");
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
  f = f < 0 ? 0 : f;
  f = f > UNLIMITED_HZ ? UNLIMITED_HZ : f;
  anim_freq = f;
  al_set_timer_speed (timer, f > 0 ? 1.0 / f : 1.0 / UNLIMITED_HZ);
  if (f > 0) ui_msg (0, "TIME FREQ: %iHz", f);
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

  ui_msg (priority, text);
}

void
ui_undo_pass (struct undo *u, int dir, char *prefix)
{
  if (replay_mode != NO_REPLAY) {
    print_replay_mode (0);
    return;
  }

  char *text;
  char *dir_str = (dir >= 0) ? "REDO" : "UNDO";
  static char *undo_msg = NULL;

  bool b = can_undo (u, dir);

  if (undo_msg) al_free (undo_msg);

  if (! b) {
    if (prefix) xasprintf (&undo_msg, "NO FURTHER %s %s", prefix, dir_str);
    else xasprintf (&undo_msg, "NO FURTHER %s", dir_str);
    editor_msg (undo_msg, EDITOR_CYCLES_3);
    return;
  }

  undo_pass (u, dir, &text);

  if (prefix) xasprintf (&undo_msg, "%s %s: %s", prefix, dir_str, text);
  else xasprintf (&undo_msg, "%s: %s", dir_str, text);
  editor_msg (undo_msg, EDITOR_CYCLES_3);
}






void
ui_resurrect (void)
{
  if (replay_mode == NO_REPLAY) {
    struct anim *k = get_anim_by_id (current_kid_id);
    kid_resurrect (k);
  } else print_replay_mode (0);
}

void
ui_kill_enemy (void)
{
  if (replay_mode == NO_REPLAY) {
    struct anim *k = get_anim_by_id (current_kid_id);
    struct anim *ke = get_anim_by_id (k->enemy_id);
    if (ke) {
      survey (_m, pos, &ke->f, NULL, &ke->p, NULL);
      anim_die (ke);
      play_audio (&guard_hit_audio, NULL, ke->id);
    }
  } else print_replay_mode (0);
}

void
ui_float (void)
{
  if (replay_mode == NO_REPLAY) {
    struct anim *k = get_anim_by_id (current_kid_id);
    float_kid (k);
  } else print_replay_mode (0);
}

void
ui_immortal (bool immortal)
{
  if (replay_mode == NO_REPLAY) {
    char *key = "IMMORTAL MODE";
    char *value = immortal ? "ON" : "OFF";
    struct anim *k = get_anim_by_id (current_kid_id);
    if (k->current_lives <= 0 && immortal) kid_resurrect (k);
    immortal_mode = immortal;
    k->immortal = immortal;
    ui_msg (0, "%s: %s", key, value);
    ui_save_setting (key, value);
    cheat_menu ();
  } else print_replay_mode (0);
}

void
ui_fill_life (void)
{
  if (replay_mode == NO_REPLAY) {
    struct anim *k = get_anim_by_id (current_kid_id);
    increase_kid_current_lives (k);
  } else print_replay_mode (0);
}

void
ui_add_life (void)
{
  if (replay_mode == NO_REPLAY) {
    struct anim *k = get_anim_by_id (current_kid_id);
    increase_kid_total_lives (k);
  }
  else print_replay_mode (0);
}





void
ui_version (void)
{
  ui_msg (0, "MININIM %s", VERSION);
}

void
ui_about_screen (bool value)
{
  if (value) {
    enable_menu (false);
    if (! message_box_thread_id) {
      message_box_thread_id =
        create_thread (message_box_thread, &about_dialog);
      al_start_thread (message_box_thread_id);
    }
    about_screen = true;
    pause_animation (true);
  } else if (! message_box_thread_id) {
    about_screen = false;
    pause_animation (false);
    create_main_menu ();
  }
}

void
ui_next_display_mode (void)
{
  char *key = "DISPLAY MODE";
  char *value;

  if (display_mode < 0)
    ui_msg (0, "DISPLAY MODE: DESKTOP");
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
        al_resize_display (display, d.width, d.height);
        ui_msg (0, "%s: %ix%i", key, d.width, d.height);
        xasprintf (&value, "%i", display_mode);
        ui_save_setting (key, value);
        al_free (value);
      } else ui_msg (0, "DISPLAY MODES QUERYING FAILED");
    } else ui_msg (0, "NO DISPLAY MODE AVAILABLE");
  }
}





error_t
ui_save_setting (char *key, char *value)
{
  ALLEGRO_CONFIG *config = NULL;

  if (al_filename_exists (config_filename))
    config = al_load_config_file (config_filename);
  else if (value) {
    if (! al_make_directory (user_settings_dir))
      return al_get_errno ();
    config = al_create_config ();
  } else return 0;

  if (! config) return al_get_errno ();

  ALLEGRO_CONFIG_ENTRY *iterator;
  char const *entry = al_get_first_config_entry (config, NULL, &iterator);

  while (entry) {
    if (! strcasecmp (entry, key)) {
      al_remove_config_key (config, NULL, entry);
      entry = al_get_first_config_entry (config, NULL, &iterator);
    } else entry = al_get_next_config_entry (&iterator);
  }

  if (value) al_set_config_value (config, NULL, key, value);

  if (! al_save_config_file (config_filename, config)) {
    al_destroy_config (config);
    return al_get_errno ();
  }

  al_destroy_config (config);

  return 0;
}

bool
ui_msg (int priority, const char *template, ...)
{
  va_list ap;
  va_start (ap, template);

  char *text;
  vasprintf (&text, template, ap);
  bool r = draw_bottom_text (NULL, text, priority);
  al_free (text);

  va_end (ap);
  return r;
}

bool
ui_msg_clear (int priority)
{
  return draw_bottom_text (NULL, NULL, priority);
}





bool
in_mirror_mode (void)
{
  return ! flip_gamepad_vertical && flip_gamepad_horizontal
    && screen_flags == ALLEGRO_FLIP_HORIZONTAL;
}

void
mirror_mode (bool mirror)
{
  if (mirror) {
    flip_gamepad_vertical = false;
    flip_gamepad_horizontal = true;
    screen_flags = ALLEGRO_FLIP_HORIZONTAL;
  } else {
    flip_gamepad_vertical = false;
    flip_gamepad_horizontal = false;
    screen_flags = 0;
  }
}

void
ui_mirror_mode (bool mirror)
{
  char *key = "MIRROR MODE";
  char *value = mirror ? "ON" : "OFF";

  mirror_mode (mirror);

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting ("DISPLAY FLIP MODE", NULL);
  ui_save_setting ("GAMEPAD FLIP MODE", NULL);
  ui_save_setting (key, value);
  game_menu ();
  view_menu ();
  gamepad_menu ();
}
