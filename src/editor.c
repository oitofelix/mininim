/*
  editor.c -- editor module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

static char menu_step_ext (struct pos *p, int min, int max);
static char menu_event_ext (struct pos *p);
static char menu_select_room (enum edit up_edit, char *prefix);
static char menu_link (enum dir dir);
static void mouse2guard (int i);
static char menu_skill (char *prefix, int *skill, int max, enum edit up_edit);

static int last_event;
static struct mouse_coord last_mouse_coord;
static struct pos last_event2floor_pos;
static bool reciprocal_links, locally_unique_links,
  globally_unique_links;
static bool b0, b1, b2, b3, b4, b5;
static int guard_index;
static int b, r, s, t, min, max;
static struct con con_copy;
static struct con room_copy[FLOORS][PLACES];
static char *msg;
static uint64_t msg_cycles;

enum edit edit;
enum edit last_edit = EDIT_MAIN;

void
editor (void)
{
  if (edit == EDIT_NONE) return;

  active_menu = true;

  struct menu_item main_menu[] =
    {{'C', "CONSTRUCTION>"},
     {'E', "EVENT>"},
     {'R', "ROOM>"},
     {'K', "KID>"},
     {'G', "GUARD>"},
     {'L', "LEVEL>"},
     {0}};

  struct menu_item con_menu[] =
    {{'F', "FOREGROUND>"},
     {'B', "BACKGROUND>"},
     {'E', "EXTENSION*"},
     {'I', "INFO"},
     {'A', "CLEAR"},
     {'R', "RANDOM"},
     {'H', "EXCHANGE CONS"},
     {'C', "COPY"},
     {'P', "PASTE"},
     {0}};

  struct menu_item con_exchange_menu[] =
    {{'H', "HORIZONTAL"},
     {'V', "VERTICAL"},
     {'R', "RANDOM"},
     {0}};

  struct menu_item fg_menu[] =
    {{'F', "FLOOR>"},
     {'P', "PILLAR>"},
     {'W', "WALL"},
     {'D', "DOOR>"},
     {'C', "CHOPPER"},
     {'M', "MIRROR"},
     {'R', "CARPET>"},
     {'A', "ARCH>"},
     {0}};

  struct menu_item floor_menu[] =
    {{'N', "NO FLOOR"},
     {'F', "FLOOR"},
     {'B', "BROKEN FLOOR"},
     {'S', "SKELETON FLOOR"},
     {'L', "LOOSE FLOOR"},
     {'P', "SPIKES FLOOR"},
     {'O', "OPENER FLOOR"},
     {'C', "CLOSER FLOOR"},
     {'T', "STUCK FLOOR"},
     {'H', "HIDDEN FLOOR"},
     {0}};

  struct menu_item pillar_menu[] =
    {{'P', "PILLAR"},
     {'T', "BIG PILLAR TOP"},
     {'B', "BIG PILLAR BOTTOM"},
     {'A', "ARCH BOTTOM"},
     {0}};

  struct menu_item door_menu[] =
    {{'D', "DOOR"},
     {'L', "LEVEL DOOR"},
     {0}};

  struct menu_item carpet_menu[] =
    {{'C', "CARPET"},
     {'T', "TRAVERSABLE CARPET"},
     {0}};

  struct menu_item arch_menu[] =
    {{'M', "ARCH TOP MID"},
     {'S', "ARCH TOP SMALL"},
     {'L', "ARCH TOP LEFT"},
     {'R', "ARCH TOP RIGHT"},
     {0}};

  struct menu_item bg_menu[] =
    {{'N', "NO BRICKS"},
     {'G', "NO BG"},
     {'0', "BRICKS 00"},
     {'1', "BRICKS 01"},
     {'2', "BRICKS 02"},
     {'3', "BRICKS 03"},
     {'T', "TORCH"},
     {'W', "WINDOW"},
     {'B', "BALCONY"},
     {0}};

  struct menu_item items_menu[] =
    {{'N', "NO ITEM"},
     {'E', "EMPTY POTION"},
     {'S', "SMALL LIFE POTION"},
     {'B', "BIG LIFE POTION"},
     {'P', "SMALL POISON POTION"},
     {'O', "BIG POISON POTION"},
     {'F', "FLOAT POTION"},
     {'L', "FLIP POTION"},
     {'A', "ACTIVATION POTION"},
     {'W', "SWORD"},
     {0}};

  struct menu_item loose_floor_ext_menu[] =
    {{'C', "CAN'T FALL"},
     {0}};

  struct menu_item carpet_ext_menu[] =
    {{'0', "CARPET 00"},
     {'1', "CARPET 01"},
     {'A', "ARCH CARPET LEFT"},
     {0}};

  struct menu_item tcarpet_ext_menu[] =
    {{'0', "CARPET 00"},
     {'1', "CARPET 01"},
     {'A', "ARCH CARPET LEFT"},
     {'B', "ARCH CARPET RIGHT 00"},
     {'C', "ARCH CARPET RIGHT 01"},
     {0}};

  struct menu_item event_menu[] =
    {{'D', "EVENT->DOOR<"},
     {'F', "EVENT->FLOOR<"},
     {'R', "DOOR->EVENT<"},
     {'S', "SET EVENT<"},
     {0}};

  struct menu_item room_menu[] =
    {{'J', "JUMP TO ROOM<"},
     {'L', "ROOM LINKING>"},
     {'S', "LINKING SETTINGS<"},
     {'X', "EXCHANGE ROOM<"},
     {'A', "CLEAR"},
     {'R', "RANDOM"},
     {'D', "DECORATION"},
     {'H', "EXCHANGE CONS"},
     {'C', "COPY"},
     {'P', "PASTE"},
     {0}};

  struct menu_item link_menu[] =
    {{'L', "LEFT<"},
     {'R', "RIGHT<"},
     {'A', "ABOVE<"},
     {'B', "BELOW<"},
     {0}};

  struct menu_item linking_settings_menu[] =
    {{'R', "RECIPROCAL"},
     {'L', "LOCALLY UNIQUE"},
     {'G', "GLOBALLY UNIQUE"},
     {0}};

  struct menu_item kid_menu[] =
    {{'P', "PLACE KID"},
     {'S', "SET START POSITION"},
     {'J', "JUMP TO START POSITION"},
     {'D', "TOGGLE START DIRECTION"},
     {'W', "TOGGLE HAS SWORD"},
     {0}};

  struct menu_item level_menu[] =
    {{'N', "NOMINAL NUMBER"},
     {'E', "ENVIRONMENT<"},
     {'H', "HUE<"},
     {'S', "SAVE LEVEL"},
     {'L', "RELOAD LEVEL"},
     {0}};

  struct menu_item environment_menu[] =
    {{'D', "DUNGEON"},
     {'P', "PALACE"},
     {0}};

  struct menu_item hue_menu[] =
    {{'N', "NONE"},
     {'G', "GREEN"},
     {'R', "GRAY"},
     {'Y', "YELLOW"},
     {'B', "BLUE"},
     {0}};

  struct menu_item guard_menu[] =
    {{'G', "SELECT GUARD<"},
     {'P', "PLACE GUARD"},
     {'S', "SET START POSITION"},
     {'J', "JUMP TO START POSITION"},
     {'D', "TOGGLE START DIRECTION"},
     {'K', "SKILL>"},
     {'L', "LIVES<"},
     {'T', "TYPE<"},
     {'Y', "STYLE<"},
     {0}};

  struct menu_item skill_menu[] =
    {{'A', "ATTACK<"},
     {'B', "COUNTER ATTACK<"},
     {'D', "DEFENSE<"},
     {'E', "COUNTER DEFENSE<"},
     {'V', "ADVANCE<"},
     {'R', "RETURN<"},
     {'F', "REFRACTION PERIOD<"},
     {'X', "EXTRA LIVES<"},
     {'L', "LEGACY TEMPLATES<"},
     {0}};

  struct menu_item guard_type_menu[] =
    {{'D', "DISABLED"},
     {'G', "GUARD"},
     {'F', "FAT GUARD"},
     {'V', "VIZIER"},
     {'S', "SKELETON"},
     {'H', "SHADOW"},
     {0}};

  struct pos p = mouse_pos;
  struct anim *k;
  static struct guard *g;
  static struct pos p0;

  char *fg_str = NULL, *bg_str = NULL, *ext_str = NULL;
  bool free_ext_str;
  char *str = NULL, c, *f, *d;

  enum confg fg;
  enum conbg bg;
  union conext ext;
  struct con room_buf[FLOORS][PLACES], room_buf2[FLOORS][PLACES];
  static struct skill skill_buf;

  /* display message if available */
  if (msg_cycles > 0 && msg && ! key.keyboard.keycode) {
    msg_cycles--;
    draw_bottom_text (NULL, msg, 0);
    memset (&key, 0, sizeof (key));
    menu_help = 0;
    return;
  } else msg_cycles = 0;

  switch (edit) {
  case EDIT_NONE: break;
  case EDIT_MAIN:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (menu_enum (main_menu, NULL)) {
    case 'C': edit = EDIT_CON; break;
    case 'E': edit = EDIT_EVENT; break;
    case 'R': edit = EDIT_ROOM; break;
    case 'K': edit = EDIT_KID; break;
    case 'G': edit = EDIT_GUARD; break;
    case 'L': edit = EDIT_LEVEL; break;
    }
    break;
  case EDIT_CON:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    switch (menu_enum (con_menu, "C>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'F': edit = EDIT_FG; break;
    case 'B': edit = EDIT_BG; break;
    case 'E': edit = EDIT_EXT; break;
    case 'I': edit = EDIT_INFO; break;
    case 'A':
      register_con_undo (&undo, &p,
                         NO_FLOOR, NO_BG, NO_ITEM,
                         true, false, false, true,
                         -1, "CLEAR CON");
      break;
    case 'R':
      register_con_undo (&undo, &p,
                         prandom (ARCH_TOP_SMALL),
                         prandom (BALCONY),
                         0,
                         true, true, true, true,
                         -1, "RANDOM CON");
      break;
    case 'H': edit = EDIT_CON_EXCHANGE; break;
    case 'C':
      con_copy = *con (&p);
      editor_msg ("COPIED", 12);
      break;
    case 'P':
      register_con_undo (&undo, &p,
                         con_copy.fg, con_copy.bg, con_copy.ext.step,
                         true, true, true, true, -1, "PASTE CON");
      break;
    }
    break;
  case EDIT_CON_EXCHANGE:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    switch (menu_enum (con_exchange_menu, "CH>")) {
    case -1: case 1: edit = EDIT_CON; break;
    case 'H':
      reflect_pos_h (&p, &p0);
      register_exchange_pos_undo (&undo, &p, &p0, true, true, "H. EXCHANGE CON");
      break;
    case 'V':
      reflect_pos_v (&p, &p0);
      register_exchange_pos_undo (&undo, &p, &p0, true, false, "V. EXCHANGE CON");
      break;
    case 'R':
      random_pos (&global_level, &p0);
      p0.room = p.room;
      register_exchange_pos_undo (&undo, &p, &p0, true, false, "R. EXCHANGE CON");
      break;
    }
    break;
  case EDIT_FG:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    switch (menu_enum (fg_menu, "CF>")) {
    case -1: case 1: edit = EDIT_CON; break;
    case 'F': edit = EDIT_FLOOR; break;
    case 'P': edit = EDIT_PILLAR; break;
    case 'W':
      if (con (&p)->fg == WALL) break;
      register_con_undo (&undo, &p,
                         WALL, MIGNORE, MIGNORE,
                         true, false, true, true,
                         -1, "WALL");
      break;
    case 'D': edit = EDIT_DOOR; break;
    case 'C':
      if (con (&p)->fg == CHOPPER) break;
      register_con_undo (&undo, &p,
                         CHOPPER, MIGNORE, MIGNORE,
                         true, true, true, true,
                         -1, "CHOPPER");
      break;
    case 'M':
      if (con (&p)->fg == MIRROR) break;
      register_con_undo (&undo, &p,
                         MIRROR, MIGNORE, MIGNORE,
                         true, true, true, true,
                         -1, "MIRROR");
      break;
    case 'R': edit = EDIT_CARPET; break;
    case 'A': edit = EDIT_ARCH; break;
    }
    break;
  case EDIT_FLOOR:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = menu_enum (floor_menu, "CFF>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    if ((c == 'L' && con (&p)->fg == LOOSE_FLOOR)
        || (c == 'P' && con (&p)->fg == SPIKES_FLOOR)
        || (c == 'O' && con (&p)->fg == OPENER_FLOOR)
        || (c == 'C' && con (&p)->fg == CLOSER_FLOOR))
      break;

    switch (c) {
    case 'N': fg = NO_FLOOR; break;
    case 'F': fg = FLOOR; break;
    case 'B': fg = BROKEN_FLOOR; break;
    case 'S': fg = SKELETON_FLOOR; break;
    case 'L': fg = LOOSE_FLOOR; break;
    case 'P': fg = SPIKES_FLOOR; break;
    case 'O': fg = OPENER_FLOOR; break;
    case 'C': fg = CLOSER_FLOOR; break;
    case 'T': fg = STUCK_FLOOR; break;
    case 'H': fg = HIDDEN_FLOOR; break;
    }

    register_con_undo (&undo, &p,
                       fg, MIGNORE, MIGNORE,
                       true, true, false, true,
                       -1, get_confg_name (fg));
    break;
  case EDIT_PILLAR:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = menu_enum (pillar_menu, "CFP>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    switch (c) {
    case 'P': fg = PILLAR; break;
    case 'T': fg = BIG_PILLAR_TOP; break;
    case 'B': fg = BIG_PILLAR_BOTTOM; break;
    case 'A': fg = ARCH_BOTTOM; break;
    }

    register_con_undo (&undo, &p,
                       fg, MIGNORE, MIGNORE,
                       true, false, false, true,
                       -1, get_confg_name (fg));
    break;
  case EDIT_DOOR:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = menu_enum (door_menu, "CFD>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    if ((c == 'D' && con (&p)->fg == DOOR)
        || (c == 'L' && con (&p)->fg == LEVEL_DOOR))
      break;

    switch (c) {
    case 'D': fg = DOOR; break;
    case 'L': fg = LEVEL_DOOR; break;
    }

    register_con_undo (&undo, &p,
                       fg, MIGNORE, MIGNORE,
                       true, true, false, true,
                       -1, get_confg_name (fg));
    break;
  case EDIT_CARPET:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = menu_enum (carpet_menu, "CFR>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    switch (c) {
    case 'C': fg = CARPET; break;
    case 'T': fg = TCARPET; break;
    }

    register_con_undo (&undo, &p,
                       fg, MIGNORE, MIGNORE,
                       true, false, false, true,
                       -1, get_confg_name (fg));
    break;
  case EDIT_ARCH:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = menu_enum (arch_menu, "CFA>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    switch (c) {
    case 'M': fg = ARCH_TOP_MID; break;
    case 'S': fg = ARCH_TOP_SMALL; break;
    case 'L': fg = ARCH_TOP_LEFT; break;
    case 'R': fg = ARCH_TOP_RIGHT; break;
    }

    register_con_undo (&undo, &p,
                       fg, MIGNORE, MIGNORE,
                       true, false, false, true,
                       -1, get_confg_name (fg));
    break;
  case EDIT_BG:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = menu_enum (bg_menu, "CB>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_CON; break;
    }

    switch (c) {
    case 'N': bg = NO_BRICKS; break;
    case 'G': bg = NO_BG; break;
    case '0': bg = BRICKS_00; break;
    case '1': bg = BRICKS_01; break;
    case '2': bg = BRICKS_02; break;
    case '3': bg = BRICKS_03; break;
    case 'T': bg = TORCH; break;
    case 'W': bg = WINDOW; break;
    case 'B': bg = BALCONY; break;
    }

    register_con_undo (&undo, &p,
                       MIGNORE, bg, MIGNORE,
                       false, false, (bg == BALCONY), true,
                       -1, get_conbg_name (bg));
    break;
  case EDIT_EXT:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }

    switch (con (&p)->fg) {
    case FLOOR:
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      c = menu_enum (items_menu, "CE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_CON; break;
      }

      switch (c) {
      case 'N': ext.item = NO_ITEM; break;
      case 'E': ext.item = EMPTY_POTION; break;
      case 'S': ext.item = SMALL_LIFE_POTION; break;
      case 'B': ext.item = BIG_LIFE_POTION; break;
      case 'P': ext.item = SMALL_POISON_POTION; break;
      case 'O': ext.item = BIG_POISON_POTION; break;
      case 'F': ext.item = FLOAT_POTION; break;
      case 'L': ext.item = FLIP_POTION; break;
      case 'A': ext.item = ACTIVATION_POTION; break;
      case 'W': ext.item = SWORD; break;
      }

      register_con_undo (&undo, &p,
                         MIGNORE, MIGNORE, ext.item,
                         false, false, false, true,
                         CHPOS_NONE, get_item_name (ext.item));
      break;
    case LOOSE_FLOOR:
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      b0 = con (&p)->ext.cant_fall;
      c = menu_bool (loose_floor_ext_menu, "CE>", false, &b0);
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_CON; break;
      }

      register_con_undo (&undo, &p,
                         MIGNORE, MIGNORE, b0,
                         true, true, false, true,
                         CHPOS_NONE, "CAN'T FALL EXTENSION");

      break;
    case SPIKES_FLOOR:
      menu_step_ext (&p, 0, SPIKES_FLOOR_MAX_STEP);
      break;
    case OPENER_FLOOR:
      menu_event_ext (&p);
      break;
    case CLOSER_FLOOR:
      menu_event_ext (&p);
      break;
    case DOOR:
      menu_step_ext (&p, 0, DOOR_MAX_STEP);
      break;
    case LEVEL_DOOR:
      menu_step_ext (&p, -LEVEL_DOOR_MAX_STEP - 1, LEVEL_DOOR_MAX_STEP);
      break;
    case CHOPPER:
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      r = con (&p)->ext.step;
      b = con (&p)->ext.step & 0x80;
      min = b ? 128 : 0;
      max = b ? 133 : 5;
      s = menu_int (&r, &b, min, max, "CE>STEP", "B");
      if (! s) break;

      if (s == -1 || s == 1) {
        edit = EDIT_CON; break;
      }

      register_con_undo (&undo, &p,
                         MIGNORE, MIGNORE, b ? r | 0x80 : r & ~ 0x80,
                         true, true, false, true,
                         CHPOS_CHOPPER, "STEP EXTENSION");
      break;
    case CARPET:
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      c = menu_enum (carpet_ext_menu, "CE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_CON; break;
      }

      switch (c) {
      case '0': ext.design = CARPET_00; break;
      case '1': ext.design = CARPET_01; break;
      case 'A': ext.design = ARCH_CARPET_LEFT; break;
      }

      register_con_undo (&undo, &p,
                         MIGNORE, MIGNORE, ext.design,
                         false, false, false, true,
                         CHPOS_CARPET_DESIGN, "DESIGN EXTENSION");
      break;
    case TCARPET:
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      c = menu_enum (tcarpet_ext_menu, "CE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_CON; break;
      }

      switch (c) {
      case '0': ext.design = CARPET_00; break;
      case '1': ext.design = CARPET_01; break;
      case 'A': ext.design = ARCH_CARPET_LEFT; break;
      case 'B': ext.design = ARCH_CARPET_RIGHT_00; break;
      case 'C': ext.design = ARCH_CARPET_RIGHT_01; break;
      }

      register_con_undo (&undo, &p,
                         MIGNORE, MIGNORE, ext.design,
                         false, false, false, true,
                         CHPOS_CARPET_DESIGN, "DESIGN EXTENSION");
      break;
    default:
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      draw_bottom_text (NULL, "NO EXTENSION", 0);
      if (was_menu_return_pressed (true)) edit = EDIT_CON;
      break;
    }
    break;
  case EDIT_INFO:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT CONSTRUCTION", 1);
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);

    if (was_menu_return_pressed (true)) edit = EDIT_CON;

    free_ext_str = false;

    fg_str = get_confg_name (con (&p)->fg);
    bg_str = get_conbg_name (con (&p)->bg);

    switch (con (&p)->fg) {
    case FLOOR:
      ext_str = get_item_name (con (&p)->ext.item);
      break;
    case LOOSE_FLOOR:
      ext_str = con (&p)->ext.cant_fall ? "CAN'T FALL" : "FALL";
      break;
    case SPIKES_FLOOR: case DOOR: case LEVEL_DOOR: case CHOPPER:
      xasprintf (&ext_str, "%i", con (&p)->ext.step);
      free_ext_str = true;
      break;
    case OPENER_FLOOR: case CLOSER_FLOOR:
      xasprintf (&ext_str, "%i", con (&p)->ext.event);
      free_ext_str = true;
      break;
    case CARPET:
      switch (con (&p)->ext.design) {
      case CARPET_00: ext_str = "CARPET 00"; break;
      case CARPET_01: ext_str = "CARPET 01"; break;
      case ARCH_CARPET_LEFT: ext_str = "ARCH CARPET LEFT"; break;
      default: ext_str = "UNKNOWN EXTENSION"; break;
      }
      break;
    case TCARPET:
      switch (con (&p)->ext.design) {
      case CARPET_00: ext_str = "CARPET 00"; break;
      case CARPET_01: ext_str = "CARPET 01"; break;
      case ARCH_CARPET_RIGHT_00: ext_str = "ARCH CARPET RIGHT 00"; break;
      case ARCH_CARPET_RIGHT_01: ext_str = "ARCH CARPET RIGHT 01"; break;
      case ARCH_CARPET_LEFT: ext_str = "ARCH CARPET LEFT"; break;
      default: ext_str = "UNKNOWN EXTENSION"; break;
      }
      break;
    default: ext_str = "NO EXTENSION"; break;
    }

    xasprintf (&str, "%s/%s/%s", fg_str, bg_str, ext_str);
    draw_bottom_text (NULL, str, 0);
    al_free (str);
    if (free_ext_str) al_free (ext_str);

    break;
  case EDIT_EVENT:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (menu_enum (event_menu, "E>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'D':
      edit = EDIT_EVENT2DOOR;
      get_mouse_coord (&last_mouse_coord);
      s = last_event;
      break;
    case 'F':
      edit = EDIT_EVENT2FLOOR;
      get_mouse_coord (&last_mouse_coord);
      invalid_pos (&last_event2floor_pos);
      t = last_event;
      next_pos_by_pred (&last_event2floor_pos, 0, is_event_at_pos, &t);
      break;
    case 'R':
      edit = EDIT_DOOR2EVENT;
      invalid_pos (&p0);
      break;
    case 'S':
      edit = EDIT_EVENT_SET;
      s = last_event;
      b = global_level.event[last_event].next;
      break;
    }
    break;
  case EDIT_EVENT2DOOR:
    if (! is_valid_pos (&global_level.event[s].p)
        || ! is_door (&global_level.event[s].p)) {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      al_set_mouse_xy (display, 0, 0);
    } else {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      set_mouse_pos (&global_level.event[s].p);
    }
    b = global_level.event[s].next;
    switch (menu_int (&s, &b, 0, EVENTS - 1, "ED>EVENT", "N")) {
    case -1: set_mouse_coord (&last_mouse_coord); edit = EDIT_EVENT; break;
    case 0: break;
    case 1:
      edit = EDIT_EVENT;
      last_event = s;
      break;
    }
    break;
  case EDIT_EVENT2FLOOR:
    if (! is_valid_pos (&last_event2floor_pos)) {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      al_set_mouse_xy (display, 0, 0);
    } else {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      set_mouse_pos (&last_event2floor_pos);
    }
    switch (menu_list (&s, &r, t, 0, EVENTS - 1, "EF>EVENT")) {
    case -1: set_mouse_coord (&last_mouse_coord); edit = EDIT_EVENT; break;
    case 0: break;
    case 1:
      edit = EDIT_EVENT;
      last_event = t;
      break;
    default:
      if (s) {
        if (t + s >= 0 && t + s < EVENTS) {
          t += s;
          invalid_pos (&last_event2floor_pos);
        } else s = 0;
      }
      if (s || r)
        next_pos_by_pred (&last_event2floor_pos, r, is_event_at_pos, &t);
      break;
    }
    break;
  case EDIT_DOOR2EVENT:
    if (! is_valid_pos (&p) || ! is_door (&p)) {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      editor_msg ("SELECT DOOR", 1);
      if (was_menu_return_pressed (true)) edit = EDIT_EVENT;
      t = -1;
    } else {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      if (! peq (&p, &p0)) {
        t = -1;
        next_int_by_pred (&t, 0, 0, EVENTS - 1, is_pos_at_event, &p);
        p0 = p;
      }
      switch (menu_list (NULL, &r, t, 0, EVENTS - 1, "ER>EVENT")) {
      case -1: edit = EDIT_EVENT; break;
      case 0: break;
      case 1: last_event = t; edit = EDIT_EVENT; break;
      default:
        if (r) next_int_by_pred (&t, r, 0, EVENTS - 1, is_pos_at_event, &p);
        break;
      }
    }
    break;
  case EDIT_EVENT_SET:
    if (! is_valid_pos (&p) || ! is_door (&p)) {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      editor_msg ("SELECT DOOR", 1);
      if (was_menu_return_pressed (true)) edit = EDIT_EVENT;
    } else {
      set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      switch (menu_int (&s, &b, 0, EVENTS - 1, "ES>EVENT", "N")) {
      case -1: edit = EDIT_EVENT; break;
      case 0: break;
      case 1:
        register_event_undo (&undo, s, &p, b ? true : false, "EVENT");
        last_event = s;
        edit = EDIT_EVENT;
        break;
      }
    }
    break;
  case EDIT_ROOM:
    mr.select_cycles = SELECT_CYCLES;
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    mr_focus_mouse ();
    switch (menu_enum (room_menu, "R>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'J':
      get_mouse_coord (&last_mouse_coord);
      edit = EDIT_JUMP_ROOM;
      break;
    case 'L': edit = EDIT_LINK; break;
    case 'S': edit = EDIT_LINKING_SETTINGS;
      b0 = reciprocal_links;
      b1 = locally_unique_links;
      b2 = globally_unique_links;
      break;
    case 'X':
      get_mouse_coord (&last_mouse_coord);
      edit = EDIT_ROOM_EXCHANGE; break;
    case 'A':
      new_pos (&p0, &global_level, mr.room, -1, -1);
      for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
        for (p0.place = 0; p0.place < PLACES; p0.place++) {
          room_buf[p0.floor][p0.place] =
            (struct con) {.fg = NO_FLOOR, .bg = NO_BG, .ext.step = 0};
        }
      register_room_undo (&undo, mr.room, room_buf, "CLEAR ROOM");
      break;
    case 'R':
      new_pos (&p0, &global_level, mr.room, -1, -1);
      for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
        for (p0.place = 0; p0.place < PLACES; p0.place++)
          room_buf[p0.floor][p0.place] =
            (struct con) {.fg = prandom (ARCH_TOP_SMALL), .bg = prandom (BALCONY),
                          .ext.step = 0};
      register_room_undo (&undo, mr.room, room_buf, "RANDOM ROOM");
      break;
    case 'D':
      memcpy (&room_buf2, &global_level.con[mr.room], sizeof (room_buf2));
      new_pos (&p0, &global_level, mr.room, -1, -1);
      for (p0.floor = 0; p0.floor < FLOORS; p0.floor++)
        for (p0.place = 0; p0.place < PLACES; p0.place++)
          decorate_pos (&p0);
      memcpy (&room_buf, &global_level.con[mr.room], sizeof (room_buf));
      memcpy (&global_level.con[mr.room], &room_buf2, sizeof (room_buf2));
      register_room_undo (&undo, mr.room, room_buf, "DECORATE ROOM");
      break;
    case 'H': edit = EDIT_ROOM_CON_EXCHANGE; break;
    case 'C':
      memcpy (&room_copy, &global_level.con[mr.room], sizeof (room_copy));
      editor_msg ("COPIED", 12);
      break;
    case 'P':
      register_room_undo (&undo, mr.room, room_copy, "PASTE ROOM");
      break;
    }
    break;
  case EDIT_ROOM_CON_EXCHANGE:
    mr_focus_mouse ();
    mr.select_cycles = SELECT_CYCLES;
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (menu_enum (con_exchange_menu, "RH>")) {
    case -1: case 1: edit = EDIT_ROOM; break;
    case 'H':
      register_h_room_con_exchange_undo
        (&undo, mr.room, "ROOM H.CON EXCHANGE");
      break;
    case 'V':
      register_v_room_con_exchange_undo
        (&undo, mr.room, "ROOM V.CON EXCHANGE");
      break;
    case 'R':
      register_random_room_con_exchange_undo
        (&undo, mr.room, false, false, "ROOM RANDOM CON EXCHANGE");
      break;
    }
    break;
  case EDIT_JUMP_ROOM:
    menu_select_room (EDIT_ROOM, "RJ>ROOM");
    break;
  case EDIT_LINK:
    mr.select_cycles = SELECT_CYCLES;
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    mr_focus_mouse ();
    switch (menu_enum (link_menu, "RL>")) {
    case -1: case 1: edit = EDIT_ROOM; break;
    case 'L':
      get_mouse_coord (&last_mouse_coord);
      set_mouse_room (roomd (&global_level, mr.room, LEFT));
      edit = EDIT_LINK_LEFT; break;
    case 'R':
      get_mouse_coord (&last_mouse_coord);
      set_mouse_room (roomd (&global_level, mr.room, RIGHT));
      edit = EDIT_LINK_RIGHT; break;
    case 'A':
      get_mouse_coord (&last_mouse_coord);
      set_mouse_room (roomd (&global_level, mr.room, ABOVE));
      edit = EDIT_LINK_ABOVE; break;
    case 'B':
      get_mouse_coord (&last_mouse_coord);
      set_mouse_room (roomd (&global_level, mr.room, BELOW));
      edit = EDIT_LINK_BELOW; break;
    }
    break;
  case EDIT_LINK_LEFT:
    menu_link (LEFT);
    break;
  case EDIT_LINK_RIGHT:
    menu_link (RIGHT);
    break;
  case EDIT_LINK_ABOVE:
    menu_link (ABOVE);
    break;
  case EDIT_LINK_BELOW:
    menu_link (BELOW);
    break;
  case EDIT_ROOM_EXCHANGE:
    r = menu_select_room (EDIT_ROOM, "RX>ROOM");

    mr.room_select = last_mouse_coord.c.room;

    if (r == 1) {
      struct room_linking l[ROOMS];
      memcpy (&l, &global_level.link, sizeof (l));

      int room0 = last_mouse_coord.c.room;
      int room1 = mr.room;

      exchange_rooms (&global_level, room0, room1);

      register_link_undo (&undo, l, "ROOM EXCHANGE");
      last_mouse_coord.c.room = room1;
      last_mouse_coord.mr.room = room1;
      set_mouse_coord (&last_mouse_coord);
      mr.room_select = -1;
    } else if (r == -1) mr.room_select = -1;
    break;
  case EDIT_LINKING_SETTINGS:
    mr_focus_mouse ();
    mr.select_cycles = SELECT_CYCLES;
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    switch (menu_bool (linking_settings_menu, "RS>", false, &b0,
                       &b1, &b2)) {
    case -1: edit = EDIT_ROOM; break;
    case 0: break;
    case 1:
      reciprocal_links = b0;
      locally_unique_links = b1;
      globally_unique_links = b2;
      edit = EDIT_ROOM;
      break;
    }
    break;
  case EDIT_KID:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (menu_enum (kid_menu, "K>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'P':
      ui_place_kid (&p);
      break;
    case 'J':
      set_mouse_pos (&global_level.start_pos);
      break;
    case 'S':
      if (! is_valid_pos (&p)) {
        editor_msg ("SELECT CONSTRUCTION", 12);
        break;
      }
      register_start_pos_undo (&undo, &p, "START POSITION");
      break;
    case 'D':
      if (! is_pos_visible (&global_level.start_pos)) {
        editor_msg ("START POS NOT VISIBLE", 12);
        break;
      }
      register_toggle_start_dir_undo (&undo, "START DIRECTION");
      break;
    case 'W':
      if (! is_pos_visible (&global_level.start_pos)) {
        editor_msg ("START POS NOT VISIBLE", 12);
        break;
      }
      register_toggle_has_sword_undo (&undo, "HAS SWORD");
      break;
    }
    break;
  case EDIT_LEVEL:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    xasprintf (&str, "L%i>", global_level.number);
    switch (menu_enum (level_menu, str)) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'N': edit = EDIT_NOMINAL_NUMBER;
      s = global_level.nominal_number;
      break;
    case 'E': edit = EDIT_ENVIRONMENT;
      b = em;
      b0 = (global_level.em == DUNGEON) ? true : false;
      b1 = (global_level.em == PALACE) ? true : false;
      break;
    case 'H': edit = EDIT_HUE;
      b = hue;
      b0 = (global_level.hue == HUE_NONE) ? true : false;
      b1 = (global_level.hue == HUE_GREEN) ? true : false;
      b2 = (global_level.hue == HUE_GRAY) ? true : false;
      b3 = (global_level.hue == HUE_YELLOW) ? true : false;
      b4 = (global_level.hue == HUE_BLUE) ? true : false;
      break;
    case 'S':
      xasprintf (&d, "%sdata/levels/", user_data_dir);
      if (! al_make_directory (d)) {
        error (0, al_get_errno (), "%s (%s): failed to create native level directory",
               __func__, d);
        al_free (d);
        goto save_failed;
      }
      xasprintf (&f, "%s%02d.mim", d, global_level.number);
      if (! save_native_level (&global_level, f)) {
        error (0, al_get_errno (), "%s (%s): failed to save native level file",
               __func__, f);
        al_free (f);
        al_free (d);
        goto save_failed;
      }
      copy_level (vanilla_level, &global_level);
      editor_msg ("LEVEL HAS BEEN SAVED", 18);
      break;
    save_failed:
      editor_msg ("LEVEL SAVE FAILED", 18);
      break;
    case 'L':
      editor_msg ("LEVEL RELOADED", 18);
      copy_level (&global_level, vanilla_level);
      destroy_cons ();
      register_cons ();
      update_cache (em, vm);
      break;
    }
    al_free (str);
   break;
  case EDIT_NOMINAL_NUMBER:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    xasprintf (&str, "L%iN>N.NUMBER", global_level.number);
    switch (menu_int (&global_level.nominal_number, NULL, 0, INT_MAX, str, NULL)) {
    case -1: edit = EDIT_LEVEL; global_level.nominal_number = s; break;
    case 0: break;
    case 1:
      edit = EDIT_LEVEL;
      register_int_undo (&undo, &global_level.nominal_number, s, (undo_f) int_undo,
                         "LEVEL NOMINAL NUMBER");
      break;
    default: break;
    }
    al_free (str);
    break;
  case EDIT_ENVIRONMENT:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    xasprintf (&str, "L%iE>", global_level.number);
    b0 = b1 = false;
    if (global_level.em == DUNGEON) b0 = true;
    if (global_level.em == PALACE) b1 = true;
    em = global_level.em;
    switch (menu_bool (environment_menu, str, true, &b0, &b1)) {
    case -1: edit = EDIT_LEVEL; global_level.em = b; em = b; break;
    case 0: break;
    case 1:
      edit = EDIT_LEVEL;
      register_int_undo (&undo, (int *) &global_level.em, b, (undo_f) level_environment_undo,
                         "LEVEL ENVIRONMENT");
      break;
    default:
      if (b0) global_level.em = DUNGEON;
      if (b1) global_level.em = PALACE;
      em = global_level.em;
      break;
    }
    al_free (str);
    break;
  case EDIT_HUE:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    xasprintf (&str, "L%iH>", global_level.number);
    b0 = b1 = b2 = b3 = b4 = 0;
    if (global_level.hue == HUE_NONE) b0 = true;
    if (global_level.hue == HUE_GREEN) b1 = true;
    if (global_level.hue == HUE_GRAY) b2 = true;
    if (global_level.hue == HUE_YELLOW) b3 = true;
    if (global_level.hue == HUE_BLUE) b4 = true;
    hue = global_level.hue;
    switch (menu_bool (hue_menu, str, true, &b0, &b1, &b2, &b3, &b4)) {
    case -1: edit = EDIT_LEVEL; global_level.hue = b; hue = b; break;
    case 0: break;
    case 1:
      edit = EDIT_LEVEL;
      register_int_undo (&undo, (int *) &global_level.hue, b, (undo_f) level_hue_undo,
                         "LEVEL HUE");
      break;
    default:
      if (b0) global_level.hue = HUE_NONE;
      if (b1) global_level.hue = HUE_GREEN;
      if (b2) global_level.hue = HUE_GRAY;
      if (b3) global_level.hue = HUE_YELLOW;
      if (b4) global_level.hue = HUE_BLUE;
      hue = global_level.hue;
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD:
    g = &global_level.guard[guard_index];
    set_system_mouse_cursor (is_guard_by_type (g->type)
                             ? ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT
                             : ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
    xasprintf (&str, "G%i>", guard_index);
    switch (menu_enum (guard_menu, str)) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'G': edit = EDIT_GUARD_SELECT;
      s = guard_index; get_mouse_coord (&last_mouse_coord); break;
    case 'P':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", 12);
        break;
      }
      if (! is_valid_pos (&p)) {
        editor_msg ("SELECT CONSTRUCTION", 12);
        break;
      }
      k = get_guard_anim_by_level_id (guard_index);
      if (! k) {
        editor_msg ("NO LIVE INSTANCE", 12);
        break;
      }
      guard_resurrect (k);
      place_frame (&k->f, &k->f, get_guard_normal_bitmap (k->type), &p,
                   k->f.dir == LEFT ? +16 : +22, +14);
      place_on_the_ground (&k->f, &k->f.c);
      guard_normal (k);
      update_depressible_floor (k, -7, -26);
      break;
    case 'S':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", 12);
        break;
      }
      if (! is_valid_pos (&p)) {
        editor_msg ("SELECT CONSTRUCTION", 12);
        break;
      }
      register_guard_start_pos_undo (&undo, guard_index, &p, "GUARD START POSITION");
      break;
    case 'J':
      get_mouse_coord (&last_mouse_coord);
      mouse2guard (guard_index); break;
    case 'D':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", 12);
        break;
      }
      if (! is_pos_visible (&g->p)) {
        editor_msg ("START POS NOT VISIBLE", 12);
        break;
      }
      register_toggle_guard_start_dir_undo
        (&undo, guard_index, "GUARD START DIRECTION");
      break;
    case 'K':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", 12);
        break;
      }
      edit = EDIT_GUARD_SKILL; break;
    case 'L':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", 12);
        break;
      }
      edit = EDIT_GUARD_LIVES;
      s = g->total_lives; break;
    case 'T': edit = EDIT_GUARD_TYPE;
      b = g->type;
      b0 = (! is_guard_by_type (g->type)) ? true : false;
      b1 = (g->type == GUARD) ? true : false;
      b2 = (g->type == FAT_GUARD) ? true : false;
      b3 = (g->type == VIZIER) ? true : false;
      b4 = (g->type == SKELETON) ? true : false;
      b5 = (g->type == SHADOW) ? true : false;
      break;
    case 'Y':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", 12);
        break;
      }
      edit = EDIT_GUARD_STYLE;
      b = g->style; break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SELECT:
    mouse2guard (s);
    xasprintf (&str, "G%iG>GUARD", guard_index);
    switch (menu_int (&s, NULL, 0, GUARDS - 1, str, NULL)) {
    case -1: edit = EDIT_GUARD;
      set_mouse_coord (&last_mouse_coord); break;
    case 0: break;
    case 1:
      guard_index = s;
      edit = EDIT_GUARD;
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SKILL:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    g = &global_level.guard[guard_index];
    xasprintf (&str, "G%iK>", guard_index);
    c = menu_enum (skill_menu, str);
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_GUARD; break;
    }

    memcpy (&skill_buf, &g->skill, sizeof (skill_buf));

    switch (c) {
    case 'A': edit = EDIT_GUARD_SKILL_ATTACK;
      s = g->skill.attack_prob + 1; break;
    case 'B': edit = EDIT_GUARD_SKILL_COUNTER_ATTACK;
      s = g->skill.counter_attack_prob + 1; break;
    case 'D': edit = EDIT_GUARD_SKILL_DEFENSE;
      s = g->skill.defense_prob + 1; break;
    case 'E': edit = EDIT_GUARD_SKILL_COUNTER_DEFENSE;
      s = g->skill.counter_defense_prob + 1; break;
    case 'V': edit = EDIT_GUARD_SKILL_ADVANCE;
      s = g->skill.advance_prob + 1; break;
    case 'R': edit = EDIT_GUARD_SKILL_RETURN;
      s = g->skill.return_prob + 1; break;
    case 'F': edit = EDIT_GUARD_SKILL_REFRACTION;
      s = g->skill.refraction; break;
    case 'X': edit = EDIT_GUARD_SKILL_EXTRA_LIFE;
      s = g->skill.extra_life; break;
    case 'L': edit = EDIT_SKILL_LEGACY_TEMPLATES;
      s = 0;
      break;
    }
    al_free (str);
    break;
  case EDIT_SKILL_LEGACY_TEMPLATES:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    g = &global_level.guard[guard_index];
    xasprintf (&str, "G%iKL>L.SKILL", guard_index);
    c = menu_int (&s, NULL, 0, 11, str, NULL);
    if (! c) break;
    switch (c) {
    case -1:
      g->skill = skill_buf;
      edit = EDIT_GUARD_SKILL;
      break;
    case 1: edit = EDIT_GUARD_SKILL;
      register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                 "GUARD LEGACY SKILL");
      break;
    default:
      get_legacy_skill (s, &g->skill);
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SKILL_ATTACK:
    c = menu_skill ("KA>ATTACK", &g->skill.attack_prob, 100, EDIT_GUARD_SKILL);
    if (c == 1)
      register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                 "GUARD ATTACK SKILL");
    break;
  case EDIT_GUARD_SKILL_COUNTER_ATTACK:
    c = menu_skill ("KB>C.ATTACK", &g->skill.counter_attack_prob, 100, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD COUNTER ATTACK SKILL");
    break;
  case EDIT_GUARD_SKILL_DEFENSE:
    c = menu_skill ("KD>DEFENSE", &g->skill.defense_prob, 100, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD DEFENSE SKILL");
    break;
  case EDIT_GUARD_SKILL_COUNTER_DEFENSE:
    c = menu_skill ("KE>C.DEFENSE", &g->skill.counter_defense_prob, 100, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD COUNTER DEFENSE SKILL");
    break;
  case EDIT_GUARD_SKILL_ADVANCE:
    c = menu_skill ("KV>ADVANCE", &g->skill.advance_prob, 100, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD ADVANCE SKILL");
    break;
  case EDIT_GUARD_SKILL_RETURN:
    c = menu_skill ("KR>RETURN", &g->skill.return_prob, 100, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD RETURN SKILL");
    break;
  case EDIT_GUARD_SKILL_REFRACTION:
    c = menu_skill ("KF>REFRACTION", &g->skill.refraction, INT_MAX, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                             "GUARD REFRACTION SKILL");
    break;
  case EDIT_GUARD_SKILL_EXTRA_LIFE:
    c = menu_skill ("KX>EXT.LIFE", &g->skill.extra_life, INT_MAX, EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD EXTRA LIFE SKILL");
    break;
  case EDIT_GUARD_LIVES:
    c = menu_skill ("L>LIVES", &g->total_lives, INT_MAX, EDIT_GUARD);
    if (c == 1) register_guard_lives_undo (&undo, guard_index, s,
                                           "GUARD TOTAL LIVES");
    break;
  case EDIT_GUARD_TYPE:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    g = &global_level.guard[guard_index];
    if (! is_guard_by_type (g->type)
        && g->p.room == 0 && g->p.floor == 0 && g->p.place == 0)
      invalid_pos (&g->p);
    xasprintf (&str, "G%iT>", guard_index);
    b0 = b1 = b2 = b3 = b4 = b5 = 0;
    if (g->type == NO_ANIM) b0 = true;
    if (g->type == GUARD) b1 = true;
    if (g->type == FAT_GUARD) b2 = true;
    if (g->type == VIZIER) b3 = true;
    if (g->type == SKELETON) b4 = true;
    if (g->type == SHADOW) b5 = true;
    switch (menu_bool (guard_type_menu, str, true, &b0, &b1, &b2, &b3, &b4, &b5)) {
    case -1: edit = EDIT_GUARD; g->type = b; break;
    case 0: break;
    case 1:
      edit = EDIT_GUARD;
      if (is_guard_by_type (g->type) && ! g->total_lives)
        g->total_lives = 3;
      if (is_guard_by_type (g->type) && ! g->skill.attack_prob)
        get_legacy_skill (0, &g->skill);
      register_guard_type_undo (&undo, guard_index, b,
                                "GUARD TYPE");
      break;
    default:
      if (b0) g->type = NO_ANIM;
      if (b1) g->type = GUARD;
      if (b2) g->type = FAT_GUARD;
      if (b3) g->type = VIZIER;
      if (b4) g->type = SKELETON;
      if (b5) g->type = SHADOW;
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_STYLE:
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    g = &global_level.guard[guard_index];
    xasprintf (&str, "G%iY>STYLE", guard_index);
    switch (menu_int (&g->style, NULL, 0, 7, str, NULL)) {
    case -1: edit = EDIT_GUARD; g->style = b; break;
    case 0: break;
    case 1:
      edit = EDIT_GUARD;
      register_guard_style_undo (&undo, guard_index, b,
                                 "GUARD STYLE");
      break;
    }
    al_free (str);
    break;
  }
}

void
enter_exit_editor (void)
{
  if (edit == EDIT_NONE) enter_editor ();
  else exit_editor ();
}

void
enter_editor (void)
{
  if (cutscene) return;
  edit = last_edit;
  memset (&key, 0, sizeof (key));
  show_mouse_cursor ();
}

void
exit_editor (void)
{
  last_edit = edit;
  edit = EDIT_NONE;
  msg = NULL;
  msg_cycles = 0;
  reset_menu ();
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
  if (is_fullscreen ()) hide_mouse_cursor ();
  if (game_paused)
    draw_bottom_text (NULL, "GAME PAUSED", 0);
  else draw_bottom_text (NULL, NULL, 0);
}

static char
menu_step_ext (struct pos *p, int min, int max)
{
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  int r = con (p)->ext.step;
  char c = menu_int (&r, NULL, min, max, "CE>STEP", NULL);
  if (! c) return c;

  if (c == -1 || c == 1) {
    edit = EDIT_CON; return c;
  }

  enum changed_pos_reason reason;

  switch (con (p)->fg) {
  case SPIKES_FLOOR:
    reason = CHPOS_SPIKES;
    break;
  case DOOR:
    reason = CHPOS_OPEN_DOOR;
    break;
  case LEVEL_DOOR:
    reason = CHPOS_OPEN_LEVEL_DOOR;
    break;
  default: assert (false);
  }

  register_con_undo (&undo, p,
                     MIGNORE, MIGNORE, r,
                     true, true, false, true,
                     reason, "STEP EXTENSION");

  return c;
}

static char
menu_event_ext (struct pos *p)
{
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  int r = con (p)->ext.event;
  char c = menu_int (&r, NULL, -EVENTS, EVENTS - 1, "CE>EVENT", NULL);
  if (! c) return c;

  if (c == -1 || c == 1) {
    edit = EDIT_CON; return c;
  }

  register_con_undo (&undo, p,
                     MIGNORE, MIGNORE, r,
                     true, true, false, true,
                     CHPOS_NONE, "EVENT EXTENSION");
  return c;
}

static char
menu_select_room (enum edit up_edit, char *prefix)
{
  mr_focus_mouse ();
  int room = mr.room;
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char r = menu_int (&room, NULL, 0, ROOMS - 1, prefix, NULL);
  switch (r) {
  case -1: edit = up_edit;
    set_mouse_coord (&last_mouse_coord);
    break;
  case 0: break;
  case 1: edit = up_edit; break;
  default: set_mouse_room (room); break;
  }
  return r;
}

static char
menu_link (enum dir dir)
{
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char *prefix, c;

  switch (dir) {
  case LEFT: c = 'L'; break;
  case RIGHT: c = 'R'; break;
  case ABOVE: c = 'A'; break;
  case BELOW: c = 'B'; break;
  }

  xasprintf (&prefix, "RL%c>ROOM", c);
  char r = menu_select_room (EDIT_LINK, prefix);
  al_free (prefix);

  mr.room_select = last_mouse_coord.c.room;

  if (r == 1) {
    struct room_linking l[ROOMS];
    memcpy (&l, &global_level.link, sizeof (l));

    int room0 = last_mouse_coord.c.room;
    int room1 = mr.room;

    *roomd_ptr (&global_level, room0, dir) = room1;
    if (reciprocal_links) make_reciprocal_link (&global_level, room0, room1, dir);

    if (locally_unique_links) {
      make_link_locally_unique (&global_level, room0, dir);
      if (reciprocal_links)
        make_link_locally_unique (&global_level, room1, opposite_dir (dir));
    }

    if (globally_unique_links) {
      make_link_globally_unique (&global_level, room0, dir);
      if (reciprocal_links)
        make_link_globally_unique (&global_level, room1, opposite_dir (dir));
    }

    register_link_undo (&undo, l, "LINK");
    set_mouse_coord (&last_mouse_coord);
    mr.room_select = -1;
  } else if (r == -1) mr.room_select = -1;

  return r;
}

static void
mouse2guard (int i)
{
  struct guard *g = &global_level.guard[i];
  if (is_guard_by_type (g->type)
      && is_valid_pos (&g->p)) {
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    set_mouse_pos (&g->p);
  }
  else {
    set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
    al_set_mouse_xy (display, 0, 0);
  }
}

static char
menu_skill (char *prefix, int *skill, int max, enum edit up_edit)
{
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char *str;
  xasprintf (&str, "G%i%s", guard_index, prefix);
  int a = (max <= 100) ? *skill + 1 : *skill;
  char c = menu_int (&a, NULL, 0, max, str, NULL);
  *skill = (max <= 100) ? a - 1 : a;
  al_free (str);
  switch (c) {
  case -1: edit = up_edit;
    *skill = (max <= 100) ? s - 1 : s; break;
  case 0: break;
  case 1: edit = up_edit; break;
  }
  return c;
}

void
editor_msg (char *m, uint64_t cycles)
{
  if (edit != EDIT_NONE) {
    msg_cycles = cycles;
    msg = m;
  }
  draw_bottom_text (NULL, m, 0);
}

void
ui_place_kid (struct pos *p)
{
  if (! is_valid_pos (p)) {
    editor_msg ("SELECT CONSTRUCTION", 12);
    return;
  }
  struct anim *k = get_anim_by_id (current_kid_id);
  if (! k) {
    editor_msg ("NO LIVE INSTANCE", 12);
    return;
  }
  kid_resurrect (k);
  place_frame (&k->f, &k->f, kid_normal_00, p,
               k->f.dir == LEFT ? +22 : +28, +15);
  kid_normal (k);
  if (! game_paused) update_depressible_floor (k, -4, -10);
}
