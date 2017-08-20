/*
  anim.h -- animation module;

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

#ifndef MININIM_ANIM_H
#define MININIM_ANIM_H

/* functions */
void play_anim (anim_callback_t draw_callback,
                anim_callback_t compute_callback,
                anim_callback_t cleanup_callback);
void pause_animation (bool val);
void cutscene_mode (bool val);
void change_anim_freq (int f);



void draw_frame (ALLEGRO_BITMAP *bitmap, struct frame *f);
void draw_xframe (ALLEGRO_BITMAP *bitmap, struct frame *f,
                  struct frame_offset *xf);
struct coord *xframe_coord (struct frame *f, struct frame_offset *xf,
                            struct coord *c);
struct frame *xframe_frame (struct frame *f, struct frame_offset *xf,
                            struct frame *nf);
struct frame *splash_frame (ALLEGRO_BITMAP *splash, struct frame *f,
                            struct frame *nf);
struct frame *next_frame (struct frame *f, struct frame *nf,
                          struct frame_offset *fo);
struct frame *prev_frame (struct frame *f, struct frame *pf);
void select_frame (struct actor *a, struct frameset *fs, int i);
void select_xframe (struct frame_offset *xf, struct frameset *fs, int j);
bool frame_eq (struct frame *f0, struct frame *f1);
struct frame *invert_frame_dir (struct frame *f0, struct frame *f1);
bool wait_anim (int cycles);


/* variables */
extern anim_callback_t anim_draw_callback, anim_compute_callback,
  anim_cleanup_callback;
extern bool pause_anim;
extern enum quit_anim quit_anim; /* set to true to quit animation */
extern bool next_frame_inv; /* invert draw_anim offset interpretation  */
extern bool cutscene; /* don't apply physics if set */
extern uint64_t anim_cycle;
extern ALLEGRO_EVENT_QUEUE *event_queue;
extern ALLEGRO_TIMER *timer;
ALLEGRO_TIMER *menu_timer;
ALLEGRO_TIMER *iup_timer;
extern int anim_freq;
extern int anim_freq_real;

#endif	/* MININIM_ANIM_H */
