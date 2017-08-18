/*
  types.h -- types definitions;

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

#ifndef MININIM_TYPES_H
#define MININIM_TYPES_H

enum file_type {
  UNKNOWN_FILE_TYPE, CONFIG_FILE_TYPE, GAME_SAVE_FILE_TYPE, LEVEL_FILE_TYPE,
};

typedef int (* m_comparison_fn_t) (const void *, const void *);

struct key {
  int modifiers;
  int keycode;
};

/* environment mode */
enum em {
  DUNGEON = 0, PALACE = 1,
};

#define ORIGINAL_EM (PALACE + 1)

enum hue {
  HUE_NONE = 0, HUE_GREEN = 1, HUE_GRAY = 2, HUE_YELLOW = 3,
  HUE_BLUE = 4,
};

#define HUE_ORIGINAL (HUE_BLUE + 1)

/* gamepad mode */
enum gpm {
  KEYBOARD, JOYSTICK,
};

struct pos {
  struct level *l;
  int room, floor, place;
};

struct frameset {
  ALLEGRO_BITMAP *frame;
  int dx, dy;
};

struct dim {
  lua_Number x, y, w, h, fx, bx;
};

enum dir {
  LEFT = 0, RIGHT = 1, ABOVE = 2, BELOW = 3,
};

struct coord {
  struct level *l;
  int room;
  lua_Number x, y;
  int prev_room;
  enum dir xd;
};

struct rect {
  struct coord c;
  lua_Number w, h;
};

struct star {
  int x, y;
  int color;
};

struct pixel_coord {
  int x, y;
};

struct bitmap_rcoord {
  ALLEGRO_BITMAP *b;
  struct pixel_coord tl, tr, bl, br, mt, mbo, ml, mr, m;
};

struct drawn_rectangle {
  ALLEGRO_BITMAP *bitmap;
  int x, y, w, h;
};

struct clipping_rectangle {
  ALLEGRO_BITMAP *bitmap;
  int x, y, w, h;
};

enum edit {
  EDIT_NONE, EDIT_MAIN, EDIT_TILE, EDIT_FG, EDIT_FLOOR, EDIT_PILLAR,
  EDIT_DOOR, EDIT_CARPET, EDIT_ARCH, EDIT_BG, EDIT_EXT,
  EDIT_JUMP_ROOM, EDIT_NOMINAL_INFO, EDIT_EVENT, EDIT_EVENT_SET,
  EDIT_EVENT2TILE,
  EDIT_EVENT2FLOOR, EDIT_TILE2EVENT, EDIT_ROOM, EDIT_LINK,
  EDIT_LINK_LEFT, EDIT_LINK_RIGHT, EDIT_LINK_ABOVE, EDIT_LINK_BELOW,
  EDIT_ROOM_EXCHANGE, EDIT_LINKING_SETTINGS, EDIT_KID, EDIT_LEVEL,
  EDIT_ENVIRONMENT, EDIT_HUE, EDIT_GUARD, EDIT_GUARD_SELECT, EDIT_GUARD_SKILL,
  EDIT_GUARD_SKILL_ATTACK, EDIT_GUARD_SKILL_COUNTER_ATTACK,
  EDIT_GUARD_SKILL_DEFENSE,
  EDIT_GUARD_SKILL_COUNTER_DEFENSE, EDIT_GUARD_SKILL_ADVANCE,
  EDIT_GUARD_SKILL_RETURN,
  EDIT_GUARD_SKILL_REFRACTION, EDIT_GUARD_SKILL_EXTRA_HP, EDIT_GUARD_HP,
  EDIT_GUARD_TYPE, EDIT_GUARD_STYLE, EDIT_SKILL_LEGACY_TEMPLATES,
  EDIT_NOMINAL_NUMBER, EDIT_MIRROR_TILE, EDIT_ROOM_MIRROR,
  EDIT_ROOM_MIRROR_TILES,
  EDIT_ROOM_MIRROR_LINKS, EDIT_ROOM_MIRROR_BOTH, EDIT_LEVEL_MIRROR,
  EDIT_LEVEL_MIRROR_TILES, EDIT_LEVEL_MIRROR_LINKS, EDIT_LEVEL_MIRROR_BOTH,
  EDIT_LEVEL_JUMP, EDIT_LEVEL_EXCHANGE, EDIT_NUMERICAL_INFO, EDIT_NUMERICAL_FG,
  EDIT_NUMERICAL_BG, EDIT_NUMERICAL_EXT,
};

struct multi_room {
  int w, h;
  int x, y;
  int dx, dy;
  int room;
  int select_cycles;
  int flicker;
  ALLEGRO_COLOR color;
  enum mr_fit_mode {
    MR_FIT_NONE, MR_FIT_STRETCH, MR_FIT_RATIO,
  } fit_mode;
  int fit_w, fit_h;
  int room_select;
  bool full_update;
  bool busy;

  struct multi_room_cell {
    ALLEGRO_BITMAP *screen;
    ALLEGRO_BITMAP *cache;
    bool done;
    int room;

    struct stars {
      ALLEGRO_BITMAP *b;
      struct coord c;
      struct star *s;
      size_t count;
    } stars[FLOORS][PLACES];
  } **cell;

  struct {
    int w, h, x, y, room;

    int level;
    enum em em;
    /* enum vm vm; */
    /* int hgc; */
    enum hue hue;
    struct pos selection_pos;
    enum edit edit;

    int display_width, display_height;

   struct {
      int room;
    } **cell;
  } last;
};

struct mr_origin {
  int w, h, x, y, room;
};

struct mouse_coord {
  struct coord c;
  struct mr_origin mr;
  int x, y;
  int sx, sy;
};

struct mr_room_list {
  int *room;
  size_t nmemb;
};

enum gm {
  ORIGINAL_GM, GUARD_GM, FAT_GM, VIZIER_GM, SKELETON_GM, SHADOW_GM,
};

enum actor_type {
  NO_ACTOR = 0, KID = 1, SHADOW = 2, GUARD = 3, FAT = 4,
  SKELETON = 5, VIZIER = 6, PRINCESS = 7, MOUSE = 8,
};

struct skill {
  int attack_prob;
  int counter_attack_prob;
  int defense_prob;
  int counter_defense_prob;
  int advance_prob;
  int return_prob;
  int refraction;
  int extra_hp;
};

struct gamepad_state {
  bool up, down, left, right, shift, enter, ctrl, alt;
};

enum replay_mode {
  NO_REPLAY, RECORD_REPLAY, PLAY_REPLAY
};

struct level {
  void (*start) (void);
  void (*special_events) (void);
  void (*end) (struct pos *p);
  struct level *(*next_level) (struct level *l, int n);
  void (*cutscene) (void);
  int n;
  int nominal_n;

  struct pos start_pos;
  enum dir start_dir;
  bool has_sword;

  enum em em;

  enum hue hue;

  struct room_linking {
    int l, r, a, b;
  } link[ROOMS];

  struct level_event {
    struct pos p;
    bool next;
  } event[EVENTS];

  struct guard {
    enum actor_type type;
    struct pos p;
    enum dir dir;
    struct skill skill;
    int total_hp;
    int style;
  } guard[GUARDS];

  struct tile {

    enum tile_fg {
      NO_FLOOR = 0,
      FLOOR = 1,
      BROKEN_FLOOR = 2,
      SKELETON_FLOOR = 3,
      LOOSE_FLOOR = 4,
      SPIKES_FLOOR = 5,
      OPENER_FLOOR = 6,
      CLOSER_FLOOR = 7,
      STUCK_FLOOR = 8,
      HIDDEN_FLOOR = 9,
      PILLAR = 10,
      BIG_PILLAR_BOTTOM = 11,
      BIG_PILLAR_TOP = 12,
      WALL = 13,
      DOOR = 14,
      LEVEL_DOOR = 15,
      CHOMPER = 16,
      MIRROR = 17,
      CARPET = 18,
      TCARPET = 19,
      ARCH_BOTTOM = 20,
      ARCH_TOP_LEFT = 21,
      ARCH_TOP_RIGHT = 22,
      ARCH_TOP_MID = 23,
      ARCH_TOP_SMALL = 24,
      TILE_FGS
    } fg;

    enum tile_bg {
      NO_BRICKS = 0,
      BRICKS_1 = 1,
      BRICKS_2 = 2,
      BRICKS_3 = 3,
      BRICKS_4 = 4,
      BRICKS_5 = 5,
      TORCH = 6,
      WINDOW = 7,
      BALCONY = 8,
      TILE_BGS
    } bg;

    int ext;

    int fake;

  } tile[ROOMS][FLOORS][PLACES];
};

enum item {
  NO_ITEM = 0,
  EMPTY_POTION = 1,
  SMALL_HP_POTION = 2,
  BIG_HP_POTION = 3,
  SMALL_POISON_POTION = 4,
  BIG_POISON_POTION = 5,
  FLOAT_POTION = 6,
  FLIP_POTION = 7,
  ACTIVATION_POTION = 8,
  SWORD = 9,
  ITEMS
} item;

enum should_draw {
  SHOULD_DRAW_NONE, SHOULD_DRAW_PART, SHOULD_DRAW_FULL,
};

struct legacy_level {
  uint8_t foretable[LROOMS][LFLOORS][LPLACES];
  uint8_t backtable[LROOMS][LFLOORS][LPLACES];
  uint8_t door_1[LEVENTS];
  uint8_t door_2[LEVENTS];
  uint8_t link[LROOMS][4];
  uint8_t unknown_1[64];
  uint8_t start_position[3];
  uint8_t unknown_2[3];
  uint8_t unknown_3;
  uint8_t guard_location[LROOMS];
  uint8_t guard_direction[LROOMS];
  uint8_t unknown_4a[LROOMS];
  uint8_t unknown_4b[LROOMS];
  uint8_t guard_skill[LROOMS];
  uint8_t unknown_4c[LROOMS];
  uint8_t guard_color[LROOMS];
  uint8_t unknown_4d[16];
  uint8_t signature[2];
} __attribute__((packed));

struct diffset {
  struct diff {
    struct diff_line {
      void *forward;
      void *backward;
      size_t count;
      size_t offset;
    } *line;
    size_t unit_size;
    size_t count;
    char *desc;
  } *diff;

  size_t count;
  int current;
};

/* avoid "'struct' declared inside parameter list" error for the
   ACTION definition */
struct actor *_action;
typedef void (*ACTION) (struct actor *a);

struct actor {
  enum actor_type type;
  enum actor_type original_type;

  int id;
  int shadow_of;
  int level_id;

  struct frame {
    int parent_id;
    struct coord c;
    ALLEGRO_BITMAP *b;
    struct coord oc;
    ALLEGRO_BITMAP *ob;
    enum dir dir;
    int flip;
  } f;

  struct frame of;

  struct frame_offset {
    ALLEGRO_BITMAP *b;
    lua_Number dx, dy;
  } fo;

  struct frame_offset xf;

  struct collision_info {
    struct pos kid_p, tile_p;
  } ci;

  struct gamepad_state key;

  ACTION oaction;
  ACTION action;
  ACTION next_action;
  ACTION hang_caller;
  int i, oi, j, wait, repeat, cinertia, inertia,
    total_hp, current_hp;
  bool reverse, collision, fall, hit_ceiling, hit_ceiling_fake,
    just_hanged, hang, hang_limit, misstep, uncrouch_slowly,
    keep_sword_fast, turn, shadow, hit_by_loose_floor,
    invisible, has_sword, hurt, controllable, fight,
    edge_detection, auto_taken_sword, constrained_turn_run;

  int ignore_danger;

  ALLEGRO_BITMAP *splash_bitmap;
  bool splash;

  int enemy_defended_my_attack, enemy_counter_attacked_myself,
    i_counter_defended, placed_at_attack_frame;
  bool i_initiated_attack;
  bool attack_range_far, attack_range_near, hurt_enemy_in_counter_attack;
  int angry;

  int no_walkf_timer, death_timer;

  struct skill skill;
  int refraction;

  uint64_t float_timer;

  int dc, df, dl, dcl, dch, dcd;

  bool immortal, poison_immune, loose_floor_immune, fall_immune,
    spikes_immune, chomper_immune;

  int sword_immune;

  int enemy_id;
  int enemy_refraction;

  bool dont_draw_hp;

  int style;

  enum tile_fg tile_fg;

  enum item item;

  uint64_t alert_cycle;

  struct pos p, item_pos, hang_pos, enemy_pos, cross_mirror_p;

  enum death_reason {
    NO_DEATH, FALL_DEATH, LOOSE_FLOOR_DEATH, POTION_DEATH,
    SPIKES_DEATH, CHOMPER_DEATH, FIGHT_DEATH, SHADOW_FIGHT_DEATH
  } death_reason;

  bool glory_sample;

  /* depressible floor */
  struct pos df_pos[2];
  struct pos df_posb[2];

  struct mr_origin mr_origin;

  bool ctrl_left, ctrl_right, alt_up;

  uint64_t selection_cycle;
};

typedef struct coord *(*coord_f) (struct frame *, struct coord *);
typedef struct pos *(*pos_f) (struct coord *, struct pos *);
typedef struct coord *(*pos2coord_f) (struct pos *, struct coord *);
typedef bool (*pos_pred) (struct pos *, void *);
typedef bool (*int_pred) (int, void *);
typedef struct pos * (*pos_trans) (struct pos *);

struct room_dist {
  int dist;
  bool visited;
};

typedef intptr_t (*load_resource_f) (const char *);

typedef void *(*thread_f)(ALLEGRO_THREAD *thread, void *arg);

enum video_effect_type {
  VIDEO_NO_EFFECT, VIDEO_OFF, VIDEO_FADE_IN,
  VIDEO_FADE_OUT, VIDEO_ROLL_RIGHT, VIDEO_FLICKERING,
};

struct video_effect {
  enum video_effect_type type;
  int duration;
  ALLEGRO_COLOR color;
};

typedef ALLEGRO_COLOR (*palette) (ALLEGRO_COLOR c);

enum quit_anim {
  NO_QUIT, RESTART_LEVEL, NEXT_LEVEL, RESTART_GAME,
  QUIT_GAME, OUT_OF_TIME, CUTSCENE_KEY_PRESS, CUTSCENE_END,
  REPLAY_COMPLETE, REPLAY_INCOMPLETE, REPLAY_OUT_OF_TIME,
  REPLAY_NEXT, REPLAY_RESTART_LEVEL, LOAD_GAME,
};

enum rendering {
  BOTH_RENDERING, VIDEO_RENDERING, AUDIO_RENDERING, NONE_RENDERING,
};

enum validate_replay_chain {
  NONE_VALIDATE_REPLAY_CHAIN, READ_VALIDATE_REPLAY_CHAIN,
  WRITE_VALIDATE_REPLAY_CHAIN,
};

struct audio_source {
  union {
    ALLEGRO_SAMPLE *sample;
    char *stream;
  } data;

  enum audio_type {
    AUDIO_SAMPLE, AUDIO_STREAM
  } type;
};

struct audio_instance {
  struct audio_source *source;

  union audio_instance_data {
    ALLEGRO_SAMPLE_INSTANCE *sample;
    ALLEGRO_AUDIO_STREAM *stream;
  } data;

  union audio_position {
    unsigned int sample;
    double stream;
  } position;

  bool played;
  uint64_t anim_cycle;
  struct pos p;
  int actor_id;
  float volume;
};

typedef bool (*tile_fg_set) (enum tile_fg);

typedef void (*room_callback_f) (int, int);

enum wall_correlation {
  SWS, SWW, WWS, WWW
};

struct int_range {
  int a, b;
};

struct float_range {
  float a, b;
};

enum opt_arg_type {
  ARG_TYPE_NULL, ARG_TYPE_BOOL, ARG_TYPE_INT, ARG_TYPE_FLOAT, ARG_TYPE_ENUM,
};

enum semantics {
  NATIVE_SEMANTICS = 0, LEGACY_SEMANTICS = 1,
};

enum movements {
  NATIVE_MOVEMENTS = 0, LEGACY_MOVEMENTS = 1,
};

enum options {
  VIDEO_MODE_OPTION = 256, ENVIRONMENT_MODE_OPTION, GUARD_MODE_OPTION,
  SOUND_GAIN_OPTION, DISPLAY_FLIP_MODE_OPTION, GAMEPAD_FLIP_MODE_OPTION,
  MIRROR_MODE_OPTION, BLIND_MODE_OPTION, IMMORTAL_MODE_OPTION,
  TOTAL_HP_OPTION, START_LEVEL_OPTION, TIME_LIMIT_OPTION,
  KCA_OPTION, KCD_OPTION, DATA_PATH_OPTION, FULLSCREEN_OPTION,
  WINDOW_POSITION_OPTION, WINDOW_DIMENSIONS_OPTION,
  INHIBIT_SCREENSAVER_OPTION, PRINT_PATHS_OPTION, PRINT_DISPLAY_MODES_OPTION,
  LEVEL_MODULE_OPTION, SKIP_TITLE_OPTION, START_TIME_OPTION,
  LOAD_CONFIG_OPTION, IGNORE_MAIN_CONFIG_OPTION, IGNORE_ENVIRONMENT_OPTION,
  JOYSTICK_AXIS_THRESHOLD_OPTION, JOYSTICK_BUTTON_THRESHOLD_OPTION,
  JOYSTICK_AXIS_OPTION, JOYSTICK_BUTTON_OPTION, JOYSTICK_INFO_OPTION,
  START_POS_OPTION, HUE_MODE_OPTION, CONVERT_LEVELS_OPTION, MULTI_ROOM_OPTION,
  DISPLAY_MODE_OPTION, MULTI_ROOM_FIT_MODE_OPTION, TIME_FREQUENCY_OPTION,
  MIRROR_LEVEL_OPTION, SEMANTICS_OPTION, MOVEMENTS_OPTION,
  RECORD_REPLAY_OPTION, REPLAY_INFO_OPTION, RENDERING_OPTION,
  VALIDATE_REPLAY_CHAIN_OPTION, GAMEPAD_RUMBLE_GAIN_OPTION, SCREAM_OPTION,
  RANDOM_SEED_OPTION, GAMEPAD_MODE_OPTION, PRINT_REPLAY_FAVORITES_OPTION,
  REPLAY_FAVORITE_OPTION,
};

enum level_module {
  NATIVE_LEVEL_MODULE, LEGACY_LEVEL_MODULE, PLV_LEVEL_MODULE,
  DAT_LEVEL_MODULE, CONSISTENCY_LEVEL_MODULE,
};

struct bmenu_item {
  char key;
  char *desc;
};

enum tile_diff {
  TILE_DIFF_NO_DIFF, TILE_DIFF_FG, TILE_DIFF_BG,
  TILE_DIFF_EXT, TILE_DIFF_FAKE, TILE_DIFF_MIXED,
};

struct dialog {
  char *initial_path, *title, *patterns;
  int mode;
};

struct message_box {
  char *title, *heading, *text, *buttons;
  int flags;
};

/*********
 * TILES *
 *********/

struct opener_floor {
  struct pos p;
  int event;
  bool pressed;
  bool prev_pressed;
  bool noise;
  uint64_t priority;
};

struct closer_floor {
  struct pos p;
  int event;
  bool pressed;
  bool prev_pressed;
  bool noise;
  uint64_t priority;
};

struct door {
  struct pos p;
  int i;
  enum {
    NO_DOOR_ACTION, OPEN_DOOR, STAY_OPEN_DOOR,
    CLOSE_DOOR, ABRUPTLY_CLOSE_DOOR,
  } action;
  int wait;
  bool noise;
  uint64_t priority;
};

struct level_door {
  struct pos p;
  int i;
  bool broken;
  enum {
    NO_LEVEL_DOOR_ACTION, OPEN_LEVEL_DOOR, CLOSE_LEVEL_DOOR,
  } action;
  bool no_stairs;
  uint64_t priority;
};

struct loose_floor {
  struct pos p, original_pos;
  int i;
  int resist;
  int state;
  bool cant_fall;
  bool broken;

  enum {
    NO_LOOSE_FLOOR_ACTION,
    SHAKE_LOOSE_FLOOR,
    RELEASE_LOOSE_FLOOR,
    FALL_LOOSE_FLOOR,
    BROKEN_LOOSE_FLOOR,
  } action;

  struct frame f;
};

struct spikes_floor {
  struct pos p;
  int i, wait, state;
  bool inactive;
  bool activate;
};

struct chomper {
  struct pos p;
  int i;
  int wait;
  bool blood;
  bool activate;
  bool inactive;
  bool alert;
};

/********
 * UNDO *
 ********/

typedef void (*undo_f) (void *data, int dir);

struct undo {
  struct undo_pass {
    void *data;
    undo_f f;
    char *desc;
  } *pass;

  size_t count;
  int current;
};

union tile_state {
  struct closer_floor closer_floor;
  struct opener_floor opener_floor;
  struct spikes_floor spikes_floor;
  struct loose_floor loose_floor;
  struct level_door level_door;
  struct door door;
  struct chomper chomper;
};

struct tile_undo {
  struct tile b, f;
  union tile_state bs, fs;
  struct pos p;
};

struct mirror_pos_undo {
  struct pos p0, p1;
  bool invert_dir;
};

struct level_undo {
  struct level b; struct level f;
};

struct event_undo {
  int e;
  struct level_event b, f;
};

struct random_room_mirror_tile_undo {
  struct pos p[FLOORS][PLACES];
  int room;
  bool invert_dir;
};

struct link_undo {
  struct room_linking b[ROOMS], f[ROOMS];
};

struct start_pos_undo {
  struct pos b, f;
};

struct guard_start_pos_undo {
  int i;
  struct pos b, f;
};

struct guard_skill_undo {
  int i;
  struct skill b_skill, f_skill;
};

struct indexed_int_undo {
  int i, b, f;
};

struct int_undo {
  int *i, b, f;
};

/* misc */

struct tile_copy {
  struct tile c;
  union tile_state cs;
};

struct room_copy {
  struct tile_copy c[FLOORS][PLACES];
};

#endif	/* MININIM_TYPES_H */
