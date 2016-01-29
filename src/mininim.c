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

static char **argv;
static size_t argc;
static char **eargv;
static size_t eargc;
static char **cargv;
static size_t cargc;

char *resources_dir,
  *temp_dir,
  *user_home_dir,
  *user_documents_dir,
  *user_data_dir,
  *user_settings_dir,
  *system_data_dir = PKGDATADIR,
  *data_dir,
  *exe_filename,
  *config_filename;

enum option_phase {
  CONFIGURATION_FILE_OPTION_PHASE, ENVIRONMENT_VARIABLES_OPTION_PHASE,
  COMMAND_LINE_OPTION_PHASE,
} option_phase;

ALLEGRO_THREAD *load_config_dialog_thread, *save_game_dialog_thread;

ALLEGRO_TIMER *play_time;
enum vm vm = VGA;
enum em em = DUNGEON;
enum em original_em = DUNGEON;
bool force_em = false;
enum gm gm = ORIGINAL_GM;
bool immortal_mode;
int initial_total_lives = KID_INITIAL_TOTAL_LIVES, total_lives;
int initial_current_lives = KID_INITIAL_CURRENT_LIVES, current_lives;
int start_level = 1;
int time_limit = TIME_LIMIT;
int start_time = START_TIME;
struct skill skill = {.counter_attack_prob = INITIAL_KCA,
                      .counter_defense_prob = INITIAL_KCD};
static bool sound_disabled_cmd;
static bool skip_title;

static error_t parser (int key, char *arg, struct argp_state *state);
static void draw_loading_screen (void);
static void print_paths (void);
static char *env_option_name (const char *option_name);
static char *config_option_name (const char *option_name);
static void get_paths (void);

enum options {
  VIDEO_MODE_OPTION = 256, ENVIRONMENT_MODE_OPTION, GUARD_MODE_OPTION,
  SOUND_OPTION, DISPLAY_FLIP_MODE_OPTION, KEYBOARD_FLIP_MODE_OPTION,
  MIRROR_MODE_OPTION, BLIND_MODE_OPTION, IMMORTAL_MODE_OPTION,
  TOTAL_LIVES_OPTION, START_LEVEL_OPTION, TIME_LIMIT_OPTION,
  KCA_OPTION, KCD_OPTION, DATA_PATH_OPTION, FULLSCREEN_OPTION,
  WINDOW_POSITION_OPTION, WINDOW_DIMENSIONS_OPTION,
  INHIBIT_SCREENSAVER_OPTION, PRINT_PATHS_OPTION,
  LEVEL_MODULE_OPTION, SKIP_TITLE_OPTION, START_TIME_OPTION,
};

enum level_module {
  LEGACY_LEVEL_MODULE, CONSISTENCY_LEVEL_MODULE,
} level_module;

static struct argp_option options[] = {
  /* Level */
  {NULL, 0, NULL, 0, "Level:", 0},
  {"level-module", LEVEL_MODULE_OPTION, "LEVEL-MODULE", 0, "Select level module.  A level module determines a way to generate consecutive levels for use by the engine.  Valid values for LEVEL-MODULE are: LEGACY and CONSISTENCY.  LEGACY is the module designed to read the original unarchived PoP 1 DOS level files.  CONSISTENCY is the module designed to generate random-corrected levels for accessing the engine robustness.  The default is LEGACY.", 0},
  {"start-level", START_LEVEL_OPTION, "N", 0, "Make the kid start at level N.  The default is 1.  Valid integers range from 1 to INT_MAX.  This can be changed in-game by the SHIFT+L keystroke.", 0},

  /* Game */
  {NULL, 0, NULL, 0, "Game:", 0},
  {"immortal-mode", IMMORTAL_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable immortal mode.  In immortal mode the kid can't be harmed.  The default is FALSE.  This can be changed in-game by the I key.", 0},
  {"total-lives", TOTAL_LIVES_OPTION, "N", 0, "Make the kid start with N total lives.  The default is 3.  Valid integers range from 1 to 10.  This can be changed in-game by the SHIFT+T keystroke.", 0},
  {"time-limit", TIME_LIMIT_OPTION, "N", 0, "Set the time limit to complete the game to N seconds.  The default is 3600.  Valid integers range from 1 to INT_MAX.  This can be changed in-game by the + and - keys.", 0},
  {"start-time", START_TIME_OPTION, "N", 0, "Set the play time counter.  The default is 0.  Valid integers range from 0 to INT_MAX.", 0},
  {"kca", KCA_OPTION, "N", 0, "Set kid's counter attack skill to N.  The default is 0.  Valid integers range from 0 to 100.  This can be changed in-game by the CTRL+= and CTRL+- keys.", 0},
  {"kcd", KCD_OPTION, "N", 0, "Set kid's counter defense skill to N.  The default is 0.  Valid integers range from 0 to 100.  This can be changed in-game by the ALT+= and ALT+- keys.", 0},

  /* Rendering */
  {NULL, 0, NULL, 0, "Rendering:", 0},
  {"video-mode", VIDEO_MODE_OPTION, "VIDEO-MODE", 0, "Select video mode.  Valid values for VIDEO-MODE are: VGA, EGA, CGA and HGC.  The default is VGA.  This can be changed in-game by the F12 key.", 0},
  {"environment-mode", ENVIRONMENT_MODE_OPTION, "ENVIRONMENT-MODE", 0, "Select environment mode.  Valid values for ENVIRONMENT-MODE are: ORIGINAL, DUNGEON and PALACE.  The 'ORIGINAL' value gives level modules autonomy in this choice for each particular level.  This is the default.  This can be changed in-game by the F11 key.", 0},
  {"guard-mode", GUARD_MODE_OPTION, "GUARD-MODE", 0, "Select guard mode.  Valid values for GUARD-MODE are: ORIGINAL, GUARD, FAT-GUARD, VIZIER, SKELETON and SHADOW.  The 'ORIGINAL' value gives level modules autonomy in this choice for each particular guard.  This is the default.  This can be changed in-game by the F10 key.", 0},
  {"display-flip-mode", DISPLAY_FLIP_MODE_OPTION, "DISPLAY-FLIP-MODE", 0, "Select display flip mode.  Valid values for DISPLAY-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+I keystroke.", 0},
  {"mirror-mode", MIRROR_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable mirror mode.  In mirror mode the screen and the keyboard are flipped horizontally.  This is equivalent of specifying both the options --display-flip-mode=horizontal and --keyboard-flip-mode=horizontal.  The default is FALSE.  This can be changed in-game by the SHIFT+I and SHIFT+K keystrokes for the display and keyboard, respectively.", 0},
  {"blind-mode", BLIND_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable blind mode.  In blind mode background and non-animated sprites are not drawn.  The default is FALSE.  This can be changed in-game by the SHIFT+B keystroke.", 0},

  /* Window */
  {NULL, 0, NULL, 0, "Window:", 0},
  {"fullscreen", FULLSCREEN_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable fullscreen mode.  In fullscreen mode the window spans the entire screen.  The default is FALSE.  This can be changed in-game by the F key.", 0},
  {"window-position", WINDOW_POSITION_OPTION, "X,Y", 0, "Place the window at screen coordinates X,Y.  The default is to let this choice to the window manager.  The values X and Y are integers and must be separated by a comma.", 0},
  {"window-dimensions", WINDOW_DIMENSIONS_OPTION, "WxH", 0, "Set window width and height to W and H, respectively.  The default is 640x400.  The values W and H are strictly positive integers and must be separated by an 'x'.", 0},
  {"inhibit-screensaver", INHIBIT_SCREENSAVER_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Prevent the system screensaver from starting up.  The default is FALSE.", 0},

  /* Others */
  {NULL, 0, NULL, 0, "Others", 0},
  {"sound", SOUND_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable sound.  The default is TRUE.  This can be changed in-game by the CTRL+S keystroke.", 0},
  {"keyboard-flip-mode", KEYBOARD_FLIP_MODE_OPTION, "KEYBOARD-FLIP-MODE", 0, "Select keyboard flip mode.  Valid values for KEYBOARD-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+K keystroke.", 0},
  {"data-path", DATA_PATH_OPTION, "PATH", 0, "Set data path to PATH.  Normally, the data files are looked for in the current working directory, then in the user data directory, then in the resources directory, and finally in the system data directory.  If this option is given, before looking there the data files are looked for in PATH.", 0},
  {"print-paths", PRINT_PATHS_OPTION, NULL, 0, "Print paths and exit.", 0},
  {"skip-title", SKIP_TITLE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Skip title screen.  The default is FALSE.", 0},

  /* Help */
  {NULL, 0, NULL, 0, "Help:", -1},
  {0},
};

static const char *doc = "MININIM: The Advanced Prince of Persia Engine\n(a childhood dream)\v\
Long option names are case sensitive.  Option values are case insensitive.   Both can be partially specified as long as they are kept unambiguous.  BOOLEAN is an integer equating to 0, or any sub-string (including the null string) of 'FALSE', 'OFF' or 'NO' to disable the respective feature, and any other value (even no string at all) to enable it.  For any non-specified option the documented default applies.  Integers can be specified in any of the formats defined by the C language.  Key and keystroke references are based on the default mapping.  For every command line option of the form 'x-y' there is an equivalent environment variable option 'MININIM_X_Y' and an equivalent configuration file option 'x y'.  The increasing order of precedence for options is: configuration file, environment variables, and then command line.";

struct argp_child argp_child = { NULL };

static struct argp argp = {options, parser, NULL, NULL, &argp_child, NULL, NULL};

static char *
key_to_option_name (int key)
{
  char *option_name;

  size_t i;
  for (i = 0; options[i].name != NULL
         || options[i].key != 0
         || options[i].arg != NULL
         || options[i].flags != 0
         || options[i].doc != NULL
         || options[i].group != 0; i++)
    if (options[i].key == key) {
      switch (option_phase) {
      case CONFIGURATION_FILE_OPTION_PHASE:
        return config_option_name (options[i].name);
      case ENVIRONMENT_VARIABLES_OPTION_PHASE:
        return env_option_name (options[i].name);
      case COMMAND_LINE_OPTION_PHASE:
        xasprintf (&option_name, "%s", options[i].name);
        return option_name;
      }
    }

  return NULL;
}

static void
option_enum_value_error (int key, char *arg, struct argp_state *state,
                         char **enum_vals, bool invalid)
{
  char *msg = "";
  char *option_name = key_to_option_name (key);

  switch (option_phase) {
  case CONFIGURATION_FILE_OPTION_PHASE:
    msg = invalid
      ? "is not a valid value for the configuration file option"
      : "is an ambiguous value for the configuration file option";
    break;
  case ENVIRONMENT_VARIABLES_OPTION_PHASE:
    msg = invalid
      ? "is not a valid value for the option environment variable"
      : "is an ambiguous value for the environment variable option";
    break;
  case COMMAND_LINE_OPTION_PHASE:
    msg = invalid
      ? "is not a valid value for the command line option"
      : "is an ambiguous value for the command line option";
    break;
  }

  char *prefix = invalid ? "" : arg;

  char *valid_values = NULL;
  size_t i;
  for (i = 0; enum_vals[i] != NULL; i++)
    if (strcasestr (enum_vals[i], prefix) == enum_vals[i]) {
      char *tmpstr = valid_values;
      if (! valid_values)
        xasprintf (&valid_values, "'%s'", enum_vals[i]);
      else
        xasprintf (&valid_values, "%s, '%s'", valid_values, enum_vals[i]);
      if (tmpstr) al_free (tmpstr);
    }

  char *msg2;
  if (invalid) xasprintf (&msg2, "%s", "Valid values are:");
  else xasprintf (&msg2, "Valid values starting with '%s' are:", arg);

  char *error_template = "'%s' %s '%s'.\n%s %s.\n";
  if (option_phase == CONFIGURATION_FILE_OPTION_PHASE
      && state->flags & ARGP_SILENT)
    al_append_native_text_log (state->input, error_template,
                               arg, msg, option_name, msg2, valid_values);
  else argp_error (state, error_template,
                   arg, msg, option_name, msg2, valid_values);

  al_free (option_name);
  al_free (msg2);
  al_free (valid_values);
}

static error_t
optval_to_enum (int *retval, int key, char *arg, struct argp_state *state,
                char **enum_vals)
{
  size_t i;
  int optval = -1;
  bool ambiguous = false;

  for (i = 0; enum_vals[i] != NULL; i++) {
    if (strcasestr (enum_vals[i], arg) == enum_vals[i]) {
      if (! strcasecmp (enum_vals[i], arg)) {
        *retval = i;
        return 0;
      }
      if (optval != -1) ambiguous = true;
      optval = i;
    }
  }

  if (optval == -1) {
    option_enum_value_error (key, arg, state, enum_vals, true);
    return EINVAL;
  }
  else if (ambiguous) {
    option_enum_value_error (key, arg, state, enum_vals, false);
    return EINVAL;
  }

  *retval = optval;

  return 0;
}

static bool
optval_to_bool (char *arg)
{
  int i;
  char *FALSE = "FALSE";
  char *OFF = "OFF";
  char *NO = "NO";

  if (! arg) return true;

  if (sscanf (arg, "%i", &i) == 1 && i == 0) return false;
  if (strcasestr (FALSE, arg) == FALSE) return false;
  if (strcasestr (OFF, arg) == OFF) return false;
  if (strcasestr (NO, arg) == NO) return false;

  return true;
}

static error_t
optval_to_int (int *retval, int key, char *arg, struct argp_state *state,
               int min, int max)
{
  int i;
  if (sscanf (arg, "%i", &i) != 1
      || i < min || i > max) {
    char *option_name = key_to_option_name (key);
    char *error_template = "'%s' is not a valid integer for the option '%s'.\n\
Valid integers range from %i to %i.\n";
    if (option_phase == CONFIGURATION_FILE_OPTION_PHASE
      && state->flags & ARGP_SILENT)
      al_append_native_text_log (state->input, error_template,
                                 arg, option_name, min, max);
    else argp_error (state, error_template, arg, option_name, min, max);
    al_free (option_name);
    return EINVAL;
  }
  *retval = i;
  return 0;
}

static error_t
optval_to_int_pair (int *a, int *b, int key, char *arg, struct argp_state *state,
                    int min, int max, char s, char A, char B)
{
  char *template;
  int _a, _b;
  xasprintf (&template, "%%i%c%%i", s);

  if (sscanf (arg, template, &_a, &_b) != 2
      || _a < min || _a > max || _b < min || _b > max) {
    char *option_name = key_to_option_name (key);
    char *error_template = "'%s' is not a valid integer pair for the option '%s'.\n\
Valid values have the form %c%c%c where %c and %c range from %i to %i.\n";
    if (option_phase == CONFIGURATION_FILE_OPTION_PHASE
        && state->flags & ARGP_SILENT)
      al_append_native_text_log (state->input, error_template,
                                 arg, option_name, A, s, B, A, B, min, max);
    else argp_error (state, error_template,
                     arg, option_name, A, s, B, A, B, min, max);
    al_free (template);
    al_free (option_name);
    return EINVAL;
  }

  *a = _a;
  *b = _b;
  al_free (template);
  return 0;
}

static error_t
parser (int key, char *arg, struct argp_state *state)
{
  int x, y, i, e;

  char *level_module_enum[] = {"LEGACY", "CONSISTENCY", NULL};

  char *video_mode_enum[] = {"VGA", "EGA", "CGA", "HGC", NULL};

  char *environment_mode_enum[] = {"ORIGINAL", "DUNGEON", "PALACE", NULL};

  char *guard_mode_enum[] = {"ORIGINAL", "GUARD", "FAT-GUARD",
                             "VIZIER", "SKELETON", "SHADOW", NULL};

  char *display_flip_mode_enum[] = {"NONE", "VERTICAL", "HORIZONTAL",
                                    "VERTICAL-HORIZONTAL", NULL};

  char *keyboard_flip_mode_enum[] = {"NONE", "VERTICAL", "HORIZONTAL",
                                     "VERTICAL-HORIZONTAL", NULL};

  switch (key) {
  case LEVEL_MODULE_OPTION:
    e = optval_to_enum (&i, key, arg, state, level_module_enum);
    if (e) return e;
    switch (i) {
    case 0: level_module = LEGACY_LEVEL_MODULE; break;
    case 1: level_module = CONSISTENCY_LEVEL_MODULE; break;
    }
    break;
  case VIDEO_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, video_mode_enum);
    if (e) return e;
    switch (i) {
    case 0: vm = VGA; break;
    case 1: vm = EGA; break;
    case 2: vm = CGA; break;
    case 3: vm = CGA, hgc = true; break;
    }
    break;
  case ENVIRONMENT_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, environment_mode_enum);
    if (e) return e;
    switch (i) {
    case 0: force_em = false; break;
    case 1: force_em = true, em = DUNGEON; break;
    case 2: force_em = true, em = PALACE; break;
    }
    break;
  case GUARD_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, guard_mode_enum);
    if (e) return e;
    switch (i) {
    case 0: gm = ORIGINAL_GM; break;
    case 1: gm = GUARD_GM; break;
    case 2: gm = FAT_GUARD_GM; break;
    case 3: gm = VIZIER_GM; break;
    case 4: gm = SKELETON_GM; break;
    case 5: gm = SHADOW_GM; break;
    }
    break;
  case SOUND_OPTION:
    sound_disabled_cmd = ! optval_to_bool (arg);
    break;
  case DISPLAY_FLIP_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, display_flip_mode_enum);
    if (e) return e;
    switch (i) {
    case 0: screen_flags = 0; break;
    case 1: screen_flags = ALLEGRO_FLIP_VERTICAL; break;
    case 2: screen_flags = ALLEGRO_FLIP_HORIZONTAL; break;
    case 3: screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL; break;
    }
    break;
  case KEYBOARD_FLIP_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, keyboard_flip_mode_enum);
    if (e) return e;
    switch (i) {
    case 0:
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = false;
      break;
    case 1:
      flip_keyboard_vertical = true;
      flip_keyboard_horizontal = false;
      break;
    case 2:
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = true;
      break;
    case 3:
      flip_keyboard_vertical = true;
      flip_keyboard_horizontal = true;
      break;
    }
    break;
  case MIRROR_MODE_OPTION:
    if (optval_to_bool (arg)) {
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = true;
      screen_flags = ALLEGRO_FLIP_HORIZONTAL;
    } else {
      flip_keyboard_vertical = false;
      flip_keyboard_horizontal = false;
      screen_flags = 0;
    }
    break;
  case BLIND_MODE_OPTION:
    no_room_drawing = optval_to_bool (arg);
    break;
  case IMMORTAL_MODE_OPTION:
    immortal_mode = optval_to_bool (arg);
    break;
  case TOTAL_LIVES_OPTION:
    e = optval_to_int (&i, key, arg, state, 1, 10);
    if (e) return e;
    initial_total_lives = i;
    break;
  case START_LEVEL_OPTION:
    e = optval_to_int (&i, key, arg, state, 1, INT_MAX);
    if (e) return e;
    start_level = i;
    break;
  case TIME_LIMIT_OPTION:
    e = optval_to_int (&i, key, arg, state, 1, INT_MAX);
    if (e) return e;
    time_limit = i;
    break;
  case START_TIME_OPTION:
    e = optval_to_int (&i, key, arg, state, 0, INT_MAX);
    if (e) return e;
    start_time = i;
    break;
  case KCA_OPTION:
    e = optval_to_int (&i, key, arg, state, 0, 100);
    if (e) return e;
    skill.counter_attack_prob = i - 1;
    break;
  case KCD_OPTION:
    e = optval_to_int (&i, key, arg, state, 0, 100);
    if (e) return e;
    skill.counter_defense_prob = i - 1;
    break;
  case DATA_PATH_OPTION:
    xasprintf (&data_dir, "%s", arg);
    break;
  case FULLSCREEN_OPTION:
    if (optval_to_bool (arg))
      al_set_new_display_flags (al_get_new_display_flags ()
                                | ALLEGRO_FULLSCREEN_WINDOW);
    else al_set_new_display_flags (al_get_new_display_flags ()
                                   & ~ALLEGRO_FULLSCREEN_WINDOW);
    break;
  case WINDOW_POSITION_OPTION:
    e = optval_to_int_pair (&x, &y, key, arg, state, INT_MIN, INT_MAX, ',',
                            'X', 'Y');
    if (e) return e;
    al_set_new_window_position (x, y);
    break;
  case WINDOW_DIMENSIONS_OPTION:
    e = optval_to_int_pair (&display_width, &display_height, key, arg, state, 1,
                            INT_MAX, 'x', 'W', 'H');
    if (e) return e;
    break;
  case INHIBIT_SCREENSAVER_OPTION:
    al_inhibit_screensaver (optval_to_bool (arg));
    break;
  case PRINT_PATHS_OPTION:
    print_paths ();
    exit (0);
  case SKIP_TITLE_OPTION:
    skip_title = optval_to_bool (arg);
    break;
  default:
    return ARGP_ERR_UNKNOWN;
  }
  return 0;
}

static void
version (FILE *stream, struct argp_state *state)
{
  uint32_t allegro_version = al_get_allegro_version ();
  int allegro_major = allegro_version >> 24;
  int allegro_minor = (allegro_version >> 16) & 255;
  int allegro_revision = (allegro_version >> 8) & 255;
  int allegro_release = allegro_version & 255;

  fprintf (stream,
           "%s (%s) %s\n\n"	/* mininim (MININIM) a.b */

           /* TRANSLATORS: Use "Félix" in place of "Fe'lix" */
           "Copyright (C) %s " PACKAGE_COPYRIGHT_HOLDER " <%s>\n\n"

           "%s\n\n" /* License GPLv3+... */
           "%s\n\n" /* Written by... */
           "Using Allegro %i.%i.%i[%i].\n", /* Using Allegro... */
           PACKAGE, PACKAGE_NAME, VERSION,
           "2015, 2016", "oitofelix@gnu.org",
           "\
License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>\n\
This is free software: you are free to change and redistribute it.\n\
There is NO WARRANTY, to the extent permitted by law.",

           /* TRANSLATORS: Use "Félix" in place of "F'elix" */
           "Written by Bruno Fe'lix Rezende Ribeiro.",
           allegro_major, allegro_minor, allegro_revision, allegro_release);
}

void
tolower_str (char *str)
{
  size_t i;
  for (i = 0; str[i] != 0; i++) str[i] = tolower (str[i]);
}

void
toupper_str (char *str)
{
  size_t i;
  for (i = 0; str[i] != 0; i++) str[i] = toupper (str[i]);
}

void
repl_str_char (char *str, char a, char b)
{
  size_t i;
  for (i = 0; str[i] != 0; i++) if (str[i] == a) str[i] = b;
}

static char *
env_option_name (const char *option_name)
{
  char *env_opt_name;
  xasprintf (&env_opt_name, "MININIM_%s", option_name);
  toupper_str (env_opt_name);
  repl_str_char (env_opt_name, '-', '_');
  return env_opt_name;
}

void
get_env_args (size_t *eargc, char ***eargv, struct argp_option *options)
{
  size_t i;

  *eargv = add_to_array (&argv[0], 1, *eargv, eargc, *eargc, sizeof (argv[0]));

  for (i = 0; options[i].name != NULL
         || options[i].key != 0
         || options[i].arg != NULL
         || options[i].flags != 0
         || options[i].doc != NULL
         || options[i].group != 0; i++) {
    if (! options[i].name) continue;
    char *env_opt_name = env_option_name (options[i].name);
    char *env_opt_value = getenv (env_opt_name);
    if (env_opt_value) {
      char *option;
      xasprintf (&option, "--%s=%s", options[i].name, env_opt_value);
      *eargv = add_to_array (&option, 1, *eargv, eargc, *eargc, sizeof (option));
    }

    al_free (env_opt_name);
  }
}

static char *
config_option_name (const char *option_name)
{
  char *config_opt_name;
  xasprintf (&config_opt_name, "%s", option_name);
  tolower_str (config_opt_name);
  repl_str_char (config_opt_name, '-', ' ');
  return config_opt_name;
}

void
get_config_args (size_t *cargc, char ***cargv, struct argp_option *options,
                 char *filename)
{
  size_t i;

  *cargv = add_to_array (&argv[0], 1, *cargv, cargc, *cargc, sizeof (argv[0]));

  ALLEGRO_CONFIG *config = al_load_config_file (filename);
  if (! config) return;

  for (i = 0; options[i].name != NULL
         || options[i].key != 0
         || options[i].arg != NULL
         || options[i].flags != 0
         || options[i].doc != NULL
         || options[i].group != 0; i++) {
    if (! options[i].name) continue;
    char *config_opt_name = config_option_name (options[i].name);
    const char *config_opt_value = al_get_config_value (config, NULL, config_opt_name);
    if (config_opt_value) {
      char *option;
      xasprintf (&option, "--%s=%s", options[i].name, config_opt_value);
      *cargv = add_to_array (&option, 1, *cargv, cargc, *cargc, sizeof (option));
    }

    al_free (config_opt_name);
  }
}

int
main (int _argc, char **_argv)
{
  /* make command-line arguments available globally */
  argc = _argc;
  argv = _argv;

  /* initialize Allegro */
  al_init ();

  /* get global paths */
  get_paths ();

  /* get environment variable arguments */
  get_env_args (&eargc, &eargv, options);

  /* get configuration file arguments */
  get_config_args (&cargc, &cargv, options, config_filename);

  /* size_t i; */
  /* for (i = 0; i < cargc; i++) printf ("%s\n", cargv[i]); */
  /* exit (0); */

  argp_program_version_hook = version;
  argp.doc = doc;

  option_phase = CONFIGURATION_FILE_OPTION_PHASE;
  argp_parse (&argp, cargc, cargv, 0, NULL, NULL);
  option_phase = ENVIRONMENT_VARIABLES_OPTION_PHASE;
  argp_parse (&argp, eargc, eargv, 0, NULL, NULL);
  option_phase = COMMAND_LINE_OPTION_PHASE;
  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  init_video ();
  init_audio ();
  if (sound_disabled_cmd) enable_audio (false);
  init_keyboard ();
  init_dialog ();

  draw_loading_screen ();

  load_samples ();
  load_level ();
  load_cutscenes ();

  if (skip_title) goto play_game;

 restart_game:
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

 play_game:
  total_lives = initial_total_lives;
  current_lives = initial_current_lives;

  if (! play_time) play_time = create_timer (1.0);
  al_set_timer_count (play_time, start_time);
  al_start_timer (play_time);

  /* play_level_1 (); */
  switch (level_module) {
  case LEGACY_LEVEL_MODULE: default:
    play_legacy_level (start_level);
    break;
  case CONSISTENCY_LEVEL_MODULE:
    play_consistency_level (start_level);
    break;
  }

  if (quit_anim == RESTART_GAME) goto restart_game;

 quit_game:
  unload_level ();
  unload_cutscenes ();
  unload_samples ();

  finalize_video ();
  finalize_audio ();
  finalize_keyboard ();
  finalize_dialog ();

  fprintf (stderr, "MININIM: Hope you enjoyed it!\n");

  return 0;
}

static void
draw_loading_screen (void)
{
  int x = 138;
  int y = 40;
  int w = al_get_bitmap_width (icon);
  int h = al_get_bitmap_height (icon);
  draw_filled_rectangle (screen, x - 1, y - 1, x + w, y + h, WHITE);
  draw_bitmap (icon, screen, x, y, 0);
  draw_text (screen, "Loading....", ORIGINAL_WIDTH / 2.0, ORIGINAL_HEIGHT / 2.0,
             ALLEGRO_ALIGN_CENTRE);
  show ();
}

static void
get_paths (void)
{
  /* get resources path string */
  ALLEGRO_PATH *resources_path = al_get_standard_path (ALLEGRO_RESOURCES_PATH);
  resources_dir = (char *) al_path_cstr (resources_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&resources_dir, "%s", resources_dir);
  al_destroy_path (resources_path);

  /* get temp path string */
  ALLEGRO_PATH *temp_path = al_get_standard_path (ALLEGRO_TEMP_PATH);
  temp_dir = (char *) al_path_cstr (temp_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&temp_dir, "%s", temp_dir);
  al_destroy_path (temp_path);

  /* get user home path string */
  ALLEGRO_PATH *user_home_path = al_get_standard_path (ALLEGRO_USER_HOME_PATH);
  user_home_dir = (char *) al_path_cstr (user_home_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&user_home_dir, "%s", user_home_dir);
  al_destroy_path (user_home_path);

  /* get user documents path string */
  ALLEGRO_PATH *user_documents_path = al_get_standard_path (ALLEGRO_USER_DOCUMENTS_PATH);
  user_documents_dir = (char *) al_path_cstr (user_documents_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&user_documents_dir, "%s", user_documents_dir);
  al_destroy_path (user_documents_path);

  /* get user data path string */
  ALLEGRO_PATH *user_data_path = al_get_standard_path (ALLEGRO_USER_DATA_PATH);
  user_data_dir = (char *) al_path_cstr (user_data_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&user_data_dir, "%s", user_data_dir);
  al_destroy_path (user_data_path);

  /* get user settings path string */
  ALLEGRO_PATH *user_settings_path = al_get_standard_path (ALLEGRO_USER_SETTINGS_PATH);
  user_settings_dir = (char *) al_path_cstr (user_settings_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&user_settings_dir, "%s", user_settings_dir);
  al_destroy_path (user_settings_path);

  /* get executable file name */
  ALLEGRO_PATH *exename_path = al_get_standard_path (ALLEGRO_EXENAME_PATH);
  exe_filename = (char *) al_path_cstr (exename_path, ALLEGRO_NATIVE_PATH_SEP);
  xasprintf (&exe_filename, "%s", exe_filename);
  al_destroy_path (exename_path);

  /* get config file name */
  xasprintf (&config_filename, "%smininim.ini", user_settings_dir);
}

static void
print_paths (void)
{
  printf ("Resources: %s\n", resources_dir);
  printf ("Temporary: %s\n", temp_dir);
  printf ("User home: %s\n", user_home_dir);
  printf ("User documents: %s\n", user_documents_dir);
  printf ("User data: %s\n", user_data_dir);
  printf ("User settings: %s\n", user_settings_dir);
  printf ("Executable file: %s\n", exe_filename);
  printf ("Configuration file: %s\n", config_filename);
}

void *
load_config_dialog (ALLEGRO_THREAD *thread, void *arg)
{
  ALLEGRO_FILECHOOSER *dialog =
    create_native_file_dialog (user_home_dir, "Load configuration file",
                               "*.*", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
  show_native_file_dialog (display, dialog);

  char *filename = NULL;
  if (al_get_native_file_dialog_count (dialog)) {
    filename = (char *) al_get_native_file_dialog_path (dialog, 0);
    xasprintf (&filename, "%s", filename);
  }

  al_destroy_native_file_dialog (dialog);
  al_set_thread_should_stop (thread);
  return filename;
}

void *
save_game_dialog (ALLEGRO_THREAD *thread, void *arg)
{
  ALLEGRO_FILECHOOSER *dialog =
    create_native_file_dialog (user_home_dir, "Save game",
                               "*.*", ALLEGRO_FILECHOOSER_SAVE);
  show_native_file_dialog (display, dialog);

  char *filename = NULL;
  if (al_get_native_file_dialog_count (dialog)) {
    filename = (char *) al_get_native_file_dialog_path (dialog, 0);

    int r = 1;
    if (al_filename_exists (filename))
      r = al_show_native_message_box
        (display, "Warning",
         "File already exists!",
         "Do you want to overwrite it?",
         NULL,
         ALLEGRO_MESSAGEBOX_WARN | ALLEGRO_MESSAGEBOX_YES_NO);

    if (r == 1) xasprintf (&filename, "%s", filename);
    else filename = NULL;
  }

  al_destroy_native_file_dialog (dialog);
  al_set_thread_should_stop (thread);
  return filename;
}

ALLEGRO_TEXTLOG *
load_config (char *filename)
{
  char **cargv = NULL;
  size_t cargc = 0;

  ALLEGRO_TEXTLOG *textlog =
    open_native_text_log ("Configuration loading results",
                          ALLEGRO_TEXTLOG_MONOSPACE);

  al_append_native_text_log (textlog, "Loading %s...\n", filename);

  get_config_args (&cargc, &cargv, options, filename);
  option_phase = CONFIGURATION_FILE_OPTION_PHASE;

  bool parse_error = false;
  int arg_index = 0;
  error_t e;

  do {
    e = argp_parse (&argp, cargc - arg_index, &cargv[arg_index],
                    ARGP_SILENT, &arg_index, textlog);
    arg_index++;
    if (! parse_error) parse_error = e;
  } while (e);

  destroy_array ((void **) &cargv, &cargc);

  char *error_str;
  if (! parse_error) {
    al_close_native_text_log (textlog);
    textlog = NULL;
    error_str = "CONFIGURATION LOADED SUCCESSFULLY";
  } else error_str = "CONFIGURATION LOADED WITH ERRORS";

  draw_bottom_text (NULL, error_str);

  return textlog;
}

void
save_game (char *filename)
{
  ALLEGRO_CONFIG *config = create_config ();
  char *start_level_str, *start_time_str,
    *total_lives_str, *kca_str, *kcd_str;

  xasprintf (&start_level_str, "%i", level.number);
  xasprintf (&start_time_str, "%i", al_get_timer_count (play_time));
  xasprintf (&total_lives_str, "%i", total_lives);
  xasprintf (&kca_str, "%i", skill.counter_attack_prob + 1);
  xasprintf (&kcd_str, "%i", skill.counter_defense_prob + 1);

  al_add_config_comment (config, NULL, "MININIM save file");
  al_set_config_value (config, NULL, "start level", start_level_str);
  al_set_config_value (config, NULL, "start time", start_time_str);
  al_set_config_value (config, NULL, "total lives", total_lives_str);
  al_set_config_value (config, NULL, "kca", kca_str);
  al_set_config_value (config, NULL, "kcd", kcd_str);

  char *error_str = al_save_config_file (filename, config)
    ? "GAME HAS BEEN SAVED"
    : "GAME SAVE FAILED";

  draw_bottom_text (NULL, error_str);

  al_destroy_config (config);
  al_free (start_level_str);
  al_free (start_time_str);
  al_free (total_lives_str);
  al_free (kca_str);
  al_free (kcd_str);
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
