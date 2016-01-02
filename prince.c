/*
  prince.c -- MININIM main module;

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

#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include "prince.h"
#include "kernel/video.h"
#include "kernel/audio.h"
#include "kernel/keyboard.h"
#include "kernel/timer.h"
#include "kernel/xerror.h"
#include "engine/level.h"
#include "levels/title.h"
#include "levels/level-1.h"
#include "levels/consistency-level.h"
#include "levels/legacy-level.h"

ALLEGRO_TIMER *play_time;
enum em em = DUNGEON;
enum vm vm = VGA;

char *program_name;

int
main (int argc, char **argv)
{
  program_name = "MININIM";

  al_init ();
  init_video ();
  init_audio ();
  init_keyboard ();

  draw_text (screen, "Loading....", ORIGINAL_WIDTH / 2.0, ORIGINAL_HEIGHT / 2.0,
             ALLEGRO_ALIGN_CENTRE);
  show ();

  load_level ();
  load_title ();

 restart_game:
  clear_bitmap (screen, BLACK);
  clear_bitmap (uscreen, TRANSPARENT_COLOR);
  title_started = false;
  stop_sample ();
  play_title ();
  stop_sample ();

  if (! play_time) play_time = create_timer (60.0);
  al_set_timer_count (play_time, 0);
  al_start_timer (play_time);

  /* play_level_1 (); */
  /* play_consistency_level (); */
  play_legacy_level ();
  if (quit_anim == RESTART_GAME) goto restart_game;

  unload_level ();
  unload_title ();

  finalize_video ();
  finalize_audio ();
  finalize_keyboard ();

  fprintf (stderr, "MININIM: Hope you enjoyed it!\n");

  return 0;
}

int
max_int (int a, int b)
{
  return (a > b) ? a : b;
}

int
min_int (int a, int b)
{
  return (a < b) ? a : b;
}
