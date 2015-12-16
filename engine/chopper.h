/*
  chopper.h -- chopper module;

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

#ifndef FREEPOP_CHOPPER_H
#define FREEPOP_CHOPPER_H

#include "kernel/video.h"

#define CHOPPER_WAIT 10

/* bitmaps */
#define VDUNGEON_CHOPPER_BOTTOM_01 "dat/vdungeon/chopper/frame01 bottom.png"
#define VDUNGEON_CHOPPER_TOP_01 "dat/vdungeon/chopper/frame01 top.png"
#define VDUNGEON_CHOPPER_FG_01 "dat/vdungeon/chopper/behind pillar frame01.png"
#define VDUNGEON_CHOPPER_BLOOD_01 "dat/vdungeon/chopper/blood/res1317.png"
#define VDUNGEON_CHOPPER_BOTTOM_02 "dat/vdungeon/chopper/frame02 bottom.png"
#define VDUNGEON_CHOPPER_TOP_02 "dat/vdungeon/chopper/frame02 top.png"
#define VDUNGEON_CHOPPER_FG_02 "dat/vdungeon/chopper/behind pillar frame02.png"
#define VDUNGEON_CHOPPER_BLOOD_02 "dat/vdungeon/chopper/blood/res1316.png"
#define VDUNGEON_CHOPPER_03 "dat/vdungeon/chopper/chopper frame03.png"
#define VDUNGEON_CHOPPER_FG_03 "dat/vdungeon/chopper/behind pillar frame03.png"
#define VDUNGEON_CHOPPER_BLOOD_03 "dat/vdungeon/chopper/blood/res1314.png"
#define VDUNGEON_CHOPPER_04 "dat/vdungeon/chopper/chopper frame04.png"
#define VDUNGEON_CHOPPER_FG_04 "dat/vdungeon/chopper/behind pillar frame04.png"
#define VDUNGEON_CHOPPER_BLOOD_04 "dat/vdungeon/chopper/blood/res1315.png"
#define VDUNGEON_CHOPPER_BOTTOM_05 "dat/vdungeon/chopper/frame05 bottom.png"
#define VDUNGEON_CHOPPER_TOP_05 "dat/vdungeon/chopper/frame05 top.png"
#define VDUNGEON_CHOPPER_FG_05 "dat/vdungeon/chopper/behind pillar frame05.png"
#define VDUNGEON_CHOPPER_BLOOD_05 "dat/vdungeon/chopper/blood/res1318.png"

/* sounds */
#define CHOPPER_SAMPLE "dat/digisnd2/chopper.ogg"

/* types */
struct chopper {
  struct pos p;
  int i;
  int wait;
  bool blood;
};

void load_vdungeon_chopper (void);
void unload_chopper (void);
void load_chopper_samples (void);
void unload_chopper_samples (void);
void register_chopper (struct pos *p);
int compare_choppers (const void *d0, const void *d1);
struct chopper *chopper_at_pos (struct pos *p);
bool should_chomp (struct pos *p);
void compute_choppers (void);
void sample_choppers (void);
void draw_chopper_left (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_chopper_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p);
void draw_chopper_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
void draw_chopper_fg_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch);
struct coord *chopper_top_coord (struct pos *p, struct coord *c);
struct coord *chopper_bottom_coord_01 (struct pos *p, struct coord *c);
struct coord *chopper_bottom_coord_02 (struct pos *p, struct coord *c);
struct coord *chopper_bottom_coord_05 (struct pos *p, struct coord *c);
struct coord * chopper_blood_01_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_02_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_03_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_04_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_05_coord (struct pos *p, struct coord *c);
ALLEGRO_COLOR blood_palette (ALLEGRO_COLOR c);

#endif	/* FREEPOP_CHOPPER_H */
