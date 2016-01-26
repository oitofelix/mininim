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

static bool sound_disabled_cmd;

static error_t parser (int key, char *arg, struct argp_state *state);

enum options {
  VIDEO_MODE_OPTION = 256, ENVIRONMENT_MODE_OPTION, GUARD_MODE_OPTION,
  NO_SOUND_OPTION, DISPLAY_FLIP_OPTION, KEYBOARD_FLIP_OPTION,
};

static struct argp_option options[] = {
  {"video-mode", VIDEO_MODE_OPTION, "VIDEO-MODE", 0, "Select video mode.  Valid values for VIDEO-MODE are: VGA, EGA, CGA and HGC.  The default is VGA.  This can be changed in-game by the F12 key.", 0},
  {"environment-mode", ENVIRONMENT_MODE_OPTION, "ENVIRONMENT-MODE", 0, "Select environment mode.  Valid values for ENVIRONMENT-MODE are: ORIGINAL, DUNGEON and PALACE.  The 'ORIGINAL' value gives level modules autonomy in this choice for each particular level.  This is the default.  This can be changed in-game by the F11 key.", 0},
  {"guard-mode", GUARD_MODE_OPTION, "GUARD-MODE", 0, "Select guard mode.  Valid values for GUARD-MODE are: ORIGINAL, GUARD, FAT-GUARD, VIZIER, SKELETON and SHADOW.  The 'ORIGINAL' value gives level modules autonomy in this choice for each particular guard.  This is the default.  This can be changed in-game by the F10 key.", 0},
  {"no-sound", NO_SOUND_OPTION, NULL, 0, "Disable sound.  The default is to have sound enabled.  This can be changed in-game by the CTRL+S keystroke.", 0},
  {"display-flip", DISPLAY_FLIP_OPTION, "DISPLAY-FLIP-MODE", 0, "Select display flip mode.  Valid values for DISPLAY-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+I keystroke.", 0},
  {"keyboard-flip", KEYBOARD_FLIP_OPTION, "KEYBOARD-FLIP-MODE", 0, "Select keyboard flip mode.  Valid values for KEYBOARD-FLIP-MODE are: NONE, VERTICAL, HORIZONTAL and VERTICAL-HORIZONTAL.  The default is NONE.  This can be changed in-game by the SHIFT+K keystroke.", 0},
  {0},
};

static const char *doc = "MININIM: The Advanced Prince of Persia Engine\v";

struct argp_child argp_child = { NULL };

static struct argp argp = {options, parser, NULL, NULL, &argp_child, NULL, NULL};

static error_t
parser (int key, char *arg, struct argp_state *state)
{
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
  case NO_SOUND_OPTION: sound_disabled_cmd = true; break;
  case DISPLAY_FLIP_OPTION:
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
  case KEYBOARD_FLIP_OPTION:
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
  argp_program_version_hook = version;
  argp.doc = doc;

  argp_parse (&argp, argc, argv, 0, NULL, NULL);

  al_init ();
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
  play_legacy_level ();
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
