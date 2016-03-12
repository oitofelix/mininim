/*
  level.h -- level module;

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

#ifndef MININIM_LEVEL_H
#define MININIM_LEVEL_H

extern struct level level;
extern int room_view;
extern int retry_level;
extern int camera_follow_kid;
extern int anti_camera_room;
extern int auto_rem_time_1st_cycle;
extern bool no_room_drawing, game_paused, step_one_cycle;
extern struct level *vanilla_level;

void load_level (void);
void unload_level (void);
void play_level (struct level *level);
void register_con_at_pos (struct pos *p);
void register_cons (void);
void destroy_con_at_pos (struct pos *p);
void destroy_cons (void);
void display_remaining_time (void);
void display_skill (struct anim *k);
void pause_game (void);
void unpause_game (void);

#endif	/* MININIM_LEVEL_H */
