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

#include "mininim.h"

/* functions */
static ALLEGRO_BITMAP *create_background_pattern_bitmap (ALLEGRO_COLOR c);
static void title_anim (void);
static void draw_title_screen (ALLEGRO_BITMAP *bitmap, int i, enum vm vm);
static void draw_ending_screen (ALLEGRO_BITMAP *bitmap, int i, enum vm vm);
static void draw_princess_room (ALLEGRO_BITMAP *bitmap, enum vm vm);

static ALLEGRO_BITMAP *c_main_background, *e_main_background, *v_main_background;
static ALLEGRO_BITMAP *c_text_background, *e_text_background, *v_text_background;
static ALLEGRO_BITMAP *c_title_background_pattern, *e_title_background_pattern, *v_title_background_pattern;
static ALLEGRO_BITMAP *c_ending_background_pattern, *e_ending_background_pattern, *v_ending_background_pattern;
static ALLEGRO_BITMAP *c_princess_room, *e_princess_room, *v_princess_room;
static ALLEGRO_BITMAP *c_princess_room_pillar, *e_princess_room_pillar, *v_princess_room_pillar;

static ALLEGRO_BITMAP *message_presents, *message_author,
  *message_game_name, *message_copyright,
  *text_in_the_absence, *text_marry_jaffar, *text_credits,
  *text_the_tyrant;

bool cutscene_started;

static int clock_type = -1;

struct anim mouse, kid;

void
load_cutscenes (void)
{
  /* cga */
  c_main_background = load_bitmap (C_MAIN_BACKGROUND);
  c_text_background = load_bitmap (C_TEXT_BACKGROUND);
  c_title_background_pattern = create_background_pattern_bitmap (C_TITLE_BACKGROUND_PATTERN_COLOR);
  c_ending_background_pattern = create_background_pattern_bitmap (C_ENDING_BACKGROUND_PATTERN_COLOR);
  c_princess_room = load_bitmap (C_PRINCESS_ROOM);
  c_princess_room_pillar = load_bitmap (C_PRINCESS_ROOM_PILLAR);

  /* ega */
  e_main_background = load_bitmap (E_MAIN_BACKGROUND);
  e_text_background = load_bitmap (E_TEXT_BACKGROUND);
  e_title_background_pattern = create_background_pattern_bitmap (E_TITLE_BACKGROUND_PATTERN_COLOR);
  e_ending_background_pattern = create_background_pattern_bitmap (E_ENDING_BACKGROUND_PATTERN_COLOR);
  e_princess_room = load_bitmap (E_PRINCESS_ROOM);
  e_princess_room_pillar = load_bitmap (E_PRINCESS_ROOM_PILLAR);

  /* vga */
  v_main_background = load_bitmap (V_MAIN_BACKGROUND);
  v_text_background = load_bitmap (V_TEXT_BACKGROUND);
  v_title_background_pattern = create_background_pattern_bitmap (V_TITLE_BACKGROUND_PATTERN_COLOR);
  v_ending_background_pattern = create_background_pattern_bitmap (V_ENDING_BACKGROUND_PATTERN_COLOR);
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
  text_the_tyrant = load_bitmap (TEXT_THE_TYRANT);

  /* modules */
  load_princess ();
  load_jaffar ();
  load_clock ();
}

void
unload_cutscenes (void)
{
  /* cga */
  destroy_bitmap (c_main_background);
  destroy_bitmap (c_text_background);
  destroy_bitmap (c_title_background_pattern);
  destroy_bitmap (c_ending_background_pattern);
  destroy_bitmap (c_princess_room);
  destroy_bitmap (c_princess_room_pillar);

  /* ega */
  destroy_bitmap (e_main_background);
  destroy_bitmap (e_text_background);
  destroy_bitmap (e_title_background_pattern);
  destroy_bitmap (e_ending_background_pattern);
  destroy_bitmap (e_princess_room);
  destroy_bitmap (e_princess_room_pillar);

  /* vga */
  destroy_bitmap (v_main_background);
  destroy_bitmap (v_text_background);
  destroy_bitmap (v_title_background_pattern);
  destroy_bitmap (v_ending_background_pattern);
  destroy_bitmap (v_princess_room);
  destroy_bitmap (v_princess_room_pillar);

  /* bitmap */
  destroy_bitmap (message_presents);
  destroy_bitmap (message_author);
  destroy_bitmap (message_game_name);
  destroy_bitmap (message_copyright);
  destroy_bitmap (text_in_the_absence);
  destroy_bitmap (text_marry_jaffar);
  destroy_bitmap (text_credits);
  destroy_bitmap (text_the_tyrant);

  /* modules */
  unload_princess ();
  unload_jaffar ();
  unload_clock ();
}

static ALLEGRO_BITMAP *
create_background_pattern_bitmap (ALLEGRO_COLOR c)
{
  ALLEGRO_BITMAP *b = create_bitmap (ORIGINAL_WIDTH, ORIGINAL_HEIGHT);
  clear_bitmap (b, TRANSPARENT_COLOR);
  draw_pattern (b, 17, 16, 286, 157, c, BLACK);
  validate_bitmap_for_mingw (b);
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

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if (key.keyboard.keycode
      || button != -1) {
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
      princess.f.flip = 0;
      princess.invisible = false;
      princess_normal (&princess);

      jaffar.f.c.x = 321;
      jaffar.f.c.y = 119;
      jaffar.f.b = jaffar_normal_00;
      jaffar.f.dir = LEFT;
      jaffar.f.flip = 0;
      jaffar.style = 0;
      jaffar.invisible = false;
      jaffar_normal (&jaffar);

      mouse.invisible = true;
      kid.invisible = true;

      clock_type = -1;

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
    if (get_sample_position (si) >= 4.0 && jaffar.f.b == jaffar_normal_00) {
      jaffar.repeat = 5;
      jaffar_walk (&jaffar);
      i++;
    }
    break;
  case 17:
    jaffar.action (&jaffar);
    if (get_sample_position (si) >= 12.5 && jaffar.f.b == jaffar_normal_00) {
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
    if (princess.f.b == princess_step_back_05) {
      jaffar_raise_arms (&jaffar);
      i++;
    }
    break;
  case 20:
    jaffar.action (&jaffar);
    if (jaffar.f.b == jaffar_raise_arms_06) {
      video_effect.color = WHITE;
      start_video_effect (VIDEO_FLICKERING, SECS_TO_VCYCLES (1));
      clock_type = 0;
      i++;
    }
    break;
  case 21:
    jaffar.action (&jaffar);
    if (! is_video_effect_started () && jaffar.f.b == jaffar_raise_arms_10) {
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
    if (! wait_anim (SECS_TO_SCYCLES (5))) {
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

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i >= 11 && i <= 26) draw_princess_room (screen, vm);
  else draw_title_screen (screen, i, vm);
}

void
cutscene_01_05_11_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if (key.keyboard.keycode
      || button != -1) {
    quit_anim = true;
    return;
  }

  switch (i) {
  case 0:
    princess.f.c.x = 126;
    princess.f.c.y = 124;
    princess.f.b = princess_normal_00;
    princess.f.dir = RIGHT;
    princess.f.flip = ALLEGRO_FLIP_HORIZONTAL;
    princess.invisible = false;
    princess_normal (&princess);

    jaffar.invisible = true;
    mouse.invisible = true;
    kid.invisible = true;

    clock_type = get_clock_by_time_left ();

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_01_03_05_11_sample, -1);
    i++;
    break;
  case 1:
    if (get_sample_position (si) >= 5.5
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 2:
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) quit_anim = true;
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 2 || is_video_effect_started ())
    draw_princess_room (screen, vm);
}

void
cutscene_11_little_time_left_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if (key.keyboard.keycode
      || button != -1) {
    quit_anim = true;
    return;
  }

  switch (i) {
  case 0:
    princess.f.c.x = 200;
    princess.f.c.y = 124;
    princess.f.b = princess_normal_00;
    princess.f.dir = LEFT;
    princess.f.flip = 0;
    princess.invisible = false;
    princess_normal (&princess);

    jaffar.invisible = true;
    mouse.invisible = true;
    kid.invisible = true;

    clock_type = get_clock_by_time_left ();

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_11_little_time_left_sample, -1);
    i++;
    break;
  case 1:
    if (get_sample_position (si) >= 2.5) {
      princess_turn (&princess);
      i++;
    }
    break;
  case 2:
    princess.action (&princess);
    if (get_sample_position (si) >= 5.5
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 3:
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) quit_anim = true;
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 3 || is_video_effect_started ())
    draw_princess_room (screen, vm);
}

void
cutscene_11_anim (void)
{
  if (al_get_timer_count (play_time) >= (92 * time_limit) / 100)
    cutscene_11_little_time_left_anim ();
  else cutscene_01_05_11_anim ();
}

void
cutscene_03_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if (key.keyboard.keycode
      || button != -1) {
    quit_anim = true;
    return;
  }

  switch (i) {
  case 0:
    princess.f.c.x = 77;
    princess.f.c.y = 138;
    princess.f.b = princess_rest_00;
    princess.f.dir = RIGHT;
    princess.f.flip = 0;
    princess.invisible = false;
    princess_rest (&princess);

    jaffar.invisible = true;
    mouse.invisible = true;
    kid.invisible = true;

    clock_type = get_clock_by_time_left ();

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_01_03_05_11_sample, -1);
    i++;
    break;
  case 1:
    if (get_sample_position (si) >= 5.5
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 2:
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) quit_anim = true;
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 2 || is_video_effect_started ())
    draw_princess_room (screen, vm);
}

void
cutscene_07_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if (key.keyboard.keycode
      || button != -1) {
    quit_anim = true;
    return;
  }

  switch (i) {
  case 0:
    princess.f.c.x = 170;
    princess.f.c.y = 147;
    princess.f.b = princess_couch_10;
    princess.f.dir = RIGHT;
    princess.f.flip = 0;
    princess.invisible = false;
    princess.action = princess_couch;

    jaffar.invisible = true;

    mouse.f.c.x = 196;
    mouse.f.c.y = 164;
    mouse.f.b = mouse_run_00;
    mouse.f.dir = LEFT;
    mouse.f.flip = 0;
    mouse.action = mouse_run;
    mouse.invisible = false;

    kid.invisible = true;

    clock_type = get_clock_by_time_left ();

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_07_08_sample, -1);
    i++;
    break;
  case 1:
    if (get_sample_position (si) >= 3.5) {
      mouse_normal (&mouse);
      i++;
    }
    break;
  case 2:
    mouse.action (&mouse);
    if (get_sample_position (si) >= 5.5) {
      princess_uncouch (&princess);
      i++;
    }
    break;
  case 3:
    mouse.action (&mouse);
    princess.action (&princess);
    if (get_sample_position (si) >= 7.5
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 4:
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) quit_anim = true;
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 4 || is_video_effect_started ())
    draw_princess_room (screen, vm);
}

void
cutscene_08_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if (key.keyboard.keycode
      || button != -1) {
    quit_anim = true;
    return;
  }

  switch (i) {
  case 0:
    princess.f.c.x = 178;
    princess.f.c.y = 127;
    princess.f.b = princess_normal_00;
    princess.f.dir = RIGHT;
    princess.f.flip = ALLEGRO_FLIP_HORIZONTAL;
    princess.invisible = false;
    princess.action = princess_normal;

    jaffar.invisible = true;

    mouse.f.c.x = 320;
    mouse.f.c.y = 160;
    mouse.f.b = mouse_normal_00;
    mouse.f.dir = RIGHT;
    mouse.f.flip = ALLEGRO_FLIP_HORIZONTAL;
    mouse.action = mouse_normal;
    mouse.invisible = false;

    kid.invisible = true;

    clock_type = get_clock_by_time_left ();

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_07_08_sample, -1);
    i++;
    break;
  case 1:
    if (get_sample_position (si) >= 1) i++;
    break;
  case 2:
    mouse.action (&mouse);
    if (get_sample_position (si) >= 2.5) {
      princess_couch (&princess);
      i++;
    }
    break;
  case 3:
    if (mouse.f.c.x >= 204) mouse.action (&mouse);
    else if (princess.j) {
      mouse_normal (&mouse);
      mouse.f.c.x = 199;
      mouse.f.c.y = 160;
      princess_stroke (&princess);
      i++;
    }
    princess.action (&princess);
    break;
  case 4:
    princess.action (&princess);
    if (get_sample_position (si) >= 7.5
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 5:
    princess.action (&princess);
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) quit_anim = true;
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 5 || is_video_effect_started ())
    draw_princess_room (screen, vm);
}

void
cutscene_14_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  if ((key.keyboard.keycode || button != -1)
      && i > 5) {
    quit_anim = RESTART_GAME;
    return;
  }

  switch (i) {
  case 0:
    princess.f.c.x = 164;
    princess.f.c.y = 129;
    princess.f.b = princess_turn_embrace_00;
    princess.f.dir = LEFT;
    princess.f.flip = 0;
    princess.i = 0;
    princess.invisible = false;
    princess.action = princess_turn_embrace;

    jaffar.invisible = true;

    mouse.f.c.x = 320;
    mouse.f.c.y = 160;
    mouse.f.b = mouse_normal_00;
    mouse.f.dir = RIGHT;
    mouse.f.flip = ALLEGRO_FLIP_HORIZONTAL;
    mouse.action = mouse_normal;
    mouse.invisible = false;

    kid.f.c.x = 316;
    kid.f.c.y = 129;
    kid.f.b = kid_start_run_frameset[5].frame;
    kid.f.dir = LEFT;
    kid.f.flip = 0;
    kid.action = kid_start_run;
    kid.invisible = false;
    kid.key.left = true;
    kid.current_lives = kid.total_lives = 3;
    kid_run (&kid);

    clock_type = -1;

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_14_sample, -1);
    i++;
    break;
  case 1:
    kid.action (&kid);
    if (get_sample_position (si) >= 1) {
      kid.key.left = false;
      i++;
    }
    break;
  case 2:
    princess.action (&princess);
    if (princess.i < 8) kid.action (&kid);
    else kid.invisible = true;
    if (princess.i == 13) i++;
    break;
  case 3:
    if (mouse.f.c.x >= 228) mouse.action (&mouse);
    else {
      mouse_normal (&mouse);
      mouse.f.c.x = 224;
      mouse.f.c.y = 160;
      i++;
    }
    break;
  case 4:
    princess.action (&princess);
    if (get_sample_position (si) >= 10
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 5:
    princess.action (&princess);
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
      si = play_sample (happy_end_sample, -1);
      key.keyboard.keycode = 0;
      button = -1;
      i++;
    }
    break;
  case 6:
    if (get_sample_position (si) >= 26) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 7:
    if (get_sample_position (si) >= 52) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 8:
    if (get_sample_position (si) >= 80) {
      start_video_effect (VIDEO_ROLL_RIGHT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 9:
    if (get_sample_position (si) >= 111.1) i++;
    break;
  case 10:
    if (! is_playing_sample_instance (si)) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 11:
    if (! is_video_effect_started ()) {
      quit_anim = RESTART_GAME;
      return;
    }
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 5 || (i == 5 && is_video_effect_started ()))
    draw_princess_room (screen, vm);
  else draw_ending_screen (screen, i, vm);
}

void
cutscene_out_of_time_anim (void)
{
  static int i;
  static ALLEGRO_SAMPLE_INSTANCE *si = NULL;

  if (! cutscene_started) {
    i = 0; key.keyboard.keycode = 0; button = -1; cutscene_started = true;
  }

  /* if (key.keyboard.keycode) { */
  /*   quit_anim = RESTART_GAME; */
  /*   return; */
  /* } */

  switch (i) {
  case 0:
    princess.invisible = true;
    jaffar.invisible = true;
    mouse.invisible = true;
    kid.invisible = true;

    clock_type = get_clock_by_time_left ();

    start_video_effect (VIDEO_FADE_IN, SECS_TO_VCYCLES (1));
    si = play_sample (cutscene_out_of_time_sample, -1);
    i++;
    break;
  case 1:
    if (get_sample_position (si) >= 12
        && ! is_video_effect_started ()) {
      start_video_effect (VIDEO_FADE_OUT, SECS_TO_VCYCLES (1));
      i++;
    }
    break;
  case 2:
    if (! is_playing_sample_instance (si)
        && ! is_video_effect_started ()) quit_anim = RESTART_GAME;
    break;
  }

  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
  if (i < 2 || is_video_effect_started ())
    draw_princess_room (screen, vm);
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
    background_pattern = c_title_background_pattern;
    break;
  case EGA:
    main_background = e_main_background;
    text_background = e_text_background;
    background_pattern = e_title_background_pattern;
    break;
  case VGA:
    main_background = v_main_background;
    text_background = v_text_background;
    background_pattern = v_title_background_pattern;
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
draw_ending_screen (ALLEGRO_BITMAP *bitmap, int i, enum vm vm)
{
  ALLEGRO_BITMAP *main_background = NULL,
    *text_background = NULL,
    *background_pattern = NULL;

  switch (vm) {
  case CGA:
    main_background = c_main_background;
    text_background = c_text_background;
    background_pattern = c_ending_background_pattern;
    break;
  case EGA:
    main_background = e_main_background;
    text_background = e_text_background;
    background_pattern = e_ending_background_pattern;
    break;
  case VGA:
    main_background = v_main_background;
    text_background = v_text_background;
    background_pattern = v_ending_background_pattern;
    break;
  }

  if (hgc) {
    main_background = apply_palette (main_background, hgc_palette);
    text_background = apply_palette (text_background, hgc_palette);
  }

  clear_bitmap (bitmap, BLACK);
  switch (i) {
  default: break;
  case 6:
    draw_bitmap (text_background, bitmap, 0, 0, 0);
    draw_bitmap (background_pattern, bitmap, 0, 0, 0);
    draw_bitmap (text_the_tyrant, bitmap, 24, 25, 0); break;
  case 7: case 9:
    draw_bitmap (main_background, bitmap, 0, 0, 0); break;
    break;
  case 8:
    draw_bitmap (text_background, bitmap, 0, 0, 0);
    draw_bitmap (background_pattern, bitmap, 0, 0, 0);
    draw_bitmap (text_credits, bitmap, 24, 25, 0); break;
  case 10: case 11:
    draw_bitmap (main_background, bitmap, 0, 0, 0);
    draw_bitmap (message_game_name, bitmap, 24, 107, 0); break;
    break;
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
  draw_princess_frame (bitmap, &princess, vm);
  draw_jaffar_frame (bitmap, &jaffar, vm);
  draw_mouse_frame (bitmap, &mouse, vm);
  draw_kid_frame (bitmap, &kid, vm);
  draw_clock (bitmap, clock_type, vm);
  draw_bitmap (princess_room_pillar, bitmap, 245, 120, 0);
}
