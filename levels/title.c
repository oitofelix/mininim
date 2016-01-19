/*
  title.c -- title module;

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
#include "title.h"

/* functions */
static void title_anim (void);
static void draw_princess_room (void);

static ALLEGRO_BITMAP *main_background, *presents, *author, *game_name, *copyright,
  *text_background, *in_the_absence, *princess_room, *princess_room_bed,
  *princess_room_pillar, *marry_jaffar, *credits;

static ALLEGRO_SAMPLE *main_theme, *story_1, *princess_waiting_3,
  *jaffar_appearing_3, *story_3, *door_opening;

bool title_started;

void
load_title (void)
{
  /* bitmaps */
  main_background = load_bitmap (MAIN_BACKGROUND);
  presents = load_bitmap (PRESENTS);
  author = load_bitmap (AUTHOR);
  game_name = load_bitmap (GAME_NAME);
  copyright = load_bitmap (COPYRIGHT);
  text_background = load_bitmap (TEXT_BACKGROUND);
  in_the_absence = load_bitmap (IN_THE_ABSENCE);
  princess_room = load_bitmap (PRINCESS_ROOM);
  princess_room_bed = load_bitmap (PRINCESS_ROOM_BED);
  princess_room_pillar = load_bitmap (PRINCESS_ROOM_PILLAR);
  marry_jaffar = load_bitmap (MARRY_JAFFAR);
  credits = load_bitmap (CREDITS);

  /* music */
  main_theme = load_sample (MAIN_THEME);
  story_1 = load_sample (STORY_1);
  princess_waiting_3 = load_sample (PRINCESS_WAITING_3);
  jaffar_appearing_3 = load_sample (JAFFAR_APPEARING_3);
  story_3 = load_sample (STORY_3);
  door_opening = load_sample (DOOR_OPENING);

  /* modules */
  load_princess ();
  load_jaffar ();
  load_clock ();
}

void
unload_title (void)
{
  /* bitmap */
  al_destroy_bitmap (main_background);
  al_destroy_bitmap (presents);
  al_destroy_bitmap (author);
  al_destroy_bitmap (game_name);
  al_destroy_bitmap (copyright);
  al_destroy_bitmap (text_background);
  al_destroy_bitmap (in_the_absence);
  al_destroy_bitmap (princess_room);
  al_destroy_bitmap (princess_room_bed);
  al_destroy_bitmap (princess_room_pillar);
  al_destroy_bitmap (marry_jaffar);
  al_destroy_bitmap (credits);

  /* music */
  al_destroy_sample (main_theme);
  al_destroy_sample (story_1);
  al_destroy_sample (princess_waiting_3);
  al_destroy_sample (jaffar_appearing_3);
  al_destroy_sample (story_3);
  al_destroy_sample (door_opening);

  /* modules */
  unload_princess ();
  unload_jaffar ();
  unload_clock ();
}

void
play_title (void)
{
  cutscene = true;
  play_anim (title_anim, NULL, 8);
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
      clear_bitmap (screen, BLACK);
      draw_bitmap (main_background, screen, 0, 0, 0);
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 1:
    if (! is_video_effect_started ()) {
      si = play_sample (main_theme, -1);
      i++;
    }
    break;
  case 2:
    if (get_sample_position (si) >= 2.2) {
      draw_bitmap (presents, screen, 96, 106, 0);
      i++;
    }
    break;
  case 3:
    if (get_sample_position (si) >= 5.4) {
      clear_bitmap (screen, BLACK);
      draw_bitmap (main_background, screen, 0, 0, 0);
      i++;
    }
    break;
  case 4:
    if (get_sample_position (si) >= 6.8) {
      draw_bitmap (author, screen, 96, 122, 0);
      i++;
    }
    break;
  case 5:
    if (get_sample_position (si) >= 11.0) {
      clear_bitmap (screen, BLACK);
      draw_bitmap (main_background, screen, 0, 0, 0);
      i++;
    }
    break;
  case 6:
    if (get_sample_position (si) >= 15.2) {
      draw_bitmap (game_name, screen, 24, 107, 0);
      draw_bitmap (copyright, screen, 48, 184, 0);
      i++;
    }
    break;
  case 7:
    if (! is_playing_sample_instance (si)) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      draw_bitmap (text_background, screen, 0, 0, 0);
      draw_pattern (screen, 17, 16, 286, 157, al_map_rgb (16, 0, 97),
                    al_map_rgb (0, 0, 0));
      draw_bitmap (in_the_absence, screen, 24, 25, 0);
      si = play_sample (story_1, -1);
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
    if (! is_video_effect_started () && ! is_playing_sample_instance (si)) {
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

      draw_princess_room ();
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 10:
    if (! is_video_effect_started ()) {
      si = play_sample (princess_waiting_3, -1);
      i++;
    }
    break;
  case 11:
    if (! is_playing_sample_instance (si)) {
      si = play_sample (door_close_sample, -1);
      i++;
    }
    break;
  case 12:
    if (! is_playing_sample_instance (si)) {
      si = play_sample (door_opening, -1);
      i++;
    }
    break;
  case 13:
    if (! is_playing_sample_instance (si)) {
      princess_turn (&princess);
      i++;
    }
    break;
  case 14:
    princess.action (&princess);
    if (princess.f.b == princess_normal_00) {
      si = play_sample (jaffar_appearing_3, -1);
      jaffar_walk (&jaffar);
      i++;
    }
    break;
  case 15:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 3.0 && jaffar.f.b == jaffar_normal_00) {
      jaffar.repeat = 5;
      jaffar_walk (&jaffar);
      i++;
    }
    break;
  case 16:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 12.0 && jaffar.f.b == jaffar_normal_00) {
      jaffar_open_arms (&jaffar);
      i++;
    }
    break;
  case 17:
    princess_step_back (&princess);
    jaffar.action (&jaffar);
    i++;
    break;
  case 18:
    princess.action (&princess);
    jaffar.action (&jaffar);
    if (princess.f.b == princess_step_back_15) {
      jaffar_raise_arms (&jaffar);
      i++;
    }
    break;
  case 19:
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_raise_arms_27) {
      video_effect.color = WHITE;
      start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (1));
      princess_room_clock = clock_01;
      i++;
    }
    break;
  case 20:
    jaffar.action (&jaffar);
    if (! is_video_effect_started () && jaffar.f.b == jaffar_raise_arms_29) {
      jaffar_lower_arms (&jaffar);
      i++;
    }
    break;
  case 21:
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_normal_00) {
      si = play_sample (story_3, -1);
      i++;
    }
    break;
  case 22:
    if (get_sample_position (si) >= 1.0) {
      jaffar.repeat = 6;
      jaffar_turn_walk (&jaffar);
      i++;
    }
    break;
  case 23:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 4.0) {
      princess_look_down (&princess);
      i++;
    }
    break;
  case 24:
    princess.action (&princess);
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_normal_00) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 25:
    if (! is_video_effect_started ()) {
      draw_bitmap (text_background, screen, 0, 0, 0);
      draw_pattern (screen, 17, 16, 286, 157, al_map_rgb (16, 0, 97),
                    al_map_rgb (0, 0, 0));
      draw_bitmap (marry_jaffar, screen, 24, 25, 0);
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 26:
    if (! is_playing_sample_instance (si)) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      draw_bitmap (main_background, screen, 0, 0, 0);
      draw_bitmap (game_name, screen, 24, 107, 0);
      draw_bitmap (copyright, screen, 48, 184, 0);
      i++;
    }
    break;
  case 27:
    if (! wait_anim (SECS_TO_SCYCLES (3))) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      draw_bitmap (text_background, screen, 0, 0, 0);
      draw_pattern (screen, 17, 16, 286, 157, al_map_rgb (16, 0, 97),
                    al_map_rgb (0, 0, 0));
      draw_bitmap (credits, screen, 24, 25, 0);
      i++;
    }
    break;
  case 28:
    if (! wait_anim (SECS_TO_SCYCLES (3))) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i = 0;
    }
    break;
  }

  if (i >= 10 && i <= 24) draw_princess_room ();
}

static void
draw_princess_room (void)
{
  clear_bitmap (screen, BLACK);
  draw_bitmap (princess_room, screen, 0, 0, 0);
  draw_bitmap (princess_room_bed, screen, 0, 142, 0);
  draw_princess_room_stars ();
  draw_princess_room_fire ();
  draw_frame (screen, &princess.f);
  draw_frame (screen, &jaffar.f);
  draw_clock ();
  draw_bitmap (princess_room_pillar, screen, 240, 120, 0);
}
