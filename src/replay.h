/*
  replay.h -- replay module;

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

#ifndef MININIM_REPLAY_H
#define MININIM_REPLAY_H

struct replay {
  uint32_t version;
  uint32_t packed_boolean_config;
  uint32_t movements;
  uint32_t semantics;
  uint32_t start_level, start_time, time_limit,
    total_lives, kca, kcd;
  uint32_t random_seed;
  uint8_t *packed_gamepad_state;
  uint64_t packed_gamepad_state_nmemb;
};

/* funtions */
uint32_t pack_replay_config (void);
void unpack_replay_config (uint32_t pc);
uint8_t pack_gamepad_state (struct gamepad_state *gs);
struct gamepad_state *unpack_gamepad_state (struct gamepad_state *gs,
                                            uint8_t pgs);
struct replay *push_replay_gamepad_state (struct replay *replay,
                                          struct gamepad_state *gs);
struct gamepad_state *get_replay_gamepad_state (struct gamepad_state *gs,
                                                struct replay *replay,
                                                size_t i);
bool save_replay (char *filename, struct replay *replay);
struct replay *load_replay (struct replay *replay_ret, char *filename);
struct replay *xload_replay (char *filename);
void free_replay (struct replay *replay);
void prepare_for_recording_replay (void);
void start_recording_replay (int priority);
void create_save_replay_thread (void);
void handle_save_replay_thread (int priority);
void create_load_replay_thread (void);
void handle_load_replay_thread (int priority);
void stop_replaying (int priority);
void set_replay_mode_at_level_start (struct replay *replay);
void replay_gamepad_update (struct anim *a, struct replay *replay,
                            uint64_t cycle);
void print_replay_mode (int priority);

/* variables */
extern struct replay replay;
extern enum replay_mode level_start_replay_mode;
extern enum replay_mode replay_mode;
extern int64_t recording_replay_countdown;

extern ALLEGRO_THREAD *save_replay_dialog_thread,
  *load_replay_dialog_thread;

extern struct dialog save_replay_dialog,
  load_replay_dialog;

#endif	/* MININIM_REPLAY_H */
