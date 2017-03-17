/*
  kid.h -- kid module;

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

#ifndef MININIM_KID_H
#define MININIM_KID_H

/* bitmaps */
#define V_KID_FULL_LIFE "data/life/v-kid-full.png"
#define V_KID_EMPTY_LIFE "data/life/v-kid-empty.png"
#define V_KID_SPLASH "data/splash/v-kid.png"

/* functions */
void load_kid (void);
void unload_kid (void);
struct pos *get_kid_start_pos (struct pos *p);
struct actor *create_kid (struct actor *k0, struct actor *k1,
                         struct pos *p, enum dir dir);
void destroy_kid (struct actor *k);
void draw_kid_frame (ALLEGRO_BITMAP *bitmap, struct actor *k);
void draw_start_kid (ALLEGRO_BITMAP *bitmap);
void draw_kid_lives (ALLEGRO_BITMAP *bitmap, struct actor *k);
void draw_kid_if_at_pos (ALLEGRO_BITMAP *bitmap, struct actor *k,
                         struct pos *p);
palette get_kid_palette (void);
palette get_shadow_palette (void);
ALLEGRO_COLOR colorful_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR v_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR e_phantom_shadow_palette (ALLEGRO_COLOR c);
ALLEGRO_COLOR c_phantom_shadow_palette (ALLEGRO_COLOR c);
void increase_kid_current_lives (struct actor *k);
void increase_kid_total_lives (struct actor *k);
void float_kid (struct actor *k);
ALLEGRO_COLOR get_flicker_blood_color (void);
ALLEGRO_COLOR get_flicker_raise_sword_color (void);
ALLEGRO_COLOR get_flicker_float_color (void);
void kid_debug (void);
void place_kid (struct actor *k, int room, int floor, int place);
void kid_haptic (struct actor *k, double duration);
void kid_haptic_for_range (struct pos *p, coord_f cf, double r,
                           double duration);




void init_fellow_shadow_id (void);
int fellow_shadow_count (void);
int next_fellow_shadow_style (void);
void next_fellow_shadow (int d);
void current_fellow_shadow (void);
void create_fellow_shadow (bool select);





/* variables */
extern int current_kid_id;
extern int fellow_shadow_id[FELLOW_SHADOW_NMEMB];
extern int last_fellow_shadow_id;

extern ALLEGRO_BITMAP *v_kid_full_life, *v_kid_empty_life, *v_kid_splash;

#endif	/* MININIM_KID_H */
