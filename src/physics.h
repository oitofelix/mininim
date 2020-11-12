/*
  physics.h -- physics module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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
struct tile *tile (struct pos *p);
struct tile *tile_rel (struct pos *p, int floor, int place);

enum tile_bg bg_val (int b);
enum tile_fg fg_val (int f);
int ext_val (struct level *l, int f, int e);
union tile_state *clear_tile (struct pos *p, void *data);
union tile_state *random_tile (struct pos *p, void *data);
union tile_state *decorate_tile (struct pos *p, void *data);
union tile_state *unfake_tile (struct pos *p, void *data);
union tile_state *fg_fake_exchange (struct pos *p, void *data);

struct pos *set_tile (struct pos *p, int f, int b, int e, int ff);

struct tile_copy *copy_tile (struct tile_copy *c, struct pos *p);
union tile_state *paste_tile (struct pos *p, struct tile_copy *c);
struct room_copy *copy_room (struct room_copy *rc, struct level *l, int room);
struct level *paste_room (struct level *l, int room,
                           struct room_copy *rc, char *desc);

void apply_to_place (struct pos *p, pos_trans f, void *data, char *desc);
void apply_to_place_sel_ring
(struct sel_ring *sr, pos_trans f, void *data, char *desc);
void apply_to_room (struct level *l, int room, pos_trans f, void *data,
                    char *desc);
void apply_to_room_sel_ring
(struct sel_ring *sr, pos_trans f, void *data, char *desc);


void apply_to_level (struct level *l, pos_trans f, void *data, char *desc);
void apply_to_scope (struct pos *p, struct sel_ring *sr, pos_trans f,
                     void *data, char *base_desc, enum scope scope);

enum tile_bg bg_tile (struct tile *t);
enum tile_bg bg (struct pos *p);
enum tile_fg fg_tile (struct tile *t);
enum tile_fg fg (struct pos *p);
enum tile_fg fake_tile (struct tile *t);
enum tile_fg fake (struct pos *p);
int ext_tile (struct level *l, struct tile *t);
int ext (struct pos *p);
int fake_ext (struct pos *p);

enum tile_bg set_bg (struct pos *p, int b);
enum tile_fg set_fg (struct pos *p, int f);
enum tile_fg set_fake (struct pos *p, int ff);
int set_ext (struct pos *p, int e);

enum tile_bg bg_rel (struct pos *p, int floor, int place);
enum tile_fg fg_rel (struct pos *p, int floor, int place);
enum tile_fg fake_rel (struct pos *p, int floor, int place);
int ext_rel (struct pos *p, int floor, int place);

enum tile_bg set_bg_rel (struct pos *p, int floor,
                       int place, int b);
enum tile_fg set_fg_rel (struct pos *p, int floor,
                       int place, int f);
int set_ext_rel (struct pos *p, int floor,
                 int place, int e);

struct level_event *event (struct level *l, int e);
struct guard *guard (struct level *l, int g);
struct room_linking *llink (struct room_linking *rlink,
                            size_t room_nmemb, int room);

bool is_traversable (struct pos *p);
bool is_traversable_fake (struct pos *p);
bool is_critical (struct pos *p);
bool is_collidable_at_left (struct pos *p, struct frame *f);
bool potentially_collidable_at_left_cs (enum tile_fg t);
bool is_potentially_collidable_at_left (struct pos *p);
bool potentially_collidable_at_right_cs (enum tile_fg t);
bool is_potentially_collidable_at_right (struct pos *p);
bool is_collidable_at_right (struct pos *p, struct frame *f);
bool is_shockwave_medium (struct pos *p);
bool is_pillar (struct pos *p);
bool is_rigid_tile (struct pos *p);
bool carpet_cs (enum tile_fg t);
bool is_carpet (struct pos *p);
bool is_arch_top (struct pos *p);
bool traversable_cs (enum tile_fg t);
bool critical_cs (enum tile_fg t);
bool arch_top_cs (enum tile_fg t);
bool is_door (struct pos *p);
bool is_floor (struct pos *p);
bool is_floor_like (struct pos *p);
bool item_fg_cs (enum tile_fg t);
bool is_item_fg (struct pos *p);
bool potion_ext_cs (int t);
bool is_potion_ext (struct pos *p);
bool is_potion (struct pos *p);
bool is_sword (struct pos *p);
bool is_item (struct pos *p);
bool is_fake (struct pos *p);
bool fall_fg_cs (enum tile_fg t);
bool is_fall_fg (struct pos *p);
bool event_fg_cs (enum tile_fg t);
bool is_event_fg (struct pos *p);
bool step_fg_cs (enum tile_fg t);
bool is_step_fg (struct pos *p);
bool design_fg_cs (enum tile_fg t);
bool is_design_fg (struct pos *p);

struct pos *first_tile_fg (struct pos *p0, struct pos *p1,
                         tile_fg_set cs, struct pos *p);
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
void exchange_actor_pos (struct pos *p0, struct pos *p1, bool invert_dir);
void invert_tile_dir (struct pos *p);
void mirror_pos (struct pos *p0, struct pos *p1, bool invert_dir);
enum tile_diff tile_diff (struct level *l, struct tile *c0, struct tile *c1);
struct level *mirror_room_h (struct level *l, int room);
struct level *mirror_level_h (struct level *l);



bool is_immediately_accessible_pos (struct pos *to, struct pos *from,
                                    struct frame *f);
bool is_accessible (struct frame *f, coord_f cf, int dx0, int dx1);

struct frame *
move_frame (struct frame *f, coord_f cf, int dx,
            lua_Number move_left, lua_Number move_right);

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
void enforce_wall_collision (struct actor *a);
bool is_in_danger (struct pos *p0, struct frame *f, coord_f cf,
                   bool *misstep_ret);




int dist_next_place (struct frame *f, coord_f cf, pos_f pf,
                     int margin, bool reverse);
bool is_on_tile (struct frame *f, coord_f cf, pos_f pf,
                int margin, bool reverse, int min_dist, enum tile_fg t);
int dist_tile (struct frame *f, coord_f cf, pos_f pf,
              int margin, bool reverse, enum tile_fg t);
int
dist_chomper (struct frame *f, bool reverse);
int
dist_fall (struct frame *f, bool reverse);



bool hangable_cs (enum tile_fg t, enum dir d);
bool is_hangable (struct pos *p, enum dir d);
bool can_hang (struct actor *a, bool reverse, struct pos *hang_pos);
struct pos *get_hanged_pos (struct pos *hang_pos, enum dir d, struct pos *p);
enum tile_fg get_hanged_tile (struct pos *hang_pos, enum dir d);
bool is_free (struct pos *hang_pos, enum dir d);




bool is_constrained_pos (struct pos *p, struct frame *f);
bool is_in_front_level_door (struct frame *f, struct pos *p);
bool is_in_front_open_level_door (struct frame *f, struct pos *p);
bool is_falling (struct frame *f, coord_f cf, int dx0, int dx1);




void update_depressible_floor (struct actor *a, int dx0, int dx1);
void keep_depressible_floor (struct actor *a);
void clear_depressible_floor (struct actor *a);
void save_depressible_floor (struct actor *a);
void restore_depressible_floor (struct actor *a);
void press_depressible_floor (struct pos *p, struct actor *a);
void unhide_hidden_floor (struct pos *p);
void keep_all_depressible_floors (void);



void activate_tile (struct pos *p);

#endif	/* MININIM_PHYSICS_H */
