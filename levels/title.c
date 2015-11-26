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
#include "title.h"

/* functions */
static void title_anim (void);
static void draw_princess_room (void);

static ALLEGRO_BITMAP *main_background, *presents, *author, *game_name, *copyright,
  *text_background, *in_the_absence, *princess_room, *princess_room_bed,
  *princess_room_pillar, *marry_jaffar, *credits;

static ALLEGRO_SAMPLE *main_theme, *story_1, *princess_waiting_3,
  *jaffar_appearing_3, *story_3, *door_opening, *door_gate_opening_1;

static void
title_load (void)
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
  door_gate_opening_1 = load_sample (DOOR_GATE_OPENING_1);

  /* modules */
  load_fire ();
  load_princess ();
  load_jaffar ();
  load_clock ();
}

static void
title_unload (void)
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
  al_destroy_sample (door_gate_opening_1);

  /* modules */
  unload_fire ();
  unload_princess ();
  unload_jaffar ();
  unload_clock ();
}

void
play_title (void)
{
  cutscene = true;

  title_load ();
  play_anim (title_anim, 8);
  title_unload ();
}

static void
title_anim (void)
{
  static int i = 0;

  if (enter_key) {
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
      play_sample (main_theme);
      i++;
    }
    break;
  case 2:
    if (get_sample_position () >= 2.2) {
      draw_bitmap (presents, screen, 96, 106, 0);
      i++;
    }
    break;
  case 3:
    if (get_sample_position () >= 5.4) {
      clear_bitmap (screen, BLACK);
      draw_bitmap (main_background, screen, 0, 0, 0);
      i++;
    }
    break;
  case 4:
    if (get_sample_position () >= 6.8) {
      draw_bitmap (author, screen, 96, 122, 0);
      i++;
    }
    break;
  case 5:
    if (get_sample_position () >= 11.0) {
      clear_bitmap (screen, BLACK);
      draw_bitmap (main_background, screen, 0, 0, 0);
      i++;
    }
    break;
  case 6:
    if (get_sample_position () >= 15.2) {
      draw_bitmap (game_name, screen, 24, 107, 0);
      draw_bitmap (copyright, screen, 48, 184, 0);
      i++;
    }
    break;
  case 7:
    if (! is_playing_sample ()) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (0.5));
      draw_bitmap (text_background, screen, 0, 0, 0);
      draw_pattern (screen, 17, 16, 286, 157, al_map_rgb (16, 0, 97),
                    al_map_rgb (0, 0, 0));
      draw_bitmap (in_the_absence, screen, 24, 25, 0);
      play_sample (story_1);
      i++;
    }
    break;
  case 8:
    if (get_sample_position () >= 11.0) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 9:
    if (! is_video_effect_started () && ! is_playing_sample ()) {
      princess.c.x = 142;
      princess.c.y = 124;
      princess.frame = princess_normal_00;
      princess.dir = LEFT;
      princess.action = princess_normal;

      jaffar.c.x = 321;
      jaffar.c.y = 119;
      jaffar.frame = jaffar_normal_00;
      jaffar.dir = LEFT;
      jaffar.action = jaffar_normal;

      princess_room_clock = NULL;

      draw_princess_room ();
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 10:
    if (! is_video_effect_started ()) {
      play_sample (princess_waiting_3);
      i++;
    }
    break;
  case 11:
    if (! is_playing_sample ()) {
      play_sample (door_gate_opening_1);
      i++;
    }
    break;
  case 12:
    if (! is_playing_sample ()) {
      play_sample (door_opening);
      i++;
    }
    break;
  case 13:
    if (! is_playing_sample ()) {
      princess.action = princess_turn;
      i++;
    }
    break;
  case 14:
    if (princess.frame == princess_normal_00) {
      play_sample (jaffar_appearing_3);
      jaffar.action = jaffar_walk;
      i++;
    }
    break;
  case 15:
    if (get_sample_position () >= 3.0 && jaffar.frame == jaffar_normal_00) {
      jaffar.repeat = 5;
      jaffar.action = jaffar_walk;
      i++;
    }
    break;
  case 16:
    if (get_sample_position () >= 12.0 && jaffar.frame == jaffar_normal_00) {
      jaffar.action = jaffar_open_arms;
      i++;
    }
    break;
  case 17:
    princess.action = princess_step_back;
    i++;
    break;
  case 18:
    if (princess.frame == princess_step_back_15) {
      jaffar.action = jaffar_raise_arms;
      i++;
    }
    break;
  case 19:
    if (jaffar.frame == jaffar_raise_arms_27) {
      video_effect.color = WHITE;
      start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (1));
      princess_room_clock = clock_01;
      i++;
    }
    break;
  case 20:
    if (! is_video_effect_started () && jaffar.frame == jaffar_raise_arms_29) {
      jaffar.action = jaffar_lower_arms;
      i++;
    }
    break;
  case 21:
    if (jaffar.frame == jaffar_normal_00) {
      play_sample (story_3);
      i++;
    }
    break;
  case 22:
    if (get_sample_position () >= 1.0) {
      jaffar.action = jaffar_turn_walk;
      jaffar.repeat = 5;
      i++;
    }
    break;
  case 23:
    if (get_sample_position () >= 4.0) {
      princess.action = princess_look_down;
      i++;
    }
    break;
  case 24:
    if (jaffar.frame == jaffar_normal_00) {
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
    if (! is_playing_sample ()) {
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
  /* computation */
  princess.action ();
  jaffar.action ();

  /* drawing */
  clear_bitmap (screen, BLACK);
  draw_bitmap (princess_room, screen, 0, 0, 0);
  draw_bitmap (princess_room_bed, screen, 0, 142, 0);
  draw_princess_room_stars ();
  draw_princess_room_fire ();
  draw_anim (screen, princess);
  draw_anim (screen, jaffar);
  draw_clock ();
  draw_bitmap (princess_room_pillar, screen, 240, 120, 0);
}
