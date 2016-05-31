/*
  undo.h -- undo module;

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

#ifndef MININIM_UNDO_H
#define MININIM_UNDO_H

void register_undo (struct undo *u, void *data, undo_f f, char *desc);
void free_undo (struct undo *u);
bool can_undo (struct undo *u, int dir);
bool undo_pass (struct undo *u, int dir, char **desc);
void ui_undo_pass (struct undo *u, int dir, char *prefix);

/* CON */
void register_con_undo (struct undo *u, struct pos *p,
                        enum confg fg, enum conbg bg, int ext,
                        bool destroy, bool register, bool prepare,
                        bool ignore_intermediate, enum changed_pos_reason reason,
                        char *desc);
void con_undo (struct con_undo *d, int dir);

/* EXCHANGE POS */
void register_exchange_pos_undo (struct undo *u, struct pos *p0, struct pos *p1,
                                 bool prepare, bool invert_dir, char *desc);
void exchange_pos_undo (struct exchange_pos_undo *d, int dir);

/* ROOM */
void register_room_undo (struct undo *u, int room, struct con c[FLOORS][PLACES],
                         char *desc);
void room_undo (struct room_undo *d, int dir);

/* EVENT */
void
register_event_undo (struct undo *u, int e, struct pos *p, bool next,
                     char *desc);
void event_undo (struct event_undo *d, int dir);

/* HORIZONTAL ROOM CON EXCHANGE */
void register_h_room_con_exchange_undo (struct undo *u, int _room, char *desc);
void h_room_con_exchange_undo (int *room, int dir);

/* VERTICAL ROOM CON EXCHANGE */
void register_v_room_con_exchange_undo (struct undo *u, int _room, char *desc);
void v_room_con_exchange_undo (int *room, int dir);

/* RANDOM ROOM CON EXCHANGE */
void register_random_room_con_exchange_undo (struct undo *u, int _room,
                                             bool prepare, bool invert_dir,
                                             char *desc);
void random_room_con_exchange_undo (struct random_room_con_exchange_undo *d, int dir);

/* LINK */
void register_link_undo (struct undo *u, struct room_linking l[ROOMS],
                         char *desc);
void link_undo (struct link_undo *d, int dir);

/* START POSITION */
void register_start_pos_undo (struct undo *u, struct pos *p, char *desc);
void start_pos_undo (struct start_pos_undo *d, int dir);

/* TOGGLE START POSITION */
void register_toggle_start_dir_undo (struct undo *u, char *desc);
void toggle_start_dir_undo (struct start_pos_undo *d, int dir);

/* TOGGLE HAS SWORD */
void register_toggle_has_sword_undo (struct undo *u, char *desc);
void toggle_has_sword_undo (struct start_pos_undo *d, int dir);

/* GUARD START POSITION */
void register_guard_start_pos_undo (struct undo *u, int i, struct pos *p, char *desc);
void guard_start_pos_undo (struct guard_start_pos_undo *d, int dir);

/* TOGGLE GUARD START DIRECTION */
void register_toggle_guard_start_dir_undo (struct undo *u, int i, char *desc);
void toggle_guard_start_dir_undo (int *d, int dir);

/* GUARD SKILL */
void register_guard_skill_undo (struct undo *u, int i, struct skill *s, char *desc);
void guard_skill_undo (struct guard_skill_undo *d, int dir);

/* GUARD LIVES */
void register_guard_lives_undo (struct undo *u, int i, int l, char *desc);
void guard_lives_undo (struct indexed_int_undo *d, int dir);

/* GUARD TYPE */
void register_guard_type_undo (struct undo *u, int i, enum anim_type t, char *desc);
void guard_type_undo (struct indexed_int_undo *d, int dir);

/* GUARD STYLE */
void register_guard_style_undo (struct undo *u, int i, int t, char *desc);
void guard_style_undo (struct indexed_int_undo *d, int dir);

/* INT UNDO */
void register_int_undo (struct undo *u, int *f, int b, undo_f func, char *desc);
void int_undo (struct int_undo *d, int dir);

/* LEVEL ENVIRONMENT UNDO */
void level_environment_undo (struct int_undo *d, int dir);

/* LEVEL HUE UNDO */
void level_hue_undo (struct int_undo *d, int dir);

#endif	/* MININIM_UNDO_H */
