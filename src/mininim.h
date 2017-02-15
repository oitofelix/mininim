/*
  mininim.h -- MININIM main module;

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

#ifndef MININIM_MININIM_H
#define MININIM_MININIM_H

#include <config.h>

/* C Library */

#include <stdio.h>
#include <string.h>
#include <strings.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <float.h>
#include <assert.h>
#include <math.h>
#include <time.h>
#include <glob.h>
#include <locale.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <wchar.h>
#include "error.h"
#include "argp.h"
#include "fnmatch.h"
#include "getpass.h"
#include "nonblocking.h"
#include "progname.h"
#include "vasnprintf.h"
#include "xprintf.h"
#include "xvasprintf.h"

/* Allegro */
#define ALLEGRO_UNSTABLE

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_color.h>

/* Lua */
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

/* MININIM */
#include "compatibility.h"

#include "constants.h"
#include "types.h"
#include "colors.h"

#include "array.h"
#include "audio.h"
#include "video.h"
#include "event.h"
#include "gamepad.h"
#include "pointer.h"
#include "memory.h"
#include "random.h"
#include "video.h"
#include "file.h"
#include "dialog.h"
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
#include "fix.h"
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
#include "audio-data.h"
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

#include "bmenu.h"
#include "editor.h"
#include "debug.h"
#include "undo.h"
#include "multi-room.h"
#include "box.h"
#include "replay.h"
#include "ui.h"
#include "xmath.h"
#include "xstring.h"

#include "script.h"
#include "L_mininim.h"
#include "L_mininim.level.h"
#include "L_mininim.level_con.h"
#include "L_mininim.actor.h"
#include "L_mininim.actor_N.h"
#include "L_mininim.audio.h"
#include "L_mininim.audio_res.h"
#include "L_mininim.video.h"
#include "L_mininim.video_res.h"
#include "L_mininim.setting.h"

/* functions */
void free_argv (size_t *cargc, char ***cargv);
void *dialog_thread (ALLEGRO_THREAD *thread, void *arg);
void *message_box_thread (ALLEGRO_THREAD *thread, void *arg);
bool load_config (char *filename, enum file_type *ret_file_type,
                  int priority);
bool save_game (char *filename, int priority);
void quit_game (void);
struct level *level_module_next_level (struct level *l, int n);
char *level_module_str (enum level_module m);
char *movements_str (enum movements m);
char *semantics_str (enum semantics m);
bool save_level (struct level *l);
void handle_load_config_thread (int priority);
void handle_save_game_thread (int priority);
void handle_save_picture_thread (int priority);
void handle_message_box_thread (void);

/* variables */
extern enum level_module level_module;
extern uint64_t play_time;
extern bool play_time_stopped;
extern enum vm vm;
extern enum gm gm;
extern enum em em;
extern bool force_em;
extern enum hue hue;
extern bool force_hue;
extern enum gpm gpm;
extern char *audio_mode;
extern char *video_mode;
extern char *env_mode;
extern bool immortal_mode;
extern int initial_total_lives, total_lives,
  initial_current_lives, current_lives;
extern int start_level;
extern struct pos start_pos;
extern uint64_t start_level_time;
extern uint64_t start_time;
extern uint64_t time_limit;
extern struct skill skill;
extern bool mirror_level;
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
extern enum semantics semantics;
extern enum movements movements;
extern bool title_demo;
extern enum rendering rendering;
extern bool inhibit_screensaver;

/* screams */
extern bool scream;
extern bool kid_scream;
extern bool guard_scream;
extern bool fat_guard_scream;
extern bool shadow_scream;
extern bool skeleton_scream;
extern bool vizier_scream;
extern bool princess_scream;
extern bool mouse_scream;

extern ALLEGRO_THREAD *load_config_dialog_thread,
  *save_game_dialog_thread, *save_picture_dialog_thread,
  *message_box_thread_id;

extern struct dialog load_config_dialog, save_game_dialog,
  save_picture_dialog;

extern struct message_box about_dialog;

#endif	/* MININIM_MININIM_H */
