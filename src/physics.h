/*
  physics.h -- physics module;

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

#ifndef MININIM_PHYSICS_H
#define MININIM_PHYSICS_H

/* functions */
struct con *con (struct pos *p);
struct con *crel (struct pos *p, int floor, int place);

enum conbg bg_val (int b);
enum confg fg_val (int f);
int ext_val (int f, int e);
struct pos *clear_con (struct pos *p);
struct pos *random_con (struct pos *p);
struct pos *decorate_con (struct pos *p);
struct pos *set_con (struct pos *p, int f, int b, int e, int ff);

struct con_copy *copy_con (struct con_copy *c, struct pos *p);
struct pos *paste_con (struct pos *p, struct con_copy *c, char *desc);
struct room_copy *copy_room (struct room_copy *rc, struct level *l, int room);
struct level *paste_room (struct level *l, int room,
                           struct room_copy *rc, char *desc);

struct pos * apply_to_pos (struct pos *p, pos_trans f, char *desc);
struct level *apply_to_room (struct level *l, int room,
                             struct pos * (*f) (struct pos *),
                             char *desc);

enum conbg bg (struct pos *p);
enum confg fg (struct pos *p);
enum confg fake (struct pos *p);
int ext (struct pos *p);
int fake_ext (struct pos *p);

enum conbg set_bg (struct pos *p, int b);
enum confg set_fg (struct pos *p, int f);
enum confg set_fake (struct pos *p, int ff);
int set_ext (struct pos *p, int e);

enum conbg bg_rel (struct pos *p, int floor, int place);
enum confg fg_rel (struct pos *p, int floor, int place);
enum confg fake_rel (struct pos *p, int floor, int place);
int ext_rel (struct pos *p, int floor, int place);

enum conbg set_bg_rel (struct pos *p, int floor,
                       int place, int b);
enum confg set_fg_rel (struct pos *p, int floor,
                       int place, int f);
int set_ext_rel (struct pos *p, int floor,
                 int place, int e);

struct level_event *event (struct level *l, int e);
struct guard *guard (struct level *l, int g);
struct room_linking *llink (struct level *l, int r);

bool is_strictly_traversable (struct pos *p);
bool is_strictly_traversable_fake (struct pos *p);
bool is_traversable (struct pos *p);
bool is_collidable_at_left (struct pos *p, struct frame *f);
bool potentially_collidable_at_left_cs (enum confg t);
bool is_potentially_collidable_at_left (struct pos *p);
bool potentially_collidable_at_right_cs (enum confg t);
bool is_potentially_collidable_at_right (struct pos *p);
bool is_collidable_at_right (struct pos *p, struct frame *f);
bool is_shockwave_medium (struct pos *p);
bool is_pillar (struct pos *p);
bool is_rigid_con (struct pos *p);
bool carpet_cs (enum confg t);
bool is_carpet (struct pos *p);
bool is_arch_top (struct pos *p);
bool strictly_traversable_cs (enum confg t);
bool traversable_cs (enum confg t);
bool arch_top_cs (enum confg t);
bool is_door (struct pos *p);
bool is_floor (struct pos *p);
bool is_floor_like (struct pos *p);
bool item_fg_cs (enum confg t);
bool is_item_fg (struct pos *p);
bool is_potion_ext (struct pos *p);
bool is_potion (struct pos *p);
bool is_sword (struct pos *p);
bool is_item (struct pos *p);
bool is_fake (struct pos *p);
struct pos *first_confg (struct pos *p0, struct pos *p1,
                         confg_set cs, struct pos *p);
struct pos *next_pos_by_pred (struct pos *p, int dir,
                              pos_pred pred, void *data);
bool is_event_at_pos (struct pos *p, void *data);
int next_int_by_pred (int *n, int dir, int min, int max,
                      int_pred pred, void *data);
bool is_pos_at_event (int e, void *_p);
void exchange_falling_floor_pos (struct pos *p0, struct pos *p1);
void exchange_event_pos (struct pos *p0, struct pos *p1);
void exchange_guard_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void exchange_kid_start_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void exchange_anim_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void invert_con_dir (struct pos *p);
void mirror_pos (struct pos *p0, struct pos *p1, bool invert_dir);
enum con_diff con_diff (struct con *c0, struct con *c1);
struct level *mirror_room_h (struct level *l, int room);
struct level *mirror_level_h (struct level *l);



bool is_immediately_accessible_pos (struct pos *to, struct pos *from,
                                    struct frame *f);
bool is_accessible (struct frame *f, coord_f cf, int dx0, int dx1);

struct frame *
move_frame (struct frame *f, coord_f cf, int dx,
            int move_left, int move_right);

bool
uncollide (struct frame *f, struct frame_offset *fo, coord_f cf,
           int left, int right,
           struct frame_offset *fo_ret,
           struct collision_info *ci_ret);
bool
uncollide_static (struct frame *f,
                  struct frame_offset *fo,
                  coord_f cf_front,
                  int front_left, int front_right,
                  coord_f cf_back,
                  int back_left, int back_right,
                  struct frame_offset *fo_ret);
int
dist_collision (struct frame *f, coord_f cf, int left, int right,
                struct collision_info *ci_ret);
void enforce_wall_collision (struct frame *f);





int dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                     int margin, bool reverse);
bool is_on_con (struct frame *f, coord_f cf, pos_f pf,
                int margin, bool reverse, int min_dist, enum confg t);
int dist_con (struct frame *f, coord_f cf, pos_f pf,
              int margin, bool reverse, enum confg t);
int
dist_chopper (struct frame *f, bool reverse);
int
dist_fall (struct frame *f, bool reverse);



bool hangable_cs (enum confg t, enum dir d);
bool is_hangable_pos (struct pos *p, enum dir d);
bool can_hang (struct frame *f, bool reverse, struct pos *hang_pos);
struct pos *get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p);
enum confg get_hanged_con (struct pos *hang_pos, enum dir d);
bool is_hang_pos_critical (struct pos *hang_pos);
bool is_hang_pos_free (struct pos *hang_pos, enum dir d);




bool is_constrained_pos (struct pos *p, struct frame *f);
bool is_in_front_level_door (struct frame *f, struct pos *p);
bool is_in_front_open_level_door (struct frame *f, struct pos *p);
bool is_falling (struct frame *f, coord_f cf, int dx0, int dx1);




void update_depressible_floor (struct anim *a, int dx0, int dx1);
void keep_depressible_floor (struct anim *a);
void clear_depressible_floor (struct anim *a);
void save_depressible_floor (struct anim *a);
void restore_depressible_floor (struct anim *a);
void press_depressible_floor (struct pos *p, struct anim *a);
void unhide_hidden_floor (struct pos *p);



void activate_con (struct pos *p);
char *get_confg_name (enum confg t);
char *get_conbg_name (enum conbg t);
char *get_item_name (enum item t);

#endif	/* MININIM_PHYSICS_H */
