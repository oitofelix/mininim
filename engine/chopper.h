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

/* dungeon ega */
#define DE_CHOPPER_01 "data/chopper/de-01.png"
#define DE_CHOPPER_FG_01 "data/chopper/de-fg-01.png"
#define DE_CHOPPER_02 "data/chopper/de-02.png"
#define DE_CHOPPER_FG_02 "data/chopper/de-fg-02.png"
#define DE_CHOPPER_03 "data/chopper/de-03.png"
#define DE_CHOPPER_FG_03 "data/chopper/de-fg-03.png"
#define DE_CHOPPER_04 "data/chopper/de-04.png"
#define DE_CHOPPER_FG_04 "data/chopper/de-fg-04.png"
#define DE_CHOPPER_05 "data/chopper/de-05.png"
#define DE_CHOPPER_FG_05 "data/chopper/de-fg-05.png"

/* dungeon vga */
#define DV_CHOPPER_01 "data/chopper/dv-01.png"
#define DV_CHOPPER_FG_01 "data/chopper/dv-fg-01.png"
#define DV_CHOPPER_02 "data/chopper/dv-02.png"
#define DV_CHOPPER_FG_02 "data/chopper/dv-fg-02.png"
#define DV_CHOPPER_03 "data/chopper/dv-03.png"
#define DV_CHOPPER_FG_03 "data/chopper/dv-fg-03.png"
#define DV_CHOPPER_04 "data/chopper/dv-04.png"
#define DV_CHOPPER_FG_04 "data/chopper/dv-fg-04.png"
#define DV_CHOPPER_05 "data/chopper/dv-05.png"
#define DV_CHOPPER_FG_05 "data/chopper/dv-fg-05.png"

/* palace vga */
#define PV_CHOPPER_01 "data/chopper/pv-01.png"
#define PV_CHOPPER_FG_01 "data/chopper/pv-fg-01.png"
#define PV_CHOPPER_02 "data/chopper/pv-02.png"
#define PV_CHOPPER_FG_02 "data/chopper/pv-fg-02.png"
#define PV_CHOPPER_03 "data/chopper/pv-03.png"
#define PV_CHOPPER_FG_03 "data/chopper/pv-fg-03.png"
#define PV_CHOPPER_04 "data/chopper/pv-04.png"
#define PV_CHOPPER_FG_04 "data/chopper/pv-fg-04.png"
#define PV_CHOPPER_05 "data/chopper/pv-05.png"
#define PV_CHOPPER_FG_05 "data/chopper/pv-fg-05.png"

/* palettable */
#define CHOPPER_BLOOD_01 "data/chopper/blood-01.png"
#define CHOPPER_BLOOD_02 "data/chopper/blood-02.png"
#define CHOPPER_BLOOD_03 "data/chopper/blood-03.png"
#define CHOPPER_BLOOD_04 "data/chopper/blood-04.png"
#define CHOPPER_BLOOD_05 "data/chopper/blood-05.png"

/* sounds */
#define CHOPPER_SAMPLE "dat/digisnd2/chopper.ogg"

/* types */
struct chopper {
  struct pos p;
  int i;
  int wait;
  bool blood;
};

void load_chopper (void);
void unload_chopper (void);
void load_chopper_samples (void);
void unload_chopper_samples (void);
void register_chopper (struct pos *p);
int compare_choppers (const void *d0, const void *d1);
struct chopper *chopper_at_pos (struct pos *p);
bool should_chomp (struct pos *p);
void compute_choppers (void);
void sample_choppers (void);
void draw_chopper_left (ALLEGRO_BITMAP *bitmap, struct pos *p,
                        enum em em, enum vm vm);
void draw_chopper_left_01 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                           enum em em, enum vm vm);
void draw_chopper_left_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                           enum em em, enum vm vm);
void draw_chopper_left_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                           enum em em, enum vm vm);
void draw_chopper_left_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                           enum em em, enum vm vm);
void draw_chopper_left_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                           enum em em, enum vm vm);
void draw_chopper_fg (ALLEGRO_BITMAP *bitmap, struct pos *p,
                      enum em em, enum vm vm);
void draw_chopper_fg_01 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                         enum em em, enum vm vm);
void draw_chopper_fg_02 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                         enum em em, enum vm vm);
void draw_chopper_fg_03 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                         enum em em, enum vm vm);
void draw_chopper_fg_04 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                         enum em em, enum vm vm);
void draw_chopper_fg_05 (ALLEGRO_BITMAP *bitmap, struct pos *p, struct chopper *ch,
                         enum em em, enum vm vm);
struct coord *chopper_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_01_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_02_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_03_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_04_coord (struct pos *p, struct coord *c);
struct coord * chopper_blood_05_coord (struct pos *p, struct coord *c);
ALLEGRO_COLOR blood_palette (ALLEGRO_COLOR c);

#endif	/* FREEPOP_CHOPPER_H */
