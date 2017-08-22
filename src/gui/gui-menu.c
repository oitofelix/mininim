/*
  gui-menu.c -- graphical user interface menu module;

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
static uint16_t append_menu_item_index[APPEND_MENU_MAX_DEPTH];
static uint16_t append_menu_id;
static int append_menu_depth = -1;

static void start_menu (ALLEGRO_MENU *parent, uint16_t base_id);
static uint16_t get_menu_item_id (ALLEGRO_MENU *parent, int index,
                                  uint16_t start_id);
static uint16_t vmenu_item (int flags, ALLEGRO_BITMAP *icon,
                            ALLEGRO_MENU **submenu,
                            char const *title_template, va_list ap);

/* static uint16_t *menu_item (int flags, ALLEGRO_BITMAP *icon, bool has_submenu, */
/*                             char const *title_template, ...) */
/*   __attribute__ ((format (printf, 4, 5))); */

static uint16_t menu_hitem (bool enabled, char const *title_template, ...)
  __attribute__ ((format (printf, 2, 3)));

static uint16_t menu_sitem (bool enabled, ALLEGRO_BITMAP *icon,
                            char const *title_template, ...)
  __attribute__ ((format (printf, 3, 4)));

static void menu_ditem (bool first, uint16_t *id0, uint16_t *id1,
                        bool enabled, ALLEGRO_BITMAP *icon0,
                        ALLEGRO_BITMAP *icon1,
                        char const *title_template0,
                        char const *title_template1, ...)
  __attribute__ ((format (printf, 8, 9)));

static uint16_t menu_sub (ALLEGRO_MENU **submenu, bool enabled,
                          ALLEGRO_BITMAP *icon,
                          void (*build_f) (intptr_t data), intptr_t data,
                          char const *title_template, ...)
  __attribute__ ((format (printf, 6, 7)));

static uint16_t menu_array (ALLEGRO_MENU **submenu,
                            bool enabled, ALLEGRO_BITMAP *icon,
                            void (*build_f) (int index, int lower),
                            int id_nmemb, int nmemb, int index,
                            char const *title_template, ...)
  __attribute__ ((format (printf, 8, 9)));

static uint16_t menu_citem (bool enabled, bool checked, ALLEGRO_BITMAP *icon,
                            char const *title_template, ...)
  __attribute__ ((format (printf, 4, 5)));

static void menu_sep (char const *title_template, ...)
  __attribute__ ((format (printf, 1, 2)));

static void end_menu (void);



static void process_main_menu_event (ALLEGRO_EVENT *event);
static void process_aux_menu_event (ALLEGRO_EVENT *event);

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
static ALLEGRO_BITMAP *vm_icon (void);
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
static void jump_to_level_menu (int index, int lower);
static void statistics_widget (void);
static void pause_menu_widget (void);
static void speed_menu_widget (void);


static struct {
  /* main display menu */
  struct {
    ALLEGRO_MENU *m;
    ALLEGRO_EVENT_SOURCE *event_source;
    struct {
      ALLEGRO_MENU *m;
      struct {
        ALLEGRO_MENU *m;
      } load;
      struct {
        ALLEGRO_MENU *m;
      } save;
      struct {
        ALLEGRO_MENU *m;
      } volume;
    } game;
    struct {
      ALLEGRO_MENU *m;
      struct {
        ALLEGRO_MENU *m;
        struct {
          ALLEGRO_MENU *m;
        } fit;
        struct {
          ALLEGRO_MENU *m;
        } in;
        struct {
          ALLEGRO_MENU *m;
        } out;
      } zoom;
      struct {
        ALLEGRO_MENU *m;
        struct {
          ALLEGRO_MENU *m;
        } select;
        struct {
          ALLEGRO_MENU *m;
        } cell;
        struct {
          ALLEGRO_MENU *m;
        } page;
      } nav;
      struct {
        ALLEGRO_MENU *m;
        /* TODO: implement */
      } vm;
      struct {
        ALLEGRO_MENU *m;
      } em;
      struct {
        ALLEGRO_MENU *m;
      } hue;
      struct {
        ALLEGRO_MENU *m;
      } gm;
      struct {
        ALLEGRO_MENU *m;
      } flip;
    } view;
    struct {
      ALLEGRO_MENU *m;
      struct {
        ALLEGRO_MENU *m;
      } device;
      struct {
        ALLEGRO_MENU *m;
      } flip;
    } gamepad;
    struct {
      ALLEGRO_MENU *m;
      struct {
        ALLEGRO_MENU *m;
      } replay_favorite;
      struct {
        ALLEGRO_MENU *m;
      } jump_to_level;
    } play;
    struct {
      ALLEGRO_MENU *m;
    } editor;
    struct {
      ALLEGRO_MENU *m;
      struct {
        ALLEGRO_MENU *m;
      } time;
      struct {
        ALLEGRO_MENU *m;
      } kca;
      struct {
        ALLEGRO_MENU *m;
      } kcd;
    } cheat;
    struct {
      ALLEGRO_MENU *m;
    } help;
  } main;

  /* auxiliary popup menu */
  struct {
    ALLEGRO_MENU *m;
    ALLEGRO_EVENT_SOURCE *event_source;
  } aux;
} menu;

static struct {
  /* main display menu */
  struct {
    struct {
      struct {
        uint16_t game, config, level;
      } load;
      struct {
        uint16_t game, config, level;
      } save;
      struct {
        uint16_t off, low, medium, high;
      } volume;
      uint16_t mirror, start, restart, quit;
    } game;
    struct {
      struct {
        struct {
          uint16_t none, stretch, ratio;
        } fit;
        struct {
          uint16_t both, vertical, horizontal;
        } in;
        struct {
          uint16_t both, vertical, horizontal;
        } out;
        uint16_t reset;
      } zoom;
      struct {
        struct {
          uint16_t left, above, right, below;
        } select;
        struct {
          uint16_t left, above, right, below;
        } cell;
        struct {
          uint16_t left, above, right, below;
        } page;
        uint16_t current_room,
          home, center, coord, ind_coord;
      } nav;
      struct {
        /* TODO: implement */
      } vm;
      struct {
        uint16_t original, dungeon, palace;
      } em;
      struct {
        uint16_t original, none, green, gray, yellow, blue;
      } hue;
      struct {
        uint16_t original, guard, fat, vizier, skeleton, shadow;
      } gm;
      struct {
        uint16_t vertical, horizontal;
      } flip;
      uint16_t windowed, full_screen, room_drawing,
        inhibit_screensaver, screenshot;
    } view;
    struct {
      struct {
        uint16_t keyboard, joystick;
      } device;
      struct {
        uint16_t vertical, horizontal;
      } flip;
      uint16_t calibrate;
    } gamepad;
    struct {
      struct {
        uint16_t add;
      } replay_favorite;
      struct {
        uint16_t _id;
      } jump_to_level;
      uint16_t play_replay, record_replay,
        restart_level, next_level, previous_level,
        time, skills, pause_game, next_frame,
        reset_time_freq, toggle_time_freq_constraint,
        decrease_time_freq, increase_time_freq;
    } play;
    struct {
      uint16_t editor_mode, play_mode, undo, redo;
    } editor;
    struct {
      struct {
        uint16_t add, sub;
      } time;
      struct {
        uint16_t add, sub;
      } kca;
      struct {
        uint16_t add, sub;
      } kcd;
      uint16_t kill_enemy, resurrect, immortal,
        kid_float, fill_hp, add_hp;
    } cheat;
    struct {
      uint16_t about;
    } help;
    uint16_t unlock_selection;
  } main;

  /* auxiliary popup menu */
  struct {
    uint16_t lock_selection, unlock_selection;
  } aux;
} item;

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
  *guard_icon, *fat_icon, *vizier_icon, *skeleton_icon, *shadow_icon,
  *resurrect_icon, *death_icon, *feather_icon, *angel_icon, *hp_empty_icon,
  *hp_full_icon, *skills_icon, *time_icon, *time_add_icon, *time_sub_icon,
  *attack_icon, *attack_add_icon, *attack_sub_icon, *defense_icon,
  *defense_add_icon, *defense_sub_icon, *counter_attack_icon,
  *counter_attack_add_icon, *counter_attack_sub_icon, *counter_defense_icon,
  *counter_defense_add_icon, *counter_defense_sub_icon, *shadow_face_icon,
  *heart_icon, *plus_icon, *minus_icon, *lock_icon, *unlock_icon;




ALLEGRO_BITMAP *
load_icon (char *filename)
{
  ALLEGRO_BITMAP *b;
  int w, h;

#if WINDOWS_PORT
  w = h = 13;
#else
  w = h = 16;
#endif

  b = load_scaled_memory_bitmap (filename, w, h, 0);

  if (! b) error (0, 0, "Cannot load icon \"%s\"", filename);

  return b;
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
  nav_cell_icon = load_icon (NAV_ROW_ICON);
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
  fat_icon = load_icon (FAT_ICON);
  vizier_icon = load_icon (VIZIER_ICON);
  skeleton_icon = load_icon (SKELETON_ICON);
  shadow_icon = load_icon (SHADOW_ICON);
  resurrect_icon = load_icon (RESURRECT_ICON);
  death_icon = load_icon (DEATH_ICON);
  feather_icon = load_icon (FEATHER_ICON);
  angel_icon = load_icon (ANGEL_ICON);
  hp_empty_icon = load_icon (HP_EMPTY_ICON);
  hp_full_icon = load_icon (HP_FULL_ICON);
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
  lock_icon = load_icon (LOCK_ICON);
  unlock_icon = load_icon (UNLOCK_ICON);
}

void
unload_icons (void)
{
  al_destroy_bitmap (small_logo_icon);
  al_destroy_bitmap (open_icon);
  al_destroy_bitmap (save_icon);
  al_destroy_bitmap (reload_icon);
  al_destroy_bitmap (quit_icon);
  al_destroy_bitmap (full_screen_icon);
  al_destroy_bitmap (windows_icon);
  al_destroy_bitmap (camera_icon);
  al_destroy_bitmap (play_icon);
  al_destroy_bitmap (record_icon);
  al_destroy_bitmap (stop_icon);
  al_destroy_bitmap (eject_icon);
  al_destroy_bitmap (backward_icon);
  al_destroy_bitmap (forward_icon);
  al_destroy_bitmap (pause_icon);
  al_destroy_bitmap (previous_icon);
  al_destroy_bitmap (next_icon);
  al_destroy_bitmap (screen_icon);
  al_destroy_bitmap (right_icon);
  al_destroy_bitmap (dungeon_icon);
  al_destroy_bitmap (palace_icon);
  al_destroy_bitmap (green_icon);
  al_destroy_bitmap (gray_icon);
  al_destroy_bitmap (yellow_icon);
  al_destroy_bitmap (blue_icon);
  al_destroy_bitmap (black_icon);
  al_destroy_bitmap (vga_icon);
  al_destroy_bitmap (ega_icon);
  al_destroy_bitmap (cga_icon);
  al_destroy_bitmap (hgc_icon);
  al_destroy_bitmap (vertical_icon);
  al_destroy_bitmap (horizontal_icon);
  al_destroy_bitmap (keyboard_icon);
  al_destroy_bitmap (joystick_icon);
  al_destroy_bitmap (cancel_icon);
  al_destroy_bitmap (clock_icon);
  al_destroy_bitmap (edit_icon);
  al_destroy_bitmap (joystick2_icon);
  al_destroy_bitmap (undo_icon);
  al_destroy_bitmap (redo_icon);
  al_destroy_bitmap (screensaver_icon);
  al_destroy_bitmap (joystick3_icon);
  al_destroy_bitmap (volume_off_icon);
  al_destroy_bitmap (volume_low_icon);
  al_destroy_bitmap (volume_medium_icon);
  al_destroy_bitmap (volume_high_icon);
  al_destroy_bitmap (game_icon);
  al_destroy_bitmap (settings_icon);
  al_destroy_bitmap (zoom_none_icon);
  al_destroy_bitmap (zoom_stretch_icon);
  al_destroy_bitmap (zoom_ratio_icon);
  al_destroy_bitmap (vertical_horizontal_icon);
  al_destroy_bitmap (zoom_out_icon);
  al_destroy_bitmap (zoom_in_icon);
  al_destroy_bitmap (heading_icon);
  al_destroy_bitmap (zoom_icon);
  al_destroy_bitmap (navigation_icon);
  al_destroy_bitmap (nav_select_icon);
  al_destroy_bitmap (nav_cell_icon);
  al_destroy_bitmap (nav_page_icon);
  al_destroy_bitmap (nav_left_icon);
  al_destroy_bitmap (nav_right_icon);
  al_destroy_bitmap (nav_above_icon);
  al_destroy_bitmap (nav_below_icon);
  al_destroy_bitmap (nav_home_icon);
  al_destroy_bitmap (repeat_icon);
  al_destroy_bitmap (compass_icon);
  al_destroy_bitmap (compass2_icon);
  al_destroy_bitmap (drawing_icon);
  al_destroy_bitmap (first_icon);
  al_destroy_bitmap (last_icon);
  al_destroy_bitmap (jump_icon);
  al_destroy_bitmap (original_icon);
  al_destroy_bitmap (guard_icon);
  al_destroy_bitmap (fat_icon);
  al_destroy_bitmap (vizier_icon);
  al_destroy_bitmap (skeleton_icon);
  al_destroy_bitmap (shadow_icon);
  al_destroy_bitmap (resurrect_icon);
  al_destroy_bitmap (death_icon);
  al_destroy_bitmap (feather_icon);
  al_destroy_bitmap (angel_icon);
  al_destroy_bitmap (hp_empty_icon);
  al_destroy_bitmap (hp_full_icon);
  al_destroy_bitmap (skills_icon);
  al_destroy_bitmap (time_icon);
  al_destroy_bitmap (time_add_icon);
  al_destroy_bitmap (time_sub_icon);
  al_destroy_bitmap (attack_icon);
  al_destroy_bitmap (attack_add_icon);
  al_destroy_bitmap (attack_sub_icon);
  al_destroy_bitmap (defense_icon);
  al_destroy_bitmap (defense_add_icon);
  al_destroy_bitmap (defense_sub_icon);
  al_destroy_bitmap (counter_attack_icon);
  al_destroy_bitmap (counter_attack_add_icon);
  al_destroy_bitmap (counter_attack_sub_icon);
  al_destroy_bitmap (counter_defense_icon);
  al_destroy_bitmap (counter_defense_add_icon);
  al_destroy_bitmap (counter_defense_sub_icon);
  al_destroy_bitmap (shadow_face_icon);
  al_destroy_bitmap (heart_icon);
  al_destroy_bitmap (plus_icon);
  al_destroy_bitmap (minus_icon);
  al_destroy_bitmap (lock_icon);
  al_destroy_bitmap (unlock_icon);
}





void
start_menu (ALLEGRO_MENU *parent, uint16_t base_id)
{
  append_menu_depth++;
  assert (append_menu_depth < APPEND_MENU_MAX_DEPTH);
  if (append_menu_depth == 0) append_menu_id = base_id - 1;
  append_menu[append_menu_depth] = parent;
  append_menu_item_index[append_menu_depth] = 0;
}

uint16_t
get_menu_item_id (ALLEGRO_MENU *parent, int index, uint16_t start_id)
{
  for (uint16_t i = start_id; i <= MENU_ID_MAX; i++) {
    ALLEGRO_MENU *f_menu;
    int f_index;
    if (al_find_menu_item (parent, i, &f_menu, &f_index)
        && f_menu == parent && f_index == index) return i;
  }
  return 0;
}

uint16_t
vmenu_item (int flags, ALLEGRO_BITMAP *icon, ALLEGRO_MENU **submenu,
            char const *title_template, va_list ap)
{
  ALLEGRO_MENU **am = &append_menu[append_menu_depth];
  uint16_t *am_i = &append_menu_item_index[append_menu_depth];
  uint16_t id = ++append_menu_id;

  assert (*am);

  char *title = NULL;
  if (title_template) vasprintf (&title, title_template, ap);

  /* first time, must create submenu */
  if (submenu && ! *submenu) goto replace;

  ALLEGRO_MENU *f_menu;
  int f_index;

  if (al_find_menu_item (*am, id, &f_menu, &f_index) || submenu) {
    if ((f_menu != *am || f_index != *am_i) && ! submenu) {
      uint16_t new_id = get_menu_item_id (*am, *am_i, id + 1);
      if (new_id) append_menu_id = id = new_id;
      else {
        al_remove_menu_item (f_menu, -f_index);
        goto replace;
      }
    }

    int cflags = al_get_menu_item_flags (*am, -*am_i);
    const char *ctitle = al_get_menu_item_caption (*am, -*am_i);
    ALLEGRO_BITMAP *cicon = al_get_menu_item_icon (*am, -*am_i);

    /* item not found */
    if (cflags < 0) goto replace;

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
      if (submenu) goto replace;
      /* fprintf (stderr, "changed caption: %i, %s -> %s\n", id, ctitle, title); */
      al_set_menu_item_caption (*am, -*am_i, title);
    }

    /* update icon */
    if ((cicon && ! icon) || (! cicon && icon)
        || (cicon && icon && ! bitmap_heq (cicon, icon))) {
      /* fprintf (stderr, "changed icon: %i, %s -> %s\n", id, ctitle, title); */
      al_set_menu_item_icon (*am, -*am_i, clone_memory_bitmap (icon));
    }

    (*am_i)++;

    al_free (title);
    return id;
  }

 replace:
  al_remove_menu_item (*am, -*am_i);
  /* fprintf (stderr, "replaced menu item: %i, %s\n", id, title); */
  ALLEGRO_MENU *new_submenu = NULL;
  if (submenu) {
    new_submenu = al_create_menu ();
    *submenu = new_submenu;
  }
  al_insert_menu_item (*am, -((*am_i)++), title,
                       id, flags, clone_memory_bitmap (icon),
                       new_submenu);
  al_free (title);
  return id;
}

/* uint16_t */
/* menu_item (int flags, ALLEGRO_BITMAP *icon, ALLEGRO_MENU **submenu, */
/*            char const *title_template, ...) */
/* { */
/*   va_list ap; */
/*   va_start (ap, title_template); */
/*   uint16_t id = vmenu_item (flags, icon, submenu, title_template, ap); */
/*   va_end (ap); */
/*   return id; */
/* } */

uint16_t
menu_hitem (bool enabled, char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  uint16_t id = vmenu_item (enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                            heading_icon, NULL, title_template, ap);
  va_end (ap);
  return id;
}

uint16_t
menu_sitem (bool enabled, ALLEGRO_BITMAP *icon, char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  uint16_t id = vmenu_item (enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                            icon, NULL, title_template, ap);
  va_end (ap);
  return id;
}

void
menu_ditem (bool first, uint16_t *id0, uint16_t *id1, bool enabled,
            ALLEGRO_BITMAP *icon0, ALLEGRO_BITMAP *icon1,
            char const *title_template0, char const *title_template1, ...)
{
  va_list ap;
  va_start (ap, title_template1);
  uint16_t id = vmenu_item (enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                            first ? icon0 : icon1, NULL,
                            first ? title_template0 : title_template1,
                            ap);
  va_end (ap);
  assert (id0 != id1);
  *id0 = first ? id : 0;
  *id1 = first ? 0 : id;
}

uint16_t
menu_sub (ALLEGRO_MENU **submenu, bool enabled, ALLEGRO_BITMAP *icon,
          void (*build_f) (intptr_t data), intptr_t data,
          char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  uint16_t id = vmenu_item (enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                            icon, submenu, title_template, ap);
  va_end (ap);
  if (enabled && build_f && *submenu) {
    start_menu (*submenu, 0);
    build_f (data);
    end_menu ();
  }
  return id;
}

uint16_t
menu_array (ALLEGRO_MENU **submenu, bool enabled, ALLEGRO_BITMAP *icon,
            void (*build_f) (int index, int lower),
            int id_nmemb, int nmemb, int index,
            char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);
  uint16_t id = vmenu_item (enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED,
                            icon, submenu, title_template, ap);
  va_end (ap);
  if (enabled && build_f && *submenu) {
    start_menu (*submenu, 0);

    int lower;
    if (index <= id_nmemb / 2) lower = 0;
    else if (index >= nmemb - id_nmemb / 2)
      lower = nmemb - min_int (nmemb, id_nmemb);
    else lower = index - id_nmemb / 2;

    if (lower > 0) menu_sep ("...");

    int i;
    for (i = 0; i < min_int (nmemb, id_nmemb); i++)
      build_f (lower + i, (lower > 0) ? lower - 1 : lower);

    if (lower < nmemb - min_int (nmemb, id_nmemb)) menu_sep ("...");

    end_menu ();
  }
  return id;
}

uint16_t
menu_citem (bool enabled, bool checked, ALLEGRO_BITMAP *icon,
            char const *title_template, ...)
{
  int flags = enabled ? 0 : ALLEGRO_MENU_ITEM_DISABLED;
  flags |= checked
    ? ALLEGRO_MENU_ITEM_CHECKBOX | ALLEGRO_MENU_ITEM_CHECKED
    : 0;

  va_list ap;
  va_start (ap, title_template);
  uint16_t id = vmenu_item (flags, checked ? NULL : icon, NULL,
                            title_template, ap);
  va_end (ap);
  return id;
}

void
menu_sep (char const *title_template, ...)
{
  va_list ap;
  va_start (ap, title_template);

/* #if WINDOWS_PORT */
/*   vmenu_item (title_template ? ALLEGRO_MENU_ITEM_DISABLED : 0, */
/*               NULL, NULL, title_template, ap); */
/* #else */
  vmenu_item (ALLEGRO_MENU_ITEM_DISABLED, NULL, NULL,
              title_template ? title_template : "", ap);
/* #endif */

  va_end (ap);
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
    al_set_display_menu (display, menu.main.m);
    al_show_mouse_cursor (display);
    if (menu_timer) al_start_timer (menu_timer);
  }
}

void
hide_menu (void)
{
  if (is_fullscreen ()) {
    al_remove_display_menu (display);
    al_hide_mouse_cursor (display);
    if (menu_timer) al_stop_timer (menu_timer);
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





ALLEGRO_EVENT_SOURCE *
main_menu_event_source (void)
{
  return menu.main.event_source;
}

void
main_menu (void)
{
  memset (&item.main, 0, sizeof (item.main));
  if (! menu.main.m) {
    menu.main.m = al_create_menu ();
    menu.main.event_source = al_enable_menu_event_source (menu.main.m);
  }
  start_menu (menu.main.m, MAIN_MENU_ID_MIN);

  menu_sub (&menu.main.game.m, main_menu_enabled, NULL, game_menu,
            0, "&Game");

  menu_sub (&menu.main.view.m, main_menu_enabled, NULL, view_menu,
            0, "&View");

  menu_sub (&menu.main.gamepad.m, main_menu_enabled, NULL, gamepad_menu,
            0, "&Input");

  menu_sub (&menu.main.play.m, main_menu_enabled, NULL, play_menu,
            0, "&Play");

  menu_sub (&menu.main.editor.m, main_menu_enabled && can_edit (), NULL,
            editor_menu, 0, "&Editor");

  menu_sub (&menu.main.cheat.m, main_menu_enabled && can_edit (), NULL,
            cheat_menu, 0, "&Cheat");

  menu_sub (&menu.main.help.m, main_menu_enabled, NULL, help_menu,
            0, "He&lp");

#if WINDOWS_PORT
  item.main.unlock_selection =
    menu_sitem (selection_locked, NULL, "(L)");
#else
  if (edit != EDIT_NONE)
    item.main.unlock_selection =
      menu_sitem (selection_locked, lock_icon, "%s", "");
#endif

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
  menu_sub (&menu.main.game.load.m, true, open_icon, load_menu,
            0, "&Load");

  menu_sub (&menu.main.game.save.m, true, save_icon,  save_menu,
            0, "&Save");

  menu_sep (NULL);

  menu_sub (&menu.main.game.volume.m, true,
            volume_icon (audio_volume), volume_menu, 0,
            "&Volume (Ctrl+S)");

  item.main.game.mirror =
    menu_citem (true, in_mirror_mode (), horizontal_icon, "&Mirror");

  menu_sep (NULL);

  menu_ditem ((cutscene || title_demo) && play_time < time_limit,
              &item.main.game.start, &item.main.game.restart, true,
              right_icon, reload_icon,
              "Sta&rt (Enter)", "&Restart (Ctrl+R)");

  item.main.game.quit =
    menu_sitem (true, quit_icon, "&Quit (Ctrl+Q)");
}

void
load_menu (intptr_t index)
{
  item.main.game.load.game =
    menu_sitem (true, game_icon, "&Game... (Ctrl+L)");

  item.main.game.load.config =
    menu_sitem (true, settings_icon, "&Configuration... (Ctrl+L)");

  item.main.game.load.level =
    menu_sitem (false, NULL, "&Level file...");
}

void
save_menu (intptr_t index)
{
  item.main.game.save.game =
    menu_sitem (! cutscene && ! title_demo && replay_mode == NO_REPLAY,
                game_icon, "&Game... (Ctrl+G)");

  item.main.game.save.config =
    menu_sitem (false, settings_icon, "&Configuration...");

  item.main.game.save.level =
    menu_sitem (false, NULL, "&Level file...");
}

void
volume_menu (intptr_t index)
{
  item.main.game.volume.off =
    menu_citem (true, audio_volume == VOLUME_RANGE_MIN,
                volume_off_icon, "&Off");

  item.main.game.volume.low =
    menu_citem (true, audio_volume > VOLUME_RANGE_MIN
                && audio_volume < VOLUME_RANGE_LOW,
                volume_low_icon, "&Low");

  item.main.game.volume.medium =
    menu_citem (true, audio_volume >= VOLUME_RANGE_LOW
                && audio_volume < VOLUME_RANGE_MEDIUM,
                volume_medium_icon, "&Medium");

  item.main.game.volume.high =
    menu_citem (true, audio_volume >= VOLUME_RANGE_MEDIUM
                && audio_volume <= VOLUME_RANGE_MAX,
                volume_high_icon, "&High");
}

void
view_menu (intptr_t index)
{
  menu_ditem (is_fullscreen (),
              &item.main.view.windowed, &item.main.view.full_screen,
              true, windows_icon, full_screen_icon,
              "&Windowed (F)", "&Fullscreen (F)");

  menu_sub (&menu.main.view.zoom.m, ! cutscene && ! title_demo,
            zoom_icon, zoom_menu, 0, "&Zoom");

  menu_sub (&menu.main.view.nav.m, ! cutscene && ! title_demo,
            navigation_icon, navigation_menu, 0, "&Navigation");

  menu_sep (NULL);

  menu_sub (&menu.main.view.vm.m, true, vm_icon (), vm_menu, 0,
            "&Video (F12)");

  menu_sub (&menu.main.view.em.m, ! cutscene && ! title_demo,
            force_em ? em_icon (em) : original_icon,
            em_menu, 0, "&Environment (F11)");

  menu_sub (&menu.main.view.hue.m, ! cutscene && ! title_demo,
            force_hue ? hue_icon (hue) : original_icon,
            hue_mode_menu, 0, "&Hue (Alt+F11)");

  menu_sub (&menu.main.view.gm.m, ! cutscene && ! title_demo,
            gm_icon (gm), gm_menu, 0, "&Guard (Shift+F11)");

  menu_sub (&menu.main.view.flip.m, true,
            flip_screen_icon (screen_flags), screen_flip_menu, 0,
            "&Flip (Shift+I)");

  menu_sep (NULL);

  item.main.view.room_drawing =
    menu_citem (! cutscene && ! title_demo, ! no_room_drawing, drawing_icon,
                "Room &drawing (Shift+B)");

  item.main.view.inhibit_screensaver =
    menu_citem (true, inhibit_screensaver, screensaver_icon,
                "&Inhibit screensaver");

  menu_sep (NULL);

  item.main.view.screenshot =
    menu_sitem (true, camera_icon, "&Screenshot... (Ctrl+P)");
}

void
zoom_menu (intptr_t index)
{
  item.main.view.zoom.reset =
    menu_hitem (! cutscene && ! title_demo
                && (mr.w != 2 || mr.h != 2
                    || mr.fit_mode != MR_FIT_NONE),
                "MULTI-ROOM %ix%i", mr.w, mr.h);

  menu_sub (&menu.main.view.zoom.fit.m, ! cutscene && ! title_demo,
            zoom_fit_icon (mr.fit_mode), zoom_fit_menu, 0, "&Fit (M)");

  menu_sub (&menu.main.view.zoom.in.m,
            ! cutscene && ! title_demo && (mr.w > 1 || mr.h > 1),
            zoom_in_icon, zoom_in_menu, 0, "&In");

  menu_sub (&menu.main.view.zoom.out.m, ! cutscene && ! title_demo,
            zoom_out_icon, zoom_out_menu, 0, "&Out");
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
  item.main.view.zoom.fit.none =
    menu_citem (true, mr.fit_mode == MR_FIT_NONE, zoom_none_icon,
                "&None");

  item.main.view.zoom.fit.stretch =
    menu_citem (true, mr.fit_mode == MR_FIT_STRETCH, zoom_stretch_icon,
                "&Stretch");

  item.main.view.zoom.fit.ratio =
    menu_citem (true, mr.fit_mode == MR_FIT_RATIO, zoom_ratio_icon,
                "&Ratio");
}

void
zoom_out_menu (intptr_t index)
{
  item.main.view.zoom.out.both =
    menu_sitem (true, vertical_horizontal_icon, "&Both (])");

  item.main.view.zoom.out.vertical =
    menu_sitem (true, vertical_icon, "&Vertical (Alt+])");

  item.main.view.zoom.out.horizontal =
    menu_sitem (true, horizontal_icon, "&Horizontal (Ctrl+])");
}

void
zoom_in_menu (intptr_t index)
{
  item.main.view.zoom.in.both =
    menu_sitem (mr.w > 1 && mr.h > 1, vertical_horizontal_icon,
                "&Both ([)");

  item.main.view.zoom.in.vertical =
    menu_sitem (mr.h > 1, vertical_icon, "&Vertical (Alt+[)");

  item.main.view.zoom.in.horizontal =
    menu_sitem (mr.w > 1, horizontal_icon, "&Horizontal (Ctrl+[)");
}

void
navigation_menu (intptr_t index)
{
  item.main.view.nav.current_room =
    menu_hitem (true, "ROOM %i", mr.room);

  menu_sub (&menu.main.view.nav.select.m, true, nav_select_icon,
            nav_select_menu, 0, "Room &Selection");

  menu_sub (&menu.main.view.nav.cell.m, true, nav_cell_icon,
            nav_cell_menu, 0, "Scroll &row");

  menu_sub (&menu.main.view.nav.page.m, true, nav_page_icon,
            nav_page_menu, 0, "Scroll &page");

  struct actor *k = get_actor_by_id (current_kid_id);
  item.main.view.nav.home =
    menu_sitem (k && k->f.c.room != mr.room, nav_home_icon, "&Home (Home)");

  item.main.view.nav.center =
    menu_sitem (true, repeat_icon, "Cen&ter (Shift+Home)");

  item.main.view.nav.coord =
    menu_sitem (true, compass_icon, "C&oordinates (C)");

  item.main.view.nav.ind_coord =
    menu_sitem (true, compass2_icon, "&Indirect coordinates (Shift+C)");
}

void
nav_select_menu (intptr_t index)
{
  item.main.view.nav.select.left =
    menu_sitem (roomd (&global_level, mr.room, LEFT),
                nav_left_icon, "&Left (H)");

  item.main.view.nav.select.above =
    menu_sitem (roomd (&global_level, mr.room, ABOVE),
                nav_above_icon, "&Above (U)");

  item.main.view.nav.select.right =
    menu_sitem (roomd (&global_level, mr.room, RIGHT),
                nav_right_icon, "&Right (J)");

  item.main.view.nav.select.below =
    menu_sitem (roomd (&global_level, mr.room, BELOW),
                nav_below_icon, "&Below (N)");
}

void
nav_cell_menu (intptr_t index)
{
  item.main.view.nav.cell.left =
    menu_sitem (true, nav_left_icon, "&Left (Shift+H)");

  item.main.view.nav.cell.above =
    menu_sitem (true, nav_above_icon, "&Above (Shift+U)");

  item.main.view.nav.cell.right =
    menu_sitem (true, nav_right_icon, "&Right (Shift+J)");

  item.main.view.nav.cell.below =
    menu_sitem (true, nav_below_icon, "&Below (Shift+N)");
}

void
nav_page_menu (intptr_t index)
{
  item.main.view.nav.page.left =
    menu_sitem (true, nav_left_icon, "&Left (Alt+H)");

  item.main.view.nav.page.above =
    menu_sitem (true, nav_above_icon, "&Above (Alt+U)");

  item.main.view.nav.page.right =
    menu_sitem (true, nav_right_icon, "&Right (Alt+J)");

  item.main.view.nav.page.below =
    menu_sitem (true, nav_below_icon, "&Below (Alt+N)");
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
  item.main.view.hue.original =
    menu_citem (true, ! force_hue, original_icon, "&Original");

  item.main.view.hue.none =
    menu_citem (true, force_hue && hue == HUE_NONE, black_icon, "&None");

  item.main.view.hue.green =
    menu_citem (true, force_hue && hue == HUE_GREEN, green_icon, "&Green");

  item.main.view.hue.gray =
    menu_citem (true, force_hue && hue == HUE_GRAY, gray_icon, "G&ray");

  item.main.view.hue.yellow =
    menu_citem (true, force_hue && hue == HUE_YELLOW,
                yellow_icon, "&Yellow");

  item.main.view.hue.blue =
    menu_citem (true, force_hue && hue == HUE_BLUE, blue_icon, "&Blue");
}

ALLEGRO_BITMAP *
gm_icon (enum gm gm)
{
  switch (gm) {
  case ORIGINAL_GM: return original_icon; break;
  case GUARD_GM: return guard_icon; break;
  case FAT_GM: return fat_icon; break;
  case VIZIER_GM: return vizier_icon; break;
  case SKELETON_GM: return skeleton_icon; break;
  case SHADOW_GM: return shadow_icon; break;
  default: assert (false); return NULL; break;
  }
}

void
gm_menu (intptr_t index)
{
  item.main.view.gm.original =
    menu_citem (true, gm == ORIGINAL_GM, gm_icon (ORIGINAL_GM),
                "&Original");

  item.main.view.gm.guard =
    menu_citem (true, gm == GUARD_GM, gm_icon (GUARD_GM), "&Guard");

  item.main.view.gm.fat =
    menu_citem (true, gm == FAT_GM, gm_icon (FAT_GM), "&Fat");

  item.main.view.gm.vizier =
    menu_citem (true, gm == VIZIER_GM, gm_icon (VIZIER_GM), "&Vizier");

  item.main.view.gm.skeleton =
    menu_citem (true, gm == SKELETON_GM, gm_icon (SKELETON_GM),
                "&Skeleton");

  item.main.view.gm.shadow =
    menu_citem (true, gm == SHADOW_GM, gm_icon (SHADOW_GM), "S&hadow");
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
  item.main.view.em.original =
    menu_citem (true, ! force_em, original_icon, "&Original");

  item.main.view.em.dungeon =
    menu_citem (true, force_em && em == DUNGEON, dungeon_icon,
                "&Dungeon");

  item.main.view.em.palace =
    menu_citem (true, force_em && em == PALACE, palace_icon, "&Palace");
}

ALLEGRO_BITMAP *
vm_icon (void)
{
  /* TODO: implement */
  return NULL;
}

void
vm_menu (intptr_t index)
{
  /* TODO: implement */
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
  item.main.view.flip.vertical =
    menu_citem (true, screen_flags & ALLEGRO_FLIP_VERTICAL,
                vertical_icon, "&Vertical");

  item.main.view.flip.horizontal =
    menu_citem (true, screen_flags & ALLEGRO_FLIP_HORIZONTAL,
                horizontal_icon, "&Horizontal");
}

void
gamepad_menu (intptr_t index)
{
  menu_sub (&menu.main.gamepad.device.m, true,
            gamepad_device_icon (gpm), gamepad_device_menu, 0, "&Device");

  item.main.gamepad.calibrate =
    menu_sitem (gpm == JOYSTICK, clock_icon, "&Calibrate (Ctrl+J)");

  menu_sep (NULL);

  menu_sub (&menu.main.gamepad.flip.m, true,
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
  item.main.gamepad.device.keyboard =
    menu_citem (true, gpm == KEYBOARD, keyboard_icon,
                "&Keyboard (Ctrl+K)");

  item.main.gamepad.device.joystick =
    menu_citem (joystick, gpm == JOYSTICK, joystick_icon,
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
  item.main.gamepad.flip.vertical =
    menu_citem (true, flip_gamepad_vertical, vertical_icon,
                "&Vertical");

  item.main.gamepad.flip.horizontal =
    menu_citem (true, flip_gamepad_horizontal, horizontal_icon,
                "&Horizontal");
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
    menu_hitem (false, "REPLAYING (%zu/%zu)",
                replay_index + 1, replay_chain_nmemb);

    item.main.play.play_replay =
      menu_sitem (true, stop_icon, "&Stop (F7)");

    menu_sub (&menu.main.play.replay_favorite.m, ! title_demo,
              heart_icon, replay_favorite_menu,
              0, "Fa&vorites");

    skip_level_widget ();

    pause_menu_widget ();

    speed_menu_widget ();

    statistics_widget ();

    break;
  case RECORD_REPLAY: record_replay:

    menu_hitem (false, "RECORDING");

    item.main.play.record_replay =
      menu_sitem (true,
                  replay_mode == RECORD_REPLAY
                  ? stop_icon : eject_icon,
                  recording_replay_countdown >= 0
                  ? "&Abort recording (F7)" : "&Stop... (F7)");

    pause_menu_widget ();

    statistics_widget ();

    break;
  default: assert (false);
  case NO_REPLAY: no_replay:

    item.main.play.play_replay =
      menu_sitem (true, play_icon, "Re&play... (F7)");

    item.main.play.record_replay =
      menu_sitem (true, record_icon, "&Record... (Alt+F7)");

    menu_sub (&menu.main.play.replay_favorite.m,
              replay_favorite_nmemb > 0, heart_icon,
              replay_favorite_menu, 0, "Fa&vorites");

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
  item.main.play.replay_favorite.add =
    menu_sitem (! title_demo && replay_mode == PLAY_REPLAY,
                plus_icon, "&Add");

  size_t i;
  int m = min_int (replay_favorite_nmemb, UINT16_MAX);
  for (i = 0; i < m; i++) {
    char *filename =
      shorten_str (replay_favorite[i].filename, MENU_CAPTION_MAX_CHARS);

    replay_favorite[i].mid =
      menu_sub (&replay_favorite[i].m, true, play_icon,
                replay_favorite_item_menu, i,
                "%s: %ju", filename, replay_favorite[i].cycle);

    al_free (filename);
  }
}

void
replay_favorite_item_menu (intptr_t index)
{
  /* GO_TO_REPLAY_FAVORITE */
  menu_sitem (true, right_icon, "&Go");
  /* REPLACE_REPLAY_FAVORITE */
  menu_sitem (! title_demo && replay_mode == PLAY_REPLAY,
              jump_icon, "&Replace");
  /* REMOVE_REPLAY_FAVORITE */
  menu_sitem (true, minus_icon, "Re&move");
}

void
editor_menu (intptr_t index)
{
  menu_ditem (edit == EDIT_NONE,
              &item.main.editor.editor_mode, &item.main.editor.play_mode,
              true, edit_icon, joystick2_icon,
              "&Edit (F8)", "&Play (F8)");

  item.main.editor.undo =
    menu_sitem (can_undo (&undo, -1), undo_icon, "&Undo (Ctrl+Z)");

  item.main.editor.redo =
    menu_sitem (can_undo (&undo, +1), redo_icon, "&Redo (Ctrl+Y)");
}

void
cheat_menu (intptr_t index)
{
  struct actor *k = get_actor_by_id (current_kid_id);

  item.main.cheat.kill_enemy =
    menu_sitem (k && k->enemy_id > 0, death_icon, "&Kill enemy (K)");

  item.main.cheat.resurrect =
    menu_sitem (k && k->current_hp <= 0, resurrect_icon, "&Resurrect (R)");

  item.main.cheat.immortal =
    menu_citem (true, immortal_mode, angel_icon, "&Immortal (I)");

  item.main.cheat.kid_float =
    menu_sitem (k && (k->current_hp > 0 || k->action == kid_fall)
                && (k->float_timer == 0
                    || k->float_timer >= REFLOAT_MENU_THRESHOLD),
                feather_icon, "&Float (Shift+W)");

  item.main.cheat.fill_hp =
    menu_sitem (k && k->current_hp > 0
                && k->current_hp < k->total_hp,
                hp_empty_icon, "Fill &HP (Shift+S)");

  item.main.cheat.add_hp =
    menu_sitem (k && k->current_hp > 0
                && (k->current_hp < k->total_hp
                    || k->total_hp < MAX_HP),
                hp_full_icon,
                (! k || k->total_hp < MAX_HP)
                ? "&Add HP (Shift+T)"
                : "Fill &all HP (Shift+T)");

  menu_sub (&menu.main.cheat.time.m, true, time_icon,
            time_change_menu, 0, "&Time");

  menu_sub (&menu.main.cheat.kca.m, true, counter_attack_icon,
            kca_change_menu, 0, "Counter a&ttack");

  menu_sub (&menu.main.cheat.kcd.m, true, counter_defense_icon,
            kcd_change_menu, 0, "Counter &defense");
}

void
time_change_menu (intptr_t index)
{
  item.main.cheat.time.add =
    menu_sitem (true, time_add_icon, "&Increase (=)");

  item.main.cheat.time.sub =
    menu_sitem (time_limit - play_time > 60 * DEFAULT_HZ, time_sub_icon,
                "&Decrease (-)");
}

void
kca_change_menu (intptr_t index)
{
  struct actor *k = get_actor_by_id (current_kid_id);

  item.main.cheat.kca.add =
    menu_sitem (k && k->skill.counter_attack_prob < 99,
                counter_attack_add_icon, "&Increase (Ctrl+=)");

  item.main.cheat.kca.sub =
    menu_sitem (k && k->skill.counter_attack_prob > -1,
                counter_attack_sub_icon, "&Decrease (Ctrl+-)");
}

void
kcd_change_menu (intptr_t index)
{
  struct actor *k = get_actor_by_id (current_kid_id);

  item.main.cheat.kcd.add =
  menu_sitem (k && k->skill.counter_defense_prob < 99,
              counter_defense_add_icon, "&Increase (Alt+=)");

  item.main.cheat.kcd.sub =
    menu_sitem (k && k->skill.counter_defense_prob > -1,
                counter_defense_sub_icon, "&Decrease (Alt+-)");
}

void
help_menu (intptr_t index)
{
  item.main.help.about =
    menu_sitem (true, small_logo_icon, "&About");
}

void
skip_level_widget (void)
{
  if (cutscene || title_demo)
    item.main.play.jump_to_level._id =
      menu_array (&menu.main.play.jump_to_level.m, true, heading_icon,
                  jump_to_level_menu, JUMP_TO_LEVEL_MID_NMEMB, 14, 0,
                  "LEVEL");
  else
    item.main.play.jump_to_level._id =
      menu_array (&menu.main.play.jump_to_level.m, true, heading_icon,
                  jump_to_level_menu, JUMP_TO_LEVEL_MID_NMEMB,
                  replay_mode == PLAY_REPLAY
                  ? replay_chain_nmemb : 14,
                  replay_mode == PLAY_REPLAY
                  ? replay_index : global_level.n - 1,
                  "LEVEL %i", global_level.n);

  item.main.play.restart_level =
    menu_sitem (replay_mode == PLAY_REPLAY && ! title_demo
              ? true
              : ! cutscene && ! title_demo,
              repeat_icon, "R&epeat (Ctrl+A)");

  item.main.play.next_level =
    menu_sitem (replay_mode == PLAY_REPLAY && ! title_demo
                ? replay_index + 1 < replay_chain_nmemb
                : ! cutscene && ! title_demo && global_level.n < 14,
                next_icon, "&Next (Shift+L)");

  item.main.play.previous_level =
    menu_sitem (replay_mode == PLAY_REPLAY && ! title_demo
                ? replay_index > 0
                : ! cutscene && ! title_demo && global_level.n > 1,
                previous_icon, "Pre&vious (Shift+M)");
}

void
jump_to_level_menu (int index, int lower)
{
  jump_to_level_menu_lower = lower;
  if (replay_mode == PLAY_REPLAY)
    menu_citem (true, replay_index == index,
                NULL, "%i", replay_chain[index].start_level);
  else menu_citem (true, global_level.n == index + 1,
                   NULL, "%i", index + 1);
}

void
statistics_widget (void)
{
  menu_sep (NULL);

  item.main.play.time =
    menu_sitem
    (! cutscene && ! title_demo && recording_replay_countdown < 0,
     time_icon, "Ti&me (Space)");

  item.main.play.skills =
    menu_sitem
    (! cutscene && ! title_demo && recording_replay_countdown < 0,
     skills_icon, "S&kills (Tab)");
}

void
pause_menu_widget (void)
{
  if (is_game_paused ())
    menu_hitem (false, "CYCLE: %ju", anim_cycle);
  else menu_sep (NULL);

  item.main.play.pause_game =
    menu_sitem
    (! cutscene && ! title_demo && recording_replay_countdown < 0,
     is_game_paused () ? play_icon : pause_icon,
     is_game_paused () ? "&Continue" : "Pau&se (Esc)");

  item.main.play.next_frame =
    menu_sitem (is_game_paused () && ! cutscene && ! title_demo
                && recording_replay_countdown < 0,
                forward_icon, "Next &frame (Esc)");
}

void
speed_menu_widget (void)
{
  if (! cutscene && ! title_demo && anim_freq > 0)
    item.main.play.reset_time_freq =
      menu_hitem (anim_freq != DEFAULT_HZ, "FREQ: %iHz", anim_freq);
  else menu_sep (NULL);

  item.main.play.toggle_time_freq_constraint =
    menu_citem
    (! cutscene && ! title_demo, anim_freq > 0, cancel_icon,
     "Frequency cons&traint");

  item.main.play.decrease_time_freq =
    menu_sitem
    (anim_freq > 2 && ! cutscene && ! title_demo, backward_icon,
     "&Decrease frequecy [(]");

  item.main.play.increase_time_freq =
    menu_sitem
    (anim_freq != 0 && ! cutscene && ! title_demo, forward_icon,
     "&Increase frequecy [)]");
}






void
aux_menu (void)
{
  memset (&item.aux, 0, sizeof (item.aux));
  if (! menu.aux.m) {
    menu.aux.m = al_create_popup_menu ();
    menu.aux.event_source =
      al_enable_menu_event_source (menu.aux.m);
  }
  start_menu (menu.aux.m, AUX_MENU_ID_MIN);

  menu_ditem (! selection_locked,
              &item.aux.lock_selection, &item.aux.unlock_selection,
              true, lock_icon, unlock_icon,
              "&Lock selection", "&Unlock selection");

  end_menu ();
}

void
show_aux_menu (void)
{
  if (edit == EDIT_NONE) return;
  aux_menu ();
  al_popup_menu (menu.aux.m, display);
}

ALLEGRO_EVENT_SOURCE *
aux_menu_event_source (void)
{
  return menu.aux.event_source;
}





void
dispatch_menu_event (ALLEGRO_EVENT *event)
{
  uint16_t id = event->user.data1;

  if (id >= MAIN_MENU_ID_MIN && id <= MAIN_MENU_ID_MAX)
    process_main_menu_event (event);
  else if (id >= AUX_MENU_ID_MIN && id <= AUX_MENU_ID_MAX)
    process_aux_menu_event (event);
  else return;

  /* A bug in Allegro prevents the following better and correct code
     from working, so as a hopefully temporary workaround, we
     discriminate based on menu item ids. */
  /* if (event->user.source == menu.main.event_source) */
  /*   process_main_menu_event (event); */
  /* else if (event->user.source == menu.aux.event_source) */
  /*   process_aux_menu_event (event); */
}

void
process_main_menu_event (ALLEGRO_EVENT *event)
{
  uint16_t id = event->user.data1;
  ALLEGRO_MENU *m = (ALLEGRO_MENU *) event->user.data3;
  int i;

  if (id == item.main.game.mirror) ui_mirror_mode (! in_mirror_mode ());
  else if (id == item.main.game.start) ui_start_game ();
  else if (id == item.main.game.restart) ui_restart_game ();
  else if (id == item.main.game.quit) ui_quit_game ();
  else if (id == item.main.game.load.game) ui_load_game ();
  else if (id == item.main.game.load.config) ui_load_config ();
  else if (id == item.main.game.load.level) /* not implemented */;
  else if (id == item.main.game.save.game) ui_save_game ();
  else if (id == item.main.game.save.config) /* not implemented */;
  else if (id == item.main.game.save.level) /* not implemented */;
  else if (id == item.main.game.volume.off)
    ui_audio_volume (VOLUME_OFF);
  else if (id == item.main.game.volume.low)
    ui_audio_volume (VOLUME_LOW);
  else if (id == item.main.game.volume.medium)
    ui_audio_volume (VOLUME_MEDIUM);
  else if (id == item.main.game.volume.high)
    ui_audio_volume (VOLUME_HIGH);
  else if (id == item.main.view.full_screen
           || id == item.main.view.windowed)
    ui_full_screen ();
  else if (id == item.main.view.room_drawing)
    ui_room_drawing (no_room_drawing);
  else if (id == item.main.view.inhibit_screensaver)
    ui_inhibit_screensaver (! inhibit_screensaver);
  else if (id == item.main.view.screenshot)
    ui_screenshot ();
  else if (id == item.main.view.zoom.reset) {
    ui_zoom_fit (MR_FIT_NONE);
    ui_set_multi_room (2 - mr.w, 2 - mr.h, false);
  } else if (id == item.main.view.zoom.fit.none)
    ui_zoom_fit (MR_FIT_NONE);
  else if (id == item.main.view.zoom.fit.stretch)
    ui_zoom_fit (MR_FIT_STRETCH);
  else if (id == item.main.view.zoom.fit.ratio)
    ui_zoom_fit (MR_FIT_RATIO);
  else if (id == item.main.view.zoom.in.both)
    ui_set_multi_room (-1, -1, false);
  else if (id == item.main.view.zoom.in.vertical)
    ui_set_multi_room (+0, -1, false);
  else if (id == item.main.view.zoom.in.horizontal)
    ui_set_multi_room (-1, +0, false);
  else if (id == item.main.view.zoom.out.both)
    ui_set_multi_room (+1, +1, false);
  else if (id == item.main.view.zoom.out.vertical)
    ui_set_multi_room (+0, +1, false);
  else if (id == item.main.view.zoom.out.horizontal)
    ui_set_multi_room (+1, +0, false);
  else if (id == item.main.view.nav.current_room)
    mr.select_cycles = SELECT_CYCLES;
  else if (id == item.main.view.nav.home)
    mr_focus_room (get_actor_by_id (current_kid_id)->f.c.room);
  else if (id == item.main.view.nav.center)
    mr_center_room (mr.room);
  else if (id == item.main.view.nav.coord)
    ui_show_coordinates ();
  else if (id == item.main.view.nav.ind_coord)
    ui_show_indirect_coordinates ();
  else if (id == item.main.view.nav.select.left) mr_select_trans (LEFT);
  else if (id == item.main.view.nav.select.above) mr_select_trans (ABOVE);
  else if (id == item.main.view.nav.select.right) mr_select_trans (RIGHT);
  else if (id == item.main.view.nav.select.below) mr_select_trans (BELOW);
  else if (id == item.main.view.nav.cell.left) mr_view_trans (LEFT);
  else if (id == item.main.view.nav.cell.above) mr_view_trans (ABOVE);
  else if (id == item.main.view.nav.cell.right) mr_view_trans (RIGHT);
  else if (id == item.main.view.nav.cell.below) mr_view_trans (BELOW);
  else if (id == item.main.view.nav.page.left) mr_view_page_trans (LEFT);
  else if (id == item.main.view.nav.page.above) mr_view_page_trans (ABOVE);
  else if (id == item.main.view.nav.page.right) mr_view_page_trans (RIGHT);
  else if (id == item.main.view.nav.page.below) mr_view_page_trans (BELOW);
  else if (id == item.main.view.em.original) ui_em (ORIGINAL_EM);
  else if (id == item.main.view.em.dungeon) ui_em (DUNGEON);
  else if (id == item.main.view.em.palace) ui_em (PALACE);
  else if (id == item.main.view.hue.original) ui_hue_mode (HUE_ORIGINAL);
  else if (id == item.main.view.hue.none) ui_hue_mode (HUE_NONE);
  else if (id == item.main.view.hue.green) ui_hue_mode (HUE_GREEN);
  else if (id == item.main.view.hue.gray) ui_hue_mode (HUE_GRAY);
  else if (id == item.main.view.hue.yellow) ui_hue_mode (HUE_YELLOW);
  else if (id == item.main.view.hue.blue) ui_hue_mode (HUE_BLUE);
  else if (id == item.main.view.gm.original) ui_gm (ORIGINAL_GM);
  else if (id == item.main.view.gm.guard) ui_gm (GUARD_GM);
  else if (id == item.main.view.gm.fat) ui_gm (FAT_GM);
  else if (id == item.main.view.gm.vizier) ui_gm (VIZIER_GM);
  else if (id == item.main.view.gm.skeleton) ui_gm (SKELETON_GM);
  else if (id == item.main.view.gm.shadow) ui_gm (SHADOW_GM);
  else if (id == item.main.view.flip.vertical)
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_VERTICAL, false, false);
  else if (id == item.main.view.flip.horizontal)
    ui_flip_screen (screen_flags ^ ALLEGRO_FLIP_HORIZONTAL, false, false);
  else if (id == item.main.gamepad.calibrate) ui_gamepad_mode (JOYSTICK);
  else if (id == item.main.gamepad.device.keyboard) ui_gamepad_mode (KEYBOARD);
  else if (id == item.main.gamepad.device.joystick)
    ui_gamepad_mode (JOYSTICK);
  else if (id == item.main.gamepad.flip.vertical)
    ui_flip_gamepad (! flip_gamepad_vertical, flip_gamepad_horizontal, false);
  else if (id == item.main.gamepad.flip.horizontal)
    ui_flip_gamepad (flip_gamepad_vertical, ! flip_gamepad_horizontal, false);
  else if (id == item.main.play.play_replay) ui_play_replay ();
  else if (id == item.main.play.record_replay) ui_record_replay ();
  else if (id == item.main.play.restart_level) ui_jump_to_level_rel (+0);
  else if (id == item.main.play.next_level) ui_jump_to_level_rel (+1);
  else if (id == item.main.play.previous_level) ui_jump_to_level_rel (-1);
  else if (id == item.main.play.time) ui_time ();
  else if (id == item.main.play.skills) ui_skills ();
  else if (id == item.main.play.pause_game) ui_toggle_pause_game ();
  else if (id == item.main.play.next_frame) ui_next_frame ();
  else if (id == item.main.play.reset_time_freq)
    ui_change_anim_freq (DEFAULT_HZ);
  else if (id == item.main.play.toggle_time_freq_constraint)
    ui_toggle_time_frequency_constraint ();
  else if (id == item.main.play.decrease_time_freq)
    ui_decrease_time_frequency ();
  else if (id == item.main.play.increase_time_freq)
    ui_increase_time_frequency ();
  else if (id == item.main.play.replay_favorite.add)
    ui_add_replay_favorite ();
  else if (id == item.main.editor.editor_mode
           || id == item.main.editor.play_mode)
    ui_editor ();
  else if (id == item.main.editor.undo)
    ui_undo_pass (&undo, -1, NULL);
  else if (id == item.main.editor.redo)
    ui_undo_pass (&undo, +1, NULL);
  else if (id == item.main.cheat.kill_enemy) ui_kill_enemy ();
  else if (id == item.main.cheat.resurrect) ui_resurrect ();
  else if (id == item.main.cheat.immortal) ui_immortal (! immortal_mode);
  else if (id == item.main.cheat.kid_float) ui_float ();
  else if (id == item.main.cheat.fill_hp) ui_fill_hp ();
  else if (id == item.main.cheat.add_hp) ui_add_hp ();
  else if (id == item.main.cheat.time.add) ui_change_time (+10);
  else if (id == item.main.cheat.time.sub) ui_change_time (-10);
  else if (id == item.main.cheat.kca.add) ui_change_kca (+10);
  else if (id == item.main.cheat.kca.sub) ui_change_kca (-10);
  else if (id == item.main.cheat.kcd.add) ui_change_kcd (+10);
  else if (id == item.main.cheat.kcd.sub) ui_change_kcd (-10);
  else if (id == item.main.help.about) ui_about_screen (true);
  else if (id == item.main.unlock_selection)
    selection_locked = ! selection_locked;
  else if (m == menu.main.play.jump_to_level.m)
    ui_jump_to_level_menu (id - item.main.play.jump_to_level._id,
                           jump_to_level_menu_lower);
  else if ((i = replay_favorite_index_by_menu (m)) >= 0) {
    if (id - replay_favorite[i].mid == GO_TO_REPLAY_FAVORITE)
      ui_go_to_replay_favorite (i);
    else if (id - replay_favorite[i].mid == REPLACE_REPLAY_FAVORITE)
      ui_replace_replay_favorite (i);
    else if (id - replay_favorite[i].mid == REMOVE_REPLAY_FAVORITE)
      ui_remove_replay_favorite (i);
    else assert (false);
  }
}

void
process_aux_menu_event (ALLEGRO_EVENT *event)
{
  uint16_t id = event->user.data1;
  /* ALLEGRO_MENU *m = (ALLEGRO_MENU *) event->user.data3; */
  /* int i; */

  if (id == item.aux.lock_selection || id == item.aux.unlock_selection)
    selection_locked = ! selection_locked;
}
