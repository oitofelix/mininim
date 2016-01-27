/*
  mininim.c -- MININIM main module;

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

ALLEGRO_TIMER *play_time;
enum vm vm = VGA;
enum em em = DUNGEON;
enum em original_em = DUNGEON;
bool force_em = false;
enum gm gm = ORIGINAL_GM;
bool immortal_mode;
int initial_total_lives = KID_INITIAL_TOTAL_LIVES, total_lives;
int initial_current_lives = KID_INITIAL_CURRENT_LIVES, current_lives;
int start_level;
int time_limit = TIME_LIMIT;
struct skill skill = {.counter_attack_prob = INITIAL_KCA,
                      .counter_defense_prob = INITIAL_KCD};
char *data_path;
static bool sound_disabled_cmd;

static error_t parser (int key, char *arg, struct argp_state *state);

enum options {
  VIDEO_MODE_OPTION = 256, ENVIRONMENT_MODE_OPTION, GUARD_MODE_OPTION,
  SOUND_OPTION, DISPLAY_FLIP_MODE_OPTION, KEYBOARD_FLIP_MODE_OPTION,
  MIRROR_MODE_OPTION, BLIND_MODE_OPTION, IMMORTAL_MODE_OPTION,
  TOTAL_LIVES_OPTION, START_LEVEL_OPTION, TIME_LIMIT_OPTION,
  KCA_OPTION, KCD_OPTION, DATA_PATH_OPTION, FULLSCREEN_OPTION,
  WINDOW_POSITION_OPTION, WINDOW_DIMENSIONS_OPTION,
  INHIBIT_SCREENSAVER_OPTION
};

static struct argp_option options[] = {
  {"video-mode", VIDEO_MODE_OPTION, "VIDEO-MODE", 0, "Select video mode.  Valid values for VIDEO-MODE are: VGA, EGA, CGA and HGC.  The default is VGA.  This can be changed in-game by the F12 key.", 0},
  {"environment-mode", ENVIRONMENT_MODE_OPTION, "ENVIRONMENT-MODE", 0, "Select environment mode.  Valid values for ENVIRONMENT-MODE are: ORIGINAL, DUNGEON and PALACE.  The 'ORIGINAL' value gives level modules autonomy in this choice for each particular level.  This is the default.  This can be changed in-game by the F11 key.", 0},
  {"guard-mode", GUARD_MODE_OPTION, "GUARD-MODE", 0, "Select guard mode.  Valid values for GUARD-MODE are: ORIGINAL, GUARD, FAT-GUARD, VIZIER, SKELETON and SHADOW.  The 'ORIGINAL' value gives level modules autonomy in this choice for each particular guard.  This is the default.  This can be changed in-game by the F10 key.", 0},
  {"sound", SOUND_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable sound.  The default is TRUE.  This can be changed in-game by the CTRL+S keystroke.", 0},
  {"display-flip-mode", DISPLAY_FLIP_MODE_OPTION, "DISPLAY-FLIP-MODE", 0, "Select display flip mode.  Valid values for DISPLAY-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+I keystroke.", 0},
  {"keyboard-flip-mode", KEYBOARD_FLIP_MODE_OPTION, "KEYBOARD-FLIP-MODE", 0, "Select keyboard flip mode.  Valid values for KEYBOARD-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+K keystroke.", 0},
  {"mirror-mode", MIRROR_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable mirror mode.  In mirror mode the screen and the keyboard are flipped horizontally.  This is equivalent of specifying both the options --display-flip-mode=horizontal and --keyboard-flip-mode=horizontal.  The default is FALSE.  This can be changed in-game by the SHIFT+I and SHIFT+K keystrokes for the display and keyboard, respectively.", 0},
  {"blind-mode", BLIND_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable blind mode.  In blind mode background and non-animated sprites are not drawn.  The default is FALSE.  This can be changed in-game by the SHIFT+B keystroke.", 0},
  {"immortal-mode", IMMORTAL_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable immortal mode.  In immortal mode the kid can't be harmed.  The default is FALSE.  This can be changed in-game by the I key.", 0},
  {"total-lives", TOTAL_LIVES_OPTION, "N", 0, "Make the kid start with N total lives.  The default is 3.  Valid integers range from 1 to 10.  This can be changed in-game by the SHIFT+T keystroke.", 0},
  {"start-level", START_LEVEL_OPTION, "N", 0, "Make the kid start at level N.  The default is 1.  Valid integers range from 1 to INT_MAX.  This can be changed in-game by the SHIFT+L keystroke.", 0},
  {"time-limit", TIME_LIMIT_OPTION, "N", 0, "Set the time limit to complete the game to N seconds.  The default is 3600.  Valid integers range from 1 to INT_MAX.  This can be changed in-game by the + and - keys.", 0},
  {"kca", KCA_OPTION, "N", 0, "Set kid's counter attack skill to N.  The default is 0.  Valid integers range from 0 to 100.  This can be changed in-game by the CTRL+= and CTRL+- keys.", 0},
  {"kcd", KCD_OPTION, "N", 0, "Set kid's counter defense skill to N.  The default is 0.  Valid integers range from 0 to 100.  This can be changed in-game by the ALT+= and ALT+- keys.", 0},
  {"data-path", DATA_PATH_OPTION, "PATH", 0, "Set data path to PATH.  Normally, the data files are looked for in the current working directory, and then in the hard-coded package data directory.  If this option is given, before looking there the data files are looked for in PATH.", 0},
  {"fullscreen", FULLSCREEN_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable fullscreen mode.  In fullscreen mode the window spans the entire screen.  The default is FALSE.  This can be changed in-game by the F key.", 0},
  {"window-position", WINDOW_POSITION_OPTION, "X,Y", 0, "Place the window at screen coordinates X,Y.  The default is to let this choice to the window manager.  The values X and Y are integers and must be separated by a comma.", 0},
  {"window-dimensions", WINDOW_DIMENSIONS_OPTION, "WxH", 0, "Set window width and height to W and H, respectively.  The default is 640x400.  The values W and H are strictly positive integers and must be separated by an 'x'.", 0},
  {"inhibit-screensaver", INHIBIT_SCREENSAVER_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Prevent the system screensaver from starting up.  The default is FALSE.", 0},
  {0},
};

static const char *doc = "MININIM: The Advanced Prince of Persia Engine\n(a childhood dream)\v\
Unless otherwise noted, option values are case insensitive but must be specified in their entirety.  Long option names on the other hand, can be partially specified as long as they are kept unambiguous.  BOOLEAN is FALSE to disable the respective feature, and any other value (even the null string or no string at all) to enable it.  For any non-specified option the documented default applies.  Integers can be specified in any of the formats defined by the C language.  Key and keystroke references are based on the default mapping.";

struct argp_child argp_child = { NULL };

static struct argp argp = {options, parser, NULL, NULL, &argp_child, NULL, NULL};

static error_t
parser (int key, char *arg, struct argp_state *state)
{
  int x, y;

  switch (key) {
  case VIDEO_MODE_OPTION:
    if (! strcasecmp ("VGA", arg)) vm = VGA;
    else if (! strcasecmp ("EGA", arg)) vm = EGA;
    else if (! strcasecmp ("CGA", arg)) vm = CGA;
    else if (! strcasecmp ("HGC", arg)) vm = CGA, hgc = true;
    else argp_error (state, "'%s' is not a valid value for the option 'video-mode'.\nValid values are: VGA, EGA, CGA and HGC.", arg);
    break;
  case ENVIRONMENT_MODE_OPTION:
    if (! strcasecmp ("ORIGINAL", arg)) force_em = false;
    else if (! strcasecmp ("DUNGEON", arg)) force_em = true, em = DUNGEON;
    else if (! strcasecmp ("PALACE", arg)) force_em = true, em = PALACE;
    else argp_error (state, "'%s' is not a valid value for the option 'environment-mode'.\nValid values are: ORIGINAL, DUNGEON and PALACE.", arg);
    break;
  case GUARD_MODE_OPTION:
    if (! strcasecmp ("ORIGINAL", arg)) gm = ORIGINAL_GM;
    else if (! strcasecmp ("GUARD", arg)) gm = GUARD_GM;
    else if (! strcasecmp ("FAT-GUARD", arg)) gm = FAT_GUARD_GM;
    else if (! strcasecmp ("VIZIER", arg)) gm = VIZIER_GM;
    else if (! strcasecmp ("SKELETON", arg)) gm = SKELETON_GM;
    else if (! strcasecmp ("SHADOW", arg)) gm = SHADOW_GM;
    else argp_error (state, "'%s' is not a valid value for the option 'guard-mode'.\nValid values are: ORIGINAL, GUARD, FAT-GUARD, VIZIER, SKELETON and SHADOW.", arg);
    break;
  case SOUND_OPTION:
    if (! arg || strcasecmp ("FALSE", arg)) sound_disabled_cmd = false; /* true */
    else sound_disabled_cmd = true; /* false */
    break;
  case DISPLAY_FLIP_MODE_OPTION:
    if (! strcasecmp ("NONE", arg))
      screen_flags = 0;
    else if (! strcasecmp ("VERTICAL", arg))
      screen_flags = ALLEGRO_FLIP_VERTICAL;
    else if (! strcasecmp ("HORIZONTAL", arg))
      screen_flags = ALLEGRO_FLIP_HORIZONTAL;
    else if (! strcasecmp ("VERTICAL-HORIZONTAL", arg))
      screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL;
    else argp_error (state, "'%s' is not a valid value for the option 'display-flip'.\nValid values are: NONE, VERTICAL, HORIZONTAL, VERTICAL-HORIZONTAL.", arg);
    break;
  case KEYBOARD_FLIP_MODE_OPTION:
    if (! strcasecmp ("NONE", arg)) {
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = false;
    } else if (! strcasecmp ("VERTICAL", arg)) {
      flip_keyboard_vertical = true;
      flip_keyboard_horizontal = false;
    } else if (! strcasecmp ("HORIZONTAL", arg)) {
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = true;
    } else if (! strcasecmp ("VERTICAL-HORIZONTAL", arg)) {
      flip_keyboard_vertical = true;
      flip_keyboard_horizontal = true;
    } else argp_error (state, "'%s' is not a valid value for the option 'keyboard-flip'.\nValid values are: NONE, VERTICAL, HORIZONTAL, VERTICAL-HORIZONTAL.", arg);
    break;
  case MIRROR_MODE_OPTION:
    if (! arg || strcasecmp ("FALSE", arg)) {
      /* true */
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = true;
      screen_flags = ALLEGRO_FLIP_HORIZONTAL;
    } else {
      /* false */
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = false;
      screen_flags = 0;
    }
    break;
  case BLIND_MODE_OPTION:
    if (! arg || strcasecmp ("FALSE", arg)) {
      /* true */
      no_room_drawing = true;
    } else {
      /* false */
      no_room_drawing = false;
    }
    break;
  case IMMORTAL_MODE_OPTION:
    if (! arg || strcasecmp ("FALSE", arg)) {
      /* true */
      immortal_mode = true;
    } else {
      /* false */
      immortal_mode = false;
    }
    break;
  case TOTAL_LIVES_OPTION:
    if (sscanf (arg, "%i", &initial_total_lives) != 1
        || initial_total_lives < 1 || initial_total_lives > 10)
      argp_error (state, "'%s' is not a valid integer for the option 'total-lives'.\nValid integers range from 1 to 10.", arg);
    break;
  case START_LEVEL_OPTION:
    if (sscanf (arg, "%i", &start_level) != 1
        || start_level < 1)
      argp_error (state, "'%s' is not a valid integer for the option 'start-level'.\nValid integers range from 1 to INT_MAX.", arg);
    break;
  case TIME_LIMIT_OPTION:
    if (sscanf (arg, "%i", &time_limit) != 1
        || time_limit < 1)
      argp_error (state, "'%s' is not a valid integer for the option 'time-limit'.\nValid integers range from 1 to INT_MAX.", arg);
    break;
  case KCA_OPTION:
    if (sscanf (arg, "%i", &skill.counter_attack_prob) != 1
        || skill.counter_attack_prob < 0 || skill.counter_attack_prob > 100)
      argp_error (state, "'%s' is not a valid integer for the option 'kca'.\nValid integers range from 0 to 100.", arg);
    skill.counter_attack_prob--;
    break;
  case KCD_OPTION:
    if (sscanf (arg, "%i", &skill.counter_defense_prob) != 1
        || skill.counter_defense_prob < 0 || skill.counter_defense_prob > 100)
      argp_error (state, "'%s' is not a valid integer for the option 'kcd'.\nValid integers range from 0 to 100.", arg);
    skill.counter_defense_prob--;
    break;
  case DATA_PATH_OPTION: xasprintf (&data_path, "%s", arg); break;
  case FULLSCREEN_OPTION:
    if (! arg || strcasecmp ("FALSE", arg)) {
      /* true */
      al_set_new_display_flags (al_get_new_display_flags ()
                                | ALLEGRO_FULLSCREEN_WINDOW);
    } else {
      /* false */
      al_set_new_display_flags (al_get_new_display_flags ()
                                & ~ALLEGRO_FULLSCREEN_WINDOW);
    }
    break;
  case WINDOW_POSITION_OPTION:
    if (sscanf (arg, "%i,%i", &x, &y) != 2)
      argp_error (state, "'%s' is not a valid position for the option 'window-position'.\nValid values have the format X,Y where X and Y are integers.", arg);
    al_set_new_window_position (x, y);
    break;
  case WINDOW_DIMENSIONS_OPTION:
    if (sscanf (arg, "%ix%i", &display_width, &display_height) != 2
        || display_width < 1 || display_height < 1)
      argp_error (state, "'%s' is not a valid dimension for the option 'window-dimensions'.\nValid values have the format WxH where W and H are strictly positive integers.", arg);
    break;
  case INHIBIT_SCREENSAVER_OPTION:
    if (! arg || strcasecmp ("FALSE", arg)) {
      /* true */
      al_inhibit_screensaver (true);
    } else {
      /* false */
      al_inhibit_screensaver (false);
    }
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static void
version (FILE *stream, struct argp_state *state)
{
  fprintf (stream,
           "%s (%s) %s\n\n"	/* mininim (MININIM) a.b */

           /* TRANSLATORS: Use "Félix" in place of "Fe'lix" */
           "Copyright (C) %s " PACKAGE_COPYRIGHT_HOLDER " <%s>\n\n"

           "%s\n\n"		/* License GPLv3+... */
           "%s\n",		/* Written by... */
           PACKAGE, PACKAGE_NAME, VERSION,
           "2015, 2016", "oitofelix@gnu.org",
           "\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.",

           /* TRANSLATORS: Use "Félix" in place of "F'elix" */
           "Written by Bruno Fe'lix Rezende Ribeiro.");
}


int
main (int argc, char **argv)
{
  al_init ();

  argp_program_version_hook = version;
  argp.doc = doc;

  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  init_video ();
  init_audio ();
  if (sound_disabled_cmd) enable_audio (false);
  init_keyboard ();

  draw_text (screen, "Loading....", ORIGINAL_WIDTH / 2.0, ORIGINAL_HEIGHT / 2.0,
             ALLEGRO_ALIGN_CENTRE);
  show ();

  load_samples ();
  load_level ();
  load_cutscenes ();

 restart_game:
  total_lives = initial_total_lives;
  current_lives = initial_current_lives;

  clear_bitmap (screen, BLACK);
  clear_bitmap (uscreen, TRANSPARENT_COLOR);
  cutscene_started = false;
  stop_all_samples ();

  /* begin test */
  /* cutscene = true; */
  /* play_anim (cutscene_11_little_time_left_anim, NULL, 10); */
  /* exit (0); */
  /* end test */

  play_title ();
  stop_video_effect ();
  if (quit_anim == QUIT_GAME) goto quit_game;
  stop_all_samples ();

  if (! play_time) play_time = create_timer (1.0);
  al_set_timer_count (play_time, 0);
  al_start_timer (play_time);

  /* play_level_1 (); */
  /* play_consistency_level (); */
  play_legacy_level (start_level);
  if (quit_anim == RESTART_GAME) goto restart_game;

 quit_game:
  unload_level ();
  unload_cutscenes ();
  unload_samples ();

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