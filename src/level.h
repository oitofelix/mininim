/*
  level.h -- level module;

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

#ifndef MININIM_LEVEL_H
#define MININIM_LEVEL_H

extern struct level global_level;
extern struct level vanilla_level;
extern int retry_level;
extern int camera_follow_kid;
extern int auto_rem_time_1st_cycle;
extern bool no_room_drawing, game_paused, step_one_cycle;
extern struct undo undo;
extern int next_level_number;
extern bool ignore_level_cutscene;
extern uint64_t death_timer;

void load_level (void);
void unload_level (void);
struct level *copy_level (struct level *ld, struct level *ls);
struct level *normalize_level (struct level *l);

void replace_playing_level (struct level *l);
void play_level (struct level *level);
void *con_struct_at_pos (struct pos *p);
bool should_init (struct con *c0, struct con *c1);
void init_con_at_pos (struct pos *p);
void copy_to_con_state (union con_state *to, struct pos *from_pos);
void copy_from_con_state (struct pos *to_pos, union con_state *from);
void destroy_con_at_pos (struct pos *p);
void destroy_cons (void);
void register_con_at_pos (struct pos *p);
void register_room (int room);
void register_cons (void);
void register_anims (void);
void level_cleanup (void);
bool display_remaining_time (int priority);
void display_skill (struct anim *k);
void pause_game (bool val);
bool is_game_paused (void);
void next_level (void);

/* struct level related */

bool skill_eq (struct skill *s0, struct skill *s1);
bool room_linking_eq (struct room_linking *rl0,
                      struct room_linking *rl1);
bool level_event_eq (struct level_event *le0,
                     struct level_event *le1);
bool guard_eq (struct guard *g0, struct guard *g1);
bool con_eq (struct con *c0, struct con *c1);
bool level_eq (struct level *l0, struct level *l1);

#endif	/* MININIM_LEVEL_H */
