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

static void menu_step_ext (struct pos *p, int max);
static void menu_event_ext (struct pos *p);
static int last_event;
static struct coord last_mouse_coord;
static struct pos last_event2floor_pos;
/* static struct pos last_mouse_pos = {-1,-1,-1}; */

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
     {'K', "PLACE KID"},
     {'J', "JUMP TO ROOM"},
     {'L', "ROOM LINKING"},
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

  struct pos p = mouse_pos;
  struct anim *k;

  static int b, r, s, t, min, max;
  char *fg_str = NULL, *bg_str = NULL, *ext_str = NULL;
  bool free_ext_str;
  char *str = NULL, c;

  switch (edit) {
  case EDIT_NONE: break;
  case EDIT_MAIN:
    switch (menu_enum (main_menu, NULL)) {
    case 'C': edit = EDIT_CON; break;
    case 'E': edit = EDIT_EVENT; break;
    case 'K':
      if (p.room <= 0) break;
      k = get_anim_by_id (current_kid_id);
      place_frame (&k->f, &k->f, kid_normal_00, &p,
                   k->f.dir == LEFT ? +22 : +31, +15);
      kid_normal (k);
      update_depressible_floor (k, -4, -10);
      break;
    case 'J': edit = EDIT_JUMP_ROOM; break;
    case 'L': break;
    }
    break;
  case EDIT_CON:
    switch (menu_enum (con_menu, "C>")) {
    case BACKSPACE_KEY: edit = EDIT_MAIN; break;
    case 'F': edit = EDIT_FG; break;
    case 'B': edit = EDIT_BG; break;
    case 'E': edit = EDIT_EXT; break;
    case 'I': edit = EDIT_INFO; break;
    }
    break;
  case EDIT_FG:
    switch (menu_enum (fg_menu, "CF>")) {
    case BACKSPACE_KEY: edit = EDIT_CON; break;
    case 'F': edit = EDIT_FLOOR; break;
    case 'P': edit = EDIT_PILLAR; break;
    case 'W':
      if (con (&p)->fg == WALL) break;
      destroy_con_at_pos (&p);
      con (&p)->fg = WALL;
      update_wall_cache (room_view, em, vm);
      break;
    case 'D': edit = EDIT_DOOR; break;
    case 'C':
      if (con (&p)->fg == CHOPPER) break;
      destroy_con_at_pos (&p);
      con (&p)->fg = CHOPPER;
      register_con_at_pos (&p);
      break;
    case 'M':
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

    if (c == BACKSPACE_KEY) {
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

    if (c == BACKSPACE_KEY) {
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

    if (c == BACKSPACE_KEY) {
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

    if (c == BACKSPACE_KEY) {
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

    if (c == BACKSPACE_KEY) {
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
    switch (menu_enum (bg_menu, "CB>")) {
    case BACKSPACE_KEY: edit = EDIT_CON; break;
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
    switch (con (&p)->fg) {
    case FLOOR:
      switch (menu_enum (items_menu, "CE>")) {
      case BACKSPACE_KEY: edit = EDIT_CON; break;
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

      if (c == BACKSPACE_KEY) {
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
      case BACKSPACE_KEY: edit = EDIT_CON; break;
      case '0': con (&p)->ext.design = CARPET_00; break;
      case '1': con (&p)->ext.design = CARPET_01; break;
      case 'A': con (&p)->ext.design = ARCH_CARPET_LEFT; break;
      }
      break;
    case TCARPET:
      switch (menu_enum (tcarpet_ext_menu, "CE>")) {
      case BACKSPACE_KEY: edit = EDIT_CON; break;
      case '0': con (&p)->ext.design = CARPET_00; break;
      case '1': con (&p)->ext.design = CARPET_01; break;
      case 'A': con (&p)->ext.design = ARCH_CARPET_LEFT; break;
      case 'B': con (&p)->ext.design = ARCH_CARPET_RIGHT_00; break;
      case 'C': con (&p)->ext.design = ARCH_CARPET_RIGHT_01; break;
      }
      break;
    default:
      draw_bottom_text (NULL, "NO EXTENSION");
      if (was_key_pressed (ALLEGRO_KEY_BACKSPACE, 0, 0, true))
        edit = EDIT_CON;
      break;
    }
    break;
  case EDIT_INFO:
    if (was_key_pressed (ALLEGRO_KEY_BACKSPACE, 0, 0, true))
      edit = EDIT_CON;

    free_ext_str = false;

    switch (con (&p)->fg) {
    case NO_FLOOR: fg_str = "NO FLOOR"; break;
    case FLOOR: fg_str = "FLOOR"; break;
    case BROKEN_FLOOR: fg_str = "BROKEN FLOOR"; break;
    case SKELETON_FLOOR: fg_str = "SKELETON FLOOR"; break;
    case LOOSE_FLOOR: fg_str = "LOOSE FLOOR"; break;
    case SPIKES_FLOOR: fg_str = "SPIKES FLOOR"; break;
    case OPENER_FLOOR: fg_str = "OPENER FLOOR"; break;
    case CLOSER_FLOOR: fg_str = "CLOSER FLOOR"; break;
    case STUCK_FLOOR: fg_str = "STUCK FLOOR"; break;
    case HIDDEN_FLOOR: fg_str = "HIDDEN FLOOR"; break;
    case PILLAR: fg_str = "PILLAR"; break;
    case BIG_PILLAR_BOTTOM: fg_str = "BIG PILLAR BOTTOM"; break;
    case BIG_PILLAR_TOP: fg_str = "BIG PILLAR TOP"; break;
    case WALL: fg_str = "WALL"; break;
    case DOOR: fg_str = "DOOR"; break;
    case LEVEL_DOOR: fg_str = "LEVEL DOOR"; break;
    case CHOPPER: fg_str = "CHOPPER"; break;
    case MIRROR: fg_str = "MIRROR"; break;
    case CARPET: fg_str = "CARPET"; break;
    case TCARPET: fg_str = "TCARPET"; break;
    case ARCH_BOTTOM: fg_str = "ARCH BOTTOM"; break;
    case ARCH_TOP_LEFT: fg_str = "ARCH TOP LEFT"; break;
    case ARCH_TOP_RIGHT: fg_str = "ARCH TOP RIGHT"; break;
    case ARCH_TOP_MID: fg_str = "ARCH TOP MID"; break;
    case ARCH_TOP_SMALL: fg_str = "ARCH TOP SMALL"; break;
    default: fg_str = "UNKNOWN FG"; break;
    }

    switch (con (&p)->bg) {
    case NO_BG: bg_str = "NO BG"; break;
    case BRICKS_00: bg_str = "BRICKS 00"; break;
    case BRICKS_01: bg_str = "BRICKS 01"; break;
    case BRICKS_02: bg_str = "BRICKS 02"; break;
    case BRICKS_03: bg_str = "BRICKS 03"; break;
    case NO_BRICKS: bg_str = "NO BRICKS"; break;
    case TORCH: bg_str = "TORCH"; break;
    case WINDOW: bg_str = "WINDOW"; break;
    case BALCONY: bg_str = "BALCONY"; break;
    default: bg_str = "UNKNOWN BG"; break;
    }

    switch (con (&p)->fg) {
    case FLOOR:
      switch (con (&p)->ext.item) {
      case NO_ITEM: ext_str = "NO ITEM"; break;
      case EMPTY_POTION: ext_str = "EMPTY POTION"; break;
      case SMALL_LIFE_POTION: ext_str = "SMALL LIFE POTION"; break;
      case BIG_LIFE_POTION: ext_str = "BIG LIFE POTION"; break;
      case SMALL_POISON_POTION: ext_str = "SMALL POISON POTION"; break;
      case BIG_POISON_POTION: ext_str = "BIG POISON POTION"; break;
      case FLOAT_POTION: ext_str = "FLOAT POTION"; break;
      case FLIP_POTION: ext_str = "FLIP POTION"; break;
      case ACTIVATION_POTION: ext_str = "ACTIVATION POTION"; break;
      case SWORD: ext_str = "SWORD"; break;
      default: ext_str = "UNKNOWN EXTENSION"; break;
      }
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
    case BACKSPACE_KEY: edit = EDIT_MAIN; break;
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
    if (! is_door (&p)) {
      if (was_key_pressed (ALLEGRO_KEY_BACKSPACE, 0, 0, true)
          || was_key_pressed (0, '/', 0, true))
        edit = EDIT_EVENT;
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
    if (! is_door (&p)) {
      if (was_key_pressed (ALLEGRO_KEY_BACKSPACE, 0, 0, true)
          || was_key_pressed (0, '/', 0, true))
        edit = EDIT_EVENT;
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
  case EDIT_JUMP_ROOM:
    b = -1;
    if (menu_int (&room_view, &b, 1, ROOMS - 1, "ROOM", NULL))
      edit = EDIT_MAIN;
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

void
editor_level_change (void)
{
  last_event2floor_pos = (struct pos) {-1,-1,-1};
  next_pos_by_pred (&last_event2floor_pos, 0, is_event_at_pos, &last_event);
}

static void
menu_step_ext (struct pos *p, int max)
{
  int r = con (p)->ext.step;
  int b = -1;
  if (menu_int (&r, &b, 0, max, "CE>STEP", NULL))
    edit = EDIT_CON;
  else {
    if (con (p)->ext.step != r) {
      destroy_con_at_pos (p);
      con (p)->ext.step = r;
      register_con_at_pos (p);
    }
  }
}

static void
menu_event_ext (struct pos *p)
{
  int r = con (p)->ext.event;
  int b = -1;
  if (menu_int (&r, &b, 0, EVENTS - 1, "CE>EVENT", NULL))
    edit = EDIT_CON;
  else {
    if (con (p)->ext.event != r) {
      destroy_con_at_pos (p);
      con (p)->ext.event = r;
      register_con_at_pos (p);
    }
  }
}
