/*
  physics.h -- physics module;

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

#ifndef MININIM_PHYSICS_H
#define MININIM_PHYSICS_H

/* functions */
struct con *xcon (struct level *l, struct pos *p);
struct con *con (struct pos *p);
struct con *crel (struct pos *p, int floor, int place);
struct con *xcrel (struct level *l, struct pos *p, int floor, int place);
bool is_strictly_traversable (struct pos *p);
bool is_traversable (struct pos *p);
bool is_pillar (struct pos *p);
bool is_rigid_con (struct pos *p);
bool is_carpet (struct pos *p);
bool is_arch_top (struct pos *p);
bool strictly_traversable_cs (enum confg t);
bool traversable_cs (enum confg t);
bool arch_top_cs (enum confg t);
bool is_door (struct pos *p);
bool is_floor (struct pos *p);
struct pos *first_confg (struct pos *p0, struct pos *p1,
                         confg_set cs, struct pos *p);
struct pos *next_pos_by_pred (struct pos *p, int dir,
                              pos_pred pred, void *data);
bool is_event_at_pos (struct pos *p, void *data);
int next_int_by_pred (int *n, int dir, int min, int max,
                      int_pred pred, void *data);
bool is_pos_at_event (int e, void *_p);
void exchange_event_pos (struct pos *p0, struct pos *p1);
void exchange_guard_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void exchange_kid_start_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void exchange_anim_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void exchange_pos (struct pos *p0, struct pos *p1, bool prepare, bool invert_dir);
void decorate_pos (struct pos *p);
enum con_diff con_diff (struct con *c0, struct con *c1);



int dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                     int margin, bool reverse);
bool is_on_con (struct frame *f, coord_f cf, pos_f pf,
                int margin, bool reverse, int min_dist, enum confg t);
int dist_con (struct frame *f, coord_f cf, pos_f pf,
              int margin, bool reverse, enum confg t);
bool
is_colliding (struct frame *f, struct frame_offset *fo, int dx,
              int reverse, struct collision_info *ci);
bool
is_colliding_cf (struct frame *f, struct frame_offset *fo, int dx,
                 int reverse, struct collision_info *ci,
                 coord_f cf);
int
dist_collision (struct frame *f, int reverse,
                struct collision_info *ci);
int
dist_chopper (struct frame *f, bool reverse);
int
dist_fall (struct frame *f, bool reverse);
struct frame_offset *
uncollide (struct frame *f, struct frame_offset *fo,
           struct frame_offset *_fo, int dx,
           int reverse, struct collision_info *ci);


bool is_hangable_con (struct pos *p, enum dir d);
bool is_hangable_pos (struct pos *p, enum dir d);
bool can_hang (struct frame *f, bool reverse, struct pos *hang_pos);
struct pos *get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p);
enum confg get_hanged_con (struct pos *hang_pos, enum dir d);
bool is_hang_pos_critical (struct pos *hang_pos);
bool is_hang_pos_free (struct pos *hang_pos, enum dir d);



void update_depressible_floor (struct anim *a, int dx0, int dx1);
void keep_depressible_floor (struct anim *a);
void clear_depressible_floor (struct anim *a);
void save_depressible_floor (struct anim *a);
void restore_depressible_floor (struct anim *a);
void press_depressible_floor (struct pos *p);
void unhide_hidden_floor (struct pos *p);



void activate_con (struct pos *p);
char *get_confg_name (enum confg t);
char *get_conbg_name (enum conbg t);
char *get_item_name (enum item t);

#endif	/* MININIM_PHYSICS_H */
