/*
  replay.h -- replay module;

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

#ifndef MININIM_REPLAY_H
#define MININIM_REPLAY_H

struct replay {
  uint32_t version;
  uint32_t packed_boolean_config;
  uint32_t movements;
  uint32_t semantics;

  uint32_t start_level;
  uint32_t start_time;
  int32_t time_limit;
  uint32_t total_hp;
  uint32_t kca;
  uint32_t kcd;

  uint32_t random_seed;
  uint8_t *packed_gamepad_state;

  /* Fields below aren't properly part of the replay file format.
     Used mainly for replay results. */
  uint64_t packed_gamepad_state_nmemb;
  char *filename;
  bool complete;
  enum replay_incomplete {
    REPLAY_INCOMPLETE_NO_REASON,
    REPLAY_INCOMPLETE_STUCK, REPLAY_INCOMPLETE_DEAD,
    REPLAY_INCOMPLETE_OUT_OF_TIME,
  } reason;
  uint32_t final_total_hp;
  uint32_t final_kca;
  uint32_t final_kcd;
};

struct replay_favorite {
  char *filename;
  uint64_t cycle;
};

/* funtions */
struct replay *get_replay (void);
uint32_t pack_replay_config (void);
void unpack_replay_config (uint32_t pc);
uint8_t pack_gamepad_state (struct gamepad_state *gs);
struct gamepad_state *unpack_gamepad_state (struct gamepad_state *gs,
                                            uint8_t pgs);
struct replay *store_replay_gamepad_state (struct replay *replay,
                                           struct gamepad_state *gs,
                                           uint64_t cycle);
struct gamepad_state *get_replay_gamepad_state (struct gamepad_state *gs,
                                                struct replay *replay,
                                                size_t i);
bool save_replay (char *filename, struct replay *replay);
void save_replay_chain (void);
struct replay *load_replay (struct replay *replay_ret, char *filename);
struct replay *xload_replay (char *filename);
void free_replay (struct replay *replay);
void free_replay_chain (void);
struct replay *add_replay_file_to_replay_chain (char *filename);
int compare_replays (const void *_r0, const void *_r1);
void prepare_for_recording_replay (void);
void prepare_for_playing_replay (size_t i);
void start_recording_replay (int priority);
void create_save_replay_thread (void);
void handle_save_replay_thread (int priority);
void create_load_replay_thread (void);
void handle_load_replay_thread (int priority);
void stop_replaying (int priority);
void set_replay_mode_at_level_start (struct replay *replay);
void replay_gamepad_update (struct actor *a, struct replay *replay,
                            uint64_t cycle);
void print_replay_mode (int priority);
bool check_valid_replay_chain_pair (struct replay *r0, struct replay *r1);
char *replay_incomplete_str (enum replay_incomplete ri);
void print_replay_info (struct replay *replay);
void print_replay_chain_info (void);
void print_replay_results (struct replay *replay);
bool update_replay_progress (int *progress_ret);
bool is_dedicatedly_replaying (void);
void print_replay_chain_aborted (void);

/* replay favorites */
void add_replay_favorite (const char *filename, uint64_t cycle);
void add_current_replay_favorite (void);
void remove_replay_favorite (size_t i);
void free_replay_favorites (void);
bool print_replay_favorites (void);

/* variables */
extern struct replay recorded_replay;
extern enum replay_mode level_start_replay_mode;
extern enum replay_mode replay_mode;
extern struct replay_favorite *replay_favorite;
extern size_t replay_favorite_nmemb;
extern uint64_t replay_favorite_cycle;
extern int64_t recording_replay_countdown;
extern struct replay *replay_chain;
extern size_t replay_chain_nmemb;
extern size_t replay_index;
extern bool command_line_replay;
extern bool valid_replay_chain;
extern bool complete_replay_chain;
extern bool replay_skipped;
extern int just_skipped_replay;
extern int replay_next_number;
extern enum validate_replay_chain validate_replay_chain;

extern ALLEGRO_THREAD *save_replay_dialog_thread,
  *load_replay_dialog_thread;

extern struct dialog save_replay_dialog,
  load_replay_dialog;

#endif	/* MININIM_REPLAY_H */
