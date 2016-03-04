/*
  level-door.h -- level door module;

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

#ifndef MININIM_LEVEL_DOOR_H
#define MININIM_LEVEL_DOOR_H

/* dungeon cga */
#define DC_LEVEL_DOOR_TOP_LEFT "data/level-door/dc-top-left.png"
#define DC_LEVEL_DOOR_TOP_RIGHT "data/level-door/dc-top-right.png"
#define DC_LEVEL_DOOR_BOTTOM "data/level-door/dc-bottom.png"
#define DC_LEVEL_DOOR_FLOOR "data/level-door/dc-floor.png"
#define DC_LEVEL_DOOR_STAIRS "data/level-door/dc-stairs.png"
#define DC_LEVEL_DOOR_FRONT "data/level-door/dc-front.png"

/* palace cga */
#define PC_LEVEL_DOOR_TOP_LEFT "data/level-door/pc-top-left.png"
#define PC_LEVEL_DOOR_TOP_RIGHT "data/level-door/pc-top-right.png"
#define PC_LEVEL_DOOR_BOTTOM "data/level-door/pc-bottom.png"
#define PC_LEVEL_DOOR_FLOOR "data/level-door/pc-floor.png"
#define PC_LEVEL_DOOR_STAIRS "data/level-door/pc-stairs.png"
#define PC_LEVEL_DOOR_FRONT "data/level-door/pc-front.png"

/* dungeon ega */
#define DE_LEVEL_DOOR_TOP_LEFT "data/level-door/de-top-left.png"
#define DE_LEVEL_DOOR_TOP_RIGHT "data/level-door/de-top-right.png"
#define DE_LEVEL_DOOR_BOTTOM "data/level-door/de-bottom.png"
#define DE_LEVEL_DOOR_FLOOR "data/level-door/de-floor.png"
#define DE_LEVEL_DOOR_STAIRS "data/level-door/de-stairs.png"
#define DE_LEVEL_DOOR_FRONT "data/level-door/de-front.png"

/* palace ega */
#define PE_LEVEL_DOOR_TOP_LEFT "data/level-door/pe-top-left.png"
#define PE_LEVEL_DOOR_TOP_RIGHT "data/level-door/pe-top-right.png"
#define PE_LEVEL_DOOR_BOTTOM "data/level-door/pe-bottom.png"
#define PE_LEVEL_DOOR_FLOOR "data/level-door/pe-floor.png"
#define PE_LEVEL_DOOR_STAIRS "data/level-door/pe-stairs.png"
#define PE_LEVEL_DOOR_FRONT "data/level-door/pe-front.png"

/* dungeon vga */
#define DV_LEVEL_DOOR_TOP_LEFT "data/level-door/dv-top-left.png"
#define DV_LEVEL_DOOR_TOP_RIGHT "data/level-door/dv-top-right.png"
#define DV_LEVEL_DOOR_BOTTOM "data/level-door/dv-bottom.png"
#define DV_LEVEL_DOOR_FLOOR "data/level-door/dv-floor.png"
#define DV_LEVEL_DOOR_STAIRS "data/level-door/dv-stairs.png"
#define DV_LEVEL_DOOR_FRONT "data/level-door/dv-front.png"

/* palace vga */
#define PV_LEVEL_DOOR_TOP_LEFT "data/level-door/pv-top-left.png"
#define PV_LEVEL_DOOR_TOP_RIGHT "data/level-door/pv-top-right.png"
#define PV_LEVEL_DOOR_BOTTOM "data/level-door/pv-bottom.png"
#define PV_LEVEL_DOOR_FLOOR "data/level-door/pv-floor.png"
#define PV_LEVEL_DOOR_STAIRS "data/level-door/pv-stairs.png"
#define PV_LEVEL_DOOR_FRONT "data/level-door/pv-front.png"

/* variables */
extern struct level_door *level_door;
extern size_t level_door_nmemb;

/* functions */
void load_level_door (void);
void unload_level_door (void);
void register_level_door (struct pos *p);
int compare_level_doors (const void *d0, const void *d1);
struct level_door *level_door_at_pos (struct pos *p);
void remove_level_door (struct level_door *d);
void break_level_door (struct pos *p);
void compute_level_doors (void);
struct level_door *get_exit_level_door (int n);
void draw_level_door (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_level_door_front (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                            enum em em, enum vm vm);
void draw_level_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                         enum em em, enum vm vm);
void draw_level_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                           enum em em, enum vm vm);
void draw_level_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                            enum em em, enum vm vm);
struct coord *level_door_front_coord_base (struct pos *p, struct coord *c);
struct coord *level_door_front_coord (struct pos *p, struct coord *c, int j, int i);
struct coord *d_level_door_top_left_coord (struct pos *p, struct coord *c);
struct coord *p_level_door_top_left_coord (struct pos *p, struct coord *c);
struct coord *d_level_door_top_right_coord (struct pos *p, struct coord *c);
struct coord *p_level_door_top_right_coord (struct pos *p, struct coord *c);
struct coord *d_level_door_bottom_left_coord (struct pos *p, struct coord *c);
struct coord *p_level_door_bottom_left_coord (struct pos *p, struct coord *c);
struct coord *d_level_door_bottom_right_coord (struct pos *p, struct coord *c);
struct coord *p_level_door_bottom_right_coord (struct pos *p, struct coord *c);
struct coord *level_door_stairs_coord (struct pos *p, struct coord *c);
struct coord *level_door_floor_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_LEVEL_DOOR_H */
