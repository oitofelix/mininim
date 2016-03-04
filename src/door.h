/*
  door.h -- door module;

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

#ifndef MININIM_DOOR_H
#define MININIM_DOOR_H

/* dungeon cga */
#define DC_DOOR_LEFT "data/door/dc-left.png"
#define DC_DOOR_RIGHT "data/door/dc-right.png"
#define DC_DOOR_POLE "data/door/dc-pole.png"
#define DC_DOOR_POLE_BASE "data/door/dc-pole-base.png"
#define DC_DOOR_TOP "data/door/dc-top.png"
#define DC_DOOR_GRID "data/door/dc-grid.png"
#define DC_DOOR_GRID_TIP "data/door/dc-grid-tip.png"

/* palace cga */
#define PC_DOOR_LEFT "data/door/pc-left.png"
#define PC_DOOR_RIGHT "data/door/pc-right.png"
#define PC_DOOR_POLE "data/door/pc-pole.png"
#define PC_DOOR_POLE_BASE "data/door/pc-pole-base.png"
#define PC_DOOR_TOP "data/door/pc-top.png"
#define PC_DOOR_GRID "data/door/pc-grid.png"
#define PC_DOOR_GRID_TIP "data/door/pc-grid-tip.png"

/* dungeon ega */
#define DE_DOOR_LEFT "data/door/de-left.png"
#define DE_DOOR_RIGHT "data/door/de-right.png"
#define DE_DOOR_POLE "data/door/de-pole.png"
#define DE_DOOR_POLE_BASE "data/door/de-pole-base.png"
#define DE_DOOR_TOP "data/door/de-top.png"
#define DE_DOOR_GRID "data/door/de-grid.png"
#define DE_DOOR_GRID_TIP "data/door/de-grid-tip.png"

/* palace ega */
#define PE_DOOR_LEFT "data/door/pe-left.png"
#define PE_DOOR_RIGHT "data/door/pe-right.png"
#define PE_DOOR_POLE "data/door/pe-pole.png"
#define PE_DOOR_POLE_BASE "data/door/pe-pole-base.png"
#define PE_DOOR_TOP "data/door/pe-top.png"
#define PE_DOOR_GRID "data/door/pe-grid.png"
#define PE_DOOR_GRID_TIP "data/door/pe-grid-tip.png"

/* dungeon vga */
#define DV_DOOR_LEFT "data/door/dv-left.png"
#define DV_DOOR_RIGHT "data/door/dv-right.png"
#define DV_DOOR_POLE "data/door/dv-pole.png"
#define DV_DOOR_POLE_BASE "data/door/dv-pole-base.png"
#define DV_DOOR_TOP "data/door/dv-top.png"
#define DV_DOOR_GRID "data/door/dv-grid.png"
#define DV_DOOR_GRID_TIP "data/door/dv-grid-tip.png"

/* palace vga */
#define PV_DOOR_LEFT "data/door/pv-left.png"
#define PV_DOOR_RIGHT "data/door/pv-right.png"
#define PV_DOOR_POLE "data/door/pv-pole.png"
#define PV_DOOR_POLE_BASE "data/door/pv-pole-base.png"
#define PV_DOOR_TOP "data/door/pv-top.png"
#define PV_DOOR_GRID "data/door/pv-grid.png"
#define PV_DOOR_GRID_TIP "data/door/pv-grid-tip.png"

/* variables */
extern struct door *door;
extern size_t door_nmemb;

/* functions */
void load_door (void);
void unload_door (void);
void register_door (struct pos *p);
int compare_doors (const void *d0, const void *d1);
struct door *door_at_pos (struct pos *p);
void remove_door (struct door *d);
void open_door (int e);
void close_door (int e);
void compute_doors (void);
int door_grid_tip_y (struct pos *p);
void draw_door (ALLEGRO_BITMAP *bitmap, struct pos *p,
                enum em em, enum vm vm);
void draw_door_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_door_right (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_door_pole (ALLEGRO_BITMAP *bitmap, struct pos *p,
                     enum em em, enum vm vm);
void draw_door_pole_base (ALLEGRO_BITMAP *bitmap, struct pos *p,
                          enum em em, enum vm vm);
void draw_door_fg (ALLEGRO_BITMAP *bitmap, struct pos *p, struct frame *f,
                   enum em em, enum vm vm);
void draw_door_grid (ALLEGRO_BITMAP *bitmap, struct pos *p, int i,
                     enum em em, enum vm vm);
bool should_draw_door_grid (struct pos *p, struct frame *f);
struct coord *door_grid_coord_base (struct pos *p, struct coord *c);
struct coord *door_grid_coord (struct pos *p, struct coord *c, int j, int i);
struct coord *door_grid_tip_coord (struct pos *p, struct coord *c, int i);
struct coord *door_pole_coord (struct pos *p, struct coord *c);
struct coord *door_pole_base_coord (struct pos *p, struct coord *c);
struct coord *door_left_coord (struct pos *p, struct coord *c);
struct coord *door_right_coord (struct pos *p, struct coord *c);
struct coord *door_top_coord (struct pos *p, struct coord *c);

#endif	/* MININIM_DOOR_H */
