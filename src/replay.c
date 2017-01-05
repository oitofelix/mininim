/*
  replay.c -- replay module;

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

#include "mininim.h"

struct replay recorded_replay;

struct replay *replay_chain;
size_t replay_chain_nmemb;
size_t replay_index;

enum replay_mode level_start_replay_mode;
enum replay_mode replay_mode;

ALLEGRO_THREAD *save_replay_dialog_thread,
  *load_replay_dialog_thread;

int64_t recording_replay_countdown = -1;

bool command_line_replay;

bool valid_replay_chain = true;
bool complete_replay_chain = true;

enum validate_replay_chain validate_replay_chain =
  NONE_VALIDATE_REPLAY_CHAIN;

struct dialog save_replay_dialog = {
  .title = "Save replay",
  .patterns = "*.MRP",
  .mode = ALLEGRO_FILECHOOSER_SAVE,
};

struct dialog load_replay_dialog = {
  .title = "Load replay",
  .patterns = "*.MRP",
  .mode = ALLEGRO_FILECHOOSER_FILE_MUST_EXIST
  | ALLEGRO_FILECHOOSER_MULTIPLE,
};

struct replay *
get_replay (void)
{
  switch (replay_mode) {
  case RECORD_REPLAY: return &recorded_replay;
  case PLAY_REPLAY: return &replay_chain[replay_index];
  case NO_REPLAY: default: return NULL;
  }
}

uint32_t
pack_boolean_replay_config (void)
{
  uint32_t pc = 0;
  pc |= mirror_level ? PACKED_CONFIG_MIRROR_LEVEL_BIT : 0;
  pc |= immortal_mode ? PACKED_CONFIG_IMMORTAL_MODE_BIT : 0;
  return pc;
}

void
unpack_boolean_replay_config (uint32_t pc)
{
  mirror_level = pc & PACKED_CONFIG_MIRROR_LEVEL_BIT ? true : false;
  immortal_mode = pc & PACKED_CONFIG_IMMORTAL_MODE_BIT ? true : false;
}

uint8_t
pack_gamepad_state (struct gamepad_state *gs)
{
  uint8_t pgs = 0;
  pgs |= gs->up ? PACKED_GAMEPAD_STATE_UP_BIT : 0;
  pgs |= gs->down ? PACKED_GAMEPAD_STATE_DOWN_BIT : 0;
  pgs |= gs->left ? PACKED_GAMEPAD_STATE_LEFT_BIT : 0;
  pgs |= gs->right ? PACKED_GAMEPAD_STATE_RIGHT_BIT : 0;
  pgs |= gs->shift ? PACKED_GAMEPAD_STATE_SHIFT_BIT : 0;
  pgs |= gs->enter ? PACKED_GAMEPAD_STATE_ENTER_BIT : 0;
  return pgs;
}

struct gamepad_state *
unpack_gamepad_state (struct gamepad_state *gs, uint8_t pgs)
{
  memset (gs, 0, sizeof (* gs));
  gs->up = PACKED_GAMEPAD_STATE_UP_BIT & pgs ? true : false;
  gs->down = PACKED_GAMEPAD_STATE_DOWN_BIT & pgs ? true : false;
  gs->left = PACKED_GAMEPAD_STATE_LEFT_BIT & pgs ? true : false;
  gs->right = PACKED_GAMEPAD_STATE_RIGHT_BIT & pgs ? true : false;
  gs->shift = PACKED_GAMEPAD_STATE_SHIFT_BIT & pgs ? true : false;
  gs->enter = PACKED_GAMEPAD_STATE_ENTER_BIT & pgs ? true : false;
  return gs;
}

struct replay *
store_replay_gamepad_state (struct replay *replay,
                            struct gamepad_state *gs,
                            uint64_t cycle)
{
  replay->packed_gamepad_state_nmemb = cycle + 1;
  replay->packed_gamepad_state =
    xrealloc (replay->packed_gamepad_state,
              replay->packed_gamepad_state_nmemb
              * sizeof (* replay->packed_gamepad_state));
  replay->packed_gamepad_state[cycle] = pack_gamepad_state (gs);
  return replay;
}

struct gamepad_state *
get_replay_gamepad_state (struct gamepad_state *gs,
                          struct replay *replay,
                          size_t i)
{
  memset (gs, 0, sizeof (* gs));
  if (i < replay->packed_gamepad_state_nmemb)
    unpack_gamepad_state (gs, replay->packed_gamepad_state[i]);
  else return NULL;
  return gs;
}

bool
save_replay (char *filename, struct replay *replay)
{
  ALLEGRO_FILE *f = al_fopen (filename, "wb");
  if (! f) return false;

  /* signature */
  if (al_fwrite (f, REPLAY_FILE_SIGNATURE, sizeof (REPLAY_FILE_SIGNATURE))
      != sizeof (REPLAY_FILE_SIGNATURE)) return false;

  /* replay file format version */
  if (al_fwrite32le (f, REPLAY_FILE_FORMAT_VERSION) != 4) return false;

  /* packed boolean config */
  if (al_fwrite32le (f, replay->packed_boolean_config) != 4) return false;

  /* movements */
  if (al_fwrite32le (f, replay->movements) != 4) return false;

  /* semantics */
  if (al_fwrite32le (f, replay->semantics) != 4) return false;

  /* start level */
  if (al_fwrite32le (f, replay->start_level) != 4) return false;

  /* start time */
  if (al_fwrite32le (f, replay->start_time) != 4) return false;

  /* time limit */
  if (al_fwrite32le (f, replay->time_limit) != 4) return false;

  /* total lives */
  if (al_fwrite32le (f, replay->total_lives) != 4) return false;

  /* kca */
  if (al_fwrite32le (f, replay->kca) != 4) return false;

  /* kcd */
  if (al_fwrite32le (f, replay->kcd) != 4) return false;

  /* random seed */
  if (al_fwrite32le (f, replay->random_seed) != 4) return false;

  /* remove trailing null gamepad states */
  uint64_t *i = &replay->packed_gamepad_state_nmemb;
  while (*i > 0 && replay->packed_gamepad_state[*i - 1] == 0)
    (*i)--;

  /* packed gamepad state */
  if (*i > 0
      && al_fwrite (f, replay->packed_gamepad_state, *i) != *i)
    return false;

  al_fclose (f);

  return true;
}

void
save_replay_chain (void)
{
  size_t i;
  for (i = 0; i < replay_chain_nmemb; i++) {
    struct replay *replay = &replay_chain[i];
    if (! save_replay (replay->filename, replay))
      error (0, al_get_errno (), "failed to save replay file '%s'",
             replay->filename);
  }
}


struct replay *
load_replay (struct replay *replay_ret, char *filename)
{
  struct replay replay;
  memset (&replay, 0, sizeof (replay));

  ALLEGRO_FILE *f = al_fopen (filename, "rb");
  if (! f) return NULL;

  /* signature */
  char signature[sizeof (REPLAY_FILE_SIGNATURE)];
  al_fread (f, signature, sizeof (REPLAY_FILE_SIGNATURE));
  if (al_feof (f) || al_ferror (f)) return NULL;
  if (strcmp (signature, REPLAY_FILE_SIGNATURE))
    return NULL;

  /* replay file format version */
  replay.version = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* packed boolean config */
  replay.packed_boolean_config = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* movements */
  replay.movements = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* semantics */
  replay.semantics = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* start level */
  replay.start_level = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* start time */
  replay.start_time = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* time limit */
  replay.time_limit = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* total lives */
  replay.total_lives = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* kca */
  replay.kca = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* kcd */
  replay.kcd = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* random seed */
  replay.random_seed = al_fread32le (f);
  if (al_feof (f) || al_ferror (f)) return NULL;

  /* packed gamepad state */
  replay.packed_gamepad_state_nmemb = al_fsize (f)
    - (sizeof (REPLAY_FILE_SIGNATURE) + 11 * sizeof (uint32_t));
  if (replay.packed_gamepad_state_nmemb < 0) return NULL;
  if (replay.packed_gamepad_state_nmemb > 0) {
    replay.packed_gamepad_state =
      xmalloc (replay.packed_gamepad_state_nmemb);
    al_fread (f, replay.packed_gamepad_state,
              replay.packed_gamepad_state_nmemb);
    if (al_feof (f) || al_ferror (f)) return NULL;
  }

  /* filename */
  xasprintf (&replay.filename, "%s", filename);

  *replay_ret = replay;

  return replay_ret;
}

struct replay *
xload_replay (char *filename)
{
  return add_replay_file_to_replay_chain (filename);
}

void
free_replay (struct replay *replay)
{
  if (replay->packed_gamepad_state)
    al_free (replay->packed_gamepad_state);
  if (replay->filename)
    al_free (replay->filename);
  memset (replay, 0, sizeof (* replay));
}

void
free_replay_chain (void)
{
  size_t i;
  for (i = 0; i < replay_chain_nmemb; i++)
    free_replay (&replay_chain[i]);

  al_free (replay_chain);
  replay_chain = NULL;
  replay_chain_nmemb = 0;
  replay_index = 0;

  valid_replay_chain = true;
  complete_replay_chain = true;
}

struct replay *
add_replay_file_to_replay_chain (char *filename)
{
  struct replay replay;
  if (load_replay (&replay, filename)) {
    replay_chain =
      add_to_array (&replay, 1, replay_chain, &replay_chain_nmemb,
                    replay_chain_nmemb, sizeof (replay));

    qsort (replay_chain, replay_chain_nmemb, sizeof (replay),
           compare_replays);

    return replay_chain;
  }
  else return NULL;
}

int
compare_replays (const void *_r0, const void *_r1)
{
  struct replay *r0 = (struct replay *) _r0;
  struct replay *r1 = (struct replay *) _r1;
  int l0 = r0->start_level;
  int l1 = r1->start_level;
  return cint (&l0, &l1);
}

void
prepare_for_recording_replay (void)
{
  recording_replay_countdown = SEC2CYC (3) - 1;
}

void
prepare_for_playing_replay (size_t i)
{
  level_start_replay_mode = PLAY_REPLAY;
  quit_anim = NEXT_LEVEL;
  replay_index = i;
  struct replay *replay = &replay_chain[i];
  next_level_number = replay->start_level;
  min_legacy_level = min_int (min_legacy_level, replay->start_level);
  max_legacy_level = max_int (max_legacy_level, replay->start_level);
  ignore_level_cutscene = true;
  recording_replay_countdown = -1;
}

void
start_recording_replay (int priority)
{
  if (! command_line_replay &&
      recording_replay_countdown > 0) {
    char *text;
    xasprintf (&text, "REPLAY RECORDING IN %i",
               CYC2SEC(recording_replay_countdown) + 1);
    draw_bottom_text (NULL, text, priority);
    al_free (text);
    recording_replay_countdown--;
  } else if (recording_replay_countdown == 0) {
    level_start_replay_mode = RECORD_REPLAY;
    quit_anim = RESTART_LEVEL;
    recording_replay_countdown = -1;
    draw_bottom_text (NULL, NULL, priority);
  }
}

void
create_save_replay_thread (void)
{
  al_make_directory (user_data_dir);
  save_replay_dialog_thread =
    create_thread (dialog_thread, &save_replay_dialog);
  al_start_thread (save_replay_dialog_thread);
  level_start_replay_mode = NO_REPLAY;
  replay_mode = NO_REPLAY;
  pause_animation (true);
}

void
handle_save_replay_thread (int priority)
{
  if (! save_replay_dialog_thread
      || ! al_get_thread_should_stop (save_replay_dialog_thread))
    return;

  ALLEGRO_FILECHOOSER *dialog;
  al_join_thread (save_replay_dialog_thread, (void *) &dialog);
  al_destroy_thread (save_replay_dialog_thread);
  save_replay_dialog_thread = NULL;
  char *filename = al_get_native_file_dialog_count (dialog) > 0
    ? (char *) al_get_native_file_dialog_path (dialog, 0)
    : NULL;
  struct replay *replay = &recorded_replay;
  if (filename) {
    char *error_str = save_replay (filename, replay)
      ? "REPLAY HAS BEEN SAVED"
      : "REPLAY SAVING FAILED";
    draw_bottom_text (NULL, error_str, priority);
    al_free (save_replay_dialog.initial_path);
    xasprintf (&save_replay_dialog.initial_path, "%s", filename);
  } else
    draw_bottom_text (NULL, "REPLAY RECORDING STOPPED", priority);
  al_destroy_native_file_dialog (dialog);
  free_replay (replay);
  pause_animation (false);
}

void
create_load_replay_thread (void)
{
  load_replay_dialog_thread =
    create_thread (dialog_thread, &load_replay_dialog);
  al_start_thread (load_replay_dialog_thread);
}

void
handle_load_replay_thread (int priority)
{
  if (! load_replay_dialog_thread
      || ! al_get_thread_should_stop (load_replay_dialog_thread))
    return;

  ALLEGRO_FILECHOOSER *dialog;
  al_join_thread (load_replay_dialog_thread, (void *) &dialog);
  al_destroy_thread (load_replay_dialog_thread);
  load_replay_dialog_thread = NULL;
  size_t i;
  size_t n = al_get_native_file_dialog_count (dialog);

  free_replay_chain ();

  if (n > 0) {
    for (i = 0; i < n; i++) {
      char *filename = (char *) al_get_native_file_dialog_path (dialog, i);
      add_replay_file_to_replay_chain (filename);
    }

    bool success = replay_chain_nmemb > 0;

    char *error_str = success
      ? "REPLAY HAS BEEN LOADED"
      : "REPLAY LOADING FAILED";

    draw_bottom_text (NULL, error_str, priority);

    al_free (load_replay_dialog.initial_path);
    xasprintf (&load_replay_dialog.initial_path, "%s",
               al_get_native_file_dialog_path (dialog, n - 1));

    if (success) prepare_for_playing_replay (0);
  }

  al_destroy_native_file_dialog (dialog);
}

void
stop_replaying (int priority)
{
  free_replay_chain ();
  if (! title_demo
      && replay_mode == PLAY_REPLAY)
    draw_bottom_text (NULL, "REPLAY STOPPED", priority);
  replay_mode = NO_REPLAY;
  level_start_replay_mode = NO_REPLAY;
}

void
set_replay_mode_at_level_start (struct replay *replay)
{
  switch (replay_mode) {
  case RECORD_REPLAY:
    replay->packed_boolean_config = pack_boolean_replay_config ();
    replay->movements = movements;
    replay->semantics = semantics;
    replay->start_level = global_level.n;
    replay->start_time = start_level_time;
    replay->time_limit = time_limit;
    replay->total_lives = total_lives;
    replay->kca = skill.counter_attack_prob + 1;
    replay->kcd = skill.counter_defense_prob + 1;
    replay->random_seed = random_seed;
    break;
  case PLAY_REPLAY:
    unpack_boolean_replay_config (replay->packed_boolean_config);
    movements = replay->movements;
    semantics = replay->semantics;
    start_level_time = replay->start_time;
    time_limit = replay->time_limit;
    total_lives = replay->total_lives;
    skill.counter_attack_prob = replay->kca - 1;
    skill.counter_defense_prob = replay->kcd - 1;
    random_seed = replay->random_seed;
    break;
  case NO_REPLAY: default: break;
  }

  print_replay_mode (0);
}

void
replay_gamepad_update (struct anim *a, struct replay *replay, uint64_t cycle)
{
  memset (&a->key, 0, sizeof (a->key));

  if (replay_mode == PLAY_REPLAY) {
    if (! get_replay_gamepad_state (&a->key, replay, cycle) && ! title_demo)
      draw_bottom_text (NULL, "NO FURTHER REPLAY DATA", -1);
  } else get_gamepad_state (&a->key);

  if (replay_mode == RECORD_REPLAY && a->current_lives > 0)
    store_replay_gamepad_state (replay, &a->key, cycle);
}

void
print_replay_mode (int priority)
{
  if (title_demo) return;
  switch (replay_mode) {
  case RECORD_REPLAY:
    draw_bottom_text (NULL, "RECORDING REPLAY", priority); break;
  case PLAY_REPLAY:
    draw_bottom_text (NULL, "REPLAYING", priority); break;
  case NO_REPLAY: default: break;
  }
}

bool
check_valid_replay_chain_pair (struct replay *r0, struct replay *r1)
{
  bool valid = true;
  bool changed = false;

  if (r1->start_level != r0->start_level + 1)
    switch (validate_replay_chain) {
    case READ_VALIDATE_REPLAY_CHAIN:
    case WRITE_VALIDATE_REPLAY_CHAIN:
      /* printf ("CHANGED: --start-level %u --> %u\n", */
      /*         r1->start_level, r0->start_level + 1); */
      /* r1->start_level = r0->start_level + 1; */
      /* changed = true; */
      /* break; */
    case NONE_VALIDATE_REPLAY_CHAIN: default:
      printf ("INVALID: --start-level\n");
      valid = false;
      break;
    }

  if (r1->start_time < r0->start_time + r0->packed_gamepad_state_nmemb)
    switch (validate_replay_chain) {
    case READ_VALIDATE_REPLAY_CHAIN:
    case WRITE_VALIDATE_REPLAY_CHAIN:
      printf ("CHANGED: --start-time %u --> %u\n",
              r1->start_time, r0->start_time
              + (uint32_t) r0->packed_gamepad_state_nmemb);
      r1->start_time = r0->start_time + r0->packed_gamepad_state_nmemb;
      changed = true;
      break;
    case NONE_VALIDATE_REPLAY_CHAIN: default:
      printf ("INVALID: --start-time\n");
      valid = false;
      break;
    }

  if (r1->time_limit > r0->time_limit)
    switch (validate_replay_chain) {
    case READ_VALIDATE_REPLAY_CHAIN:
    case WRITE_VALIDATE_REPLAY_CHAIN:
      printf ("CHANGED: --time-limit %i --> %i\n",
              r1->time_limit, r0->time_limit);
      r1->time_limit = r0->time_limit;
      changed = true;
      break;
    case NONE_VALIDATE_REPLAY_CHAIN: default:
      printf ("INVALID: --time-limit\n");
      valid = false;
      break;
    }

  if (r1->total_lives > r0->final_total_lives)
    switch (validate_replay_chain) {
    case READ_VALIDATE_REPLAY_CHAIN:
    case WRITE_VALIDATE_REPLAY_CHAIN:
      printf ("CHANGED: --total-lives %u --> %u\n",
              r1->total_lives, r0->final_total_lives);
      r1->total_lives = r0->final_total_lives;
      changed = true;
      break;
    case NONE_VALIDATE_REPLAY_CHAIN: default:
      printf ("INVALID: --total-lives\n");
      valid = false;
      break;
    }

  if (r1->kca > r0->final_kca)
    switch (validate_replay_chain) {
    case READ_VALIDATE_REPLAY_CHAIN:
    case WRITE_VALIDATE_REPLAY_CHAIN:
      printf ("CHANGED: --kca %u --> %u\n",
              r1->kca, r0->final_kca);
      r1->kca = r0->final_kca;
      changed = true;
      break;
    case NONE_VALIDATE_REPLAY_CHAIN: default:
      printf ("INVALID: --kca\n");
      valid = false;
      break;
    }

  if (r1->kcd > r0->final_kcd)
    switch (validate_replay_chain) {
    case READ_VALIDATE_REPLAY_CHAIN:
    case WRITE_VALIDATE_REPLAY_CHAIN:
      printf ("CHANGED: --kcd %u --> %u\n",
              r1->kcd, r0->final_kcd);
      r1->kcd = r0->final_kcd;
      changed = true;
      break;
    case NONE_VALIDATE_REPLAY_CHAIN: default:
      printf ("INVALID: --kcd\n");
      valid = false;
      break;
    }

  if (! valid || changed) HLINE;

  return valid;
}

char *
replay_incomplete_str (enum replay_incomplete ri)
{
  switch (ri) {
  default: return "NONE";
  case REPLAY_INCOMPLETE_STUCK: return "STUCK";
  case REPLAY_INCOMPLETE_DEAD: return "DEAD";
  case REPLAY_INCOMPLETE_OUT_OF_TIME: return "OUT OF TIME";
  }
}

void
print_replay_info (struct replay *replay)
{
  printf ("File: %s\n"
          "Signature: %s\n"
          "Version: %u\n"
          "Initial: --mirror-level=%s --immortal-mode=%s --movements=%s \\\n"
          "  --semantics=%s --start-level=%u --start-time=%u --time-limit=%i \\\n"
          "  --total-lives=%u --kca=%u --kcd=%u\n"
          "Random seed: 0x%X\n"
          "Cycles: %lu\n",
          replay->filename,
          REPLAY_FILE_SIGNATURE,
          replay->version,
          replay->packed_boolean_config & PACKED_CONFIG_MIRROR_LEVEL_BIT
          ? "TRUE" : "FALSE",
          replay->packed_boolean_config & PACKED_CONFIG_IMMORTAL_MODE_BIT
          ? "TRUE" : "FALSE",
          movements_str (replay->movements),
          semantics_str (replay->semantics),
          replay->start_level,
          replay->start_time,
          replay->time_limit,
          replay->total_lives,
          replay->kca,
          replay->kcd,
          replay->random_seed,
          replay->packed_gamepad_state_nmemb);
  fflush (stdout);
}

void
print_replay_results (struct replay *replay)
{
  printf ("Complete: %s\n"
          "Reason: %s\n"
          "Final: --mirror-level=%s --immortal-mode=%s --movements=%s \\\n"
          "  --semantics=%s --start-level=%u --start-time=%lu --time-limit=%i \\\n"
          "  --total-lives=%u --kca=%i --kcd=%i\n",
          replay->complete ? "YES" : "NO",
          replay_incomplete_str (replay->reason),
          replay->packed_boolean_config & PACKED_CONFIG_MIRROR_LEVEL_BIT
          ? "TRUE" : "FALSE",
          replay->packed_boolean_config & PACKED_CONFIG_IMMORTAL_MODE_BIT
          ? "TRUE" : "FALSE",
          movements_str (replay->movements),
          semantics_str (replay->semantics),
          replay->start_level + 1,
          replay->start_time + replay->packed_gamepad_state_nmemb,
          replay->time_limit,
          replay->final_total_lives,
          replay->final_kca,
          replay->final_kcd);
  fflush (stdout);
}

void
print_replay_chain_info (void)
{
  size_t i;
  for (i = 0; i < replay_chain_nmemb; i++) {
    HLINE;
    print_replay_info (&replay_chain[i]);
  }
  HLINE;
}

bool
update_replay_progress (int *progress_ret)
{
  static int last_progress = -1;

  struct replay *replay = get_replay ();

  if (! replay) return false;

  int total = replay->packed_gamepad_state_nmemb;

  int progress = total ? round ((anim_cycle * 100.0) / total) : 100;

  progress = progress > 100 ? 100 : progress;

  bool update = progress != last_progress;
  last_progress = progress;
  if (progress_ret) *progress_ret = progress;
  return update;
}

bool
is_dedicatedly_replaying (void)
{
  return replay_mode == PLAY_REPLAY
    && (rendering == NONE_RENDERING || rendering == AUDIO_RENDERING);
}

void
print_replay_chain_aborted (void)
{
  if (! title_demo
      && replay_mode == PLAY_REPLAY) {
    HLINE;
    printf ("REPLAY CHAIN ABORTED\n");
    HLINE;
  }
}
