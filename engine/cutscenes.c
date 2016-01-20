/*
  cutscenes.c -- cutscenes module;

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

#include <stdio.h>

#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/keyboard.h"
#include "engine/anim.h"
#include "engine/stars.h"
#include "engine/fire.h"
#include "engine/princess.h"
#include "engine/jaffar.h"
#include "engine/clock.h"
#include "engine/samples.h"
#include "cutscenes.h"

/* functions */
static ALLEGRO_BITMAP *create_background_pattern_bitmap (enum vm vm);
static void title_anim (void);
static void draw_title_screen (ALLEGRO_BITMAP *bitmap, int i, enum vm vm);
static void draw_princess_room (ALLEGRO_BITMAP *bitmap, enum vm vm);

static ALLEGRO_BITMAP *c_main_background, *e_main_background, *v_main_background;
static ALLEGRO_BITMAP *c_text_background, *e_text_background, *v_text_background;
static ALLEGRO_BITMAP *c_background_pattern, *e_background_pattern, *v_background_pattern;
static ALLEGRO_BITMAP *c_princess_room, *e_princess_room, *v_princess_room;
static ALLEGRO_BITMAP *c_princess_room_pillar, *e_princess_room_pillar, *v_princess_room_pillar;

static ALLEGRO_BITMAP *message_presents, *message_author,
  *message_game_name, *message_copyright,
  *text_in_the_absence, *text_marry_jaffar, *text_credits;

bool title_started;

void
load_cutscenes (void)
{
  /* cga */
  c_main_background = load_bitmap (C_MAIN_BACKGROUND);
  c_text_background = load_bitmap (C_TEXT_BACKGROUND);
  c_background_pattern = create_background_pattern_bitmap (CGA);
  c_princess_room = load_bitmap (C_PRINCESS_ROOM);
  c_princess_room_pillar = load_bitmap (C_PRINCESS_ROOM_PILLAR);

  /* ega */
  e_main_background = load_bitmap (E_MAIN_BACKGROUND);
  e_text_background = load_bitmap (E_TEXT_BACKGROUND);
  e_background_pattern = create_background_pattern_bitmap (EGA);
  e_princess_room = load_bitmap (E_PRINCESS_ROOM);
  e_princess_room_pillar = load_bitmap (E_PRINCESS_ROOM_PILLAR);

  /* vga */
  v_main_background = load_bitmap (V_MAIN_BACKGROUND);
  v_text_background = load_bitmap (V_TEXT_BACKGROUND);
  v_background_pattern = create_background_pattern_bitmap (VGA);
  v_princess_room = load_bitmap (V_PRINCESS_ROOM);
  v_princess_room_pillar = load_bitmap (V_PRINCESS_ROOM_PILLAR);

  /* bitmaps */
  message_presents = load_bitmap (MESSAGE_PRESENTS);
  message_author = load_bitmap (MESSAGE_AUTHOR);
  message_game_name = load_bitmap (MESSAGE_GAME_NAME);
  message_copyright = load_bitmap (MESSAGE_COPYRIGHT);
  text_in_the_absence = load_bitmap (TEXT_IN_THE_ABSENCE);
  text_marry_jaffar = load_bitmap (TEXT_MARRY_JAFFAR);
  text_credits = load_bitmap (TEXT_CREDITS);

  /* modules */
  load_princess ();
  load_jaffar ();
  load_clock ();
}

void
unload_cutscenes (void)
{
  /* cga */
  al_destroy_bitmap (c_main_background);
  al_destroy_bitmap (c_text_background);
  al_destroy_bitmap (c_background_pattern);
  al_destroy_bitmap (c_princess_room);
  al_destroy_bitmap (c_princess_room_pillar);

  /* ega */
  al_destroy_bitmap (e_main_background);
  al_destroy_bitmap (e_text_background);
  al_destroy_bitmap (e_background_pattern);
  al_destroy_bitmap (e_princess_room);
  al_destroy_bitmap (e_princess_room_pillar);

  /* vga */
  al_destroy_bitmap (v_main_background);
  al_destroy_bitmap (v_text_background);
  al_destroy_bitmap (v_background_pattern);
  al_destroy_bitmap (v_princess_room);
  al_destroy_bitmap (v_princess_room_pillar);

  /* bitmap */
  al_destroy_bitmap (message_presents);
  al_destroy_bitmap (message_author);
  al_destroy_bitmap (message_game_name);
  al_destroy_bitmap (message_copyright);
  al_destroy_bitmap (text_in_the_absence);
  al_destroy_bitmap (text_marry_jaffar);
  al_destroy_bitmap (text_credits);

  /* modules */
  unload_princess ();
  unload_jaffar ();
  unload_clock ();
}

static ALLEGRO_BITMAP *
create_background_pattern_bitmap (enum vm vm)
{
  ALLEGRO_BITMAP *b = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
  clear_bitmap (b, TRANSPARENT_COLOR);

  switch (vm) {
  case CGA: break;
  case EGA: draw_pattern (b, 17, 16, 286, 157, al_map_rgb (0, 0, 168),
                          al_map_rgb (0, 0, 0)); break;
  case  VGA: draw_pattern (b, 17, 16, 286, 157, al_map_rgb (16, 0, 97),
                           al_map_rgb (0, 0, 0)); break;
  }

  return b;
}

void
play_title (void)
{
  cutscene = true;
  play_anim (title_anim, NULL, 10);
}

static void
title_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! title_started) {
    i = 0; key.keyboard.keycode = 0; title_started = true;
  }

  if (key.keyboard.keycode) {
    quit_anim = true;
    return;
  }

  switch (i) {
  case 0:
    if (! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 1:
    if (! is_video_effect_started ()) {
      si = play_sample (main_theme_sample, -1);
      i++;
    }
    break;
  case 2:
    if (get_sample_position (si) >= 2.2) i++;
    break;
  case 3:
    if (get_sample_position (si) >= 5.4) i++;
    break;
  case 4:
    if (get_sample_position (si) >= 6.8) i++;
    break;
  case 5:
    if (get_sample_position (si) >= 11.0) i++;
    break;
  case 6:
    if (get_sample_position (si) >= 15.2) i++;
    break;
  case 7:
    if (! is_playing_sample_instance (si)) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      si = play_sample (in_the_absence_sample, -1);
      i++;
    }
    break;
  case 8:
    if (get_sample_position (si) >= 11.0) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 9:
    if (! is_video_effect_started ()) i++; break;
  case 10:
    if (! is_playing_sample_instance (si)) {
      princess.f.c.x = 142;
      princess.f.c.y = 124;
      princess.f.b = princess_normal_00;
      princess.f.dir = LEFT;
      princess_normal (&princess);

      jaffar.f.c.x = 321;
      jaffar.f.c.y = 119;
      jaffar.f.b = jaffar_normal_00;
      jaffar.f.dir = LEFT;
      jaffar_normal (&jaffar);

      princess_room_clock = NULL;

      draw_princess_room (screen, vm);
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 11:
    if (! is_video_effect_started ()) {
      si = play_sample (princess_waiting_sample, -1);
      i++;
    }
    break;
  case 12:
    if (! is_playing_sample_instance (si)) {
      si = play_sample (door_close_sample, -1);
      i++;
    }
    break;
  case 13:
    if (! is_playing_sample_instance (si)) {
      si = play_sample (creaking_door_sample, -1);
      i++;
    }
    break;
  case 14:
    if (! is_playing_sample_instance (si)) {
      princess_turn (&princess);
      i++;
    }
    break;
  case 15:
    princess.action (&princess);
    if (princess.f.b == princess_normal_00) {
      si = play_sample (vizier_and_princess_sample, -1);
      jaffar_walk (&jaffar);
      i++;
    }
    break;
  case 16:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 3.0 && jaffar.f.b == jaffar_normal_00) {
      jaffar.repeat = 5;
      jaffar_walk (&jaffar);
      i++;
    }
    break;
  case 17:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 12.0 && jaffar.f.b == jaffar_normal_00) {
      jaffar_open_arms (&jaffar);
      i++;
    }
    break;
  case 18:
    princess_step_back (&princess);
    jaffar.action (&jaffar);
    i++;
    break;
  case 19:
    princess.action (&princess);
    jaffar.action (&jaffar);
    if (princess.f.b == princess_step_back_15) {
      jaffar_raise_arms (&jaffar);
      i++;
    }
    break;
  case 20:
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_raise_arms_27) {
      video_effect.color = WHITE;
      start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (1));
      princess_room_clock = clock_01;
      i++;
    }
    break;
  case 21:
    jaffar.action (&jaffar);
    if (! is_video_effect_started () && jaffar.f.b == jaffar_raise_arms_29) {
      jaffar_lower_arms (&jaffar);
      i++;
    }
    break;
  case 22:
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_normal_00) {
      si = play_sample (marry_jaffar_sample, -1);
      i++;
    }
    break;
  case 23:
    if (get_sample_position (si) >= 1.0) {
      jaffar.repeat = 6;
      jaffar_turn_walk (&jaffar);
      i++;
    }
    break;
  case 24:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 4.0) {
      princess_look_down (&princess);
      i++;
    }
    break;
  case 25:
    princess.action (&princess);
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_normal_00) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 26:
    if (! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 27:
    if (! is_playing_sample_instance (si)) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      i++;
    }
    break;
  case 28:
    if (! wait_anim (SECS_TO_SCYCLES (3))) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      i++;
    }
    break;
  case 29:
    if (! wait_anim (SECS_TO_SCYCLES (3))) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 30:
    if (! is_video_effect_started ()) i = 0; break;
  }

  if (i >= 11 && i <= 26) draw_princess_room (screen, vm);
  else draw_title_screen (screen, i, vm);
}

static void
draw_title_screen (ALLEGRO_BITMAP *bitmap, int i, enum vm vm)
{
  ALLEGRO_BITMAP *main_background = NULL,
    *text_background = NULL,
    *background_pattern = NULL;

  switch (vm) {
  case CGA:
    main_background = c_main_background;
    text_background = c_text_background;
    background_pattern = c_background_pattern;
    break;
  case EGA:
    main_background = e_main_background;
    text_background = e_text_background;
    background_pattern = e_background_pattern;
    break;
  case VGA:
    main_background = v_main_background;
    text_background = v_text_background;
    background_pattern = v_background_pattern;
    break;
  }

  if (hgc) {
    main_background = apply_palette (main_background, hgc_palette);
    text_background = apply_palette (text_background, hgc_palette);
  }

  clear_bitmap (bitmap, BLACK);
  switch (i) {
  default: break;
  case 1: case 2: case 4: case 6:
    draw_bitmap (main_background, bitmap, 0, 0, 0); break;
  case 3:
    draw_bitmap (main_background, bitmap, 0, 0, 0);
    draw_bitmap (message_presents, bitmap, 96, 106, 0); break;
  case 5:
    draw_bitmap (main_background, bitmap, 0, 0, 0);
    draw_bitmap (message_author, bitmap, 96, 122, 0); break;
  case 7: case 29: case 30:
    draw_bitmap (main_background, bitmap, 0, 0, 0);
    draw_bitmap (message_game_name, bitmap, 24, 107, 0);
    draw_bitmap (message_copyright, bitmap, 48, 184, 0); break;
  case 8: case 9:
    draw_bitmap (text_background, bitmap, 0, 0, 0);
    draw_bitmap (background_pattern, bitmap, 0, 0, 0);
    draw_bitmap (text_in_the_absence, bitmap, 24, 25, 0); break;
  case 27:
    draw_bitmap (text_background, bitmap, 0, 0, 0);
    draw_bitmap (background_pattern, bitmap, 0, 0, 0);
    draw_bitmap (text_marry_jaffar, bitmap, 24, 25, 0); break;
  case 28:
    draw_bitmap (text_background, bitmap, 0, 0, 0);
    draw_bitmap (background_pattern, bitmap, 0, 0, 0);
    draw_bitmap (text_credits, bitmap, 24, 25, 0); break;
  }

  /* printf ("step %i\n", i); */
}

static void
draw_princess_room (ALLEGRO_BITMAP *bitmap, enum vm vm)
{
  ALLEGRO_BITMAP *princess_room = NULL,
    *princess_room_pillar = NULL;

  switch (vm) {
  case CGA:
    princess_room = c_princess_room;
    princess_room_pillar = c_princess_room_pillar;
    break;
  case EGA:
    princess_room = e_princess_room;
    princess_room_pillar = e_princess_room_pillar;
    break;
  case VGA:
    princess_room = v_princess_room;
    princess_room_pillar = v_princess_room_pillar;
    break;
  }

  if (hgc) {
    princess_room = apply_palette (princess_room, hgc_palette);
    princess_room_pillar = apply_palette (princess_room_pillar, hgc_palette);
  }

  clear_bitmap (bitmap, BLACK);
  draw_bitmap (princess_room, bitmap, 0, 0, 0);
  draw_princess_room_stars (bitmap, vm);
  draw_princess_room_fire (bitmap, vm);
  draw_frame (bitmap, &princess.f);
  draw_frame (bitmap, &jaffar.f);
  draw_clock ();
  draw_bitmap (princess_room_pillar, bitmap, 245, 120, 0);
}
