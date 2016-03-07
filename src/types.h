/*
  types.h -- types definitions;

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

#ifndef MININIM_TYPES_H
#define MININIM_TYPES_H

/* video mode */
enum vm {
  CGA, EGA, VGA,
};

struct pos {
  int room, floor, place;
};

struct frameset {
  ALLEGRO_BITMAP *frame;
  int dx, dy;
};

struct coord {
  int room, x, y;
};

struct rect {
  struct coord c;
  int w, h;
};

struct dim {
  int x, y, w, h, fx, bx;
};

enum dir {
  LEFT, RIGHT, ABOVE, BELOW
};

enum carpet_design {
  CARPET_00,
  CARPET_01,
  ARCH_CARPET_LEFT,
  ARCH_CARPET_RIGHT_00,
  ARCH_CARPET_RIGHT_01,
};

enum gm {
  ORIGINAL_GM, GUARD_GM, FAT_GUARD_GM, VIZIER_GM, SKELETON_GM, SHADOW_GM
};

enum anim_type {
  NO_ANIM, KID, SHADOW, GUARD, FAT_GUARD, SKELETON, VIZIER, PRINCESS, MOUSE
};

struct skill {
  int attack_prob;
  int counter_attack_prob;
  int defense_prob;
  int counter_defense_prob;
  int advance_prob;
  int return_prob;
  int refraction;
  int extra_life;
};

struct gamepad_state {
  bool up, down, left, right, shift, enter;
};

struct level {
  void (*start) (void);
  void (*special_events) (void);
  void (*end) (struct pos *p);
  void (*next_level) (int);
  void (*cutscene) (void);
  int number;
  int nominal_number;

  struct con {
    enum confg {
      NO_FLOOR,
      FLOOR,
      BROKEN_FLOOR,
      SKELETON_FLOOR,
      LOOSE_FLOOR,
      SPIKES_FLOOR,
      OPENER_FLOOR,
      CLOSER_FLOOR,
      STUCK_FLOOR,
      HIDDEN_FLOOR,
      PILLAR,
      BIG_PILLAR_BOTTOM,
      BIG_PILLAR_TOP,
      WALL,
      DOOR,
      LEVEL_DOOR,
      CHOPPER,
      MIRROR,
      CARPET,
      TCARPET,
      ARCH_BOTTOM,
      ARCH_TOP_LEFT,
      ARCH_TOP_RIGHT,
      ARCH_TOP_MID,
      ARCH_TOP_SMALL,
    } fg;
    enum conbg {
      NO_BG,
      BRICKS_00,
      BRICKS_01,
      BRICKS_02,
      BRICKS_03,
      NO_BRICKS,
      TORCH,
      WINDOW,
      BALCONY,
    } bg;
    union conext {
      int event;
      int design;
      int step;
      bool cant_fall;
      enum item {
        NO_ITEM,
        EMPTY_POTION,
        SMALL_LIFE_POTION,
        BIG_LIFE_POTION,
        SMALL_POISON_POTION,
        BIG_POISON_POTION,
        FLOAT_POTION,
        FLIP_POTION,
        ACTIVATION_POTION,
        SWORD,
      } item;
    } ext;
  } con[ROOMS][FLOORS][PLACES];

  struct {
    int l, r, a, b;
  } link[ROOMS];

  struct level_event {
    struct pos p;
    bool next;
  } event[EVENTS];

  struct pos start_pos;
  enum dir start_dir;

  struct guard {
    enum anim_type type;
    struct pos p;
    enum dir dir;
    struct skill skill;
    int total_lives;
    int style;
  } guard[GUARDS];

  enum em {
    DUNGEON, PALACE,
  } em;

  enum hue {
    HUE_NONE, HUE_GREEN, HUE_GRAY, HUE_YELLOW, HUE_BLUE,
  } hue;
};

struct legacy_level {
  uint8_t foretable[LROOMS][FLOORS][PLACES];
  uint8_t backtable[LROOMS][FLOORS][PLACES];
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

/* avoid "'struct' declared inside parameter list" error for the
   ACTION definition */
struct anim *_action;
typedef void (*ACTION) (struct anim *a);

struct anim {
  enum anim_type type;
  enum anim_type original_type;

  int id;
  int shadow_of;

  struct frame {
    void *id;
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
    int dx, dy;
  } fo;

  struct frame_offset xf;

  struct collision_info {
    enum confg t;
    struct pos p;
  } ci;

  struct gamepad_state key;

  ACTION oaction;
  ACTION action;
  ACTION hang_caller;
  int i, j, wait, repeat, cinertia, inertia, walk,
    total_lives, current_lives;
  bool reverse, collision, fall, hit_ceiling,
    just_hanged, hang, hang_limit, misstep, uncouch_slowly,
    keep_sword_fast, turn, shadow, splash, hit_by_loose_floor,
    invisible, has_sword, hurt, controllable, fight, crossing_mirror;

  int attack_defended, counter_attacked, counter_defense;
  bool hurt_enemy_in_counter_attack;

  struct skill skill;
  int refraction;

  ALLEGRO_TIMER *floating;
  ALLEGRO_SAMPLE_INSTANCE *sample;

  int dc, df, dl, dcl, dch, dcd;

  bool immortal, poison_immune, loose_floor_immune, fall_immune,
    spikes_immune, chopper_immune, sword_immune;

  int enemy_id, oenemy_id;
  bool enemy_aware;

  bool dont_draw_lives;

  int style;

  enum confg confg;

  enum item item;

  uint64_t alert_cycle;

  struct pos p, item_pos, hang_pos, enemy_pos;

  enum death_reason {
    NO_DEATH, FALL_DEATH, LOOSE_FLOOR_DEATH, POTION_DEATH,
    SPIKES_DEATH, CHOPPER_DEATH, FIGHT_DEATH, SHADOW_FIGHT_DEATH
  } death_reason;

  /* depressible floor */
  struct pos df_pos[2];
  struct pos df_posb[2];
};

typedef struct coord *(*coord_f) (struct frame *, struct coord *);
typedef struct pos *(*pos_f) (struct coord *, struct pos *);
typedef struct coord *(*pos2coord_f) (struct pos *, struct coord *);
typedef bool (*pos_pred) (struct pos *, void*);

struct room_dist {
  int dist;
  bool visited;
};

typedef void *(*load_resource_f) (char *);

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
  NO_QUIT, RESTART_LEVEL, NEXT_LEVEL, RESTART_GAME, QUIT_GAME, OUT_OF_TIME
};

struct audio_sample {
  ALLEGRO_SAMPLE *sample;
  ALLEGRO_SAMPLE_INSTANCE *instance;
  bool played;
  uint64_t anim_cycle;
  int room;
  float volume;
};

typedef bool (*confg_set) (enum confg);

typedef void (*room_callback_f) (int, int);

struct star {
  int x, y;
  int color;
};

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

enum options {
  VIDEO_MODE_OPTION = 256, ENVIRONMENT_MODE_OPTION, GUARD_MODE_OPTION,
  SOUND_OPTION, DISPLAY_FLIP_MODE_OPTION, GAMEPAD_FLIP_MODE_OPTION,
  MIRROR_MODE_OPTION, BLIND_MODE_OPTION, IMMORTAL_MODE_OPTION,
  TOTAL_LIVES_OPTION, START_LEVEL_OPTION, TIME_LIMIT_OPTION,
  KCA_OPTION, KCD_OPTION, DATA_PATH_OPTION, FULLSCREEN_OPTION,
  WINDOW_POSITION_OPTION, WINDOW_DIMENSIONS_OPTION,
  INHIBIT_SCREENSAVER_OPTION, PRINT_PATHS_OPTION,
  LEVEL_MODULE_OPTION, SKIP_TITLE_OPTION, START_TIME_OPTION,
  LOAD_CONFIG_OPTION, IGNORE_MAIN_CONFIG_OPTION, IGNORE_ENVIRONMENT_OPTION,
  JOYSTICK_AXIS_THRESHOLD_OPTION, JOYSTICK_BUTTON_THRESHOLD_OPTION,
  JOYSTICK_AXIS_OPTION, JOYSTICK_BUTTON_OPTION, JOYSTICK_INFO_OPTION,
  START_POS_OPTION, HUE_MODE_OPTION,
};

enum level_module {
  LEGACY_LEVEL_MODULE, PLV_LEVEL_MODULE, DAT_LEVEL_MODULE, CONSISTENCY_LEVEL_MODULE,
} level_module;

enum edit {
  EDIT_NONE, EDIT_MAIN, EDIT_CON, EDIT_FG, EDIT_FLOOR, EDIT_PILLAR,
  EDIT_DOOR, EDIT_CARPET, EDIT_ARCH, EDIT_BG, EDIT_EXT,
  EDIT_JUMP_ROOM, EDIT_INFO, EDIT_EVENT, EDIT_EVENT_SET, EDIT_EVENT2DOOR,
  EDIT_EVENT2FLOOR,
};

struct menu_item {
  char key;
  char *desc;
};

/****************
 * CONSTRUCTIONS
 ****************/

struct closer_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  bool broken;
  bool unresponsive;
};

struct door {
  struct pos p;
  int i;
  enum {
    NO_DOOR_ACTION, OPEN_DOOR, CLOSE_DOOR, ABRUPTLY_CLOSE_DOOR,
  } action;
  int wait;
  bool noise;
};

struct level_door {
  struct pos p;
  int i;
  bool broken;
  enum {
    NO_LEVEL_DOOR_ACTION, OPEN_LEVEL_DOOR, CLOSE_LEVEL_DOOR,
  } action;
  bool no_stairs;
};

struct loose_floor {
  struct pos p;
  int i;
  int resist;
  int state;
  bool cant_fall;

  enum {
    NO_LOOSE_FLOOR_ACTION,
    SHAKE_LOOSE_FLOOR,
    RELEASE_LOOSE_FLOOR,
    FALL_LOOSE_FLOOR,
  } action;

  struct frame f;
};

struct mirror {
  struct pos p;
  int kid_crossing;
};

struct opener_floor {
  struct pos p;
  int event;
  bool pressed;
  bool noise;
  bool broken;
};

struct spikes_floor {
  struct pos p;
  int i, wait, state;
  bool inactive;
  int murdered_anim;
  bool activate;
};

#endif	/* MININIM_TYPES_H */
