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

int mr_w = 2, mr_h = 2;

enum level_module level_module;

struct config_info {
  enum {
    CI_CONFIGURATION_FILE, CI_ENVIRONMENT_VARIABLES,
    CI_COMMAND_LINE,
  } type;
  char *filename;
  ALLEGRO_TEXTLOG *textlog;
};

bool ignore_main_config, ignore_environment;

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
  *config_filename,
  *levels_dat_filename = "data/dat-levels/LEVELS.DAT";

char *levels_dat_compat_filename;

ALLEGRO_THREAD *load_config_dialog_thread, *save_game_dialog_thread;

ALLEGRO_TIMER *play_time;
enum vm vm = VGA;
enum em em = DUNGEON;
bool force_em = false;
enum hue hue = HUE_NONE;
bool force_hue = false;
enum gm gm = ORIGINAL_GM;
bool immortal_mode;
int initial_total_lives = KID_INITIAL_TOTAL_LIVES, total_lives;
int initial_current_lives = KID_INITIAL_CURRENT_LIVES, current_lives;
int start_level = 1;
struct pos start_pos = {-1,-1,-1};
int time_limit = TIME_LIMIT;
int start_time = START_TIME;
int start_level_time;
struct skill skill = {.counter_attack_prob = INITIAL_KCA,
                      .counter_defense_prob = INITIAL_KCD};
static bool sound_disabled_cmd;
static bool skip_title;
static bool level_module_given;
static bool inhibit_screensaver = true;

static error_t parser (int key, char *arg, struct argp_state *state);
static void draw_loading_screen (void);
static void print_paths (void);
static char *env_option_name (const char *option_name);
static char *config_option_name (const char *option_name);
static void get_paths (void);
static error_t get_config_args (size_t *cargc, char ***cargv,
                                struct argp_option *options,
                                char *filename);
static error_t
pre_parser (int key, char *arg, struct argp_state *state);
static void give_dat_compat_preference (void);

static struct argp_option options[] = {
  /* Configuration */
  {NULL, 0, NULL, 0, "Configuration:", 0},

  {"load-config", LOAD_CONFIG_OPTION, "FILE", 0, "Load configuration file FILE.  The options set in FILE have the same precedence as the equivalent command line options given at its place of occurrence.  This can be done in-game by the CTRL+L key binding.", 0},

  {"ignore-main-config", IGNORE_MAIN_CONFIG_OPTION, NULL, 0, "Ignore main configuration file.  The default is to parse it at the very beginning of each run.", 0},

  {"ignore-environment", IGNORE_ENVIRONMENT_OPTION, NULL, 0, "Ignore environment variables.  The default is to parse them after the main configuration file.", 0},

  {NULL, 0, NULL, OPTION_DOC, "There are three methods of configuration: command line options, environment variables and configuration files.  For every command line option of the form 'x-y' there is an equivalent environment variable option 'MININIM_X_Y' and an equivalent configuration file option 'x y'.  The multiple methods of configuration are cumulative but command line options override any other, while environment variables override the main configuration file.  In any method applicable later options override earlier ones.  The option '--print-paths' shows, among other things, the expected file name of the main configuration file.", 0},

  {NULL, 0, NULL, OPTION_DOC, "Notice that save files (CTRL+G) are a particular case of configuration files and should be loaded the same way.  When loaded in-game, configuration files take effect immediately, however some effects only become visible under certain conditions.  For instance save files show their effect only on game (re)start.", 0},

  /* Level */
  {NULL, 0, NULL, 0, "Level:", 0},
  {"level-module", LEVEL_MODULE_OPTION, "LEVEL-MODULE", 0, "Select level module.  A level module determines a way to generate consecutive levels for use by the engine.  Valid values for LEVEL-MODULE are: NATIVE, LEGACY, PLV, DAT and CONSISTENCY.  NATIVE is the module designed to read the native format that supports all features.  LEGACY is the module designed to read the original PoP 1 raw level files.  PLV is the module designed to read the original PoP 1 PLV extended level files.  DAT is the module designed to read the original PoP 1 LEVELS.DAT file.  CONSISTENCY is the module designed to generate random-corrected levels for accessing the engine robustness.  The default is NATIVE.", 0},
  {"convert-levels", CONVERT_LEVELS_OPTION, NULL, 0, "Batch convert levels 1 to 14 accessible by the current level module to the native format and exit.  The levels are saved in the user data directory, where they take precedence over levels in every other location.  When using this option there is no point in using any other options besides '--level-module' that must occur before this to take effect.  You can accomplish a similar result in-game on a per level basis by using the 'E>LS' command.  Notice that in that case any changes made to the level by special events (or otherwise) before you trigger the save command will be retained.", 0},
  {"start-level", START_LEVEL_OPTION, "N", 0, "Make the kid start at level N.  The default is 1.  Valid integers range from 1 to INT_MAX.  This can be changed in-game by the SHIFT+L and SHIFT+M key binding.", 0},
  {"start-pos", START_POS_OPTION, "R,F,P", 0, "Make the kid start at room R, floor F and place P.  The default is to let this decision to the level module.  R is an integer ranging from 1 to INT_MAX, F is an integer ranging from 0 to 2 and P is an integer ranging from 0 to 9.", 0},
  {NULL, 0, NULL, OPTION_DOC, "If the option '--level-module' is not given and there is a LEVELS.DAT file in the working directory, the DAT level module is automatically used to load that file.  This is a compatibility measure for applications which depend upon this legacy behavior.", 0},
  {NULL, 0, NULL, OPTION_DOC, "", 0},

  /* Time */
  {NULL, 0, NULL, 0, "Time:", 0},
  {"time-limit", TIME_LIMIT_OPTION, "N", 0, "Set the time limit to complete the game to N seconds.  The default is 3600 (1 hour).  Valid integers range from 1 to INT_MAX.  This can be changed in-game by the + and - key bindings.", 0},
  {"start-time", START_TIME_OPTION, "N", 0, "Set the play time counter to N seconds.  The default is 0.  Valid integers range from 0 to INT_MAX.", 0},

  /* Skills */
  {NULL, 0, NULL, 0, "Skills:", 0},
  {"total-lives", TOTAL_LIVES_OPTION, "N", 0, "Make the kid start with N total lives.  The default is 3.  Valid integers range from 1 to 10.  This can be changed in-game by the SHIFT+T key binding.", 0},
  {"kca", KCA_OPTION, "N", 0, "Set kid's counter attack skill to N.  The default is 0.  Valid integers range from 0 to 100.  This can be changed in-game by the CTRL+= and CTRL+- key bindings.", 0},
  {"kcd", KCD_OPTION, "N", 0, "Set kid's counter defense skill to N.  The default is 0.  Valid integers range from 0 to 100.  This can be changed in-game by the ALT+= and ALT+- key bindings.", 0},
  {"immortal-mode", IMMORTAL_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable immortal mode.  In immortal mode the kid can't be harmed.  The default is FALSE.  This can be changed in-game by the I key binding.", 0},

  /* Rendering */
  {NULL, 0, NULL, 0, "Rendering:", 0},
  {"video-mode", VIDEO_MODE_OPTION, "VIDEO-MODE", 0, "Select video mode.  Valid values for VIDEO-MODE are: VGA, EGA, CGA and HGC.  The default is VGA.  This can be changed in-game by the F12 key binding.", 0},
  {"environment-mode", ENVIRONMENT_MODE_OPTION, "ENVIRONMENT-MODE", 0, "Select environment mode.  Valid values for ENVIRONMENT-MODE are: ORIGINAL, DUNGEON and PALACE.  The ORIGINAL value gives level modules autonomy in this choice for each particular level.  This is the default.  This can be changed in-game by the F11 key binding.", 0},
  {"guard-mode", GUARD_MODE_OPTION, "GUARD-MODE", 0, "Select guard mode.  Valid values for GUARD-MODE are: ORIGINAL, GUARD, FAT-GUARD, VIZIER, SKELETON and SHADOW.  The ORIGINAL value gives level modules autonomy in this choice for each particular guard.  This is the default.  This can be changed in-game by the F10 key binding.", 0},
{"hue-mode", HUE_MODE_OPTION, "HUE-MODE", 0, "Select hue mode.  Valid values for HUE-MODE are: ORIGINAL, NONE, GREEN, GRAY, YELLOW and BLUE.  The ORIGINAL value gives level modules autonomy in this choice for each particular level.  This is the default.  For the classic behavior of the first version of the original game use NONE.  This can be changed in-game by the F9 key binding.", 0},
  {"display-flip-mode", DISPLAY_FLIP_MODE_OPTION, "DISPLAY-FLIP-MODE", 0, "Select display flip mode.  Valid values for DISPLAY-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+I key binding.", 0},
  {"mirror-mode", MIRROR_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable mirror mode.  In mirror mode the screen and the keyboard are flipped horizontally.  This is equivalent of specifying both the options --display-flip-mode=HORIZONTAL and --gamepad-flip-mode=HORIZONTAL.  The default is FALSE.  This can be changed in-game by the SHIFT+I and SHIFT+K key bindings for the display and keyboard, respectively.", 0},
  {"blind-mode", BLIND_MODE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable blind mode.  In blind mode background and non-animated sprites are not drawn.  The default is FALSE.  This can be changed in-game by the SHIFT+B key binding.", 0},

  /* Gamepad */
  {NULL, 0, NULL, 0, "Gamepad:", 0},
  {"gamepad-flip-mode", GAMEPAD_FLIP_MODE_OPTION, "GAMEPAD-FLIP-MODE", 0, "Select gamepad flip mode.  Valid values for GAMEPAD-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+K key binding.", 0},
  {"joystick-axis-threshold", JOYSTICK_AXIS_THRESHOLD_OPTION, "FUNC,VALUE", 0, "Set joystick threshold to VALUE for the axis mapped to FUNC.  Valid values for FUNC are H and V.   VALUE is a floating point ranging from 0.0 to 1.0.  The default VALUE for H is 0.1 and for Y is 0.8.", 0},
  {"joystick-button-threshold", JOYSTICK_BUTTON_THRESHOLD_OPTION, "FUNC,VALUE", 0, "Set joystick threshold to VALUE for the button mapped to FUNC.  Valid values for FUNC are: UP, RIGHT, DOWN, LEFT, ENTER, SHIFT.  VALUE is an integer ranging from 0 to 32767.  The default VALUE for any function is 100.", 0},
  {"joystick-axis", JOYSTICK_AXIS_OPTION, "FUNC,STICK,AXIS", 0, "Map function FUNC to joystick axis STICK,AXIS.  Valid values for FUNC are: H and V.  STICK,AXIS is a valid stick and axis pair.  The default STICK,AXIS for H is 0,0 and for V is 0,1.", 0},
  {"joystick-button", JOYSTICK_BUTTON_OPTION, "FUNC,BUTTON", 0, "Map function FUNC to joystick button BUTTON.  Valid values for FUNC are: UP, RIGHT, DOWN, LEFT, ENTER, SHIFT, TIME, PAUSE.  BUTTON is a valid joystick button number.  The default BUTTON values are 0, 1, 2, 3, 4, 5, 8 and 9, respectively.", 0},
  {"joystick-info", JOYSTICK_INFO_OPTION, NULL, 0, "Print information about the primary joystick and exit.", 0},

  {NULL, 0, NULL, OPTION_DOC, "The primary joystick's axis and button numbers are listed by the option '--joystick-info'.  You can find out the number of a particular axis or button by pressing it before invoking MININIM with that option.  If a stick, axis or button given to an option doesn't exist in the primary joystick, it's silently ignored.  The joystick can be activated and auto-calibrated in-game by the CTRL+J key binding.  Use this when hot-plugging a joystick or in case the joystick starts to behave oddly.  If your joystick is peculiar enough, proving the auto-calibration mechanism insufficient, the '--joystick-axis-threshold' and '--joystick-button-threshold' options may help.", 0},

  /* Window */
  {NULL, 0, NULL, 0, "Window:", 0},
  {"fullscreen", FULLSCREEN_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable fullscreen mode.  In fullscreen mode the window spans the entire screen.  The default is FALSE.  This can be changed in-game by the F key binding.", 0},
  {"window-position", WINDOW_POSITION_OPTION, "X,Y", 0, "Place the window at screen coordinates X,Y.  The default is to let this choice to the window manager.  The values X and Y are integers and must be separated by a comma.", 0},
  {"window-dimensions", WINDOW_DIMENSIONS_OPTION, "WxH", 0, "Set window width and height to W and H, respectively.  The default is 640x400.  The values W and H are strictly positive integers and must be separated by an 'x'.", 0},

  /* Paths */
  {NULL, 0, NULL, 0, "Paths:", 0},
  {"data-path", DATA_PATH_OPTION, "PATH", 0, "Set data path to PATH.  Normally, the data files are looked for in the user data directory, then in the current working directory, then in the resources directory, and finally in the system data directory.  If this option is given, after looking in the user data directory the data files are looked for in PATH.", 0},
  {"print-paths", PRINT_PATHS_OPTION, NULL, 0, "Print paths and exit.", 0},

  /* Others */
  {NULL, 0, NULL, 0, "Others", 0},
  {"sound", SOUND_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Enable/disable sound.  The default is TRUE.  This can be changed in-game by the CTRL+S key binding.", 0},
  {"skip-title", SKIP_TITLE_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Skip title screen.  The default is FALSE.", 0},
  {"inhibit-screensaver", INHIBIT_SCREENSAVER_OPTION, "BOOLEAN", OPTION_ARG_OPTIONAL, "Prevent the system screensaver from starting up.  The default is TRUE.", 0},

  /* Help */
  {NULL, 0, NULL, 0, "Help:", -1},
  {0},
};

static const char *doc = "MININIM: The Advanced Prince of Persia Engine\n(a childhood dream)\v\
Long option names are case sensitive.  Option values are case insensitive.   Both can be partially specified as long as they are kept unambiguous.  BOOLEAN is an integer equating to 0, or any sub-string (including the null string) of 'FALSE', 'OFF' or 'NO' to disable the respective feature, and any other value (even no string at all) to enable it.  For any non-specified option the documented default applies.  Integers can be specified in any of the formats defined by the C language.  Key bindings references are based on the default mapping.\n\n\
The legacy command line interface present in versions 1.0, 1.3 and 1.4 of the original game is supported for the sake of compatibility with software that use it.  Legacy level and video non-option arguments are honored, while all others are currently ignored silently.  The legacy arguments can't be used by other configuration method besides the command line.";

struct argp_child argp_child = { NULL };

static struct argp pre_argp = {options, pre_parser, NULL, NULL, &argp_child, NULL, NULL};
static struct argp argp = {options, parser, NULL, NULL, &argp_child, NULL, NULL};

static char *
key_to_option_name (int key, struct argp_state *state)
{
  char *option_name;
  struct config_info *config_info = (struct config_info *) state->input;

  size_t i;
  for (i = 0; options[i].name != NULL
         || options[i].key != 0
         || options[i].arg != NULL
         || options[i].flags != 0
         || options[i].doc != NULL
         || options[i].group != 0; i++)
    if (options[i].key == key) {
      switch (config_info->type) {
      case CI_CONFIGURATION_FILE:
        return config_option_name (options[i].name);
      case CI_ENVIRONMENT_VARIABLES:
        return env_option_name (options[i].name);
      case CI_COMMAND_LINE:
        xasprintf (&option_name, "%s", options[i].name);
        return option_name;
      }
    }

  return NULL;
}

static void
option_enum_value_error (int key, char *arg, struct argp_state *state,
                         char **enum_vals, bool invalid, int number)
{
  char *msg = NULL;
  char *option_name = key_to_option_name (key, state);
  struct config_info *config_info = (struct config_info *) state->input;

  switch (config_info->type) {
  case CI_CONFIGURATION_FILE:
    xasprintf
      (&msg, "%s", invalid
       ? "is invalid for the configuration file option"
       : "is ambiguous for the configuration file option");
    break;
  case CI_ENVIRONMENT_VARIABLES:
    xasprintf
      (&msg, "%s", invalid
       ? "is invalid for the environment variable option"
       : "is ambiguous for the environment variable option");
    break;
  case CI_COMMAND_LINE:
    xasprintf
      (&msg, "%s", invalid
       ? "is invalid for the command line option"
       : "is ambiguous for the command line option");
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

  char *config_file_prefix;
  if (config_info->type == CI_CONFIGURATION_FILE)
    xasprintf (&config_file_prefix, "%s: ", config_info->filename);
  else xasprintf (&config_file_prefix, "");

  char *error_template = "%s'%s' %s '%s' argument %i.\n%s %s.";
  if (config_info->type == CI_CONFIGURATION_FILE
      && state->flags & ARGP_SILENT)
    al_append_native_text_log (config_info->textlog, error_template,
                               config_file_prefix, arg, msg, option_name,
                               number, msg2, valid_values);
  else argp_error (state, error_template,
                   config_file_prefix, arg, msg, option_name,
                   number, msg2, valid_values);

  al_free (option_name);
  al_free (msg);
  al_free (msg2);
  al_free (valid_values);
  al_free (config_file_prefix);
}

static void
option_arg_error (int key, char *arg, struct argp_state *state, int number, char *estr)
{
  struct config_info *config_info = (struct config_info *) state->input;
  char *option_name = key_to_option_name (key, state);
  char *config_file_prefix;
  char *msg;
  char *argument_msg;

  if (config_info->type == CI_CONFIGURATION_FILE)
    xasprintf (&config_file_prefix, "%s: ", config_info->filename);
  else xasprintf (&config_file_prefix, "");

  switch (config_info->type) {
  case CI_CONFIGURATION_FILE:
    msg = "is invalid for the configuration file option";
    break;
  case CI_ENVIRONMENT_VARIABLES:
    msg = "is invalid for the environment variable option";
    break;
  case CI_COMMAND_LINE:
    msg = "is invalid for the command line option";
    break;
  }

  if (number != -1) xasprintf (&argument_msg, " argument %i", number);
  else xasprintf (&argument_msg, "");

  char *error_template = "%s'%s' %s '%s'%s.\n%s";

  if (config_info->type == CI_CONFIGURATION_FILE
      && state->flags & ARGP_SILENT)
    al_append_native_text_log (config_info->textlog, error_template,
                               config_file_prefix, arg, msg, option_name,
                               argument_msg, estr);
  else argp_error (state, error_template,
                   config_file_prefix, arg, msg, option_name,
                   argument_msg, estr);

  al_free (option_name);
  al_free (config_file_prefix);
  al_free (argument_msg);
}

static bool
optval_to_bool (char *arg)
{
  int i;
  char *FALSE_VAL = "FALSE";
  char *OFF_VAL = "OFF";
  char *NO_VAL = "NO";

  if (! arg) return true;

  if (sscanf (arg, "%i", &i) == 1 && i == 0) return false;
  if (strcasestr (FALSE_VAL, arg) == FALSE_VAL) return false;
  if (strcasestr (OFF_VAL, arg) == OFF_VAL) return false;
  if (strcasestr (NO_VAL, arg) == NO_VAL) return false;

  return true;
}

static error_t
optval_to_int (int *retval, int key, char *arg, struct argp_state *state,
               struct int_range *r, int number)
{
  int i;
  char *estr;

  if (sscanf (arg, "%i", &i) != 1) {
    option_arg_error (key, arg, state, number, "Reason: argument is not an integer.");
    return EINVAL;
  }

  if (i < r->a || i > r->b) {
    xasprintf (&estr, "Reason: argument is not in the range [%i,%i].", r->a, r->b);
    option_arg_error (key, arg, state, number, estr);
    al_free (estr);
    return EINVAL;
  }

  *retval = i;
  return 0;
}

static error_t
optval_to_float (float *retval, int key, char *arg, struct argp_state *state,
                 struct float_range *r, int number)
{
  float f;
  char *estr;

  if (sscanf (arg, "%f", &f) != 1) {
    option_arg_error (key, arg, state, number, "Reason: argument is not a float.");
    return EINVAL;
  }

  if (f < r->a || f > r->b) {
    xasprintf (&estr, "Reason: argument is not in the range [%f,%f].", r->a, r->b);
    option_arg_error (key, arg, state, number, estr);
    al_free (estr);
    return EINVAL;
  }

  *retval = f;
  return 0;
}

static error_t
optval_to_enum (int *retval, int key, char *arg, struct argp_state *state,
                char **enum_vals, int number)
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
    option_enum_value_error (key, arg, state, enum_vals, true, number);
    return EINVAL;
  }
  else if (ambiguous) {
    option_enum_value_error (key, arg, state, enum_vals, false, number);
    return EINVAL;
  }

  *retval = optval;

  return 0;
}

static error_t
option_get_args (int key, char *arg, struct argp_state *state, char s, ...)
{
  va_list ap, at, av, as, ar, atype, aval, arange;
  int i;
  error_t retval;
  char *arg2;
  xasprintf (&arg2, "%s", arg);
  char *estr;

  /* initialize argument lists */
  va_start (ap, s);
  va_start (at, s);
  va_start (av, s);
  va_start (as, s);
  va_start (ar, s);
  va_start (atype, s);
  va_start (aval, s);
  va_start (arange, s);

  /* count number of arguments */
  int num_args = 0;
  va_start (ap, s);
  while (va_arg (ap, enum opt_arg_type) != ARG_TYPE_NULL) num_args++;

  /* Nothing to do, if it expects 0 arguments */
  if (num_args == 0) {
    retval = 0;
    goto end;
  }

  /* set argument lists */
  va_start (atype, s);
  va_copy (ap, atype);
  for (i = 0; i <= num_args; i++) va_arg (ap, enum opt_arg_type);
  va_copy (aval, ap);
  for (i = 0; i < num_args; i++) va_arg (ap, void *);
  va_copy (arange, ap);

  /* get values and check ranges */
  va_copy (at, atype);
  va_copy (av, aval);
  va_copy (ar, arange);
  for (i = 0; i < num_args; i++) {
    enum opt_arg_type type = va_arg (at, enum opt_arg_type);

    char *str = strtok (i ? NULL : arg2, ",");
    if (! str) {
      xasprintf (&estr, "Reason: less than %i arguments provided.", num_args);
      option_arg_error (key, arg, state, -1, estr);
      al_free (estr);
      retval = EINVAL;
      goto end;
    }

    void *val = va_arg (av, void *);
    void *range = va_arg (ar, void *);

    assert (type == ARG_TYPE_INT || type == ARG_TYPE_FLOAT
            || type == ARG_TYPE_ENUM);
    switch (type) {
    case ARG_TYPE_BOOL:
      retval = 0;
      *(bool *) val = optval_to_bool (str);
      break;
    case ARG_TYPE_INT:
      retval = optval_to_int ((int *) val, key, str, state, range, i);
      break;
    case ARG_TYPE_FLOAT:
      retval = optval_to_float ((float *) val, key, str, state, range, i);
      break;
    case ARG_TYPE_ENUM:
      retval = optval_to_enum ((int *) val, key, str, state, range, i);
    default: break;
    }

    if (retval) goto end;
  }

  retval = 0;

 end: /* cleanup */
  va_end (ap);
  va_end (at);
  va_end (av);
  va_end (as);
  va_end (ar);
  va_end (atype);
  va_end (aval);
  va_end (arange);
  al_free (arg2);
  return retval;
}

static error_t
pre_parser (int key, char *arg, struct argp_state *state)
{
  switch (key) {
  case IGNORE_MAIN_CONFIG_OPTION:
    ignore_main_config = true;
    break;
  case IGNORE_ENVIRONMENT_OPTION:
    ignore_environment = true;
    break;
  }
  return 0;
}

static error_t
parser (int key, char *arg, struct argp_state *state)
{
  char **cargv = NULL;
  size_t cargc = 0;
  int x, y, i, e;
  struct config_info config_info;
  float float_val;
  int int_val0, int_val1, int_val2;

  char *level_module_enum[] = {"NATIVE", "LEGACY", "PLV", "DAT", "CONSISTENCY", NULL};

  char *video_mode_enum[] = {"VGA", "EGA", "CGA", "HGC", NULL};

  char *environment_mode_enum[] = {"ORIGINAL", "DUNGEON", "PALACE", NULL};

  char *hue_mode_enum[] = {"ORIGINAL", "NONE", "GREEN",
                           "GRAY", "YELLOW", "BLUE", NULL};

  char *guard_mode_enum[] = {"ORIGINAL", "GUARD", "FAT-GUARD",
                             "VIZIER", "SKELETON", "SHADOW", NULL};

  char *display_flip_mode_enum[] = {"NONE", "VERTICAL", "HORIZONTAL",
                                    "VERTICAL-HORIZONTAL", NULL};

  char *gamepad_flip_mode_enum[] = {"NONE", "VERTICAL", "HORIZONTAL",
                                    "VERTICAL-HORIZONTAL", NULL};

  char *joystick_axis_threshold_enum[] = {"H", "V", NULL};

  char *joystick_button_threshold_enum[] = {"UP", "RIGHT", "DOWN", "LEFT",
                                            "ENTER", "SHIFT", NULL};

  char *joystick_axis_enum[] = {"H", "V", NULL};

  char *joystick_button_enum[] = {"UP", "RIGHT", "DOWN", "LEFT",
                                  "ENTER", "SHIFT", "TIME", "PAUSE", NULL};

  struct int_range total_lives_range = {1, 10};
  struct int_range start_level_range = {1, INT_MAX};
  struct int_range start_pos_room_range = {1, INT_MAX};
  struct int_range start_pos_floor_range = {0, 2};
  struct int_range start_pos_place_range = {0, 9};
  struct int_range time_limit_range = {1, INT_MAX};
  struct int_range start_time_range = {0, INT_MAX};
  struct int_range kca_range = {0, 100};
  struct int_range kcd_range = {0, 100};
  struct int_range window_position_range = {INT_MIN, INT_MAX};
  struct int_range window_dimensions_range = {1, INT_MAX};
  struct float_range joystick_axis_threshold_range = {0.0,1.0};
  struct int_range joystick_button_threshold_range = {0, 32767};
  struct int_range joystick_axis_range = {0, INT_MAX};
  struct int_range joystick_button_range = {0, INT_MAX};

  switch (key) {
  case IGNORE_MAIN_CONFIG_OPTION:
    break;
  case IGNORE_ENVIRONMENT_OPTION:
    break;
  case LOAD_CONFIG_OPTION:
    e = get_config_args (&cargc, &cargv, options, arg);
    if (e) {
      argp_failure (state, -1, e, "can't load configuration file '%s'", arg);
      return e;
    }
    config_info.type = CI_CONFIGURATION_FILE;
    config_info.filename = arg;
    config_info.textlog = NULL;
    argp_parse (&argp, cargc, cargv, 0, NULL, &config_info);
    destroy_array ((void **) &cargv, &cargc);
    break;
  case LEVEL_MODULE_OPTION:
    e = optval_to_enum (&i, key, arg, state, level_module_enum, 0);
    if (e) return e;
    level_module_given = true;
    switch (i) {
    case 0: level_module = NATIVE_LEVEL_MODULE; break;
    case 1: level_module = LEGACY_LEVEL_MODULE; break;
    case 2: level_module = PLV_LEVEL_MODULE; break;
    case 3: level_module = DAT_LEVEL_MODULE; break;
    case 4: level_module = CONSISTENCY_LEVEL_MODULE; break;
    }
    break;
  case CONVERT_LEVELS_OPTION:
    give_dat_compat_preference ();
    void (*next_level_f) (int);
    struct level *l;
    char *f, *d;
    switch (level_module) {
    case NATIVE_LEVEL_MODULE: default:
      next_level_f = next_native_level;
      l = &native_level;
      break;
    case LEGACY_LEVEL_MODULE:
      next_level_f = next_legacy_level;
      l = &legacy_level;
      break;
    case PLV_LEVEL_MODULE:
      next_level_f = next_plv_level;
      l = &legacy_level;
      break;
    case DAT_LEVEL_MODULE:
      next_level_f = next_dat_level;
      l = &legacy_level;
      break;
    case CONSISTENCY_LEVEL_MODULE:
      next_level_f = next_consistency_level;
      l = &consistency_level;
      break;
    }

    xasprintf (&d, "%sdata/levels/", user_data_dir);
    if (! al_make_directory (d))
        error (-1, al_get_errno (), "%s (%s): failed to create native level directory",
               __func__, d);

    for (i = 1; i <= 14; i++) {
      next_level_f (i);
      xasprintf (&f, "%s%02d.mim", d, i);
      if (! save_native_level (l, f))
        error (-1, al_get_errno (), "%s (%s): failed to save native level file",
               __func__, f);
      al_free (f);
    }
    al_free (d);

    exit (0);
    break;
  case VIDEO_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, video_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: vm = VGA; break;
    case 1: vm = EGA; break;
    case 2: vm = CGA; break;
    case 3: vm = CGA, hgc = true; break;
    }
    break;
  case ENVIRONMENT_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, environment_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: force_em = false; break;
    case 1: force_em = true, em = DUNGEON; break;
    case 2: force_em = true, em = PALACE; break;
    }
    break;
  case HUE_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, hue_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: force_hue = false; break;
    case 1: force_hue = true, hue = HUE_NONE; break;
    case 2: force_hue = true, hue = HUE_GREEN; break;
    case 3: force_hue = true, hue = HUE_GRAY; break;
    case 4: force_hue = true, hue = HUE_YELLOW; break;
    case 5: force_hue = true, hue = HUE_BLUE; break;
    }
    break;
  case GUARD_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, guard_mode_enum, 0);
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
    e = optval_to_enum (&i, key, arg, state, display_flip_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0: screen_flags = 0; break;
    case 1: screen_flags = ALLEGRO_FLIP_VERTICAL; break;
    case 2: screen_flags = ALLEGRO_FLIP_HORIZONTAL; break;
    case 3: screen_flags = ALLEGRO_FLIP_VERTICAL | ALLEGRO_FLIP_HORIZONTAL; break;
    }
    break;
  case GAMEPAD_FLIP_MODE_OPTION:
    e = optval_to_enum (&i, key, arg, state, gamepad_flip_mode_enum, 0);
    if (e) return e;
    switch (i) {
    case 0:
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = false;
      break;
    case 1:
      flip_gamepad_vertical = true;
      flip_gamepad_horizontal = false;
      break;
    case 2:
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = true;
      break;
    case 3:
      flip_gamepad_vertical = true;
      flip_gamepad_horizontal = true;
      break;
    }
    break;
  case MIRROR_MODE_OPTION:
    if (optval_to_bool (arg)) {
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = true;
      screen_flags = ALLEGRO_FLIP_HORIZONTAL;
    } else {
      flip_gamepad_vertical = false;
      flip_gamepad_horizontal = false;
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
    e = optval_to_int (&i, key, arg, state, &total_lives_range, 0);
    if (e) return e;
    initial_total_lives = i;
    break;
  case START_LEVEL_OPTION:
    e = optval_to_int (&i, key, arg, state, &start_level_range, 0);
    if (e) return e;
    start_level = i;
    break;
  case START_POS_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_INT, ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &int_val0, &int_val1, &int_val2,
                         &start_pos_room_range, &start_pos_floor_range, &start_pos_place_range);
    if (e) return e;
    start_pos.room = int_val0;
    start_pos.floor = int_val1;
    start_pos.place = int_val2;
    break;
  case TIME_LIMIT_OPTION:
    e = optval_to_int (&i, key, arg, state, &time_limit_range, 0);
    if (e) return e;
    time_limit = i;
    break;
  case START_TIME_OPTION:
    e = optval_to_int (&i, key, arg, state, &start_time_range, 0);
    if (e) return e;
    start_time = i;
    break;
  case KCA_OPTION:
    e = optval_to_int (&i, key, arg, state, &kca_range, 0);
    if (e) return e;
    skill.counter_attack_prob = i - 1;
    break;
  case KCD_OPTION:
    e = optval_to_int (&i, key, arg, state, &kcd_range, 0);
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
    e = option_get_args (key, arg, state, ',', ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &x, &y, &window_position_range, &window_position_range);
    if (e) return e;
    al_set_new_window_position (x, y);
    break;
  case WINDOW_DIMENSIONS_OPTION:
    e = option_get_args (key, arg, state, 'x', ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &display_width, &display_height,
                         &window_dimensions_range, &window_dimensions_range);
    if (e) return e;
    break;
  case INHIBIT_SCREENSAVER_OPTION:
    inhibit_screensaver = optval_to_bool (arg);
    break;
  case PRINT_PATHS_OPTION:
    print_paths ();
    exit (0);
  case SKIP_TITLE_OPTION:
    skip_title = optval_to_bool (arg);
    break;
  case JOYSTICK_AXIS_THRESHOLD_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_ENUM, ARG_TYPE_FLOAT, 0,
                         &i, &float_val, joystick_axis_threshold_enum,
                         &joystick_axis_threshold_range);
    if (e) return e;
    switch (i) {
    case 0: joystick_h_threshold = float_val; break;
    case 1: joystick_v_threshold = float_val; break;
    }
    break;
  case JOYSTICK_BUTTON_THRESHOLD_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_ENUM, ARG_TYPE_INT, 0,
                         &i, &int_val0, joystick_button_threshold_enum,
                         &joystick_button_threshold_range);
    if (e) return e;
    switch (i) {
    case 0: joystick_up_threshold = int_val0; break;
    case 1: joystick_right_threshold = int_val0; break;
    case 2: joystick_down_threshold = int_val0; break;
    case 3: joystick_left_threshold = int_val0; break;
    case 4: joystick_enter_threshold = int_val0; break;
    case 5: joystick_shift_threshold = int_val0; break;
    }
    break;
  case JOYSTICK_AXIS_OPTION:
    e = option_get_args (key, arg, state, ',',
                         ARG_TYPE_ENUM, ARG_TYPE_INT, ARG_TYPE_INT, 0,
                         &i, &int_val0, &int_val1, joystick_axis_enum,
                         &joystick_axis_range, &joystick_axis_range);
    if (e) return e;
    switch (i) {
    case 0:
      joystick_h_stick = int_val0;
      joystick_h_axis = int_val1;
      break;
    case 1:
      joystick_v_stick = int_val0;
      joystick_v_axis = int_val1;
      break;
    }
    break;
  case JOYSTICK_BUTTON_OPTION:
    e = option_get_args (key, arg, state, ',', ARG_TYPE_ENUM, ARG_TYPE_INT, 0,
                         &i, &int_val0, joystick_button_enum, &joystick_button_range);
    if (e) return e;
    switch (i) {
    case 0: joystick_up_button = int_val0; break;
    case 1: joystick_right_button = int_val0; break;
    case 2: joystick_down_button = int_val0; break;
    case 3: joystick_left_button = int_val0; break;
    case 4: joystick_enter_button = int_val0; break;
    case 5: joystick_shift_button = int_val0; break;
    case 6: joystick_time_button = int_val0; break;
    case 7: joystick_pause_button = int_val0; break;
    }
    break;
  case JOYSTICK_INFO_OPTION:
    if (joystick_info ()) {
      error (-1, 0, "Joystick not found");
      exit (-1);
    }
    exit (0);
    break;
  case ARGP_KEY_ARG:
    /* cheat */
    if (! strcasecmp ("MEGAHIT", arg)) break;
    else if (! strcasecmp ("IMPROVED", arg)) break;
    else if (sscanf (arg, "%i", &i) == 1
             && i >= 1 && i <= INT_MAX) start_level = i;

    /* audio */
    else if (! strcasecmp ("ADLIB", arg)) break;
    else if (! strcasecmp ("COMPUADD", arg)) break;
    else if (! strcasecmp ("COVOX", arg)) break;
    else if (! strcasecmp ("DIGI", arg)) break;
    else if (! strcasecmp ("DISNEY", arg)) break;
    else if (! strcasecmp ("GBLAST", arg)) break;
    else if (! strcasecmp ("IBMG", arg)) break;
    else if (! strcasecmp ("MIDI", arg)) break;
    else if (! strcasecmp ("MVPAS", arg)) break;
    else if (! strcasecmp ("SBLAST", arg)) break;
    else if (! strcasecmp ("STDSND", arg)) break;
    else if (! strcasecmp ("TANDY", arg)) break;

    /* video */
    else if (! strcasecmp ("CGA", arg)) vm = CGA;
    else if (! strcasecmp ("DRAW", arg)) break;
    else if (! strcasecmp ("EGA", arg)) vm = EGA;
    else if (! strcasecmp ("HERC", arg)) vm = CGA, hgc = true;
    else if (! strcasecmp ("HGA", arg)) vm = CGA, hgc = true;
    else if (! strcasecmp ("MCGA", arg)) vm = VGA;
    else if (! strcasecmp ("TGA", arg)) vm = EGA;
    else if (! strcasecmp ("VGA", arg)) vm = VGA;

    /* memory */
    else if (! strcasecmp ("BYPASS", arg)) break;
    else if (! strcasecmp ("PACK", arg)) break;
    else if (! strcasecmp ("UNPACK", arg)) break;

    /* miscellaneous */
    else if (! strcasecmp ("DEMO", arg)) break;
    else if (! strcasecmp ("J", arg)) break;

    else return ARGP_ERR_UNKNOWN;
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

static error_t
get_config_args (size_t *cargc, char ***cargv, struct argp_option *options,
                 char *filename)
{
  size_t i;

  *cargv = add_to_array (&argv[0], 1, *cargv, cargc, *cargc, sizeof (argv[0]));

  ALLEGRO_CONFIG *config = al_load_config_file (filename);
  if (! config) return al_get_errno ();

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

  return 0;
}

int
main (int _argc, char **_argv)
{
  struct config_info config_info;

  /* make command-line arguments available globally */
  argc = _argc;
  argv = _argv;

  set_program_name (argv[0]);

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
  pre_argp.doc = doc;

  /* pre parser */
  config_info.type = CI_COMMAND_LINE;
  argp_parse (&pre_argp, argc, argv, 0, NULL, &config_info);

  /* parser */
  if (! ignore_main_config) {
    config_info.type = CI_CONFIGURATION_FILE;
    config_info.filename = config_filename;
    argp_parse (&argp, cargc, cargv, 0, NULL, &config_info);
  }
  if (! ignore_environment) {
    config_info.type = CI_ENVIRONMENT_VARIABLES;
    argp_parse (&argp, eargc, eargv, 0, NULL, &config_info);
  }
  config_info.type = CI_COMMAND_LINE;
  argp_parse (&argp, argc, argv, 0, NULL, &config_info);

  init_video ();
  init_audio ();
  if (sound_disabled_cmd) enable_audio (false);
  init_gamepad ();
  init_dialog ();
  init_mouse ();

  al_inhibit_screensaver (inhibit_screensaver);

  draw_loading_screen ();

  show_mouse_cursor ();
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_BUSY);

  load_samples ();
  load_level ();
  load_cutscenes ();

  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);

  /* ----------------- */
  /* save_guard_bitmaps (VGA); */
  /* exit (0); */
  /* --------------- */

  if (skip_title) goto play_game;

 restart_game:
  set_system_mouse_cursor (ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
  set_multi_room (1, 1);
  clear_bitmap (mr.cell[0][0].screen, BLACK);
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
  game_paused = false;
  total_lives = initial_total_lives;
  current_lives = initial_current_lives;

  if (! play_time) play_time = create_timer (1.0);
  al_set_timer_count (play_time, start_time);
  al_start_timer (play_time);

  give_dat_compat_preference ();

  switch (level_module) {
  case NATIVE_LEVEL_MODULE: default:
    play_native_level (start_level);
    break;
  case LEGACY_LEVEL_MODULE:
    play_legacy_level (start_level);
    break;
  case PLV_LEVEL_MODULE:
    play_plv_level (start_level);
    break;
  case DAT_LEVEL_MODULE:
    play_dat_level (start_level);
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
  finalize_gamepad ();
  finalize_dialog ();
  finalize_mouse ();

  fprintf (stderr, "MININIM: Hope you enjoyed it!\n");

  return 0;
}

static void
give_dat_compat_preference (void)
{
  if (al_filename_exists (levels_dat_compat_filename)
      && ! level_module_given) {
    level_module = DAT_LEVEL_MODULE;
    levels_dat_filename = levels_dat_compat_filename;
  }
}

static void
draw_loading_screen (void)
{
  int x = 138;
  int y = 40;
  int w = al_get_bitmap_width (icon);
  int h = al_get_bitmap_height (icon);
  ALLEGRO_BITMAP *screen = mr.cell[0][0].screen;
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

  /* get legacy LEVELS.DAT compatibility path */
  xasprintf (&levels_dat_compat_filename, "LEVELS.DAT");
}

static void
print_paths (void)
{
  printf ("Main configuration file: %s\n", config_filename);
  printf ("Executable file: %s\n", exe_filename);
  printf ("Resources: %s\n", resources_dir);
  printf ("System data: %s%c\n", system_data_dir, ALLEGRO_NATIVE_PATH_SEP);
  printf ("Temporary: %s\n", temp_dir);
  printf ("User home: %s\n", user_home_dir);
  printf ("User documents: %s\n", user_documents_dir);
  printf ("User data: %s\n", user_data_dir);
  printf ("User settings: %s\n", user_settings_dir);
}

void *
load_config_dialog (ALLEGRO_THREAD *thread, void *arg)
{
  ALLEGRO_FILECHOOSER *dialog =
    create_native_file_dialog (user_home_dir, "Load configuration file",
                               "*.*", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
  al_show_native_file_dialog (display, dialog);

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
  al_show_native_file_dialog (display, dialog);

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
  struct config_info config_info;

  ALLEGRO_TEXTLOG *textlog =
    open_native_text_log ("Configuration loading results",
                          ALLEGRO_TEXTLOG_MONOSPACE);

  al_append_native_text_log (textlog, "Loading %s...\n", filename);

  get_config_args (&cargc, &cargv, options, filename);

  config_info.type = CI_CONFIGURATION_FILE;
  config_info.filename = filename;
  config_info.textlog = textlog;

  bool parse_error = false;
  int last_arg_index, arg_index = -1;
  error_t e;

  do {
    last_arg_index = ++arg_index;
    e = argp_parse (&argp, cargc - arg_index, &cargv[arg_index],
                    ARGP_SILENT, &arg_index, &config_info);
    if (! parse_error) parse_error = e;
  } while (e && arg_index > last_arg_index);

  destroy_array ((void **) &cargv, &cargc);

  char *error_str;
  if (! parse_error) {
    al_close_native_text_log (textlog);
    textlog = NULL;
    error_str = "CONFIGURATION LOADED SUCCESSFULLY";
  } else error_str = "CONFIGURATION LOADED WITH ERRORS";

  draw_bottom_text (NULL, error_str, 0);

  return textlog;
}

void
save_game (char *filename)
{
  ALLEGRO_CONFIG *config = create_config ();
  char *start_level_str, *start_time_str,
    *total_lives_str, *kca_str, *kcd_str;

  xasprintf (&start_level_str, "%i", level.number);
  xasprintf (&start_time_str, "%i", start_level_time);
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

  draw_bottom_text (NULL, error_str, 0);

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

unsigned char
add_char (unsigned char a, signed char b)
{
  if (b < 0 && -b > a) return 0;
  if (b > 0 && b > UCHAR_MAX - a) return UCHAR_MAX;
  return a + b;
}

int
int_to_range (int i, int min, int max)
{
  if (i >= max) return max;
  else if (i <= min) return min;
  else return i;
}

float
dist_cart (float x0, float y0, float x1, float y1)
{
  int dx = x0 - x1;
  int dy = y0 - y1;
  return sqrt (dx * dx + dy * dy);
}
