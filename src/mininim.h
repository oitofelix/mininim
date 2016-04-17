/*
  mininim.h -- MININIM main module;

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

#ifndef MININIM_MININIM_H
#define MININIM_MININIM_H

#include <config.h>

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include "error.h"
#include "argp.h"
#include "progname.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>

#include "constants.h"
#include "types.h"
#include "colors.h"

#include "array.h"
#include "audio.h"
#include "video.h"
#include "event.h"
#include "gamepad.h"
#include "xmouse.h"
#include "memory.h"
#include "random.h"
#include "timer.h"
#include "video.h"
#include "xstdio.h"
#include "file.h"
#include "dialog.h"
#include "threads.h"
#include "xconfig.h"
#include "diff.h"

#include "anim.h"
#include "arch.h"
#include "balcony.h"
#include "big-pillar.h"
#include "bricks.h"
#include "broken-floor.h"
#include "carpet.h"
#include "chopper.h"
#include "clock.h"
#include "closer-floor.h"
#include "consistency.h"
#include "cutscenes.h"
#include "door.h"
#include "fight.h"
#include "fire.h"
#include "floor.h"
#include "jaffar.h"
#include "level-door.h"
#include "level.h"
#include "loose-floor.h"
#include "mirror.h"
#include "mouse.h"
#include "opener-floor.h"
#include "physics.h"
#include "pillar.h"
#include "pos.h"
#include "potion.h"
#include "princess.h"
#include "room.h"
#include "samples.h"
#include "skeleton-floor.h"
#include "spikes-floor.h"
#include "stars.h"
#include "sword.h"
#include "torch.h"
#include "window.h"
#include "dat.h"

#include "guard.h"
#include "guard-attack.h"
#include "guard-defense.h"
#include "guard-die.h"
#include "guard-fall.h"
#include "guard-hit.h"
#include "guard-normal.h"
#include "guard-vigilant.h"
#include "guard-walkb.h"
#include "guard-walkf.h"

#include "kid.h"
#include "kid-normal.h"
#include "kid-walk.h"
#include "kid-start-run.h"
#include "kid-run.h"
#include "kid-stop-run.h"
#include "kid-turn-run.h"
#include "kid-couch.h"
#include "kid-turn.h"
#include "kid-stabilize.h"
#include "kid-jump.h"
#include "kid-vjump.h"
#include "kid-run-jump.h"
#include "kid-misstep.h"
#include "kid-hang.h"
#include "kid-hang-wall.h"
#include "kid-hang-free.h"
#include "kid-climb.h"
#include "kid-unclimb.h"
#include "kid-fall.h"
#include "kid-drink.h"
#include "kid-raise-sword.h"
#include "kid-keep-sword.h"
#include "kid-take-sword.h"
#include "kid-sword-normal.h"
#include "kid-sword-walkf.h"
#include "kid-sword-walkb.h"
#include "kid-sword-defense.h"
#include "kid-sword-attack.h"
#include "kid-sword-hit.h"
#include "kid-die.h"
#include "kid-stairs.h"

#include "native-level.h"
#include "consistency-level.h"
#include "legacy-level.h"
#include "dat-level.h"
#include "plv-level.h"

#include "wall.h"
#include "wall-dcpc.h"
#include "wall-depedv.h"
#include "wall-pv.h"

#include "menu.h"
#include "editor.h"
#include "debug.h"
#include "undo.h"
#include "multi-room.h"

/* functions */
void *load_config_dialog (ALLEGRO_THREAD *thread, void *arg);
void *save_game_dialog (ALLEGRO_THREAD *thread, void *arg);
ALLEGRO_TEXTLOG *load_config (char *filename);
void save_game (char *filename);
int max_int (int a, int b);
int min_int (int a, int b);
unsigned char add_char (unsigned char a, signed char b);
int int_to_range (int i, int min, int max);
float dist_cart (float x0, float y0, float x1, float y1);

/* variables */
extern int mr_w, mr_h;
extern enum level_module level_module;
extern ALLEGRO_TIMER *play_time;
extern enum vm vm;
extern enum gm gm;
extern enum em em;
extern bool force_em;
extern enum hue hue;
extern bool force_hue;
extern bool immortal_mode;
extern int initial_total_lives, total_lives,
  initial_current_lives, current_lives;
extern int start_level;
extern struct pos start_pos;
extern int start_level_time;
extern int time_limit;
extern struct skill skill;
extern char *resources_dir,
  *temp_dir,
  *user_home_dir,
  *user_documents_dir,
  *user_data_dir,
  *user_settings_dir,
  *system_data_dir,
  *data_dir,
  *exe_filename,
  *config_filename,
  *levels_dat_filename;

extern ALLEGRO_THREAD *load_config_dialog_thread,
  *save_game_dialog_thread;

#endif	/* MININIM_MININIM_H */
