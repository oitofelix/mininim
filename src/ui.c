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

static ALLEGRO_MENU *append_menu[APPEND_MENU_MAX_DEPTH];
static int append_menu_item_index[APPEND_MENU_MAX_DEPTH];
static int append_menu_depth = -1;

static void start_menu (ALLEGRO_MENU *parent, uint16_t id);
static bool vmenu_item (uint16_t id, int flags, ALLEGRO_BITMAP *icon,
                        bool has_submenu, char const *title_template,
                        va_list ap);
bool menu_item (uint16_t id, int flags, ALLEGRO_BITMAP *icon,
                bool has_submenu, char const *title_template, ...)
  __attribute__ ((format (printf, 5, 6)));
static bool menu_hitem (uint16_t id, bool enabled,
                        char const *title_template, ...)
  __attribute__ ((format (printf, 3, 4)));
static bool menu_sitem (uint16_t id, bool enabled, ALLEGRO_BITMAP *icon,
                        char const *title_template, ...)
  __attribute__ ((format (printf, 4, 5)));
static bool menu_ditem (bool first, uint16_t id0, uint16_t id1, bool enabled,
                        ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1,
                        char const *title_template0,
                        char const *title_template1, ...)
  __attribute__ ((format (printf, 8, 9)));
static bool menu_sub (uint16_t id, bool enabled, ALLEGRO_BITMAP *icon,
                      void (*build_f) (intptr_t data), intptr_t data,
                      char const *title_template, ...)
  __attribute__ ((format (printf, 6, 7)));
static bool menu_array (uint16_t id, bool enabled, ALLEGRO_BITMAP *icon,
                        void (*build_f) (uint16_t id, int index, int lower),
                        uint64_t base_id, int id_nmemb, int nmemb, int index,
                        char const *title_template, ...)
  __attribute__ ((format (printf, 9, 10)));
static bool menu_citem (uint16_t id, bool enabled, bool checked,
                        ALLEGRO_BITMAP *icon, char const *title_template, ...)
  __attribute__ ((format (printf, 5, 6)));
static bool menu_sep (char const *title_template, ...)
  __attribute__ ((format (printf, 1, 2)));
static void end_menu (void);



static ALLEGRO_BITMAP *load_icon (char *filename);

static ALLEGRO_BITMAP *volume_icon (float volume);
static void game_menu (intptr_t index);
static void load_menu (intptr_t index);
static void save_menu (intptr_t index);
static void volume_menu (intptr_t index);

static void view_menu (intptr_t index);
static void zoom_menu (intptr_t index);
static ALLEGRO_BITMAP *zoom_fit_icon (enum mr_fit_mode fit);
static void zoom_fit_menu (intptr_t index);
static void zoom_out_menu (intptr_t index);
static void zoom_in_menu (intptr_t index);
static void navigation_menu (intptr_t index);
static void nav_select_menu (intptr_t index);
static void nav_cell_menu (intptr_t index);
static void nav_page_menu (intptr_t index);
static ALLEGRO_BITMAP *hue_icon (enum hue hue);
static void hue_mode_menu (intptr_t index);
static ALLEGRO_BITMAP *gm_icon (enum gm gm);
static void gm_menu (intptr_t index);
static ALLEGRO_BITMAP *em_icon (enum em em);
static void em_menu (intptr_t index);
static ALLEGRO_BITMAP *vm_icon (enum vm vm);
static void vm_menu (intptr_t index);
static ALLEGRO_BITMAP *flip_screen_icon (int flags);
static void screen_flip_menu (intptr_t index);

static void gamepad_menu (intptr_t index);
static ALLEGRO_BITMAP *gamepad_device_icon (enum gpm gpm);
static void gamepad_device_menu (intptr_t index);
static ALLEGRO_BITMAP *flip_gamepad_icon (bool v, bool h);
static void flip_gamepad_menu (intptr_t index);

static void play_menu (intptr_t index);
static void replay_favorite_menu (intptr_t index);
static void replay_favorite_item_menu (intptr_t index);

static void editor_menu (intptr_t index);

static void cheat_menu (intptr_t index);
static void time_change_menu (intptr_t index);
static void kca_change_menu (intptr_t index);
static void kcd_change_menu (intptr_t index);

static void help_menu (intptr_t index);

static void skip_level_widget (void);
static void jump_to_level_menu (uint16_t id, int index, int lower);
static void statistics_widget (void);
static void pause_menu_widget (void);
static void speed_menu_widget (void);

static void ui_load_game (void);
static void ui_load_config (void);
static void ui_save_game (void);
static void ui_audio_volume (float volume);
static void ui_mirror_mode (bool mirror);
static void ui_restart_game (void);
static void ui_start_game (void);
static void ui_quit_game (void);

static void ui_full_screen (void);
static void ui_zoom_fit (enum mr_fit_mode fit);
static bool ui_set_multi_room (int dw, int dh, bool correct_mouse);
static void ui_show_coordinates (void);
static void ui_show_indirect_coordinates (void);
static void ui_hue_mode (enum hue new_hue);
static void ui_gm (enum gm new_gm);
static void ui_em (enum em new_em);
static void ui_vm (enum vm new_vm);
static void ui_flip_screen (int flags, bool correct_mouse, bool save_only);
static void ui_inhibit_screensaver (bool inhibit);
static void ui_room_drawing (bool draw);
static void ui_screenshot (void);

static void ui_flip_gamepad (bool v, bool h, bool save_only);

static void ui_play_replay (void);
static void ui_record_replay (void);
static void ui_save_replay_favorites (void);
static void ui_add_replay_favorite (void);
static void ui_replace_replay_favorite (int i);
static void ui_remove_replay_favorite (int i);
static void ui_jump_to_level (int n);
static void ui_jump_to_level_rel (int d);
static void ui_jump_to_level_menu (int i);
static void ui_time (void);
static void ui_skills (void);
static void ui_decrease_time_frequency (void);
static void ui_increase_time_frequency (void);
static void ui_toggle_time_frequency_constraint (void);
static void ui_change_anim_freq (int f);
static void ui_toggle_pause_game (void);
static void ui_next_frame (void);

static void ui_undo_pass (struct undo *u, int dir, char *prefix);

static void ui_resurrect (void);
static void ui_kill_enemy (void);
static void ui_float (void);
static void ui_immortal (bool immortal);
static void ui_fill_life (void);
static void ui_add_life (void);
static void ui_change_time (int m);
static void ui_change_prob_skill (int *skill, int new);
static void ui_change_kca (int d);
static void ui_change_kcd (int d);

static void ui_version (void);
static void ui_next_display_mode (void);

static void display_skill (struct anim *k);





static ALLEGRO_MENU *main_menu_id;
bool main_menu_enabled;
static int jump_to_level_menu_lower;

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
  *nav_home_icon, *repeat_icon, *compass_icon, *compass2_icon,
  *drawing_icon, *first_icon, *last_icon, *jump_icon, *original_icon,
  *guard_icon, *fat_guard_icon, *vizier_icon, *skeleton_icon, *shadow_icon,
  *resurrect_icon, *death_icon, *feather_icon, *angel_icon, *life_empty_icon,
  *life_full_icon, *skills_icon, *time_icon, *time_add_icon, *time_sub_icon,
  *attack_icon, *attack_add_icon, *attack_sub_icon, *defense_icon,
  *defense_add_icon, *defense_sub_icon, *counter_attack_icon,
  *counter_attack_add_icon, *counter_attack_sub_icon, *counter_defense_icon,
  *counter_defense_add_icon, *counter_defense_sub_icon, *shadow_face_icon,
  *heart_icon, *plus_icon, *minus_icon;




ALLEGRO_BITMAP *
load_icon (char *filename)
{
  if (WINDOWS_PORT) return load_scaled_memory_bitmap (filename, 13, 13, 0);
  else return load_scaled_memory_bitmap (filename, 16, 16, 0);
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
  repeat_icon = load_icon (REPEAT_ICON);
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
  skills_icon = load_icon (SKILLS_ICON);
  time_icon = load_icon (TIME_ICON);
  time_add_icon = load_icon (TIME_ADD_ICON);
  time_sub_icon = load_icon (TIME_SUB_ICON);
  attack_icon = load_icon (ATTACK_ICON);
  attack_add_icon = load_icon (ATTACK_ADD_ICON);
  attack_sub_icon = load_icon (ATTACK_SUB_ICON);
  defense_icon = load_icon (DEFENSE_ICON);
  defense_add_icon = load_icon (DEFENSE_ADD_ICON);
  defense_sub_icon = load_icon (DEFENSE_SUB_ICON);
  counter_attack_icon = load_icon (COUNTER_ATTACK_ICON);
  counter_attack_add_icon = load_icon (COUNTER_ATTACK_ADD_ICON);
  counter_attack_sub_icon = load_icon (COUNTER_ATTACK_SUB_ICON);
  counter_defense_icon = load_icon (COUNTER_DEFENSE_ICON);
  counter_defense_add_icon = load_icon (COUNTER_DEFENSE_ADD_ICON);
  counter_defense_sub_icon = load_icon (COUNTER_DEFENSE_SUB_ICON);
  shadow_face_icon = load_icon (SHADOW_FACE_ICON);
  heart_icon = load_icon (HEART_ICON);
  plus_icon = load_icon (PLUS_ICON);
  minus_icon = load_icon (MINUS_ICON);
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
  destroy_bitmap (repeat_icon);
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
  destroy_bitmap (skills_icon);
  destroy_bitmap (time_icon);
  destroy_bitmap (time_add_icon);
  destroy_bitmap (time_sub_icon);
  destroy_bitmap (attack_icon);
  destroy_bitmap (attack_add_icon);
  destroy_bitmap (attack_sub_icon);
  destroy_bitmap (defense_icon);
  destroy_bitmap (defense_add_icon);
  destroy_bitmap (defense_sub_icon);
  destroy_bitmap (counter_attack_icon);
  destroy_bitmap (counter_attack_add_icon);
  destroy_bitmap (counter_attack_sub_icon);
  destroy_bitmap (counter_defense_icon);
  destroy_bitmap (counter_defense_add_icon);
  destroy_bitmap (counter_defense_sub_icon);
  destroy_bitmap (shadow_face_icon);
  destroy_bitmap (heart_icon);
  destroy_bitmap (plus_icon);
  destroy_bitmap (minus_icon);
}





void
start_menu (ALLEGRO_MENU *parent, uint16_t id)
{
  append_menu_depth++;
  assert (append_menu_depth < APPEND_MENU_MAX_DEPTH);
  if (id > 0) append_menu[append_menu_depth] = al_find_menu (parent, id);
  else append_menu[append_menu_depth] = parent;
  append_menu_item_index[append_menu_depth] = 0;
}

bool
vmenu_item (uint16_t id, int flags, ALLEGRO_BITMAP *icon,
            bool has_submenu, char const *title_template, va_list ap)
{
  ALLEGRO_MENU **am = &append_menu[append_menu_depth];
  int *am_i = &append_menu_item_index[append_menu_depth];

  if (! *am) return false;

  char *title = NULL;
  if (title_template) vasprintf (&title, title_template, ap);

  ALLEGRO_MENU *submenu = al_find_menu (*am, id);;

  if (has_submenu && ! submenu) {
    submenu = al_create_menu ();
    goto replace;
  } else if (! has_submenu && submenu) {
    submenu = NULL;
    goto replace;
  }

  ALLEGRO_MENU *f_menu;
  int f_index;

  if (al_find_menu_item (*am, id, &f_menu, &f_index)) {
    if (f_menu != *am || f_index != *am_i)
      goto replace;

    int cflags = al_get_menu_item_flags (*am, -*am_i);
    const char *ctitle = al_get_menu_item_caption (*am, -*am_i);
    ALLEGRO_BITMAP *cicon = al_get_menu_item_icon (*am, -*am_i);

    /* work around an Allegro bug in which items turned into
       (non-)checkbox by only changing their flags puts them in
       inconsistent state */
    if ((cflags & ~ALLEGRO_MENU_ITEM_DISABLED)
        != (flags & ~ALLEGRO_MENU_ITEM_DISABLED)) goto replace;

    /* update flags */
    if (cflags != flags) {
      /* fprintf (stderr, "changed flags: %i, %s (%i) -> %s (%i)\n", */
      /*          id, ctitle, cflags, title, flags); */
      al_set_menu_item_flags (*am, -*am_i, flags);
    }

    /* update caption */
    if ((ctitle && ! title) || (! ctitle && title)
        || (ctitle && title && strcmp (ctitle, title))) {
      /* fprintf (stderr, "changed caption: %i, %s -> %s\n", id, ctitle, title); */
      al_set_menu_item_caption (*am, -*am_i, title);
    }

    if (title_template) al_free (title);

    /* update icon */
    if ((cicon && ! icon) || (! cicon && icon)
        || (cicon && icon && ! bitmap_heq (cicon, icon))) {
      /* fprintf (stderr, "changed icon: %i, %s -> %s\n", id, ctitle, title); */
      al_set_menu_item_icon (*am, -*am_i, clone_memory_bitmap (icon));
    }


    (*am_i)++;

    return true;
  }

 replace:
  /* fprintf (stderr, "replaced menu item: %i, %s\n", id, title); */
  al_remove_menu_item (*am, -*am_i);
  bool r = al_insert_menu_item (*am, -((*am_i)++), title,
                                id, flags, clone_memory_bitmap (icon),
                                submenu);
  if (title_template) al_free (title);
  return r;
}

bool
menu_item (uint16_t id, int flags, ALLEGRO_BITMAP *icon,
           bool has_submenu, char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  bool r = vmenu_item (id, flags, icon, has_submenu, title_template, ap);
  va_end (ap);
  return r;
}

bool
menu_hitem (uint16_t id, bool enabled, char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  bool r = vmenu_item (id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                       heading_icon, false, title_template, ap);
  va_end (ap);
  return r;
}

bool
menu_sitem (uint16_t id, bool enabled, ALLEGRO_BITMAP *icon,
            char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  bool r = vmenu_item (id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                       icon, false, title_template, ap);
  va_end (ap);
  return r;
}

bool
menu_ditem (bool first, uint16_t id0, uint16_t id1, bool enabled,
            ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1,
            char const *title_template0, char const *title_template1, ...)
{
  va_list ap;
  va_start (ap, title_template1);
  bool r = vmenu_item (first ? id0 : id1,
                       enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                       first ? icon0 : icon1, false,
                       first ? title_template0 : title_template1,
                       ap);
  va_end (ap);
  return r;
}

bool
menu_sub (uint16_t id, bool enabled, ALLEGRO_BITMAP *icon,
          void (*build_f) (intptr_t data), intptr_t data,
          char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  bool r = vmenu_item (id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                       icon, true, title_template, ap);
  va_end (ap);
  if (enabled && build_f && r) {
    start_menu (append_menu[append_menu_depth], id);
    build_f (data);
    end_menu ();
  }
  return r;
}

bool
menu_array (uint16_t id, bool enabled, ALLEGRO_BITMAP *icon,
            void (*build_f) (uint16_t id, int index, int lower),
            uint64_t base_id, int id_nmemb, int nmemb, int index,
            char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  bool r = vmenu_item (id, enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                       icon, true, title_template, ap);
  va_end (ap);
  if (enabled && build_f && r) {
    start_menu (append_menu[append_menu_depth], id);

    int lower;
    if (index <= id_nmemb / 2) lower = 0;
    else if (index >= nmemb - id_nmemb / 2)
      lower = nmemb - min_int (nmemb, id_nmemb);
    else lower = index - id_nmemb / 2;

    if (lower > 0) menu_sep ("...");

    int i;
    for (i = 0; i < min_int (nmemb, id_nmemb); i++)
      build_f (base_id + i, lower + i, lower);

    if (lower < nmemb - min_int (nmemb, id_nmemb)) menu_sep ("...");

    end_menu ();
  }
  return r;
}

bool
menu_citem (uint16_t id, bool enabled, bool checked, ALLEGRO_BITMAP *icon,
            char const *title_template, ...)
{
  int flags = enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED;
  flags |= checked
    ? ALLEGRO_MENU_ITEM_CHECKBOX | ALLEGRO_MENU_ITEM_CHECKED
    : 0;

  va_list ap;
  va_start (ap, title_template);
  bool r = vmenu_item (id, flags, checked ? NULL : icon, false,
                       title_template, ap);
  va_end (ap);
  return r;
}

bool
menu_sep (char const *title_template, ...)
{
  int i = 0;
  bool r;

  va_list ap;
  va_start (ap, title_template);

  ALLEGRO_MENU *menu;
  int index;
  while (al_find_menu_item (main_menu_id, SEP_MID + i, &menu, &index)
         && (menu != append_menu[append_menu_depth]
             || index != append_menu_item_index[append_menu_depth]))
    i++;

  assert (i < SEP_MID_NMEMB);

  if (WINDOWS_PORT)
    r = vmenu_item (SEP_MID + i,
                    title_template ? ALLEGRO_MENU_ITEM_DISABLED : 0,
                    NULL, false, title_template, ap);
  else
    r = vmenu_item (SEP_MID + i, ALLEGRO_MENU_ITEM_DISABLED, NULL, false,
                    title_template ? title_template : "",
                    title_template ? ap : NULL);

  va_end (ap);
  return r;
}

void
end_menu (void)
{
  assert (append_menu_depth >= 0);
  int am_d = append_menu_depth--;
  if (! append_menu[am_d]) return;
  while (al_remove_menu_item (append_menu[am_d],
                              -append_menu_item_index[am_d]));
}





void
show_menu (void)
{
  if (display_mode < 0) {
    al_set_display_menu (display, main_menu_id);
    al_show_mouse_cursor (display);
  }
}

void
hide_menu (void)
{
  if (is_fullscreen ()) {
    al_remove_display_menu (display);
    al_hide_mouse_cursor (display);
  }
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
  return al_get_display_menu (display) ? true : false;
}





void
main_menu (void)
{
  if (! main_menu_id) main_menu_id = al_create_menu ();
  start_menu (main_menu_id, 0);

  menu_sub (GAME_MID, main_menu_enabled, NULL, game_menu, 0, "&Game");

  menu_sub (VIEW_MID, main_menu_enabled, NULL, view_menu, 0, "&View");

  menu_sub (GAMEPAD_MID, main_menu_enabled, NULL, gamepad_menu, 0, "&Input");

  menu_sub (PLAY_MID, main_menu_enabled, NULL, play_menu, 0, "&Play");

  menu_sub (EDITOR_MID, main_menu_enabled && can_edit (),
            NULL, editor_menu, 0, "&Editor");

  menu_sub (CHEAT_MID, main_menu_enabled && can_edit (),
            NULL, cheat_menu, 0, "&Cheat");

  menu_sub (HELP_MID, main_menu_enabled, NULL, help_menu, 0, "He&lp");

  end_menu ();
}

ALLEGRO_BITMAP *
volume_icon (float volume)
{
  if (volume == VOLUME_RANGE_MIN) return volume_off_icon;
  else if (volume < VOLUME_RANGE_LOW) return volume_low_icon;
  else if (volume < VOLUME_RANGE_MEDIUM) return volume_medium_icon;
  else if (volume <= VOLUME_RANGE_MAX) return volume_high_icon;
  else assert (false);
  return NULL;
}

void
game_menu (intptr_t index)
{
  menu_sub (LOAD_MID, true, open_icon, load_menu, 0, "&Load");

  menu_sub (SAVE_MID, true, save_icon,  save_menu, 0, "&Save");

  menu_sep (NULL);

  menu_sub (VOLUME_MID, true, volume_icon (audio_volume),
            volume_menu, 0, "&Volume (Ctrl+S)");

  menu_citem (MIRROR_MODE_MID, true, in_mirror_mode (), horizontal_icon,
              "&Mirror");

  menu_sep (NULL);

  menu_ditem ((cutscene || title_demo) && play_time < time_limit,
              START_GAME_MID, RESTART_GAME_MID, true,
              right_icon, reload_icon,
              "Sta&rt (Enter)", "&Restart (Ctrl+R)");

  menu_sitem (QUIT_GAME_MID, true, quit_icon, "&Quit (Ctrl+Q)");
}

void
load_menu (intptr_t index)
{
  menu_sitem (LOAD_GAME_MID, true, game_icon, "&Game... (Ctrl+L)");

  menu_sitem (LOAD_CONFIG_MID, true, settings_icon,
              "&Configuration... (Ctrl+L)");

  menu_sitem (LOAD_LEVEL_FILE_MID, false, NULL, "&Level file...");
}

void
save_menu (intptr_t index)
{
  menu_sitem (SAVE_GAME_MID,
              ! cutscene && ! title_demo && replay_mode == NO_REPLAY,
              game_icon, "&Game... (Ctrl+G)");

  menu_sitem (SAVE_CONFIG_MID, false, settings_icon,
              "&Configuration...");

  menu_sitem (SAVE_LEVEL_FILE_MID, false, NULL, "&Level file...");
}

void
volume_menu (intptr_t index)
{
  menu_citem (VOLUME_OFF_MID, true,
              audio_volume == VOLUME_RANGE_MIN,
              volume_off_icon, "&Off");

  menu_citem (VOLUME_LOW_MID, true,
              audio_volume > VOLUME_RANGE_MIN
              && audio_volume < VOLUME_RANGE_LOW,
              volume_low_icon, "&Low");

  menu_citem (VOLUME_MEDIUM_MID, true,
              audio_volume >= VOLUME_RANGE_LOW
              && audio_volume < VOLUME_RANGE_MEDIUM,
              volume_medium_icon, "&Medium");

  menu_citem (VOLUME_HIGH_MID, true,
              audio_volume >= VOLUME_RANGE_MEDIUM
              && audio_volume <= VOLUME_RANGE_MAX,
              volume_high_icon, "&High");
}

void
view_menu (intptr_t index)
{
  menu_ditem (is_fullscreen (), FULL_SCREEN_MID, FULL_SCREEN_MID, true,
              windows_icon, full_screen_icon,
              "&Windowed (F)", "&Fullscreen (F)");

  menu_sub (ZOOM_MID, ! cutscene && ! title_demo, zoom_icon,
            zoom_menu, 0, "&Zoom");

  menu_sub (NAV_MID, ! cutscene && ! title_demo, navigation_icon,
            navigation_menu, 0, "&Navigation");

  menu_sep (NULL);

  menu_sub (VM_MID, true, vm_icon (vm), vm_menu, 0, "&Video (F12)");

  menu_sub (EM_MID, ! cutscene && ! title_demo,
            force_em ? em_icon (em) : original_icon,
            em_menu, 0, "&Environment (F11)");

  menu_sub (HUE_MODE_MID, ! cutscene && ! title_demo,
            force_hue ? hue_icon (hue) : original_icon,
            hue_mode_menu, 0, "&Hue (Alt+F11)");

  menu_sub (GM_MID, ! cutscene && ! title_demo, gm_icon (gm),
            gm_menu, 0, "&Guard (Shift+F11)");

  menu_sub (FLIP_SCREEN_MID, true, flip_screen_icon (screen_flags),
            screen_flip_menu, 0, "&Flip (Shift+I)");

  menu_sep (NULL);

  menu_citem (ROOM_DRAWING_MID, ! cutscene && ! title_demo,
              ! no_room_drawing, drawing_icon,
              "Room &drawing (Shift+B)");

  menu_citem (INHIBIT_SCREENSAVER_MID, true,
              inhibit_screensaver, screensaver_icon,
              "&Inhibit screensaver");

  menu_sep (NULL);

  menu_sitem (SCREENSHOT_MID, true, camera_icon, "&Screenshot... (Ctrl+P)");
}

void
zoom_menu (intptr_t index)
{
  menu_hitem (ZOOM_RESET_MID, ! cutscene && ! title_demo
              && (mr.w != 2 || mr.h != 2
                  || mr.fit_mode != MR_FIT_NONE),
              "MULTI-ROOM %ix%i", mr.w, mr.h);

  menu_sub (ZOOM_FIT_MID, ! cutscene && ! title_demo,
            zoom_fit_icon (mr.fit_mode), zoom_fit_menu, 0, "&Fit (M)");

  menu_sub (ZOOM_IN_MID,
            ! cutscene && ! title_demo && (mr.w > 1 || mr.h > 1),
            zoom_in_icon, zoom_in_menu, 0, "&In");

  menu_sub (ZOOM_OUT_MID, ! cutscene && ! title_demo, zoom_out_icon,
            zoom_out_menu, 0, "&Out");
}

ALLEGRO_BITMAP *
zoom_fit_icon (enum mr_fit_mode fit)
{
  switch (fit) {
  case MR_FIT_NONE: return zoom_none_icon; break;
  case MR_FIT_STRETCH: return zoom_stretch_icon; break;
  case MR_FIT_RATIO: return zoom_ratio_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
zoom_fit_menu (intptr_t index)
{
  menu_citem (ZOOM_NONE_MID, true,
              mr.fit_mode == MR_FIT_NONE, zoom_none_icon,
              "&None");

  menu_citem (ZOOM_STRETCH_MID, true,
              mr.fit_mode == MR_FIT_STRETCH, zoom_stretch_icon,
              "&Stretch");

  menu_citem (ZOOM_RATIO_MID, true,
              mr.fit_mode == MR_FIT_RATIO, zoom_ratio_icon,
              "&Ratio");
}

void
zoom_out_menu (intptr_t index)
{
  menu_sitem (ZOOM_OUT_BOTH_MID, true, vertical_horizontal_icon,
              "&Both (])");

  menu_sitem (ZOOM_OUT_VERTICAL_MID, true, vertical_icon,
              "&Vertical (Alt+])");

  menu_sitem (ZOOM_OUT_HORIZONTAL_MID, true, horizontal_icon,
              "&Horizontal (Ctrl+])");
}

void
zoom_in_menu (intptr_t index)
{
  menu_sitem (ZOOM_IN_BOTH_MID, mr.w > 1 && mr.h > 1,
              vertical_horizontal_icon, "&Both ([)");

  menu_sitem (ZOOM_IN_VERTICAL_MID, mr.h > 1, vertical_icon,
              "&Vertical (Alt+[)");

  menu_sitem (ZOOM_IN_HORIZONTAL_MID, mr.w > 1,
              horizontal_icon, "&Horizontal (Ctrl+[)");
}

void
navigation_menu (intptr_t index)
{
  menu_hitem (NAV_CURRENT_ROOM_SELECT_MID, true, "ROOM %i", mr.room);

  menu_sub (NAV_SELECT_MID, true, nav_select_icon,
            nav_select_menu, 0, "&Selection");

  menu_sub (NAV_CELL_MID, true, nav_cell_icon, nav_cell_menu, 0, "Scroll &row");

  menu_sub (NAV_PAGE_MID, true, nav_page_icon, nav_page_menu, 0, "Scroll &page");

  struct anim *k = get_anim_by_id (current_kid_id);
  menu_sitem (NAV_HOME_MID, k && k->f.c.room != mr.room,
              nav_home_icon, "&Home (Home)");

  menu_sitem (NAV_CENTER_MID, true, repeat_icon, "Cen&ter (Shift+Home)");

  menu_sitem (COORDINATES_MID, true, compass_icon, "C&oordinates (C)");

  menu_sitem (INDIRECT_COORDINATES_MID, true, compass2_icon,
              "&Indirect coordinates (Shift+C)");
}

void
nav_select_menu (intptr_t index)
{
  menu_sitem (NAV_SELECT_LEFT_MID,
              roomd (&global_level, mr.room, LEFT),
              nav_left_icon, "&Left (H)");

  menu_sitem (NAV_SELECT_ABOVE_MID,
              roomd (&global_level, mr.room, ABOVE),
              nav_above_icon, "&Above (U)");

  menu_sitem (NAV_SELECT_RIGHT_MID,
              roomd (&global_level, mr.room, RIGHT),
              nav_right_icon, "&Right (J)");

  menu_sitem (NAV_SELECT_BELOW_MID,
              roomd (&global_level, mr.room, BELOW),
              nav_below_icon, "&Below (N)");
}

void
nav_cell_menu (intptr_t index)
{
  menu_sitem (NAV_CELL_LEFT_MID, true,
              nav_left_icon, "&Left (Shift+H)");

  menu_sitem (NAV_CELL_ABOVE_MID, true,
              nav_above_icon, "&Above (Shift+U)");

  menu_sitem (NAV_CELL_RIGHT_MID, true,
              nav_right_icon, "&Right (Shift+J)");

  menu_sitem (NAV_CELL_BELOW_MID, true,
              nav_below_icon, "&Below (Shift+N)");
}

void
nav_page_menu (intptr_t index)
{
  menu_sitem (NAV_PAGE_LEFT_MID, true,
              nav_left_icon, "&Left (Alt+H)");

  menu_sitem (NAV_PAGE_ABOVE_MID, true,
              nav_above_icon, "&Above (Alt+U)");

  menu_sitem (NAV_PAGE_RIGHT_MID, true,
              nav_right_icon, "&Right (Alt+J)");

  menu_sitem (NAV_PAGE_BELOW_MID, true,
              nav_below_icon, "&Below (Alt+N)");
}

ALLEGRO_BITMAP *
hue_icon (enum hue hue)
{
  switch (hue) {
  case HUE_NONE: return black_icon; break;
  case HUE_GREEN: return green_icon; break;
  case HUE_GRAY: return gray_icon; break;
  case HUE_YELLOW: return yellow_icon; break;
  case HUE_BLUE: return blue_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
hue_mode_menu (intptr_t index)
{
  menu_citem (HUE_ORIGINAL_MID, true, ! force_hue,
              original_icon, "&Original");

  menu_citem (HUE_NONE_MID, true, force_hue && hue == HUE_NONE,
              black_icon, "&None");

  menu_citem (HUE_GREEN_MID, true, force_hue && hue == HUE_GREEN,
              green_icon, "&Green");

  menu_citem (HUE_GRAY_MID, true, force_hue && hue == HUE_GRAY,
              gray_icon, "G&ray");

  menu_citem (HUE_YELLOW_MID, true, force_hue && hue == HUE_YELLOW,
              yellow_icon, "&Yellow");

  menu_citem (HUE_BLUE_MID, true, force_hue && hue == HUE_BLUE,
              blue_icon, "&Blue");
}

ALLEGRO_BITMAP *
gm_icon (enum gm gm)
{
  switch (gm) {
  case ORIGINAL_GM: return original_icon; break;
  case GUARD_GM: return guard_icon; break;
  case FAT_GUARD_GM: return fat_guard_icon; break;
  case VIZIER_GM: return vizier_icon; break;
  case SKELETON_GM: return skeleton_icon; break;
  case SHADOW_GM: return shadow_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
gm_menu (intptr_t index)
{
  menu_citem (ORIGINAL_GM_MID, true, gm == ORIGINAL_GM,
              gm_icon (ORIGINAL_GM), "&Original");

  menu_citem (GUARD_GM_MID, true, gm == GUARD_GM,
              gm_icon (GUARD_GM), "&Guard");

  menu_citem (FAT_GUARD_GM_MID, true, gm == FAT_GUARD_GM,
              gm_icon (FAT_GUARD_GM), "&Fat guard");

  menu_citem (VIZIER_GM_MID, true, gm == VIZIER_GM,
              gm_icon (VIZIER_GM), "&Vizier");

  menu_citem (SKELETON_GM_MID, true, gm == SKELETON_GM,
              gm_icon (SKELETON_GM), "&Skeleton");

  menu_citem (SHADOW_GM_MID, true, gm == SHADOW_GM,
              gm_icon (SHADOW_GM), "S&hadow");
}

ALLEGRO_BITMAP *
em_icon (enum em em)
{
  switch (em) {
  case DUNGEON: return dungeon_icon; break;
  case PALACE: return palace_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
em_menu (intptr_t index)
{
  menu_citem (ORIGINAL_EM_MID, true, ! force_em, original_icon,
              "&Original");

  menu_citem (DUNGEON_MID, true, force_em && em == DUNGEON, dungeon_icon,
              "&Dungeon");

  menu_citem (PALACE_MID, true, force_em && em == PALACE, palace_icon,
              "&Palace");
}

ALLEGRO_BITMAP *
vm_icon (enum vm vm)
{
  switch (vm) {
  case VGA: return vga_icon; break;
  case EGA: return ega_icon; break;
  case CGA: return hgc ? hgc_icon : cga_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
vm_menu (intptr_t index)
{
  menu_citem (VGA_MID, true, vm == VGA, vga_icon, "&VGA");

  menu_citem (EGA_MID, true, vm == EGA, ega_icon, "&EGA");

  menu_citem (CGA_MID, true, vm == CGA && ! hgc, cga_icon, "&CGA");

  menu_citem (HGC_MID, true, vm == CGA && hgc, hgc_icon, "&HGC");
}

ALLEGRO_BITMAP *
flip_screen_icon (int flags)
{
  switch (flags) {
  case 0: return screen_icon;
  case ALLEGRO_FLIP_VERTICAL: return vertical_icon;
  case ALLEGRO_FLIP_HORIZONTAL: return horizontal_icon;
  case ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL:
    return vertical_horizontal_icon;
  default: assert (false); return NULL;
  }
}

void
screen_flip_menu (intptr_t index)
{
  menu_citem (FLIP_SCREEN_VERTICAL_MID, true,
              screen_flags & ALLEGRO_FLIP_VERTICAL,
              vertical_icon, "&Vertical");

  menu_citem (FLIP_SCREEN_HORIZONTAL_MID, true,
              screen_flags & ALLEGRO_FLIP_HORIZONTAL,
              horizontal_icon, "&Horizontal");
}

void
gamepad_menu (intptr_t index)
{
  menu_sub (GAMEPAD_DEVICE_MID, true, gamepad_device_icon (gpm),
            gamepad_device_menu, 0, "&Device");

  menu_sitem (GAMEPAD_CALIBRATE_MID, gpm == JOYSTICK, clock_icon,
              "&Calibrate (Ctrl+J)");

  menu_sep (NULL);

  menu_sub (FLIP_GAMEPAD_MID, true,
            flip_gamepad_icon (flip_gamepad_vertical,
                               flip_gamepad_horizontal),
            flip_gamepad_menu, 0, "&Flip (Shift+K)");
}

ALLEGRO_BITMAP *
gamepad_device_icon (enum gpm gpm)
{
  switch (gpm) {
  case KEYBOARD: return keyboard_icon; break;
  case JOYSTICK: return joystick_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
gamepad_device_menu (intptr_t index)
{
  menu_citem (KEYBOARD_MODE_MID, true, gpm == KEYBOARD, keyboard_icon,
              "&Keyboard (Ctrl+K)");

  menu_citem (JOYSTICK_MODE_MID, joystick, gpm == JOYSTICK, joystick_icon,
              "&Joystick (Ctrl+J)");
}

ALLEGRO_BITMAP *
flip_gamepad_icon (bool v, bool h)
{
  if (! v && ! h) return joystick3_icon;
  else if (v && ! h) return vertical_icon;
  else if (! v && h) return horizontal_icon;
  else if (v && h) return vertical_horizontal_icon;
  else assert (false);
  return NULL;
}

void
flip_gamepad_menu (intptr_t index)
{
  menu_citem (FLIP_GAMEPAD_VERTICAL_MID, true, flip_gamepad_vertical,
              vertical_icon, "&Vertical");

  menu_citem (FLIP_GAMEPAD_HORIZONTAL_MID, true, flip_gamepad_horizontal,
              horizontal_icon, "&Horizontal");
}

void
play_menu (intptr_t index)
{
  if (recording_replay_countdown >= 0
      || level_start_replay_mode == RECORD_REPLAY)
    goto record_replay;

  if (title_demo) goto no_replay;

  switch (replay_mode) {
  case PLAY_REPLAY:
    menu_hitem (REPLAYING_HEADER_MID, false, "REPLAYING (%zu/%zu)",
                replay_index + 1, replay_chain_nmemb);

    menu_sitem (PLAY_REPLAY_MID, true, stop_icon, "&Stop (F7)");

    menu_sub (REPLAY_FAVORITE_MID, ! title_demo,
              heart_icon, replay_favorite_menu, 0, "Fa&vorites");

    skip_level_widget ();

    pause_menu_widget ();

    speed_menu_widget ();

    statistics_widget ();

    break;
  case RECORD_REPLAY: record_replay:

    menu_hitem (RECORDING_HEADER_MID, false, "RECORDING");

    menu_sitem
      (RECORD_REPLAY_MID, true,
       replay_mode == RECORD_REPLAY
       ? stop_icon : eject_icon,
       recording_replay_countdown >= 0
       ? "&Abort recording (F7)" : "&Stop... (F7)");

    pause_menu_widget ();

    statistics_widget ();

    break;
  default: assert (false);
  case NO_REPLAY: no_replay:

    menu_sitem (PLAY_REPLAY_MID, true, play_icon, "Re&play... (F7)");

    menu_sitem (RECORD_REPLAY_MID, true, record_icon,
                "&Record... (Alt+F7)");

    menu_sub (REPLAY_FAVORITE_MID,
              replay_favorite_nmemb > 0,
              heart_icon, replay_favorite_menu, 0, "Fa&vorites");

    skip_level_widget ();

    pause_menu_widget ();

    speed_menu_widget ();

    statistics_widget ();

    break;
  }
}

void
replay_favorite_menu (intptr_t index)
{
  menu_sitem (ADD_REPLAY_FAVORITE_MID,
              ! title_demo && replay_mode == PLAY_REPLAY,
              plus_icon, "&Add");

  size_t i;
  int m = min_int (replay_favorite_nmemb, REPLAY_FAVORITE_MID_NMEMB);
  for (i = 0; i < m; i++) {
    char *filename =
      shorten_str (replay_favorite[i].filename, MENU_CAPTION_MAX_CHARS);
    menu_sub (REPLAY_FAVORITE_0_MID + i, true, play_icon,
              replay_favorite_item_menu, i, "%s: %ju", filename,
              replay_favorite[i].cycle);
    al_free (filename);
  }
}

void
replay_favorite_item_menu (intptr_t index)
{
  menu_sitem (GO_TO_REPLAY_FAVORITE_0_MID + index,
              true, right_icon, "&Go");
  menu_sitem (REPLACE_REPLAY_FAVORITE_0_MID + index,
              ! title_demo && replay_mode == PLAY_REPLAY,
              jump_icon, "&Replace");
  menu_sitem (REMOVE_REPLAY_FAVORITE_0_MID + index, true,
              minus_icon, "Re&move");
}

void
editor_menu (intptr_t index)
{
  menu_ditem (edit == EDIT_NONE,
              EDIT_MODE_MID, EDIT_MODE_MID,
              true, edit_icon, joystick2_icon,
              "&Edit (F8)", "&Play (F8)");

  menu_sitem (UNDO_MID, can_undo (&undo, -1), undo_icon,
              "&Undo (Ctrl+Z)");

  menu_sitem (REDO_MID, can_undo (&undo, +1), redo_icon,
              "&Redo (Ctrl+Y)");
}

void
cheat_menu (intptr_t index)
{
  struct anim *k = get_anim_by_id (current_kid_id);

  menu_sitem (KILL_ENEMY_MID, k && k->enemy_id > 0,
              death_icon, "&Kill enemy (K)");

  menu_sitem (RESURRECT_MID, k && k->current_lives <= 0,
              resurrect_icon, "&Resurrect (R)");

  menu_citem (IMMORTAL_MID, true, immortal_mode, angel_icon,
              "&Immortal (I)");

  menu_sitem (FLOAT_MID, k && (k->current_lives > 0 || is_kid_fall (&k->f))
              && (k->float_timer == 0
                  || k->float_timer >= REFLOAT_MENU_THRESHOLD),
              feather_icon, "&Float (Shift+W)");

  menu_sitem (FILL_LIFE_MID, k && k->current_lives > 0
              && k->current_lives < k->total_lives,
              life_empty_icon, "Fill &single container (Shift+S)");

  menu_sitem (ADD_LIFE_MID,
              k && k->current_lives > 0
              && (k->current_lives < k->total_lives
                  || k->total_lives < MAX_LIVES),
              life_full_icon,
              (! k || k->total_lives < MAX_LIVES)
              ? "&Add container (Shift+T)"
              : "Fill &all containers (Shift+T)");

  menu_sub (TIME_CHANGE_MID, true, time_icon, time_change_menu, 0, "&Time");
  menu_sub (KCA_CHANGE_MID, true, counter_attack_icon,
            kca_change_menu, 0, "Counter a&ttack");
  menu_sub (KCD_CHANGE_MID, true, counter_defense_icon,
            kcd_change_menu, 0, "Counter &defense");
}

void
time_change_menu (intptr_t index)
{
  menu_sitem (TIME_ADD_MID, true, time_add_icon, "&Increase (=)");

  menu_sitem (TIME_SUB_MID, time_limit - play_time > 60 * DEFAULT_HZ,
              time_sub_icon, "&Decrease (-)");
}

void
kca_change_menu (intptr_t index)
{
  struct anim *k = get_anim_by_id (current_kid_id);

  menu_sitem (KCA_ADD_MID, k && k->skill.counter_attack_prob < 99,
              counter_attack_add_icon, "&Increase (Ctrl+=)");

  menu_sitem (KCA_SUB_MID, k && k->skill.counter_attack_prob > -1,
              counter_attack_sub_icon, "&Decrease (Ctrl+-)");
}

void
kcd_change_menu (intptr_t index)
{
  struct anim *k = get_anim_by_id (current_kid_id);

  menu_sitem (KCD_ADD_MID, k && k->skill.counter_defense_prob < 99,
              counter_defense_add_icon, "&Increase (Alt+=)");

  menu_sitem (KCD_SUB_MID, k && k->skill.counter_defense_prob > -1,
              counter_defense_sub_icon, "&Decrease (Alt+-)");
}

void
help_menu (intptr_t index)
{
  menu_sitem (ABOUT_MID, true, small_logo_icon, "&About");
}

void
skip_level_widget (void)
{
  if (cutscene || title_demo)
    menu_array (JUMP_TO_LEVEL_MID, true, heading_icon, jump_to_level_menu,
                JUMP_TO_LEVEL_1_MID, JUMP_TO_LEVEL_MID_NMEMB,
                14, 0, "LEVEL");
  else menu_array (JUMP_TO_LEVEL_MID, true, heading_icon, jump_to_level_menu,
                   JUMP_TO_LEVEL_1_MID, JUMP_TO_LEVEL_MID_NMEMB,
                   replay_mode == PLAY_REPLAY ? replay_chain_nmemb : 14,
                   replay_mode == PLAY_REPLAY
                   ? replay_index : global_level.n - 1,
                   "LEVEL %i", global_level.n);

  menu_sitem (RESTART_LEVEL_MID, replay_mode == PLAY_REPLAY && ! title_demo
              ? true
              : ! cutscene && ! title_demo,
              repeat_icon, "R&epeat (Ctrl+A)");

  menu_sitem
    (NEXT_LEVEL_MID, replay_mode == PLAY_REPLAY && ! title_demo
     ? replay_index + 1 < replay_chain_nmemb
     : ! cutscene && ! title_demo && global_level.n < 14,
     next_icon, "&Next (Shift+L)");

  menu_sitem
    (PREVIOUS_LEVEL_MID, replay_mode == PLAY_REPLAY && ! title_demo
     ? replay_index > 0
     : ! cutscene && ! title_demo && global_level.n > 1,
     previous_icon, "Pre&vious (Shift+M)");
}

void
jump_to_level_menu (uint16_t id, int index, int lower)
{
  jump_to_level_menu_lower = lower;
  if (replay_mode == PLAY_REPLAY)
    menu_citem (id, true, replay_index == index,
                NULL, "%i", replay_chain[index].start_level);
  else menu_citem (id, true, global_level.n == index + 1,
                   NULL, "%i", index + 1);
}

void
statistics_widget (void)
{
  menu_sep (NULL);

  menu_sitem
    (TIME_MID, ! cutscene && ! title_demo && recording_replay_countdown < 0,
     time_icon, "Ti&me (Space)");

  menu_sitem
    (SKILLS_MID, ! cutscene && ! title_demo && recording_replay_countdown < 0,
     skills_icon, "S&kills (Tab)");
}

void
pause_menu_widget (void)
{
  if (is_game_paused ())
    menu_hitem (CYCLE_HEADER_MID, false, "CYCLE: %ju", anim_cycle);
  else menu_sep (NULL);

  menu_sitem
    (TOGGLE_PAUSE_GAME_MID,
     ! cutscene && ! title_demo && recording_replay_countdown < 0,
     is_game_paused () ? play_icon : pause_icon,
     is_game_paused () ? "&Continue" : "Pau&se (Esc)");

  menu_sitem
    (NEXT_FRAME_MID,
     is_game_paused () && ! cutscene && ! title_demo
     && recording_replay_countdown < 0,
     forward_icon, "Next &frame (Esc)");
}

void
speed_menu_widget (void)
{
  if (! cutscene && ! title_demo && anim_freq > 0)
    menu_hitem (RESET_TIME_FREQ_MID, anim_freq != DEFAULT_HZ,
                "FREQ: %iHz", anim_freq);
  else menu_sep (NULL);

  menu_citem
    (TOGGLE_TIME_FREQ_CONSTRAINT_MID,
     ! cutscene && ! title_demo, anim_freq > 0, cancel_icon,
     "Frequency cons&traint");

  menu_sitem
    (DECREASE_TIME_FREQ_MID, anim_freq > 2 && ! cutscene && ! title_demo,
     backward_icon, "&Decrease frequecy [(]");

  menu_sitem
    (INCREASE_TIME_FREQ_MID, anim_freq != 0 && ! cutscene && ! title_demo,
     forward_icon, "&Increase frequecy [)]");
}






void
menu_mid (ALLEGRO_EVENT *event)
{
  /********/
  /* MENU */
  /********/

  uint16_t mid = event->user.data1;

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


    /* PLAY */
  case PLAY_REPLAY_MID:
    ui_play_replay ();
    break;
  case RECORD_REPLAY_MID:
    ui_record_replay ();
    break;
  case ADD_REPLAY_FAVORITE_MID:
    ui_add_replay_favorite ();
    break;
  case RESTART_LEVEL_MID:
    ui_jump_to_level_rel (+0);
    break;
  case PREVIOUS_LEVEL_MID:
    ui_jump_to_level_rel (-1);
    break;
  case NEXT_LEVEL_MID:
    ui_jump_to_level_rel (+1);
    break;
  case TIME_MID:
    ui_time ();
    break;
  case SKILLS_MID:
    ui_skills ();
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
  case TIME_ADD_MID:
    ui_change_time (+10);
    break;
  case TIME_SUB_MID:
    ui_change_time (-10);
    break;
  case KCA_ADD_MID:
    ui_change_kca (+10);
    break;
  case KCA_SUB_MID:
    ui_change_kca (-10);
    break;
  case KCD_ADD_MID:
    ui_change_kcd (+10);
    break;
  case KCD_SUB_MID:
    ui_change_kcd (-10);
    break;



    /* HELP */
  case ABOUT_MID:
    ui_about_screen (true);
    break;



    /* jump to level N */
  case JUMP_TO_LEVEL_1_MID ...
    JUMP_TO_LEVEL_1_MID + JUMP_TO_LEVEL_MID_NMEMB - 1:
    ui_jump_to_level_menu (mid - JUMP_TO_LEVEL_1_MID);
    break;



    /* replay favorites */
  case GO_TO_REPLAY_FAVORITE_0_MID ...
    GO_TO_REPLAY_FAVORITE_0_MID + REPLAY_FAVORITE_MID_NMEMB - 1:
    ui_go_to_replay_favorite (mid - GO_TO_REPLAY_FAVORITE_0_MID);
    break;

  case REPLACE_REPLAY_FAVORITE_0_MID ...
    REPLACE_REPLAY_FAVORITE_0_MID + REPLAY_FAVORITE_MID_NMEMB - 1:
    ui_replace_replay_favorite (mid - REPLACE_REPLAY_FAVORITE_0_MID);
    break;

  case REMOVE_REPLAY_FAVORITE_0_MID ...
    REMOVE_REPLAY_FAVORITE_0_MID + REPLAY_FAVORITE_MID_NMEMB - 1:
    ui_remove_replay_favorite (mid - REMOVE_REPLAY_FAVORITE_0_MID);
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
    ui_jump_to_level_rel (+0);

  /* SHIFT+L: warp to next level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_L))
    ui_jump_to_level_rel (+1);

  /* SHIFT+M: warp to previous level */
  else if (was_key_pressed (ALLEGRO_KEYMOD_SHIFT, ALLEGRO_KEY_M))
    ui_jump_to_level_rel (-1);

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
    ui_time ();

  /* TAB: display skill */
  else if (was_key_pressed (0, ALLEGRO_KEY_TAB))
    ui_skills ();

  /* +: increase time limit and display remaining time */
  else if (! active_menu
           && (was_key_pressed (0, ALLEGRO_KEY_EQUALS)
               || was_key_pressed (ALLEGRO_KEYMOD_SHIFT,
                                   ALLEGRO_KEY_EQUALS)))
    ui_change_time (+1);

  /* -: decrease time limit and display remaining time */
  else if (! active_menu
           && (was_key_pressed (0, ALLEGRO_KEY_MINUS)
               || was_char_pressed ('_')))
    ui_change_time (-1);

  /* CTRL+=: increase counter attack skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_EQUALS))
    ui_change_kca (+1);

  /* CTRL+-: decrease counter attack skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_CTRL, ALLEGRO_KEY_MINUS))
    ui_change_kca (-1);

  /* ALT+=: increase counter defense skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_EQUALS))
    ui_change_kcd (+1);

  /* ALT+-: decrease counter defense skill */
  else if (was_key_pressed (ALLEGRO_KEYMOD_ALT, ALLEGRO_KEY_MINUS))
    ui_change_kcd (-1);

  /* ESC: pause game */
  if (step_cycle > 0) {
    game_paused = false;
    step_cycle--;
  } else if (step_cycle == 0) {
    game_paused = true;
    step_cycle = -1;
  }

  if (was_key_pressed (0, ALLEGRO_KEY_ESCAPE)
      || was_joystick_button_pressed (joystick_pause_button)) {
    if (is_game_paused ()) {
      step_cycle = 0;
      game_paused = false;
    } else pause_game (true);
  } else if (is_game_paused ()
             && (! active_menu || ! was_bmenu_key_pressed ())
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
}

void
ui_load_game (void)
{
  load_config_dialog_thread =
    al_create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_load_config (void)
{
  load_config_dialog_thread =
    al_create_thread (dialog_thread, &load_config_dialog);
  al_start_thread (load_config_dialog_thread);
}

void
ui_save_game (void)
{
  save_game_dialog_thread =
    al_create_thread (dialog_thread, &save_game_dialog);
  al_start_thread (save_game_dialog_thread);
  pause_animation (true);
}

void
ui_audio_volume (float volume)
{
  char *key = "SOUND GAIN";
  char *value;
  value = xasprintf ("%.1f", volume);

  set_audio_volume (volume);

  char *status = NULL;
  if (volume == VOLUME_RANGE_MIN) status = "OFF";
  else if (volume < VOLUME_RANGE_LOW) status = "LOW";
  else if (volume < VOLUME_RANGE_MEDIUM) status = "MEDIUM";
  else if (volume <= VOLUME_RANGE_MAX) status = "HIGH";

  ui_msg (0, "SOUND %s", status);

  ui_save_setting (NULL, key, value);
  al_free (value);
}

void
ui_screenshot (void)
{
  save_picture_dialog_thread =
    al_create_thread (dialog_thread, &save_picture_dialog);
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
      al_show_mouse_cursor (display);
      show_menu ();
    } else {
      al_set_display_flag (display, ALLEGRO_FULLSCREEN_WINDOW, true);
      value = "ON";
      if (edit == EDIT_NONE) hide_menu ();
    }

    ui_msg (0, "%s: %s", key, value);

    ui_save_setting (NULL, key, value);
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

  ui_save_setting (NULL, key, value);
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

  value = xasprintf ("%ix%i", mr.w, mr.h);
  ui_save_setting (NULL, key, value);
  al_free (value);

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

  ui_save_setting (NULL, key, value);
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

  ui_save_setting (NULL, key, value);
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

  ui_save_setting (NULL, key, value);
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
  case VGA: value = "VGA"; video_mode = "VGA"; break;
  case EGA: value = "EGA"; video_mode = "EGA"; break;
  case CGA: value = hgc ? "HGC" : "CGA"; video_mode = "CGA"; break;
  }

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (NULL, key, value);
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

  ui_save_setting (NULL, key, value);

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
  ui_save_setting (NULL, "MIRROR MODE", NULL);
}

void
ui_inhibit_screensaver (bool inhibit)
{
  char *key = "INHIBIT SCREENSAVER";
  char *value = inhibit ? "ON" : "OFF";

  inhibit_screensaver = inhibit;
  al_inhibit_screensaver (inhibit_screensaver);

  ui_msg (0, "%s: %s", key, value);

  ui_save_setting (NULL, key, value);
}

void
ui_room_drawing (bool draw)
{
  char *key = "ROOM DRAWING";
  char *value = draw ? "ON" : "OFF";

  no_room_drawing = ! draw;
  force_full_redraw = true;

  ui_msg (0, "%s: %s", key, value);
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
    ui_save_setting (NULL, key, value);
    break;
  case JOYSTICK:
    if (calibrate_joystick ()) {
      enum gpm old_gpm = gpm;
      gpm = JOYSTICK;
      value = "JOYSTICK";
      ui_save_setting (NULL, key, value);
      gamepad_rumble (1.0, 0.6);
      ui_msg (0, "JOYSTICK %s",
              old_gpm == JOYSTICK ? "CALIBRATED" : "MODE");
    } else ui_msg (0, "JOYSTICK NOT FOUND");
    break;
  default: assert (false);
  }
}

void
ui_flip_gamepad (bool v, bool h, bool save_only)
{
  char *key = "GAMEPAD FLIP MODE";
  char *value = NULL;

  if (! v && ! h) value = "NONE";
  else if (v && ! h) value = "VERTICAL";
  else if (! v && h) value = "HORIZONTAL";
  else if (v && h) value = "VERTICAL-HORIZONTAL";

  ui_save_setting (NULL, key, value);

  if (save_only) return;

  flip_gamepad_vertical = v;
  flip_gamepad_horizontal = h;

  ui_msg (0, "%s: %s", key, value);

  ui_flip_screen (screen_flags, false, true);
  ui_save_setting (NULL, "MIRROR MODE", NULL);
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
  }
}

void
ui_jump_to_level (int n)
{
  switch (replay_mode) {
  case NO_REPLAY:
    if (n == global_level.n) {
      quit_anim = RESTART_LEVEL;
    } else {
      ignore_level_cutscene = true;
      next_level_number = n;
      start_level_time = play_time;
      quit_anim = NEXT_LEVEL;
    }
    break;
  case PLAY_REPLAY:
    if (n == replay_index)
      quit_anim = REPLAY_RESTART_LEVEL;
    else if (0 <= n && n < replay_chain_nmemb) {
      replay_next_number = n;
      quit_anim = REPLAY_NEXT;
    } else ui_msg (0, "%s", n < (int) replay_index
                   ? "NO PREVIOUS REPLAY" : "NO NEXT REPLAY");
    break;
  default: print_replay_mode (0); break;
  }
}

void
ui_jump_to_level_rel (int d)
{
  ui_jump_to_level
    (replay_mode == PLAY_REPLAY ? replay_index + d : global_level.n + d);
}

void
ui_jump_to_level_menu (int i)
{
  if (title_demo) goto no_replay;

  int n;
  switch (replay_mode) {
  case NO_REPLAY: no_replay:
    n = i + 1 + jump_to_level_menu_lower;
    if (n == global_level.n) return;
    break;
  case PLAY_REPLAY:
    n = i + jump_to_level_menu_lower;
    if (n == replay_index) return;
    break;
  default: return;
  }
  ui_jump_to_level (n);
}

void
ui_time (void)
{
  display_remaining_time (0);
}

void
ui_skills (void)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  display_skill (k);
}

void
ui_increase_time_frequency (void)
{
  if (anim_freq >= DEFAULT_HZ)
    ui_change_anim_freq (next_multiple (anim_freq, DEFAULT_HZ));
  else ui_change_anim_freq (next_multiple (anim_freq, 2));
}

void
ui_decrease_time_frequency (void)
{
  if (anim_freq > DEFAULT_HZ)
    ui_change_anim_freq (next_multiple (anim_freq, -DEFAULT_HZ));
  else if (anim_freq > 2)
    ui_change_anim_freq (next_multiple (anim_freq, -2));
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
  change_anim_freq (f);
  if (f > 0) ui_msg (0, "TIME FREQ: %iHz", f);
}

void
ui_toggle_pause_game (void)
{
  if (is_game_paused ()) pause_game (false);
  else pause_game (true);
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

  ui_msg (priority, "%s", text);
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
    if (prefix) undo_msg = xasprintf ("NO FURTHER %s %s", prefix, dir_str);
    else undo_msg = xasprintf ("NO FURTHER %s", dir_str);
    editor_msg (undo_msg, EDITOR_CYCLES_3);
    return;
  }

  undo_pass (u, dir, &text);

  if (prefix) undo_msg = xasprintf ("%s %s: %s", prefix, dir_str, text);
  else undo_msg = xasprintf ("%s: %s", dir_str, text);
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
    ui_save_setting (NULL, key, value);
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
    total_lives = k->total_lives;
  } else print_replay_mode (0);
}

void
ui_change_time (int m)
{
  if (replay_mode == NO_REPLAY) {
    static int last_r_min = -1;

    if (last_r_min < 0)
      last_r_min = precise_unit (time_limit - play_time, 60 * DEFAULT_HZ);

    int64_t r = time_limit - play_time;
    if (m < 0 && r <= 60 * DEFAULT_HZ) return;
    else if (m > 0 && r <= 60 * DEFAULT_HZ)
      time_limit = play_time + max_int (2, m) * 60 * DEFAULT_HZ;
    else if (m < 0 && r <= -m * 60 * DEFAULT_HZ)
      time_limit = play_time + 60 * DEFAULT_HZ;
    else time_limit = play_time + next_multiple (r, m * 60 * DEFAULT_HZ);

    int r_min = precise_unit (time_limit - play_time, 60 * DEFAULT_HZ);
    if (r_min == last_r_min) {
      time_limit += m * 60 * DEFAULT_HZ;
      last_r_min = precise_unit (time_limit - play_time, 60 * DEFAULT_HZ);
    } else last_r_min = r_min;

    display_remaining_time (0);
  } else print_replay_mode (0);
}

void
ui_change_prob_skill (int *holder, int new)
{
  if (replay_mode == NO_REPLAY) {
    struct anim *k = get_anim_by_id (current_kid_id);
    new = new > -1 ? new : -1;
    new = new < 99 ? new : 99;
    *holder = new;
    display_skill (k);
    skill = k->skill;
  } else print_replay_mode (0);
}

void
ui_change_kca (int d)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  ui_change_prob_skill
    (&k->skill.counter_attack_prob,
     next_multiple (k->skill.counter_attack_prob + 1, d) - 1);
}

void
ui_change_kcd (int d)
{
  struct anim *k = get_anim_by_id (current_kid_id);
  ui_change_prob_skill
    (&k->skill.counter_defense_prob,
     next_multiple (k->skill.counter_defense_prob + 1, d) - 1);
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
    main_menu_enabled = false;
    if (! message_box_thread_id) {
      message_box_thread_id =
        al_create_thread (message_box_thread, &about_dialog);
      al_start_thread (message_box_thread_id);
    }
    about_screen = true;
    pause_animation (true);
  } else if (! message_box_thread_id) {
    main_menu_enabled = true;
    about_screen = false;
    pause_animation (false);
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
        value = xasprintf ("%i", display_mode);
        ui_save_setting (NULL, key, value);
        al_free (value);
      } else ui_msg (0, "DISPLAY MODES QUERYING FAILED");
    } else ui_msg (0, "NO DISPLAY MODE AVAILABLE");
  }
}





void
ui_save_replay_favorites (void)
{
  size_t i;
  int m = min_int (replay_favorite_nmemb, REPLAY_FAVORITE_MID_NMEMB);
  for (i = 0; i < m; i++) {
    char *section = xasprintf ("REPLAY FAVORITE %zu", i);

    char *key = "FILE";
    char *value = xasprintf ("%s", replay_favorite[i].filename);
    ui_save_setting (section, key, value);
    al_free (value);

    key = "CYCLE";
    value = xasprintf ("%ju", replay_favorite[i].cycle);
    ui_save_setting (section, key, value);
    al_free (value);

    al_free (section);
  }

  char *section = NULL;
  do {
    if (section) al_free (section);
    section = xasprintf ("REPLAY FAVORITE %zu", i++);
  } while (ui_save_setting (section, NULL, NULL));
  al_free (section);
}

void
ui_load_replay_favorites (void)
{
  free_replay_favorites ();
  size_t i = 0;

  while (i < REPLAY_FAVORITE_MID_NMEMB) {
    char *section = xasprintf ("REPLAY FAVORITE %zu", i);

    char *file_str = ui_get_setting (section, "FILE");
    char *cycle_str = ui_get_setting (section, "CYCLE");

    al_free (section);

    if (! file_str || ! cycle_str) {
      if (file_str) al_free (file_str);
      if (cycle_str) al_free (cycle_str);
      break;
    }

    uint64_t cycle;
    if (sscanf (cycle_str, "%zu", &cycle) != 1) break;
    add_replay_favorite (file_str, cycle);

    al_free (file_str);
    al_free (cycle_str);

    i++;
  }
}

void
ui_add_replay_favorite (void)
{
  assert (replay_mode == PLAY_REPLAY);
  size_t i = 0;
  for (i = 0; i < replay_favorite_nmemb; i++)
    if (replay_favorite[i].cycle == anim_cycle
        && ! strcmp (replay_favorite[i].filename,
                     replay_chain[replay_index].filename)) {
      ui_msg (0, "DUPLICATE REPLAY FAVORITE");
      return;
    }
  if (replay_favorite_nmemb < REPLAY_FAVORITE_MID_NMEMB) {
    add_current_replay_favorite ();
    ui_save_replay_favorites ();
    ui_msg (0, "REPLAY FAVORITE ADDED");
  } else ui_msg (0, "NO FAVORITE FREE REPLAY SLOT");
}

void
ui_go_to_replay_favorite (int i)
{
  if (replay_mode != NO_REPLAY) stop_replaying (0);
  free_replay_chain ();
  add_replay_file_to_replay_chain (replay_favorite[i].filename);
  replay_favorite_cycle = replay_favorite[i].cycle;
  anim_freq = 0;
  prepare_for_playing_replay (0);
}

void
ui_replace_replay_favorite (int i)
{
  assert (replay_mode == PLAY_REPLAY);
  al_free (replay_favorite[i].filename);
  replay_favorite[i].filename =
    xasprintf ("%s", replay_chain[replay_index].filename);
  replay_favorite[i].cycle = anim_cycle;
  ui_save_replay_favorites ();
  ui_msg (0, "REPLAY FAVORITE REPLACED");
}

void
ui_remove_replay_favorite (int i)
{
  remove_replay_favorite (i);
  ui_save_replay_favorites ();
  ui_msg (0, "REPLAY FAVORITE REMOVED");
}




bool
ui_save_setting (char *section, char *key, char *value)
{
  ALLEGRO_CONFIG *config = NULL;

  if (al_filename_exists (config_filename)) {
    config = al_load_config_file (config_filename);
    if (! config) goto error;
    const char *file_type_str =
      al_get_config_value (config, NULL, "FILE TYPE");
    if (! file_type_str
        || strcasecmp (file_type_str, "MININIM CONFIGURATION")) {
      al_destroy_config (config);
      config = al_create_config ();
    }
  } else if (value) {
    if (! al_make_directory (user_settings_dir)) goto error;
    config = al_create_config ();
  } else return false;

  if (! config) goto error;

  remove_config_entry (config, NULL, "FILE TYPE");
  al_set_config_value (config, NULL, "FILE TYPE", "MININIM CONFIGURATION");

  bool existing;
  if (key) {
    existing = remove_config_entry (config, section, key);
    if (value) al_set_config_value (config, section, key, value);
  } else existing = remove_config_section (config, section);

  if (! al_save_config_file (config_filename, config)) goto error;

  al_destroy_config (config);

  return existing;

 error:
  if (config) al_destroy_config (config);
  error (0, al_get_errno (), "can't save setting '[%s] %s=%s' to '%s'",
         section, key, value, config_filename);
  return false;
}

char *
ui_get_setting (char *section, char *key)
{
  ALLEGRO_CONFIG *config = NULL;

  config = al_load_config_file (config_filename);
  if (! config) goto error;
  const char *file_type_str =
    al_get_config_value (config, NULL, "FILE TYPE");
  if (! file_type_str
      || strcasecmp (file_type_str, "MININIM CONFIGURATION"))
    goto error;

  const char *value = al_get_config_value (config, section, key);
  if (value) value = xasprintf ("%s", value);

  al_destroy_config (config);

  return (char *) value;

 error:
  if (config) al_destroy_config (config);
  error (0, al_get_errno (), "can't get setting '[%s] %s' from '%s'",
         section, key, config_filename);
  return NULL;
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

  ui_save_setting (NULL, "DISPLAY FLIP MODE", NULL);
  ui_save_setting (NULL, "GAMEPAD FLIP MODE", NULL);
  ui_save_setting (NULL, key, value);
}




bool
display_remaining_time (int priority)
{
  int t = time_limit - play_time;
  if (t < 0) t = 0;
  int tm = t > (60 * DEFAULT_HZ)
    ? precise_unit (t, 60 * DEFAULT_HZ)
    : precise_unit (t, 1 * DEFAULT_HZ);
  return ui_msg (priority, "%i %s", tm,
                 t > (60 * DEFAULT_HZ)
                 ? "MINUTES LEFT"
                 : "SECONDS LEFT");
}

void
display_skill (struct anim *k)
{
  struct anim *ke = get_anim_by_id (k->enemy_id);
  if (ke)
    ui_msg (0, "KCA%i KCD%i A%i CA%i D%i CD%i",
            k->skill.counter_attack_prob + 1,
            k->skill.counter_defense_prob + 1,
            ke->skill.attack_prob + 1,
            ke->skill.counter_attack_prob + 1,
            ke->skill.defense_prob + 1,
            ke->skill.counter_defense_prob + 1);
  else
    ui_msg (0, "KCA%i KCD%i",
            k->skill.counter_attack_prob + 1,
            k->skill.counter_defense_prob + 1);
}
