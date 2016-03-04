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

enum edit edit;
static struct pos last_mouse_pos = {-1,-1,-1};

void
editor (void)
{
  char *text, c;

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
    {{'F', "FOREGROUND"},
     {'B', "BACKGROUND"},
     {'E', "EXTENSION"},
     {'K', "PLACE KID"},
     {'L', "ROOM LINKING"},
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

  struct pos p = mouse_pos;
  struct anim *k;

  switch (edit) {
  case EDIT_NONE: break;
  case EDIT_MAIN:
    switch (process_menu (main_menu, NULL)) {
    case ESCAPE_KEY:
      edit = EDIT_NONE;
      draw_bottom_text (NULL, NULL);
      break;
    case 'F': edit = EDIT_FG; break;
    case 'B': break;
    case 'E': break;
    case 'K':
      k = get_anim_by_id (current_kid_id);
      place_frame (&k->f, &k->f, kid_normal_00, &p,
                   k->f.dir == LEFT ? +22 : +31, +15);
      kid_normal (k);
      update_depressible_floor (k, -4, -10);
      break;
    case 'L': break;
    default: break;
    }
    break;
  case EDIT_FG:
    switch (process_menu (fg_menu, "F>")) {
    case ESCAPE_KEY: edit = EDIT_MAIN; break;
    case 'F': edit = EDIT_FLOOR; break;
    default: break;
    }
    break;
  case EDIT_FLOOR:
    c = process_menu (floor_menu, "FF>");
    if (! c) break;

    if ((c == 'L' && con (&p)->fg == LOOSE_FLOOR)
        || (c == 'P' && con (&p)->fg == SPIKES_FLOOR)
        || (c == 'O' && con (&p)->fg == OPENER_FLOOR)
        || (c == 'C' && con (&p)->fg == CLOSER_FLOOR))
      break;

    destroy_con_at_pos (&p);

    switch (c) {
    case ESCAPE_KEY: edit = EDIT_FG; break;
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
    default: break;
    }

    register_con_at_pos (&p);

    break;
  }
}
