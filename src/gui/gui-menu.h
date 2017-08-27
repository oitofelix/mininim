/*
  gui-menu.h -- graphical user interface menu module;

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

#ifndef MININIM_GUI_MENU_H
#define MININIM_GUI_MENU_H

#define JUMP_TO_LEVEL_MID_NMEMB 21
#define REPLAY_FAVORITE_MID_NMEMB 10

#define MENU_ID_MIN 1
#define MENU_ID_MAX ((1 << 16) - 1)
#define MAIN_MENU_ID_MIN MENU_ID_MIN
#define MAIN_MENU_ID_MAX (MENU_ID_MAX / 2)
#define AUX_MENU_ID_MIN (MAIN_MENU_ID_MAX + 1)
#define AUX_MENU_ID_MAX MENU_ID_MAX

/* functions */
void init_gui_menu (void);
void finalize_gui_menu (void);

ALLEGRO_EVENT_SOURCE *main_menu_event_source (void);
void main_menu (void);

ALLEGRO_EVENT_SOURCE *aux_menu_event_source (void);
void show_aux_menu (void);
void aux_menu (void);

void dispatch_menu_event (ALLEGRO_EVENT *event);

void show_menu (void);
void hide_menu (void);
bool is_showing_menu (void);
void toggle_menu_visibility (void);

bool main_menu_enabled;

#endif	/* MININIM_GUI_MENU_H */
