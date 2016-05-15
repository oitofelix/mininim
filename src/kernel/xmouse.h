/*
  xmouse.h -- xmouse module;

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

#ifndef MININIM_XMOUSE_H
#define MININIM_XMOUSE_H

/* variables */
extern struct pos mouse_pos;

/* functions */
void init_mouse (void);
void finalize_mouse (void);
ALLEGRO_EVENT_SOURCE *get_mouse_event_source (void);
struct mouse_coord *get_mouse_coord (struct mouse_coord *m);
struct pos *get_mouse_pos (struct pos *p);
void set_mouse_coord (struct mouse_coord *m);
void set_mouse_pos (struct pos *p);
void set_mouse_room (int room);
void set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR id);
void show_mouse_cursor (void);
void hide_mouse_cursor (void);

#endif	/* MININIM_XMOUSE_H */
