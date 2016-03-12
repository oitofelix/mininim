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

static char menu_step_ext (struct pos *p, int max);
static char menu_event_ext (struct pos *p);
static char menu_select_room (enum edit up_edit, char *prefix);
static char menu_link (enum dir dir);
static void mouse2guard (int i);
static char menu_skill (char *prefix, int *skill, int max, enum edit up_edit);

static int last_event;
static struct coord last_mouse_coord;
static struct pos last_event2floor_pos;
/* static struct pos last_mouse_pos = {-1,-1,-1}; */
static bool reciprocal_links, locally_unique_links,
  globally_unique_links;
static bool b0, b1, b2, b3, b4, b5;
static int guard_index;
static int b, r, s, t, min, max;

enum edit edit;
enum edit last_edit = EDIT_MAIN;

void
editor (void)
{
  if (edit == EDIT_NONE) return;

  /* if (last_mouse_pos.room == -1) */
  /*   last_mouse_pos = mouse_pos; */
  /* else if (! peq (&last_mouse_pos, &mouse_pos)) { */
  /*   edit = EDIT_NONE; */
  /*   draw_bottom_text (NULL, NULL); */
  /*   last_mouse_pos = (struct pos) {-1,-1,-1}; */
  /*   return; */
  /* } */

  struct menu_item main_menu[] =
    {{'C', "CONSTRUCTION"},
     {'E', "EVENT"},
     {'R', "ROOM"},
     {'K', "KID"},
     {'G', "GUARD"},
     {'L', "LEVEL"},
     {0}};

  struct menu_item con_menu[] =
    {{'F', "FOREGROUND"},
     {'B', "BACKGROUND"},
     {'E', "EXTENSION"},
     {'I', "INFO"},
     {0}};

  struct menu_item fg_menu[] =
    {{'F', "FLOOR"},
     {'P', "PILLAR"},
     {'W', "WALL"},
     {'D', "DOOR"},
     {'C', "CHOPPER"},
     {'M', "MIRROR"},
     {'R', "CARPET"},
     {'A', "ARCH"},
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
    {{'F', "FALL"},
     {'C', "CAN'T FALL"},
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
    {{'D', "EVENT->DOOR"},
     {'F', "EVENT->FLOOR"},
     {'R', "DOOR->EVENT"},
     {'S', "SET EVENT"},
     {0}};

  struct menu_item room_menu[] =
    {{'J', "JUMP TO ROOM"},
     {'L', "ROOM LINKING"},
     {'S', "LINKING SETTINGS"},
     {'X', "EXCHANGE ROOM"},
     {0}};

  struct menu_item link_menu[] =
    {{'L', "LEFT"},
     {'R', "RIGHT"},
     {'A', "ABOVE"},
     {'B', "BELOW"},
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
    {{'E', "ENVIRONMENT"},
     {'H', "HUE"},
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
    {{'S', "SELECT GUARD"},
     {'J', "JUMP TO START POSITION"},
     {'P', "SET START POSITION"},
     {'D', "TOGGLE START DIRECTION"},
     {'K', "SKILL"},
     {'L', "LIVES"},
     {'T', "TYPE"},
     {'Y', "STYLE"},
     {0}};

  struct menu_item skill_menu[] =
    {{'A', "ATTACK"},
     {'B', "COUNTER ATTACK"},
     {'D', "DEFENSE"},
     {'E', "COUNTER DEFENSE"},
     {'V', "ADVANCE"},
     {'R', "RETURN"},
     {'F', "REFRACTION"},
     {'X', "EXTRA LIFE"},
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
  struct guard *g;

  char *fg_str = NULL, *bg_str = NULL, *ext_str = NULL;
  bool free_ext_str;
  char *str = NULL, c, *f, *d;
  static char *msg = NULL;
  static uint64_t msg_cycles = 0;

  switch (edit) {
  case EDIT_NONE: break;
  case EDIT_MAIN:
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
    switch (menu_enum (con_menu, "C>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'F': edit = EDIT_FG; break;
    case 'B': edit = EDIT_BG; break;
    case 'E': edit = EDIT_EXT; break;
    case 'I': edit = EDIT_INFO; break;
    }
    break;
  case EDIT_FG:
    switch (menu_enum (fg_menu, "CF>")) {
    case -1: case 1: edit = EDIT_CON; break;
    case 'F': edit = EDIT_FLOOR; break;
    case 'P': edit = EDIT_PILLAR; break;
    case 'W':
      if (! is_valid_pos (&p)) break;
      if (con (&p)->fg == WALL) break;
      destroy_con_at_pos (&p);
      con (&p)->fg = WALL;
      update_wall_cache (room_view, em, vm);
      break;
    case 'D': edit = EDIT_DOOR; break;
    case 'C':
      if (! is_valid_pos (&p)) break;
      if (con (&p)->fg == CHOPPER) break;
      destroy_con_at_pos (&p);
      con (&p)->fg = CHOPPER;
      register_con_at_pos (&p);
      break;
    case 'M':
      if (! is_valid_pos (&p)) break;
      if (con (&p)->fg == MIRROR) break;
      destroy_con_at_pos (&p);
      con (&p)->fg = MIRROR;
      register_con_at_pos (&p);
      create_mirror_bitmaps (room_view, room_view);
      break;
    case 'R': edit = EDIT_CARPET; break;
    case 'A': edit = EDIT_ARCH; break;
    }
    break;
  case EDIT_FLOOR:
    c = menu_enum (floor_menu, "CFF>");
    if (! c) break;
    if (! is_valid_pos (&p)) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    if ((c == 'L' && con (&p)->fg == LOOSE_FLOOR)
        || (c == 'P' && con (&p)->fg == SPIKES_FLOOR)
        || (c == 'O' && con (&p)->fg == OPENER_FLOOR)
        || (c == 'C' && con (&p)->fg == CLOSER_FLOOR))
      break;

    destroy_con_at_pos (&p);

    switch (c) {
    case 'N': con (&p)->fg = NO_FLOOR; break;
    case 'F': con (&p)->fg = FLOOR; break;
    case 'B': con (&p)->fg = BROKEN_FLOOR; break;
    case 'S': con (&p)->fg = SKELETON_FLOOR; break;
    case 'L': con (&p)->fg = LOOSE_FLOOR; break;
    case 'P': con (&p)->fg = SPIKES_FLOOR; break;
    case 'O': con (&p)->fg = OPENER_FLOOR; break;
    case 'C': con (&p)->fg = CLOSER_FLOOR; break;
    case 'T': con (&p)->fg = STUCK_FLOOR; break;
    case 'H': con (&p)->fg = HIDDEN_FLOOR; break;
    }

    register_con_at_pos (&p);

    break;
  case EDIT_PILLAR:
    c = menu_enum (pillar_menu, "CFP>");
    if (! c) break;
    if (! is_valid_pos (&p)) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    destroy_con_at_pos (&p);

    switch (c) {
    case 'P': con (&p)->fg = PILLAR; break;
    case 'T': con (&p)->fg = BIG_PILLAR_TOP; break;
    case 'B': con (&p)->fg = BIG_PILLAR_BOTTOM; break;
    case 'A': con (&p)->fg = ARCH_BOTTOM; break;
    }
    break;
  case EDIT_DOOR:
    c = menu_enum (door_menu, "CFD>");
    if (! c) break;
    if (! is_valid_pos (&p)) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    if ((c == 'D' && con (&p)->fg == DOOR)
        || (c == 'L' && con (&p)->fg == LEVEL_DOOR))
      break;

    destroy_con_at_pos (&p);

    switch (c) {
    case 'D': con (&p)->fg = DOOR; break;
    case 'L': con (&p)->fg = LEVEL_DOOR; break;
    }

    register_con_at_pos (&p);

    break;
  case EDIT_CARPET:
    c = menu_enum (carpet_menu, "CFR>");
    if (! c) break;
    if (! is_valid_pos (&p)) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    destroy_con_at_pos (&p);

    switch (c) {
    case 'C': con (&p)->fg = CARPET; break;
    case 'T': con (&p)->fg = TCARPET; break;
    }

    break;
  case EDIT_ARCH:
    c = menu_enum (arch_menu, "CFA>");
    if (! c) break;
    if (! is_valid_pos (&p)) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    destroy_con_at_pos (&p);

    switch (c) {
    case 'M': con (&p)->fg = ARCH_TOP_MID; break;
    case 'S': con (&p)->fg = ARCH_TOP_SMALL; break;
    case 'L': con (&p)->fg = ARCH_TOP_LEFT; break;
    case 'R': con (&p)->fg = ARCH_TOP_RIGHT; break;
    }

    break;
  case EDIT_BG:
    c = menu_enum (bg_menu, "CB>");
    if (! is_valid_pos (&p)) break;
    switch (c) {
    case -1: case 1: edit = EDIT_CON; break;
    case 'N': con (&p)->bg = NO_BRICKS; break;
    case 'G': con (&p)->bg = NO_BG; break;
    case '0': con (&p)->bg = BRICKS_00; break;
    case '1': con (&p)->bg = BRICKS_01; break;
    case '2': con (&p)->bg = BRICKS_02; break;
    case '3': con (&p)->bg = BRICKS_03; break;
    case 'T': con (&p)->bg = TORCH; break;
    case 'W': con (&p)->bg = WINDOW; break;
    case 'B':
      con (&p)->bg = BALCONY;
      compute_stars_position (room_view, room_view);
      break;
    }
    break;
  case EDIT_EXT:
    if (! is_valid_pos (&p)) {
      draw_bottom_text (NULL, "NO EXTENSION");
      if (was_menu_return_pressed ()) edit = EDIT_CON;
      break;
    }

    switch (con (&p)->fg) {
    case FLOOR:
      switch (menu_enum (items_menu, "CE>")) {
      case -1: case 1: edit = EDIT_CON; break;
      case 'N': con (&p)->ext.item = NO_ITEM; break;
      case 'E': con (&p)->ext.item = EMPTY_POTION; break;
      case 'S': con (&p)->ext.item = SMALL_LIFE_POTION; break;
      case 'B': con (&p)->ext.item = BIG_LIFE_POTION; break;
      case 'P': con (&p)->ext.item = SMALL_POISON_POTION; break;
      case 'O': con (&p)->ext.item = BIG_POISON_POTION; break;
      case 'F': con (&p)->ext.item = FLOAT_POTION; break;
      case 'L': con (&p)->ext.item = FLIP_POTION; break;
      case 'A': con (&p)->ext.item = ACTIVATION_POTION; break;
      case 'W': con (&p)->ext.item = SWORD; break;
      }
      break;
    case LOOSE_FLOOR:
      c = menu_enum (loose_floor_ext_menu, "CE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_CON; break;
      }

      if ((c == 'F' && con (&p)->ext.cant_fall == false)
          || (c == 'C' && con (&p)->ext.cant_fall == true))
        break;

      destroy_con_at_pos (&p);

      switch (c) {
      case 'F': con (&p)->ext.cant_fall = false; break;
      case 'C': con (&p)->ext.cant_fall = true; break;
      }

      register_con_at_pos (&p);

      break;
    case SPIKES_FLOOR:
      menu_step_ext (&p, SPIKES_FLOOR_MAX_STEP);
      break;
    case OPENER_FLOOR:
      menu_event_ext (&p);
      break;
    case CLOSER_FLOOR:
      menu_event_ext (&p);
      break;
    case DOOR:
      menu_step_ext (&p, DOOR_MAX_STEP);
      break;
    case LEVEL_DOOR:
      menu_step_ext (&p, LEVEL_DOOR_MAX_STEP);
      break;
    case CHOPPER:
      r = con (&p)->ext.step;
      b = con (&p)->ext.step & 0x80;
      min = b ? 128 : 0;
      max = b ? 133 : 5;
      if (menu_int (&r, &b, min, max, "CE>STEP", "B"))
        edit = EDIT_CON;
      else if (r != con (&p)->ext.step
               || b != (con (&p)->ext.step & 0x80)) {
        destroy_con_at_pos (&p);
        con (&p)->ext.step = b ? r | 0x80 : r & ~ 0x80;
        register_con_at_pos (&p);
      }
      break;
    case CARPET:
      switch (menu_enum (carpet_ext_menu, "CE>")) {
      case -1: case 1: edit = EDIT_CON; break;
      case '0': con (&p)->ext.design = CARPET_00; break;
      case '1': con (&p)->ext.design = CARPET_01; break;
      case 'A': con (&p)->ext.design = ARCH_CARPET_LEFT; break;
      }
      break;
    case TCARPET:
      switch (menu_enum (tcarpet_ext_menu, "CE>")) {
      case -1: case 1: edit = EDIT_CON; break;
      case '0': con (&p)->ext.design = CARPET_00; break;
      case '1': con (&p)->ext.design = CARPET_01; break;
      case 'A': con (&p)->ext.design = ARCH_CARPET_LEFT; break;
      case 'B': con (&p)->ext.design = ARCH_CARPET_RIGHT_00; break;
      case 'C': con (&p)->ext.design = ARCH_CARPET_RIGHT_01; break;
      }
      break;
    default:
      draw_bottom_text (NULL, "NO EXTENSION");
      if (was_menu_return_pressed ()) edit = EDIT_CON;
      break;
    }
    break;
  case EDIT_INFO:
    if (! is_valid_pos (&p)) {
      draw_bottom_text (NULL, "SELECT CONSTRUCTION");
      if (was_menu_return_pressed ()) edit = EDIT_CON;
      break;
    }

    if (was_menu_return_pressed ()) edit = EDIT_CON;

    free_ext_str = false;

    fg_str = get_confg_name (&p);
    bg_str = get_conbg_name (&p);

    switch (con (&p)->fg) {
    case FLOOR:
      ext_str = get_item_name (&p);
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
    draw_bottom_text (NULL, str);
    al_free (str);
    if (free_ext_str) al_free (ext_str);

    break;
  case EDIT_EVENT:
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
      last_event2floor_pos = (struct pos) {-1,-1,-1};
      t = last_event;
      next_pos_by_pred (&last_event2floor_pos, 0, is_event_at_pos, &t);
      break;
    case 'R':
      edit = EDIT_DOOR2EVENT;
      t = -1;
      break;
    case 'S':
      edit = EDIT_EVENT_SET;
      s = last_event;
      b = level.event[last_event].next;
      break;
    }
    break;
  case EDIT_EVENT2DOOR:
    if (! is_valid_pos (&level.event[s].p)
        || ! is_door (&level.event[s].p)) {
      struct coord c = {room_view, 0, 0};
      set_mouse_coord (&c);
    } else set_mouse_pos (&level.event[s].p);
    b = level.event[s].next;
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
      struct coord c = {room_view, 0, 0};
      set_mouse_coord (&c);
    } else set_mouse_pos (&last_event2floor_pos);
    switch (menu_list (&s, &r, t, "EF>EVENT")) {
    case -1: set_mouse_coord (&last_mouse_coord); edit = EDIT_EVENT; break;
    case 0:
      if (s) {
        if (t + s >= 0 && t + s < EVENTS) {
          t += s;
          last_event2floor_pos = (struct pos) {-1,-1,-1};
        } else s = 0;
      }
      if (s || r)
        next_pos_by_pred (&last_event2floor_pos, r, is_event_at_pos, &t);
      break;
    case 1:
      edit = EDIT_EVENT;
      last_event = t;
      break;
    }
    break;
  case EDIT_DOOR2EVENT:
    if (! is_valid_pos (&p) || ! is_door (&p)) {
      if (was_menu_return_pressed ()) edit = EDIT_EVENT;
      draw_bottom_text (NULL, "SELECT DOOR");
      memset (&key, 0, sizeof (key));
    } else {
      if (t < 0) next_int_by_pred (&t, 0, 0, EVENTS - 1, is_pos_at_event, &p);
      switch (menu_list (NULL, &r, t, "ER>EVENT")) {
      case -1: edit = EDIT_EVENT; break;
      case 0:
        if (r) next_int_by_pred (&t, r, 0, EVENTS - 1, is_pos_at_event, &p);
        break;
      case 1: last_event = t; edit = EDIT_EVENT; break;
      }
    }
    break;
  case EDIT_EVENT_SET:
    if (! is_valid_pos (&p) || ! is_door (&p)) {
      if (was_menu_return_pressed ()) edit = EDIT_EVENT;
      draw_bottom_text (NULL, "SELECT DOOR");
      memset (&key, 0, sizeof (key));
    } else {
      switch (menu_int (&s, &b, 0, EVENTS - 1, "ES>EVENT", "N")) {
      case -1: edit = EDIT_EVENT; break;
      case 0: break;
      case 1:
        level.event[s].p = p;
        level.event[s].next = b ? true : false;
        last_event = s;
        edit = EDIT_EVENT;
        break;
      }
    }
    break;
  case EDIT_ROOM:
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
    }
    break;
  case EDIT_JUMP_ROOM:
    menu_select_room (EDIT_ROOM, "RJ>ROOM");
    break;
  case EDIT_LINK:
    switch (menu_enum (link_menu, "RL>")) {
    case -1: case 1: edit = EDIT_ROOM; break;
    case 'L':
      get_mouse_coord (&last_mouse_coord);
      room_view = roomd (room_view, LEFT);
      edit = EDIT_LINK_LEFT; break;
    case 'R':
      get_mouse_coord (&last_mouse_coord);
      room_view = roomd (room_view, RIGHT);
      edit = EDIT_LINK_RIGHT; break;
    case 'A':
      get_mouse_coord (&last_mouse_coord);
      room_view = roomd (room_view, ABOVE);
      edit = EDIT_LINK_ABOVE; break;
    case 'B':
      get_mouse_coord (&last_mouse_coord);
      room_view = roomd (room_view, BELOW);
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

    if (r == 1) {
      int room0 = last_mouse_coord.room;
      int room1 = room_view;

      exchange_rooms (room0, room1);

      last_mouse_coord.room = room1;
      set_mouse_coord (&last_mouse_coord);
      update_wall_cache (room_view, em, vm);
    }
    break;
  case EDIT_LINKING_SETTINGS:
    switch (menu_bool (linking_settings_menu, "RS>", false, &b0, &b1, &b2)) {
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
    draw_start_kid (screen, vm);
    switch (menu_enum (kid_menu, "K>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'P':
      if (! is_valid_pos (&p)) break;
      k = get_anim_by_id (current_kid_id);
      place_frame (&k->f, &k->f, kid_normal_00, &p,
                   k->f.dir == LEFT ? +22 : +28, +15);
      kid_normal (k);
      update_depressible_floor (k, -4, -10);
      break;
    case 'J':
      set_mouse_pos (&level.start_pos);
      break;
    case 'S':
      if (! is_valid_pos (&p)) break;
      level.start_pos = p;
      break;
    case 'D':
      if (room_view != level.start_pos.room) break;
      level.start_dir = (level.start_dir == LEFT) ? RIGHT : LEFT;
      break;
    case 'W':
      if (room_view != level.start_pos.room) break;
      level.has_sword = ! level.has_sword;
      break;
    }
    break;
  case EDIT_LEVEL:
    if (msg_cycles > 0 && msg) {
      msg_cycles--;
      draw_bottom_text (NULL, msg);
      if (was_menu_return_pressed ()) msg_cycles = 0;
      memset (&key, 0, sizeof (key));
      break;
    }
    switch (menu_enum (level_menu, "L>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'E': edit = EDIT_ENVIRONMENT;
      b0 = (level.em == DUNGEON) ? true : false;
      b1 = (level.em == PALACE) ? true : false;
      break;
    case 'H': edit = EDIT_HUE;
      b0 = (level.hue == HUE_NONE) ? true : false;
      b1 = (level.hue == HUE_GREEN) ? true : false;
      b2 = (level.hue == HUE_GRAY) ? true : false;
      b3 = (level.hue == HUE_YELLOW) ? true : false;
      b4 = (level.hue == HUE_BLUE) ? true : false;
      break;
    case 'S':
      msg_cycles = 18;
      xasprintf (&d, "%sdata/levels/", user_data_dir);
      if (! al_make_directory (d)) {
        error (0, al_get_errno (), "%s (%s): failed to create native level directory",
               __func__, d);
        al_free (d);
        goto save_failed;
      }
      xasprintf (&f, "%s%02d.mim", d, level.number);
      if (! save_native_level (&level, f)) {
        error (0, al_get_errno (), "%s (%s): failed to save native level file",
               __func__, f);
        al_free (f);
        al_free (d);
        goto save_failed;
      }
      *vanilla_level = level;
      msg = "LEVEL HAS BEEN SAVED";
      break;
    save_failed:
      msg = "LEVEL SAVE FAILED";
      break;
    case 'L':
      msg_cycles = 18;
      msg = "LEVEL RELOADED";
      level = *vanilla_level;
      destroy_cons ();
      register_cons ();
      break;
    }
   break;
  case EDIT_ENVIRONMENT:
    switch (menu_bool (environment_menu, "LE>", true, &b0, &b1)) {
    case -1: edit = EDIT_LEVEL; break;
    case 0: break;
    case 1:
      level.em = em;
      edit = EDIT_LEVEL;
      break;
    default:
      if (b0) em = DUNGEON;
      if (b1) em = PALACE;
      break;
    }
    break;
  case EDIT_HUE:
    switch (menu_bool (hue_menu, "LH>", true, &b0, &b1, &b2, &b3, &b4)) {
    case -1: edit = EDIT_LEVEL; break;
    case 0: break;
    case 1:
      level.hue = hue;
      edit = EDIT_LEVEL;
      break;
    default:
      if (b0) hue = HUE_NONE;
      if (b1) hue = HUE_GREEN;
      if (b2) hue = HUE_GRAY;
      if (b3) hue = HUE_YELLOW;
      if (b4) hue = HUE_BLUE;
      break;
    }
    break;
  case EDIT_GUARD:
    draw_start_guards (screen, vm);
    g = &level.guard[guard_index];
    xasprintf (&str, "G%i>", guard_index);
    switch (menu_enum (guard_menu, str)) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'S': edit = EDIT_GUARD_SELECT;
      s = guard_index; get_mouse_coord (&last_mouse_coord); break;
    case 'J': mouse2guard (guard_index); break;
    case 'P':
      if (! is_valid_pos (&p)) break;
      g->p = p;
      break;
    case 'D':
      if (room_view != g->p.room) break;
      g->dir = (g->dir == LEFT) ? RIGHT : LEFT;
      break;
    case 'K': edit = EDIT_GUARD_SKILL; break;
    case 'L': edit = EDIT_GUARD_LIVES;
      s = g->total_lives; break;
    case 'T': edit = EDIT_GUARD_TYPE;
      b = g->type;
      b0 = (g->type == NO_ANIM) ? true : false;
      b1 = (g->type == GUARD) ? true : false;
      b2 = (g->type == FAT_GUARD) ? true : false;
      b3 = (g->type == VIZIER) ? true : false;
      b4 = (g->type == SKELETON) ? true : false;
      b5 = (g->type == SHADOW) ? true : false;
      break;
    case 'Y': edit = EDIT_GUARD_STYLE;
      b = g->style; break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SELECT:
    draw_start_guards (screen, vm);
    mouse2guard (s);
    switch (menu_int (&s, NULL, 0, GUARDS - 1, "GS>GUARD", NULL)) {
    case -1: edit = EDIT_GUARD;
      set_mouse_coord (&last_mouse_coord); break;
    case 0: break;
    case 1:
      guard_index = s;
      edit = EDIT_GUARD;
      break;
    }
    break;
  case EDIT_GUARD_SKILL:
    draw_start_guards (screen, vm);
    g = &level.guard[guard_index];
    xasprintf (&str, "G%iK>", guard_index);
    switch (menu_enum (skill_menu, str)) {
    case -1: case 1: edit = EDIT_GUARD; break;
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
    }
    al_free (str);
    break;
  case EDIT_GUARD_SKILL_ATTACK:
    menu_skill
      ("KA>ATTACK", &level.guard[guard_index].skill.attack_prob,
       100, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_COUNTER_ATTACK:
    menu_skill
      ("KB>C.ATTACK", &level.guard[guard_index].skill.counter_attack_prob,
       100, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_DEFENSE:
    menu_skill
      ("KD>DEFENSE", &level.guard[guard_index].skill.defense_prob,
       100, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_COUNTER_DEFENSE:
    menu_skill
      ("KE>C.DEFENSE", &level.guard[guard_index].skill.counter_defense_prob,
       100, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_ADVANCE:
    menu_skill
      ("KV>ADVANCE", &level.guard[guard_index].skill.advance_prob,
       100, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_RETURN:
    menu_skill
      ("KR>RETURN", &level.guard[guard_index].skill.return_prob,
       100, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_REFRACTION:
    menu_skill
      ("KF>REFRACTION", &level.guard[guard_index].skill.refraction,
       INT_MAX, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_SKILL_EXTRA_LIFE:
    menu_skill
      ("KX>EXT.LIFE", &level.guard[guard_index].skill.extra_life,
       INT_MAX, EDIT_GUARD_SKILL);
    break;
  case EDIT_GUARD_LIVES:
    menu_skill
      ("L>LIVES", &level.guard[guard_index].total_lives,
       INT_MAX, EDIT_GUARD);
    break;
  case EDIT_GUARD_TYPE:
    draw_start_guards (screen, vm);
    g = &level.guard[guard_index];
    xasprintf (&str, "G%iT>", guard_index);
    switch (menu_bool (guard_type_menu, str, true, &b0, &b1, &b2, &b3, &b4, &b5)) {
    case -1: edit = EDIT_GUARD; g->type = b; break;
    case 0: break;
    case 1: edit = EDIT_GUARD; break;
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
    draw_start_guards (screen, vm);
    g = &level.guard[guard_index];
    switch (menu_int (&g->style, NULL, 0, 7, "GY>STYLE", NULL)) {
    case -1: edit = EDIT_GUARD; g->style = b; break;
    case 0: break;
    case 1: edit = EDIT_GUARD; break;
    }
    break;
  }
}

void
enter_editor (void)
{
  edit = last_edit;
  memset (&key, 0, sizeof (key));
}

void
exit_editor (void)
{
  last_edit = edit;
  edit = EDIT_NONE;
  reset_menu ();
  if (game_paused)
    draw_bottom_text (NULL, "GAME PAUSED");
  else draw_bottom_text (NULL, NULL);
}

static char
menu_step_ext (struct pos *p, int max)
{
  int r = con (p)->ext.step;
  char c = menu_int (&r, NULL, 0, max, "CE>STEP", NULL);
  if (c)
    edit = EDIT_CON;
  else {
    if (con (p)->ext.step != r) {
      destroy_con_at_pos (p);
      con (p)->ext.step = r;
      register_con_at_pos (p);
    }
  }
  return c;
}

static char
menu_event_ext (struct pos *p)
{
  int r = con (p)->ext.event;
  char c = menu_int (&r, NULL, 0, EVENTS - 1, "CE>EVENT", NULL);
  if (c)
    edit = EDIT_CON;
  else {
    if (con (p)->ext.event != r) {
      destroy_con_at_pos (p);
      con (p)->ext.event = r;
      register_con_at_pos (p);
    }
  }
  return c;
}

static char
menu_select_room (enum edit up_edit, char *prefix)
{
  char r = menu_int (&room_view, NULL, 0, ROOMS - 1, prefix, NULL);
  switch (r) {
  case -1: edit = up_edit; set_mouse_coord (&last_mouse_coord); break;
  case 0: break;
  case 1: edit = up_edit; break;
  }
  return r;
}

static char
menu_link (enum dir dir)
{
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

  if (r == 1) {
    int room0 = last_mouse_coord.room;
    int room1 = room_view;

    *roomd_ptr (room0, dir) = room1;
    if (reciprocal_links) make_reciprocal_link (room0, room1, dir);

    if (locally_unique_links) {
      make_link_locally_unique (room0, dir);
      if (reciprocal_links) make_link_locally_unique (room1, opposite_dir (dir));
    }

    if (globally_unique_links) {
      make_link_globally_unique (room0, dir);
      if (reciprocal_links) make_link_globally_unique (room1, opposite_dir (dir));
    }

    set_mouse_coord (&last_mouse_coord);
    update_wall_cache (room_view, em, vm);
  }

  return r;
}

static void
mouse2guard (int i)
{
  struct guard *g = &level.guard[i];
  if (g->type != NO_ANIM) set_mouse_pos (&g->p);
  else {
    struct coord c = {room_view, 0, 0};
    set_mouse_coord (&c);
  }
}

static char
menu_skill (char *prefix, int *skill, int max, enum edit up_edit)
{
  draw_start_guards (screen, vm);
  char *str;
  xasprintf (&str, "G%i%s", guard_index, prefix);
  char c = menu_int (&s, NULL, 0, max, str, NULL);
  al_free (str);
  switch (c) {
  case -1: edit = up_edit; break;
  case 0: break;
  case 1:
    if (max <= 100) *skill = s - 1;
    else *skill = s;
    edit = up_edit;
    break;
  }
  return c;
}
