/*
  editor.c -- editor module;

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

static char bmenu_int_ext (struct pos *p, int steps, int fases,
                          char *prefix, char *b_str);
static char bmenu_select_room (enum edit up_edit, char *prefix);
static char bmenu_select_level (enum edit up_edit, char *prefix);
static char bmenu_link (enum dir dir);
static void mouse2guard (int i);
static char bmenu_skill (char *prefix, int *skill, int max, enum edit up_edit);

static int last_event;
static struct mouse_coord last_mouse_coord;
static struct pos last_event2floor_pos;
static bool reciprocal_links, locally_unique_links,
  globally_unique_links;
static bool b0, b1, b2, b3, b4, b5;
static int guard_index;
static int bb, r, s, t;
static struct tile_copy tile_copy;
static struct room_copy room_copy;
static struct level level_copy;
static char *msg;
static uint64_t msg_cycles;

enum edit edit;
enum edit last_edit = EDIT_MAIN;
uint64_t editor_register;

bool selection_locked;

void
change_tile_fg (struct pos *p, enum tile_fg f)
{
  register_tile_undo (&undo, p,
                      f, MIGNORE, MIGNORE, MIGNORE,
                      NULL, (intptr_t) change_tile_fg,
                      tile_fg_str[f]);
}

void
change_tile_fake (struct pos *p, enum tile_fg f)
{
  char *str = xasprintf ("FAKE %s", tile_fg_str[f]);
  register_tile_undo (&undo, p,
                      MIGNORE, MIGNORE, MIGNORE, f,
                      NULL, (intptr_t) change_tile_fake, str);
  al_free (str);
}

void
change_tile_bg (struct pos *p, enum tile_bg b)
{
  register_tile_undo (&undo, p,
                      MIGNORE, b, MIGNORE, MIGNORE,
                      NULL, (intptr_t) change_tile_bg,
                      tile_bg_str[b]);
}

void
select_pos (struct mr *mr, struct pos *p)
{
  struct pos np;
  npos (p, &np);

  if (! np.room) return;

  selection_locked = true;
  selection_pos = np;
  mr_scroll_into_view (mr, np.room);
}

void
change_tile_ext (struct pos *p, int e)
{
  char *str;
  if (is_item_fg (p)) str = xasprintf ("%s", tile_item_str[e]);
  else if (is_fall_fg (p)) str = xasprintf ("FALL EXT %i", e);
  else if (is_event_fg (p)) str = xasprintf ("EVENT EXT %i", e);
  else if (is_step_fg (p)) str = xasprintf ("STEP EXT %i", e);
  else if (is_design_fg (p)) str = xasprintf ("DESIGN EXT %i", e);
  else {
    assert (false);
    return;
  }

  register_tile_undo (&undo, p,
                      MIGNORE, MIGNORE, e, MIGNORE,
                      NULL, (intptr_t) change_tile_ext, str);

  al_free (str);
}


bool
can_edit (void)
{
  return ! cutscene && ! title_demo && replay_mode == NO_REPLAY;
}

void
editor (void)
{
  if (edit == EDIT_NONE) return;

  global_mr.select_cycles = SELECT_CYCLES;
  gui_run_callback_IFn ("_UPDATE_CB", gui_editor_dialog);

  struct bmenu_item bmain_menu[] =
    {{'T', "TILE>"},
     {'E', "EVENT>"},
     {'R', "ROOM>"},
     {'K', "KID>"},
     {'G', "GUARD>"},
     {'L', "LEVEL>"},
     {0}};

  struct bmenu_item tile_menu[] =
    {{'F', "FOREGROUND>"},
     {'B', "BACKGROUND>"},
     {'E', "EXTENSION*"},
     {'#', "NUMERICAL EXTENSION<"},
     {'K', "FAKE>"},
     {'-', "UNFAKE"},
     {'+', "FG<->FAKE"},
     {'I', "NOMINAL INFO"},
     {'N', "NUMERICAL INFO"},
     {'A', "CLEAR TILE"},
     {'R', "RANDOMIZE TILE"},
     {'D', "DECORATE TILE"},
     {'M', "MIRROR>"},
     {'C', "COPY TILE"},
     {'P', "PASTE TILE"},
     {'!', "FIX TILE"},
     {0}};

  struct bmenu_item mirror_menu[] =
    {{'T', "TILES>"},
     {'L', "LINKS>"},
     {'B', "TILES+LINKS>"},
     {0}};

  struct bmenu_item mirror_dir_menu[] =
    {{'H', "HORIZONTAL"},
     {'V', "VERTICAL"},
     {'B', "HORIZONTAL+VERTICAL"},
     {'R', "RANDOM"},
     {0}};

  struct bmenu_item mirror_tile_menu[] =
    {{'H', "HORIZONTAL"},
     {'V', "VERTICAL"},
     {'B', "HORIZONTAL+VERTICAL"},
     {'R', "RANDOM"},
     {'A', "LEFT"},
     {'D', "RIGHT"},
     {'W', "ABOVE"},
     {'S', "BELOW"},
     {0}};

  struct bmenu_item fg_menu[] =
    {{'#', "NUMERICAL<"},
     {'F', "FLOOR>"},
     {'P', "PILLAR>"},
     {'W', "WALL"},
     {'D', "DOOR>"},
     {'C', "CHOMPER"},
     {'M', "MIRROR"},
     {'R', "CARPET>"},
     {'A', "ARCH>"},
     {0}};

  struct bmenu_item floor_menu[] =
    {{'N', "NO FLOOR"},
     {'F', "FLOOR"},
     {'B', "BROKEN FLOOR"},
     {'S', "SKELETON FLOOR"},
     {'L', "LOOSE FLOOR"},
     {'P', "SPIKES FLOOR"},
     {'O', "OPENER FLOOR"},
     {'C', "CLOSER FLOOR"},
     {'T', "STUCK FLOOR"},
     {'H', "HIDDEN FLOOR"},
     {0}};

  struct bmenu_item pillar_menu[] =
    {{'P', "PILLAR"},
     {'T', "BIG PILLAR TOP"},
     {'B', "BIG PILLAR BOTTOM"},
     {'A', "ARCH BOTTOM"},
     {0}};

  struct bmenu_item door_menu[] =
    {{'D', "DOOR"},
     {'L', "LEVEL DOOR"},
     {0}};

  struct bmenu_item carpet_menu[] =
    {{'C', "CARPET"},
     {'T', "TRAVERSABLE CARPET"},
     {0}};

  struct bmenu_item arch_menu[] =
    {{'M', "ARCH TOP MID"},
     {'S', "ARCH TOP SMALL"},
     {'L', "ARCH TOP LEFT"},
     {'R', "ARCH TOP RIGHT"},
     {0}};

  struct bmenu_item bg_menu[] =
    {{'#', "NUMERICAL<"},
     {'N', "NO BRICKS"},
     {'1', "BRICKS 1"},
     {'2', "BRICKS 2"},
     {'3', "BRICKS 3"},
     {'4', "BRICKS 4"},
     {'5', "BRICKS 5"},
     {'T', "TORCH"},
     {'W', "WINDOW"},
     {'B', "BALCONY"},
     {0}};

  struct bmenu_item items_menu[] =
    {{'N', "NO ITEM"},
     {'E', "EMPTY POTION"},
     {'S', "SMALL HP POTION"},
     {'B', "BIG HP POTION"},
     {'P', "SMALL POISON POTION"},
     {'O', "BIG POISON POTION"},
     {'F', "FLOAT POTION"},
     {'L', "FLIP POTION"},
     {'A', "ACTIVATION POTION"},
     {'W', "SWORD"},
     {0}};

  struct bmenu_item loose_floor_ext_menu[] =
    {{'C', "CAN'T FALL"},
     {0}};

  struct bmenu_item carpet_ext_menu[] =
    {{'1', "DESIGN 1"},
     {'2', "DESIGN 2"},
     {'3', "DESIGN 3"},
     {'4', "DESIGN 4"},
     {'5', "DESIGN 5"},
     {'6', "DESIGN 6"},
     {0}};

  struct bmenu_item event_menu[] =
    {{'C', "EVENT->TILE<"},
     {'F', "EVENT->FLOOR<"},
     {'R', "TILE->EVENT<"},
     {'S', "SET EVENT<"},
     {0}};

  struct bmenu_item room_menu[] =
    {{'J', "JUMP<"},
     {'L', "ROOM LINKING>"},
     {'S', "LINKING SETTINGS<"},
     {'X', "EXCHANGE<"},
     {'A', "CLEAR ROOM"},
     {'R', "RANDOMIZE ROOM"},
     {'D', "DECORATE ROOM"},
     {'M', "MIRROR>"},
     {'C', "COPY ROOM"},
     {'P', "PASTE ROOM"},
     {'!', "FIX ROOM"},
     {0}};

  struct bmenu_item link_menu[] =
    {{'L', "LEFT<"},
     {'R', "RIGHT<"},
     {'A', "ABOVE<"},
     {'B', "BELOW<"},
     {0}};

  struct bmenu_item linking_settings_menu[] =
    {{'R', "RECIPROCAL"},
     {'L', "LOCALLY UNIQUE"},
     {'G', "GLOBALLY UNIQUE"},
     {0}};

  struct bmenu_item kid_menu[] =
    {{'P', "PLACE KID"},
     {'S', "SET START POSITION"},
     {'J', "JUMP TO START POSITION"},
     {'D', "TOGGLE START DIRECTION"},
     {'W', "TOGGLE HAS SWORD"},
     {0}};

  struct bmenu_item level_menu[] =
    {{'J', "JUMP<"},
     {'X', "EXCHANGE<"},
     {'A', "CLEAR LEVEL"},
     {'R', "RANDOMIZE LEVEL"},
     {'D', "DECORATE LEVEL"},
     {'M', "MIRROR>"},
     {'C', "COPY LEVEL"},
     {'P', "PASTE LEVEL"},
     {'N', "NOMINAL NUMBER"},
     {'E', "ENVIRONMENT<"},
     {'H', "HUE<"},
     {'S', "SAVE LEVEL"},
     {'L', "RELOAD LEVEL"},
     {'!', "FIX LEVEL"},
     {0}};

  struct bmenu_item environment_menu[] =
    {{'D', "DUNGEON"},
     {'P', "PALACE"},
     {0}};

  struct bmenu_item hue_menu[] =
    {{'N', "NONE"},
     {'G', "GREEN"},
     {'R', "GRAY"},
     {'Y', "YELLOW"},
     {'B', "BLUE"},
     {0}};

  struct bmenu_item guard_menu[] =
    {{'G', "SELECT GUARD<"},
     {'P', "PLACE GUARD"},
     {'S', "SET START POSITION"},
     {'J', "JUMP TO START POSITION"},
     {'D', "TOGGLE START DIRECTION"},
     {'K', "SKILL>"},
     {'H', "HP<"},
     {'T', "TYPE<"},
     {'Y', "STYLE<"},
     {0}};

  struct bmenu_item skill_menu[] =
    {{'A', "ATTACK<"},
     {'B', "COUNTER ATTACK<"},
     {'D', "DEFENSE<"},
     {'E', "COUNTER DEFENSE<"},
     {'V', "ADVANCE<"},
     {'R', "RETURN<"},
     {'F', "REFRACTION PERIOD<"},
     {'X', "EXTRA HP<"},
     {'L', "LEGACY TEMPLATES<"},
     {0}};

  struct bmenu_item guard_type_menu[] =
    {{'D', "DISABLED"},
     {'G', "GUARD"},
     {'F', "FAT GUARD"},
     {'V', "VIZIER"},
     {'S', "SKELETON"},
     {'H', "SHADOW"},
     {0}};

  struct pos p = selection_pos;
  static struct guard *g;
  static struct pos p0;
  static bool fake_fg;

  char *fg_str = NULL, *bg_str = NULL, *ext_str = NULL, *fake_str = NULL;
  bool free_ext_str;
  char *str = NULL, c;
  int i;

  struct room_linking *l;

  enum tile_fg f;
  enum tile_bg b;
  int e;
  static struct skill skill_buf;

  /* display message if available */
  if (msg_cycles > EDITOR_CYCLES_0 && msg
      && (was_key_pressed (0, ALLEGRO_KEY_BACKSPACE)
          || was_char_pressed ('/'))) {
    msg_cycles = 0;
    ui_msg (0, "%s", msg);
    bmenu_help = 0;
 } else if (msg_cycles > 0 && msg) msg_cycles--;
  else msg_cycles = 0;

  editor_register = EDITOR_CYCLES_3;
  active_menu = true;

  switch (edit) {
  case EDIT_NONE: break;
  case EDIT_MAIN:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (bmain_menu, NULL)) {
    case 'T': edit = EDIT_TILE; break;
    case 'E': edit = EDIT_EVENT; break;
    case 'R': edit = EDIT_ROOM; break;
    case 'K': edit = EDIT_KID; break;
    case 'G': edit = EDIT_GUARD; break;
    case 'L': edit = EDIT_LEVEL; break;
    }
    break;
  case EDIT_TILE:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_MAIN;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    switch (bmenu_enum (tile_menu, "T>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'F': edit = EDIT_FG;
      fake_fg = false; break;
    case 'B': edit = EDIT_BG; break;
    case 'E': edit = EDIT_EXT; break;
    case '#': edit = EDIT_NUMERICAL_EXT; break;
    case 'K': edit = EDIT_FG;
      fake_fg = true; break;
    case '-':
      register_tile_undo (&undo, &p,
                          MIGNORE, MIGNORE, MIGNORE, NO_FAKE,
                          NULL, (intptr_t) "UNFAKE", "UNFAKE");
      break;
    case '+':
      register_tile_undo (&undo, &p,
                          fake (&p), MIGNORE, MIGNORE, fg (&p),
                          NULL, (intptr_t) "FG<->FAKE", "FG<->FAKE");
      break;
    case 'I': edit = EDIT_NOMINAL_INFO; break;
    case 'N': edit = EDIT_NUMERICAL_INFO; break;
    case 'A':
      apply_to_place (&p, clear_tile, NULL, "CLEAR TILE");
      break;
    case 'R':
      apply_to_place (&p, random_tile, NULL, "RANDOMIZE TILE");
      break;
    case 'D':
      apply_to_place (&p, decorate_tile, NULL, "DECORATE TILE");
      break;
    case 'M': edit = EDIT_MIRROR_TILE; break;
    case 'C':
      copy_tile (&tile_copy, &p);
      editor_msg ("COPY TILE", EDITOR_CYCLES_3);
      break;
    case 'P':
      apply_to_place (&p, (pos_trans) paste_tile, &tile_copy, "PASTE TILE");
      break;
    case '!':
      apply_to_place (&p, fix_tile, NULL, "FIX TILE");
      break;
    }
    break;
  case EDIT_MIRROR_TILE:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    switch (bmenu_enum (mirror_tile_menu, "TM>")) {
    case -1: case 1: edit = EDIT_TILE; break;
    case 'H':
      reflect_pos_h (&p, &p0);
      register_mirror_pos_undo (&undo, &p, &p0, true, "MIRROR TILE H.");
      break;
    case 'V':
      reflect_pos_v (&p, &p0);
      register_mirror_pos_undo (&undo, &p, &p0, false, "MIRROR TILE V.");
      break;
    case 'B':
      reflect_pos_h (&p, &p0);
      reflect_pos_v (&p0, &p0);
      register_mirror_pos_undo (&undo, &p, &p0, true, "MIRROR TILE H+V.");
      break;
    case 'R':
      random_pos (&global_level, &p0);
      p0.room = p.room;
      register_mirror_pos_undo (&undo, &p, &p0, false, "MIRROR TILE R.");
      break;
    case 'A':
      prel (&p, &p0, +0, -1);
      register_mirror_pos_undo (&undo, &p, &p0, false, "MIRROR TILE LEFT");
      break;
    case 'D':
      prel (&p, &p0, +0, +1);
      register_mirror_pos_undo (&undo, &p, &p0, false, "MIRROR TILE RIGHT");
      break;
    case 'W':
      prel (&p, &p0, -1, +0);
      register_mirror_pos_undo (&undo, &p, &p0, false, "MIRROR TILE ABOVE");
      break;
    case 'S':
      prel (&p, &p0, +1, +0);
      register_mirror_pos_undo (&undo, &p, &p0, false, "MIRROR TILE BELOW");
      break;
    }
    break;
  case EDIT_FG:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    switch (bmenu_enum (fg_menu, fake_fg ? "TK>" : "TF>")) {
    case -1: case 1: edit = EDIT_TILE; break;
    case '#': edit = EDIT_NUMERICAL_FG; break;
    case 'F': edit = EDIT_FLOOR; break;
    case 'P': edit = EDIT_PILLAR; break;
    case 'W':
      if ((! fake_fg && fg (&p) == WALL)
          || (fake_fg && fake (&p) == WALL)) break;

      if (fake_fg) change_tile_fake (&p, WALL);
      else change_tile_fg (&p, WALL);

      break;
    case 'D': edit = EDIT_DOOR; break;
    case 'C':
      if ((! fake_fg && fg (&p) == CHOMPER)
          || (fake_fg && fake (&p) == CHOMPER)) break;

      if (fake_fg) change_tile_fake (&p, CHOMPER);
      else change_tile_fg (&p, CHOMPER);

      break;
    case 'M':
      if ((! fake_fg && fg (&p) == MIRROR)
          || (fake_fg && fake (&p) == MIRROR)) break;

      if (fake_fg) change_tile_fake (&p, MIRROR);
      else change_tile_fg (&p, MIRROR);

      break;
    case 'R': edit = EDIT_CARPET; break;
    case 'A': edit = EDIT_ARCH; break;
    }
    break;
  case EDIT_NUMERICAL_FG:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_FG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    i = fake_fg ? fake (&p): fg (&p);
    switch (bmenu_int (&i, NULL, INT_MIN, INT_MAX,
                      fake_fg ? "CK#>FG #" : "CF#>FG #", NULL)) {
    case -1: edit = EDIT_FG; break;
    case 0: break;
    case 1: edit = EDIT_FG; break;
    default:
      editor_register = EDITOR_CYCLES_NONE;

      if (fake_fg) change_tile_fake (&p, i);
      else change_tile_fg (&p, i);

      editor_register = EDITOR_CYCLES_3;
      break;
    }
    break;
  case EDIT_FLOOR:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_FG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = bmenu_enum (floor_menu, fake_fg ? "TKF>" : "TFF>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    f = fake_fg ? fake (&p) : fg (&p);
    if ((c == 'L' && f == LOOSE_FLOOR) || (c == 'P' && f == SPIKES_FLOOR)
        || (c == 'O' && f == OPENER_FLOOR) || (c == 'C' && f == CLOSER_FLOOR))
      break;

    switch (c) {
    case 'N': f = NO_FLOOR; break;
    case 'F': f = FLOOR; break;
    case 'B': f = BROKEN_FLOOR; break;
    case 'S': f = SKELETON_FLOOR; break;
    case 'L': f = LOOSE_FLOOR; break;
    case 'P': f = SPIKES_FLOOR; break;
    case 'O': f = OPENER_FLOOR; break;
    case 'C': f = CLOSER_FLOOR; break;
    case 'T': f = STUCK_FLOOR; break;
    case 'H': f = HIDDEN_FLOOR; break;
    }

    if (fake_fg) change_tile_fake (&p, f);
    else change_tile_fg (&p, f);

    break;
  case EDIT_PILLAR:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_FG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = bmenu_enum (pillar_menu, fake_fg ? "TKP>" : "TFP>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    switch (c) {
    case 'P': f = PILLAR; break;
    case 'T': f = BIG_PILLAR_TOP; break;
    case 'B': f = BIG_PILLAR_BOTTOM; break;
    case 'A': f = ARCH_BOTTOM; break;
    }

    if (fake_fg) change_tile_fake (&p, f);
    else change_tile_fg (&p, f);

    break;
  case EDIT_DOOR:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_FG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = bmenu_enum (door_menu, fake_fg ? "TKD>" : "TFD>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    f = fake_fg ? fake (&p) : fg (&p);
    if ((c == 'D' && f == DOOR) || (c == 'L' && f == LEVEL_DOOR))
      break;

    switch (c) {
    case 'D': f = DOOR; break;
    case 'L': f = LEVEL_DOOR; break;
    }

    if (fake_fg) change_tile_fake (&p, f);
    else change_tile_fg (&p, f);

    break;
  case EDIT_CARPET:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_FG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = bmenu_enum (carpet_menu, fake_fg ? "TKR>" :"TFR>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    switch (c) {
    case 'C': f = CARPET; break;
    case 'T': f = TCARPET; break;
    }

    if (fake_fg) change_tile_fake (&p, f);
    else change_tile_fg (&p, f);

    break;
  case EDIT_ARCH:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_FG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = bmenu_enum (arch_menu, fake_fg ? "TKA>" : "TFA>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_FG; break;
    }

    switch (c) {
    case 'M': f = ARCH_TOP_MID; break;
    case 'S': f = ARCH_TOP_SMALL; break;
    case 'L': f = ARCH_TOP_LEFT; break;
    case 'R': f = ARCH_TOP_RIGHT; break;
    }

    if (fake_fg) change_tile_fake (&p, f);
    else change_tile_fg (&p, f);

    break;
  case EDIT_BG:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    c = bmenu_enum (bg_menu, "TB>");
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_TILE; break;
    }

    if (c == '#') {
      edit = EDIT_NUMERICAL_BG; break;
    }

    switch (c) {
    case 'N': b = NO_BRICKS; break;
    case '1': b = BRICKS_1; break;
    case '2': b = BRICKS_2; break;
    case '3': b = BRICKS_3; break;
    case '4': b = BRICKS_4; break;
    case '5': b = BRICKS_5; break;
    case 'T': b = TORCH; break;
    case 'W': b = WINDOW; break;
    case 'B': b = BALCONY; break;
    }

    change_tile_bg (&p, b);

    break;
  case EDIT_NUMERICAL_BG:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_BG;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    i = bg (&p);
    switch (bmenu_int (&i, NULL, INT_MIN, INT_MAX, "CB#>BG #", NULL)) {
    case -1: edit = EDIT_BG; break;
    case 0: break;
    case 1: edit = EDIT_BG; break;
    default:
      editor_register = EDITOR_CYCLES_NONE;

      change_tile_bg (&p, i);

      editor_register = EDITOR_CYCLES_3;
      break;
    }
    break;
  case EDIT_EXT:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;
      break;
    }

    switch (fg (&p)) {
    case FLOOR: case BROKEN_FLOOR: case SKELETON_FLOOR:
    case STUCK_FLOOR: case HIDDEN_FLOOR: case PILLAR:
    case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      c = bmenu_enum (items_menu, "TE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_TILE; break;
      }

      switch (c) {
      case 'N': e = NO_ITEM; break;
      case 'E': e = EMPTY_POTION; break;
      case 'S': e = SMALL_HP_POTION; break;
      case 'B': e = BIG_HP_POTION; break;
      case 'P': e = SMALL_POISON_POTION; break;
      case 'O': e = BIG_POISON_POTION; break;
      case 'F': e = FLOAT_POTION; break;
      case 'L': e = FLIP_POTION; break;
      case 'A': e = ACTIVATION_POTION; break;
      case 'W': e = SWORD; break;
      }

      change_tile_ext (&p, e);

      break;
    case LOOSE_FLOOR:
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      b0 = ext (&p);
      c = bmenu_bool (loose_floor_ext_menu, "TE>", false, &b0);
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_TILE; break;
      }

      change_tile_ext (&p, b0);

      break;
    case SPIKES_FLOOR:
      bmenu_int_ext (&p, SPIKES_STEPS, SPIKES_FASES, "CE>STEP", NULL);
      break;
    case OPENER_FLOOR:
    case CLOSER_FLOOR:
      bmenu_int_ext (&p, global_level.event_nmemb, 2, "CE>EVENT", "B");
      break;
    case DOOR:
      bmenu_int_ext (&p, DOOR_STEPS, DOOR_FASES, "CE>STEP", NULL);
      break;
    case LEVEL_DOOR:
      bmenu_int_ext (&p, LEVEL_DOOR_STEPS, LEVEL_DOOR_FASES, "CE>STEP", "B");
      break;
    case CHOMPER:
      bmenu_int_ext (&p, CHOMPER_STEPS, CHOMPER_FASES, "CE>STEP", "B");
      break;
    case CARPET:
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      c = bmenu_enum (carpet_ext_menu, "TE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_TILE; break;
      }

      switch (c) {
      case '1': e = 0; break;
      case '2': e = 1; break;
      case '3': e = 2; break;
      case '4': e = 3; break;
      case '5': e = 4; break;
      case '6': e = 5; break;
      }

      change_tile_ext (&p, e);

      break;
    case TCARPET:
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      c = bmenu_enum (carpet_ext_menu, "TE>");
      if (! c) break;

      if (c == -1 || c == 1) {
        edit = EDIT_TILE; break;
      }

      switch (c) {
      case '1': e = 0; break;
      case '2': e = 1; break;
      case '3': e = 2; break;
      case '4': e = 3; break;
      case '5': e = 4; break;
      case '6': e = 5; break;
      }

      change_tile_ext (&p, e);

      break;
    default:
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      ui_msg (0, "NO EXTENSION");
      if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;
      break;
    }
    break;
  case EDIT_NUMERICAL_EXT:
    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    i = ext (&p);
    switch (bmenu_int (&i, NULL, INT_MIN, INT_MAX, "CE#>EXT #", NULL)) {
    case -1: edit = EDIT_TILE; break;
    case 0: break;
    case 1: edit = EDIT_TILE; break;
    default:
      editor_register = EDITOR_CYCLES_NONE;

      change_tile_ext (&p, i);

      editor_register = EDITOR_CYCLES_3;
      break;
    }
    break;
  case EDIT_NOMINAL_INFO:
    if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;

    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);

    free_ext_str = false;

    f = fg (&p);
    fg_str = tile_fg_str[f];
    bg_str = tile_bg_str[bg (&p)];
    fake_str = is_fake (&p) ? tile_fg_str[fake (&p)] : NULL;

    e = ext (&p);

    switch (f) {
    case FLOOR: case BROKEN_FLOOR: case SKELETON_FLOOR:
    case STUCK_FLOOR: case HIDDEN_FLOOR: case PILLAR:
    case BIG_PILLAR_BOTTOM: case ARCH_BOTTOM:
      ext_str = tile_item_str[e];
      break;
    case LOOSE_FLOOR:
      ext_str = e ? "CAN'T FALL" : "FALL";
      break;
    case SPIKES_FLOOR: case DOOR: case LEVEL_DOOR: case CHOMPER:
      ext_str = xasprintf ("%i", e);
      free_ext_str = true;
      break;
    case OPENER_FLOOR: case CLOSER_FLOOR:
      ext_str = xasprintf ("%i", e);
      free_ext_str = true;
      break;
    case CARPET:
    case TCARPET:
      switch (e) {
      case 0: ext_str = "DESIGN 1"; break;
      case 1: ext_str = "DESIGN 2"; break;
      case 2: ext_str = "DESIGN 3"; break;
      case 3: ext_str = "DESIGN 4"; break;
      case 4: ext_str = "DESIGN 5"; break;
      case 5: ext_str = "DESIGN 6"; break;
      default: ext_str = "UNKNOWN EXTENSION"; break;
      }
      break;
    default: ext_str = "NO EXTENSION"; break;
    }

    if (is_fake (&p))
      ui_msg (0, "%s/%s/%s/%s", fg_str, bg_str, ext_str, fake_str);
    else ui_msg (0, "%s/%s/%s", fg_str, bg_str, ext_str);
    if (free_ext_str) al_free (ext_str);
    break;
  case EDIT_NUMERICAL_INFO:
    if (was_bmenu_return_pressed (true)) edit = EDIT_TILE;

    if (! is_valid_pos (&p)) {
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      break;
    }
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);

    npos (&p, &p0);
    ui_msg (0, "[%i,%i,%i,%i](%i,%i,%i,%i)",
            global_level.n, p0.room, p0.floor, p0.place,
            fg (&p), bg (&p), ext (&p), is_fake (&p) ? fake (&p) : -1);
    break;
  case EDIT_EVENT:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (event_menu, "E>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'T':
      edit = EDIT_EVENT2TILE;
      get_mouse_coord (&global_mr, &last_mouse_coord);
      s = last_event;
      break;
    case 'F':
      edit = EDIT_EVENT2FLOOR;
      get_mouse_coord (&global_mr, &last_mouse_coord);
      invalid_pos (&last_event2floor_pos);
      t = last_event;
      next_pos_by_pred (&last_event2floor_pos, 0, is_event_at_pos, &t);
      break;
    case 'R':
      edit = EDIT_TILE2EVENT;
      invalid_pos (&p0);
      break;
    case 'S':
      edit = EDIT_EVENT_SET;
      s = last_event;
      bb = event (&global_level, last_event)->next;
      break;
    }
    break;
  case EDIT_EVENT2TILE:
    if (! is_valid_pos (&event (&global_level, s)->p)) {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      al_set_mouse_xy (display, 0, 0);
    } else {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      set_mouse_pos (&global_mr, &event (&global_level, s)->p);
    }
    bb = event (&global_level, s)->next;
    switch (bmenu_int (&s, &bb, 0,
                       global_level.event_nmemb - 1, "ED>EVENT", "N")) {
    case -1: set_mouse_coord (&global_mr, &last_mouse_coord);
      edit = EDIT_EVENT; break;
    case 0: break;
    case 1:
      edit = EDIT_EVENT;
      last_event = s;
      break;
    }
    break;
  case EDIT_EVENT2FLOOR:
    if (! is_valid_pos (&last_event2floor_pos)) {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      al_set_mouse_xy (display, 0, 0);
    } else {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
      set_mouse_pos (&global_mr, &last_event2floor_pos);
    }
    switch (bmenu_list (&s, &r, t, 0,
                        global_level.event_nmemb - 1, "EF>EVENT")) {
    case -1: set_mouse_coord (&global_mr, &last_mouse_coord);
      edit = EDIT_EVENT; break;
    case 0: break;
    case 1:
      edit = EDIT_EVENT;
      last_event = t;
      break;
    default:
      if (s) {
        if (t + s >= 0 && t + s < global_level.event_nmemb) {
          t += s;
          invalid_pos (&last_event2floor_pos);
        } else s = 0;
      }
      if (s || r)
        next_pos_by_pred (&last_event2floor_pos, r, is_event_at_pos, &t);
      break;
    }
    break;
  case EDIT_TILE2EVENT:
    if (! is_valid_pos (&p)) {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      if (was_bmenu_return_pressed (true)) edit = EDIT_EVENT;
      t = -1;
    } else {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      if (! peq (&p, &p0)) {
        t = -1;
        next_int_by_pred (&t, 0, 0,
                          global_level.event_nmemb - 1, is_pos_at_event, &p);
        p0 = p;
      }
      switch (bmenu_list (NULL, &r, t, 0,
                          global_level.event_nmemb - 1, "ER>EVENT")) {
      case -1: edit = EDIT_EVENT; break;
      case 0: break;
      case 1: last_event = t; edit = EDIT_EVENT; break;
      default:
        if (r) next_int_by_pred
                 (&t, r, 0, global_level.event_nmemb - 1, is_pos_at_event, &p);
        break;
      }
    }
    break;
  case EDIT_EVENT_SET:
    if (! is_valid_pos (&p)) {
      al_set_system_mouse_cursor
        (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
      editor_msg ("SELECT TILE", EDITOR_CYCLES_0);
      if (was_bmenu_return_pressed (true)) edit = EDIT_EVENT;
    } else {
      al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
      switch (bmenu_int (&s, &bb, 0, global_level.event_nmemb - 1,
                         "ES>EVENT", "N")) {
      case -1: edit = EDIT_EVENT; break;
      case 0: break;
      case 1:
        register_event_undo (&undo, s, &p, bb ? true : false, "EVENT");
        last_event = s;
        edit = EDIT_EVENT;
        break;
      }
    }
    break;
  case EDIT_ROOM:
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    mr_focus_mouse (&global_mr);
    switch (bmenu_enum (room_menu, "R>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'J':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      edit = EDIT_JUMP_ROOM;
      break;
    case 'L': edit = EDIT_LINK; break;
    case 'S': edit = EDIT_LINKING_SETTINGS;
      b0 = reciprocal_links;
      b1 = locally_unique_links;
      b2 = globally_unique_links;
      break;
    case 'X':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      edit = EDIT_ROOM_EXCHANGE; break;
    case 'A':
      apply_to_room (&global_level, global_mr.room, clear_tile, NULL,
                     "CLEAR ROOM");
      break;
    case 'R':
      apply_to_room (&global_level, global_mr.room, random_tile, NULL,
                     "RANDOMIZE ROOM");
      break;
    case 'D':
      apply_to_room (&global_level, global_mr.room, decorate_tile, NULL,
                     "DECORATE ROOM");
      break;
    case 'M': edit = EDIT_ROOM_MIRROR; break;
    case 'C':
      copy_room (&room_copy, &global_level, global_mr.room);
      editor_msg ("COPY ROOM", EDITOR_CYCLES_3);
      break;
    case 'P':
      paste_room (&global_level, global_mr.room, &room_copy, "PASTE ROOM");
      break;
    case '!':
      apply_to_room (&global_level, global_mr.room, fix_tile, NULL, "FIX ROOM");
      break;
    }
    break;
  case EDIT_ROOM_MIRROR:
    mr_focus_mouse (&global_mr);
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (mirror_menu, "RM>")) {
    case -1: case 1: edit = EDIT_ROOM; break;
    case 'T': edit = EDIT_ROOM_MIRROR_TILES; break;
    case 'L': edit = EDIT_ROOM_MIRROR_LINKS; break;
    case 'B': edit = EDIT_ROOM_MIRROR_BOTH; break;
    }
    break;
  case EDIT_ROOM_MIRROR_TILES:
    mr_focus_mouse (&global_mr);
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (mirror_dir_menu, "RMC>")) {
    case -1: case 1: edit = EDIT_ROOM_MIRROR; break;
    case 'H':
      register_h_room_mirror_tile_undo
        (&undo, global_mr.room, "ROOM MIRROR TILES H.");
      break;
    case 'V':
      register_v_room_mirror_tile_undo
        (&undo, global_mr.room, "ROOM MIRROR TILES V.");
      break;
    case 'B':
      register_h_room_mirror_tile_undo
        (&undo, global_mr.room, NULL);
      register_v_room_mirror_tile_undo
        (&undo, global_mr.room, "ROOM MIRROR TILES H+V.");
      break;
    case 'R':
      register_random_room_mirror_tile_undo
        (&undo, global_mr.room, false, "ROOM MIRROR TILES R.");
      break;
    }
    break;
  case EDIT_ROOM_MIRROR_LINKS:
    mr_focus_mouse (&global_mr);
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (mirror_dir_menu, "RML>")) {
    case -1: case 1: edit = EDIT_ROOM_MIRROR; break;
    case 'H':
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, LEFT, RIGHT);
      register_link_undo (&undo, l, "ROOM MIRROR LINKS H.");
      destroy_array ((void **) &l, NULL);
      break;
    case 'V':
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, ABOVE, BELOW);
      register_link_undo (&undo, l, "ROOM MIRROR LINKS V.");
      destroy_array ((void **) &l, NULL);
      break;
    case 'B':
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, LEFT, RIGHT);
      editor_mirror_link (global_mr.room, ABOVE, BELOW);
      register_link_undo (&undo, l, "ROOM MIRROR LINKS H+V.");
      destroy_array ((void **) &l, NULL);
      break;
    case 'R':
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, random_dir (), random_dir ());
      register_link_undo (&undo, l, "ROOM MIRROR LINKS R.");
      destroy_array ((void **) &l, NULL);
      break;
    }
    break;
  case EDIT_ROOM_MIRROR_BOTH:
    mr_focus_mouse (&global_mr);
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (mirror_dir_menu, "RMB>")) {
    case -1: case 1: edit = EDIT_ROOM_MIRROR; break;
    case 'H':
      register_h_room_mirror_tile_undo (&undo, global_mr.room, NULL);
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, LEFT, RIGHT);
      register_link_undo (&undo, l, "ROOM MIRROR TILES+LINKS H.");
      destroy_array ((void **) &l, NULL);
      break;
    case 'V':
      register_v_room_mirror_tile_undo
        (&undo, global_mr.room, NULL);
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, ABOVE, BELOW);
      register_link_undo (&undo, l, "ROOM MIRROR TILES+LINKS V.");
      destroy_array ((void **) &l, NULL);
      break;
    case 'B':
      register_h_room_mirror_tile_undo
        (&undo, global_mr.room, NULL);
      register_v_room_mirror_tile_undo
        (&undo, global_mr.room, NULL);
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, LEFT, RIGHT);
      editor_mirror_link (global_mr.room, ABOVE, BELOW);
      register_link_undo (&undo, l, "ROOM MIRROR TILES+LINKS H+V.");
      destroy_array ((void **) &l, NULL);
      break;
    case 'R':
      register_random_room_mirror_tile_undo
        (&undo, global_mr.room, false, NULL);
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));
      editor_mirror_link (global_mr.room, random_dir (), random_dir ());
      register_link_undo (&undo, l, "ROOM MIRROR TILES+LINKS R.");
      destroy_array ((void **) &l, NULL);
      break;
    }
    break;
  case EDIT_JUMP_ROOM:
    bmenu_select_room (EDIT_ROOM, "RJ>ROOM");
    break;
  case EDIT_LINK:
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    mr_focus_mouse (&global_mr);
    switch (bmenu_enum (link_menu, "RL>")) {
    case -1: case 1: edit = EDIT_ROOM; break;
    case 'L':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      set_mouse_room (&global_mr,
                      roomd (&global_level, global_mr.room, LEFT));
      edit = EDIT_LINK_LEFT; break;
    case 'R':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      set_mouse_room (&global_mr,
                      roomd (&global_level, global_mr.room, RIGHT));
      edit = EDIT_LINK_RIGHT; break;
    case 'A':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      set_mouse_room (&global_mr,
                      roomd (&global_level, global_mr.room, ABOVE));
      edit = EDIT_LINK_ABOVE; break;
    case 'B':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      set_mouse_room (&global_mr,
                      roomd (&global_level, global_mr.room, BELOW));
      edit = EDIT_LINK_BELOW; break;
    }
    break;
  case EDIT_LINK_LEFT:
    bmenu_link (LEFT);
    break;
  case EDIT_LINK_RIGHT:
    bmenu_link (RIGHT);
    break;
  case EDIT_LINK_ABOVE:
    bmenu_link (ABOVE);
    break;
  case EDIT_LINK_BELOW:
    bmenu_link (BELOW);
    break;
  case EDIT_ROOM_EXCHANGE:
    r = bmenu_select_room (EDIT_ROOM, "RX>ROOM");

    global_mr.room_select = last_mouse_coord.c.room;

    if (r == 1) {
      l = copy_array (global_level.link, global_level.room_nmemb,
                      NULL, sizeof (*l));

      int room0 = last_mouse_coord.c.room;
      int room1 = global_mr.room;

      exchange_rooms (&global_level, room0, room1);

      register_link_undo (&undo, l, "ROOM EXCHANGE");
      last_mouse_coord.c.room = room1;
      last_mouse_coord.o.room = room1;
      set_mouse_coord (&global_mr, &last_mouse_coord);
      global_mr.room_select = -1;
      destroy_array ((void **) &l, NULL);
    } else if (r == -1) global_mr.room_select = -1;
    break;
  case EDIT_LINKING_SETTINGS:
    mr_focus_mouse (&global_mr);
    global_mr.select_cycles = SELECT_CYCLES;
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    switch (bmenu_bool (linking_settings_menu, "RS>", false, &b0,
                       &b1, &b2)) {
    case -1: edit = EDIT_ROOM; break;
    case 0: break;
    case 1:
      reciprocal_links = b0;
      locally_unique_links = b1;
      globally_unique_links = b2;
      edit = EDIT_ROOM;
      break;
    }
    break;
  case EDIT_KID:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    switch (bmenu_enum (kid_menu, "K>")) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'P':
      ui_place_kid (get_actor_by_id (current_kid_id), &p);
      break;
    case 'J':
      set_mouse_pos (&global_mr, &global_level.start_pos);
      break;
    case 'S':
      if (! is_valid_pos (&p)) {
        editor_msg ("SELECT TILE", EDITOR_CYCLES_1);
        break;
      }
      register_start_pos_undo (&undo, &p, "START POSITION");
      break;
    case 'D':
      if (! is_pos_visible (&global_mr, &global_level.start_pos)) {
        editor_msg ("START POS NOT VISIBLE", EDITOR_CYCLES_1);
        break;
      }
      register_toggle_start_dir_undo (&undo, "START DIRECTION");
      break;
    case 'W':
      if (! is_pos_visible (&global_mr, &global_level.start_pos)) {
        editor_msg ("START POS NOT VISIBLE", EDITOR_CYCLES_1);
        break;
      }
      register_toggle_has_sword_undo (&undo, "HAS SWORD");
      break;
    }
    break;
  case EDIT_LEVEL:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    str = xasprintf ("L%i>", global_level.n);
    switch (bmenu_enum (level_menu, str)) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'X':
      if (level_module != NATIVE_LEVEL_MODULE)
        editor_msg ("NATIVE LEVEL MODULE ONLY", EDITOR_CYCLES_2);
      else {
        edit = EDIT_LEVEL_EXCHANGE;
        next_level_number = global_level.n;
      }
      break;
    case 'J': edit = EDIT_LEVEL_JUMP;
      next_level_number = global_level.n;
      break;
    case 'A':
      apply_to_level (&global_level, clear_tile, NULL, "CLEAR LEVEL");
      break;
    case 'R':
      apply_to_level (&global_level, random_tile, NULL, "RANDOMIZE LEVEL");
      break;
    case 'D':
      apply_to_level (&global_level, decorate_tile, NULL, "DECORATE LEVEL");
      break;
    case 'M': edit = EDIT_LEVEL_MIRROR; break;
    case 'C':
      copy_level (&level_copy, &global_level);
      editor_msg ("COPY LEVEL", EDITOR_CYCLES_3);
      break;
    case 'P':
      register_level_undo (&undo, &level_copy, "PASTE LEVEL");
      break;
    case 'N': edit = EDIT_NOMINAL_NUMBER;
      s = global_level.nominal_n;
      break;
    case 'E': edit = EDIT_ENVIRONMENT;
      bb = em;
      b0 = (global_level.em == DUNGEON) ? true : false;
      b1 = (global_level.em == PALACE) ? true : false;
      break;
    case 'H': edit = EDIT_HUE;
      bb = hue;
      b0 = (global_level.hue == HUE_NONE) ? true : false;
      b1 = (global_level.hue == HUE_GREEN) ? true : false;
      b2 = (global_level.hue == HUE_GRAY) ? true : false;
      b3 = (global_level.hue == HUE_YELLOW) ? true : false;
      b4 = (global_level.hue == HUE_BLUE) ? true : false;
      break;
    case 'S':
      if (level_module != NATIVE_LEVEL_MODULE)
        editor_msg ("NATIVE LEVEL MODULE ONLY", EDITOR_CYCLES_2);
      else if (save_level (&global_level)) {
        copy_level (&vanilla_level, &global_level);
        editor_msg ("LEVEL SAVED", EDITOR_CYCLES_2);
      } else editor_msg ("LEVEL SAVING FAILED", EDITOR_CYCLES_2);
      break;
    case 'L':
      while (undo_pass (&undo, -1, NULL));
      editor_msg ("LEVEL RELOADED", EDITOR_CYCLES_2);
      break;
    case '!':
      apply_to_level (&global_level, fix_tile, NULL, "FIX LEVEL");
      break;
    }
    al_free (str);
   break;
  case EDIT_LEVEL_JUMP:
    str = xasprintf ("L%iJ>LEVEL", global_level.n);
    if (bmenu_select_level (EDIT_LEVEL, str) == 1
        && next_level_number != global_level.n) {
      ignore_level_cutscene = true;
      quit_anim = NEXT_LEVEL;
    }
    al_free (str);
    break;
  case EDIT_LEVEL_EXCHANGE:
    str = xasprintf ("L%iX>LEVEL", global_level.n);
    if (bmenu_select_level (EDIT_LEVEL, str) == 1
        && next_level_number != global_level.n)
      register_level_exchange_undo (&undo, next_level_number,
                                    "LEVEL EXCHANGE");
    al_free (str);
    break;
  case EDIT_LEVEL_MIRROR:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    str = xasprintf ("L%iM>", global_level.n);
    switch (bmenu_enum (mirror_menu, str)) {
    case -1: case 1: edit = EDIT_LEVEL; break;
    case 'T': edit = EDIT_LEVEL_MIRROR_TILES; break;
    case 'L': edit = EDIT_LEVEL_MIRROR_LINKS; break;
    case 'B': edit = EDIT_LEVEL_MIRROR_BOTH; break;
    }
    al_free (str);
    break;
  case EDIT_LEVEL_MIRROR_TILES:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    str = xasprintf ("L%iMC>", global_level.n);
    switch (bmenu_enum (mirror_dir_menu, str)) {
    case -1: case 1: edit = EDIT_LEVEL_MIRROR; break;
    case 'H':
      for (i = 1; i < global_level.room_nmemb; i++)
        register_h_room_mirror_tile_undo (&undo, i, NULL);
      end_undo_set (&undo, "LEVEL MIRROR TILES H.");
      break;
    case 'V':
      for (i = 1; i < global_level.room_nmemb; i++)
        register_v_room_mirror_tile_undo (&undo, i, NULL);
      end_undo_set (&undo, "LEVEL MIRROR TILES V.");
      break;
    case 'B':
      for (i = 1; i < global_level.room_nmemb; i++) {
        register_h_room_mirror_tile_undo (&undo, i, NULL);
        register_v_room_mirror_tile_undo (&undo, i, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR TILES H+V.");
      break;
    case 'R':
      for (i = 1; i < global_level.room_nmemb; i++)
        register_random_room_mirror_tile_undo
          (&undo, i, false, NULL);
      end_undo_set (&undo, "LEVEL MIRROR TILES R.");
      break;
    }
    al_free (str);
    break;
  case EDIT_LEVEL_MIRROR_LINKS:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    str = xasprintf ("L%iML>", global_level.n);
    switch (bmenu_enum (mirror_dir_menu, str)) {
    case -1: case 1: edit = EDIT_LEVEL_MIRROR; break;
    case 'H':
      for (i = 1; i < global_level.room_nmemb; i++) {
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, LEFT, RIGHT);
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR LINKS H.");
      break;
    case 'V':
      for (i = 1; i < global_level.room_nmemb; i++) {
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, ABOVE, BELOW);
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR LINKS V.");
      break;
    case 'B':
      for (i = 1; i < global_level.room_nmemb; i++) {
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, LEFT, RIGHT);
        mirror_link (&global_level, i, ABOVE, BELOW);
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR LINKS H+V.");
      break;
    case 'R':
      for (i = 1; i < global_level.room_nmemb; i++) {
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, random_dir (), random_dir ());
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR LINKS R.");
      break;
    }
    al_free (str);
    break;
  case EDIT_LEVEL_MIRROR_BOTH:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    str = xasprintf ("L%iMB>", global_level.n);
    switch (bmenu_enum (mirror_dir_menu, str)) {
    case -1: case 1: edit = EDIT_LEVEL_MIRROR; break;
    case 'H':
      for (i = 1; i < global_level.room_nmemb; i++) {
        register_h_room_mirror_tile_undo (&undo, i, NULL);
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, LEFT, RIGHT);
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS H.");
      break;
    case 'V':
      for (i = 1; i < global_level.room_nmemb; i++) {
        register_v_room_mirror_tile_undo (&undo, i, NULL);
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, ABOVE, BELOW);
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS V.");
      break;
    case 'B':
      for (i = 1; i < global_level.room_nmemb; i++) {
        register_h_room_mirror_tile_undo (&undo, i, NULL);
        register_v_room_mirror_tile_undo (&undo, i, NULL);
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, LEFT, RIGHT);
        mirror_link (&global_level, i, ABOVE, BELOW);
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS H+V.");
      break;
    case 'R':
      for (i = 1; i < global_level.room_nmemb; i++) {
        register_random_room_mirror_tile_undo
          (&undo, i, false, NULL);
        l = copy_array (global_level.link, global_level.room_nmemb,
                        NULL, sizeof (*l));
        mirror_link (&global_level, i, random_dir (), random_dir ());
        register_link_undo (&undo, l, NULL);
        destroy_array ((void **) &l, NULL);
      }
      end_undo_set (&undo, "LEVEL MIRROR TILES+LINKS R.");
      break;
    }
    al_free (str);
    break;
  case EDIT_NOMINAL_NUMBER:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    str = xasprintf ("L%iN>N.NUMBER", global_level.n);
    switch (bmenu_int (&global_level.nominal_n, NULL, 0, INT_MAX, str, NULL)) {
    case -1: edit = EDIT_LEVEL; global_level.nominal_n = s; break;
    case 0: break;
    case 1:
      edit = EDIT_LEVEL;
      register_int_undo (&undo, &global_level.nominal_n, s, (undo_f) int_undo,
                         "LEVEL NOMINAL NUMBER");
      break;
    default: break;
    }
    al_free (str);
    break;
  case EDIT_ENVIRONMENT:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    str = xasprintf ("L%iE>", global_level.n);
    b0 = b1 = false;
    if (global_level.em == DUNGEON) b0 = true;
    if (global_level.em == PALACE) b1 = true;
    em = global_level.em;
    switch (bmenu_bool (environment_menu, str, true, &b0, &b1)) {
    case -1: edit = EDIT_LEVEL; global_level.em = bb; em = bb; break;
    case 0: break;
    case 1:
      edit = EDIT_LEVEL;
      register_int_undo (&undo, (int *) &global_level.em, bb,
                         (undo_f) level_environment_undo,
                         "LEVEL ENVIRONMENT");
      break;
    default:
      if (b0) global_level.em = DUNGEON;
      if (b1) global_level.em = PALACE;
      em = global_level.em;
      break;
    }
    al_free (str);
    break;
  case EDIT_HUE:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    str = xasprintf ("L%iH>", global_level.n);
    b0 = b1 = b2 = b3 = b4 = 0;
    if (global_level.hue == HUE_NONE) b0 = true;
    if (global_level.hue == HUE_GREEN) b1 = true;
    if (global_level.hue == HUE_GRAY) b2 = true;
    if (global_level.hue == HUE_YELLOW) b3 = true;
    if (global_level.hue == HUE_BLUE) b4 = true;
    hue = global_level.hue;
    switch (bmenu_bool (hue_menu, str, true, &b0, &b1, &b2, &b3, &b4)) {
    case -1: edit = EDIT_LEVEL; global_level.hue = bb; hue = bb; break;
    case 0: break;
    case 1:
      edit = EDIT_LEVEL;
      register_int_undo (&undo, (int *) &global_level.hue, bb, (undo_f) level_hue_undo,
                         "LEVEL HUE");
      break;
    default:
      if (b0) global_level.hue = HUE_NONE;
      if (b1) global_level.hue = HUE_GREEN;
      if (b2) global_level.hue = HUE_GRAY;
      if (b3) global_level.hue = HUE_YELLOW;
      if (b4) global_level.hue = HUE_BLUE;
      hue = global_level.hue;
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD:
    g = guard (&global_level, guard_index);
    al_set_system_mouse_cursor (display, is_guard_by_type (g->type)
                             ? ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT
                             : ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
    str = xasprintf ("G%i>", guard_index);
    switch (bmenu_enum (guard_menu, str)) {
    case -1: case 1: edit = EDIT_MAIN; break;
    case 'G': edit = EDIT_GUARD_SELECT;
      s = guard_index; get_mouse_coord (&global_mr, &last_mouse_coord); break;
    case 'P':
      ui_place_guard (get_guard_actor_by_level_id (guard_index), &p);
      break;
    case 'S':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", EDITOR_CYCLES_1);
        break;
      }
      if (! is_valid_pos (&p)) {
        editor_msg ("SELECT TILE", EDITOR_CYCLES_1);
        break;
      }
      register_guard_start_pos_undo (&undo, guard_index, &p,
                                     "GUARD START POSITION");
      break;
    case 'J':
      get_mouse_coord (&global_mr, &last_mouse_coord);
      mouse2guard (guard_index); break;
    case 'D':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", EDITOR_CYCLES_1);
        break;
      }
      if (! is_pos_visible (&global_mr, &g->p)) {
        editor_msg ("START POS NOT VISIBLE", EDITOR_CYCLES_1);
        break;
      }
      register_toggle_guard_start_dir_undo
        (&undo, guard_index, "GUARD START DIRECTION");
      break;
    case 'K':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", EDITOR_CYCLES_1);
        break;
      }
      edit = EDIT_GUARD_SKILL; break;
    case 'H':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", EDITOR_CYCLES_1);
        break;
      }
      edit = EDIT_GUARD_HP;
      s = g->total_hp; break;
    case 'T': edit = EDIT_GUARD_TYPE;
      bb = g->type;
      b0 = (! is_guard_by_type (g->type)) ? true : false;
      b1 = (g->type == GUARD) ? true : false;
      b2 = (g->type == FAT) ? true : false;
      b3 = (g->type == VIZIER) ? true : false;
      b4 = (g->type == SKELETON) ? true : false;
      b5 = (g->type == SHADOW) ? true : false;
      break;
    case 'Y':
      if (! is_guard_by_type (g->type)) {
        editor_msg ("DISABLED GUARD", EDITOR_CYCLES_1);
        break;
      }
      edit = EDIT_GUARD_STYLE;
      bb = g->style; break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SELECT:
    mouse2guard (s);
    str = xasprintf ("G%iG>GUARD", guard_index);
    switch (bmenu_int (&s, NULL, 0, global_level.guard_nmemb - 1, str, NULL)) {
    case -1: edit = EDIT_GUARD;
      set_mouse_coord (&global_mr, &last_mouse_coord); break;
    case 0: break;
    case 1:
      guard_index = s;
      edit = EDIT_GUARD;
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SKILL:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
    g = guard (&global_level, guard_index);
    str = xasprintf ("G%iK>", guard_index);
    c = bmenu_enum (skill_menu, str);
    if (! c) break;

    if (c == -1 || c == 1) {
      edit = EDIT_GUARD; break;
    }

    memcpy (&skill_buf, &g->skill, sizeof (skill_buf));

    switch (c) {
    case 'A': edit = EDIT_GUARD_SKILL_ATTACK;
      s = g->skill.attack_prob + 1; break;
    case 'B': edit = EDIT_GUARD_SKILL_COUNTER_ATTACK;
      s = g->skill.counter_attack_prob + 1; break;
    case 'D': edit = EDIT_GUARD_SKILL_DEFENSE;
      s = g->skill.defense_prob + 1; break;
    case 'E': edit = EDIT_GUARD_SKILL_COUNTER_DEFENSE;
      s = g->skill.counter_defense_prob + 1; break;
    case 'V': edit = EDIT_GUARD_SKILL_ADVANCE;
      s = g->skill.advance_prob + 1; break;
    case 'R': edit = EDIT_GUARD_SKILL_RETURN;
      s = g->skill.return_prob + 1; break;
    case 'F': edit = EDIT_GUARD_SKILL_REFRACTION;
      s = g->skill.refraction; break;
    case 'X': edit = EDIT_GUARD_SKILL_EXTRA_HP;
      s = g->skill.extra_hp; break;
    case 'L': edit = EDIT_SKILL_LEGACY_TEMPLATES;
      s = 0;
      break;
    }
    al_free (str);
    break;
  case EDIT_SKILL_LEGACY_TEMPLATES:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    g = guard (&global_level, guard_index);
    str = xasprintf ("G%iKL>L.SKILL", guard_index);
    c = bmenu_int (&s, NULL, 0, 11, str, NULL);
    if (! c) break;
    switch (c) {
    case -1:
      g->skill = skill_buf;
      edit = EDIT_GUARD_SKILL;
      break;
    case 1: edit = EDIT_GUARD_SKILL;
      register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                 "GUARD LEGACY SKILL");
      break;
    default:
      get_legacy_skill (s, &g->skill);
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_SKILL_ATTACK:
    c = bmenu_skill ("KA>ATTACK", &g->skill.attack_prob, 100,
                    EDIT_GUARD_SKILL);
    if (c == 1)
      register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                 "GUARD ATTACK SKILL");
    break;
  case EDIT_GUARD_SKILL_COUNTER_ATTACK:
    c = bmenu_skill ("KB>C.ATTACK", &g->skill.counter_attack_prob, 100,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD COUNTER ATTACK SKILL");
    break;
  case EDIT_GUARD_SKILL_DEFENSE:
    c = bmenu_skill ("KD>DEFENSE", &g->skill.defense_prob, 100,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD DEFENSE SKILL");
    break;
  case EDIT_GUARD_SKILL_COUNTER_DEFENSE:
    c = bmenu_skill ("KE>C.DEFENSE", &g->skill.counter_defense_prob, 100,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD COUNTER DEFENSE SKILL");
    break;
  case EDIT_GUARD_SKILL_ADVANCE:
    c = bmenu_skill ("KV>ADVANCE", &g->skill.advance_prob, 100,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD ADVANCE SKILL");
    break;
  case EDIT_GUARD_SKILL_RETURN:
    c = bmenu_skill ("KR>RETURN", &g->skill.return_prob, 100,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD RETURN SKILL");
    break;
  case EDIT_GUARD_SKILL_REFRACTION:
    c = bmenu_skill ("KF>REFRACTION", &g->skill.refraction, INT_MAX,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                             "GUARD REFRACTION SKILL");
    break;
  case EDIT_GUARD_SKILL_EXTRA_HP:
    c = bmenu_skill ("KX>EXT.HP", &g->skill.extra_hp, INT_MAX,
                    EDIT_GUARD_SKILL);
    if (c == 1) register_guard_skill_undo (&undo, guard_index, &skill_buf,
                                           "GUARD EXTRA HP SKILL");
    break;
  case EDIT_GUARD_HP:
    c = bmenu_skill ("L>HP", &g->total_hp, INT_MAX, EDIT_GUARD);
    if (c == 1) register_guard_hp_undo (&undo, guard_index, s,
                                           "GUARD TOTAL HP");
    break;
  case EDIT_GUARD_TYPE:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    g = guard (&global_level, guard_index);
    if (! is_guard_by_type (g->type)
        && g->p.room == 0 && g->p.floor == 0 && g->p.place == 0)
      invalid_pos (&g->p);
    str = xasprintf ("G%iT>", guard_index);
    b0 = b1 = b2 = b3 = b4 = b5 = 0;
    if (g->type == NO_ACTOR) b0 = true;
    if (g->type == GUARD) b1 = true;
    if (g->type == FAT) b2 = true;
    if (g->type == VIZIER) b3 = true;
    if (g->type == SKELETON) b4 = true;
    if (g->type == SHADOW) b5 = true;
    switch (bmenu_bool (guard_type_menu, str, true, &b0, &b1, &b2, &b3, &b4, &b5)) {
    case -1: edit = EDIT_GUARD; g->type = bb; break;
    case 0: break;
    case 1:
      edit = EDIT_GUARD;
      if (is_guard_by_type (g->type) && ! g->total_hp)
        g->total_hp = 3;
      if (is_guard_by_type (g->type) && ! g->skill.attack_prob)
        get_legacy_skill (0, &g->skill);
      register_guard_type_undo (&undo, guard_index, bb,
                                "GUARD TYPE");
      break;
    default:
      if (b0) g->type = NO_ACTOR;
      if (b1) g->type = GUARD;
      if (b2) g->type = FAT;
      if (b3) g->type = VIZIER;
      if (b4) g->type = SKELETON;
      if (b5) g->type = SHADOW;
      break;
    }
    al_free (str);
    break;
  case EDIT_GUARD_STYLE:
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
    g = guard (&global_level, guard_index);
    str = xasprintf ("G%iY>STYLE", guard_index);
    switch (bmenu_int (&g->style, NULL, 0, 7, str, NULL)) {
    case -1: edit = EDIT_GUARD; g->style = bb; break;
    case 0: break;
    case 1:
      edit = EDIT_GUARD;
      register_guard_style_undo (&undo, guard_index, bb,
                                 "GUARD STYLE");
      break;
    }
    al_free (str);
    break;
  }

  editor_register = EDITOR_CYCLES_NONE;
}

int
enter_editor (Ihandle *ih)
{
  edit = last_edit;
  IupShow (gui_editor_dialog);
  return IUP_DEFAULT;
}

int
exit_editor (Ihandle *ih)
{
  IupHide (gui_editor_dialog);
  if (edit != EDIT_NONE) last_edit = edit;
  edit = EDIT_NONE;
  msg = NULL;
  msg_cycles = 0;
  reset_menu ();
  if (! is_dedicatedly_replaying ())
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_DEFAULT);
  if (is_game_paused ()) print_game_paused (0);
  else ui_msg_clear (0);
  global_mr.room_select = -1;
  return IUP_DEFAULT;
}

static char
bmenu_int_ext (struct pos *p, int steps, int fases,
              char *prefix, char *b_str)
{
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  r = typed_int (ext (p), steps, fases, NULL, &bb);
  char c = bmenu_int (&r, &bb, 0, steps * fases - 1, prefix, b_str);
  if (! c) return c;

  if (c == -1 || c == 1) {
    edit = EDIT_TILE; return c;
  }

  if (c == ' ') r += steps;

  editor_register = EDITOR_CYCLES_NONE;

  change_tile_ext (p, r);

  editor_register = EDITOR_CYCLES_3;

  return c;
}

static char
bmenu_select_room (enum edit up_edit, char *prefix)
{
  mr_focus_mouse (&global_mr);
  int room = global_mr.room;
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char r = bmenu_int (&room, NULL, 0, global_level.room_nmemb - 1,
                      prefix, NULL);
  switch (r) {
  case -1: edit = up_edit;
    set_mouse_coord (&global_mr, &last_mouse_coord);
    break;
  case 0: break;
  case 1: edit = up_edit; break;
  default: set_mouse_room (&global_mr, room); break;
  }
  return r;
}

static char
bmenu_select_level (enum edit up_edit, char *prefix)
{
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char r = bmenu_int (&next_level_number, NULL, min_legacy_level,
                     max_legacy_level, prefix, NULL);
  switch (r) {
  case -1: edit = up_edit; break;
  case 0: break;
  case 1: edit = up_edit; break;
  default: break;
  }
  return r;
}

static char
bmenu_link (enum dir dir)
{
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char *prefix, c;

  switch (dir) {
  case LEFT: c = 'L'; break;
  case RIGHT: c = 'R'; break;
  case ABOVE: c = 'A'; break;
  case BELOW: c = 'B'; break;
  }

  prefix = xasprintf ("RL%c>ROOM", c);
  char r = bmenu_select_room (EDIT_LINK, prefix);
  al_free (prefix);

  global_mr.room_select = last_mouse_coord.c.room;

  if (r == 1) {
    struct room_linking *l =
      copy_array (global_level.link, global_level.room_nmemb,
                  NULL, sizeof (*l));
    editor_link (last_mouse_coord.c.room, global_mr.room, dir);
    register_link_undo (&undo, l, "LINK");
    set_mouse_coord (&global_mr, &last_mouse_coord);
    global_mr.room_select = -1;
    destroy_array ((void **) &l, NULL);
  } else if (r == -1) global_mr.room_select = -1;

  return r;
}

static void
mouse2guard (int i)
{
  struct guard *g = guard (&global_level, i);
  if (is_guard_by_type (g->type)
      && is_valid_pos (&g->p)) {
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_LINK);
    set_mouse_pos (&global_mr, &g->p);
  }
  else {
    al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_UNAVAILABLE);
    al_set_mouse_xy (display, 0, 0);
  }
}

static char
bmenu_skill (char *prefix, int *skill, int max, enum edit up_edit)
{
  al_set_system_mouse_cursor (display, ALLEGRO_SYSTEM_MOUSE_CURSOR_QUESTION);
  char *str;
  str = xasprintf ("G%i%s", guard_index, prefix);
  int a = (max <= 100) ? *skill + 1 : *skill;
  char c = bmenu_int (&a, NULL, 0, max, str, NULL);
  *skill = (max <= 100) ? a - 1 : a;
  al_free (str);
  switch (c) {
  case -1: edit = up_edit;
    *skill = (max <= 100) ? s - 1 : s; break;
  case 0: break;
  case 1: edit = up_edit; break;
  }
  return c;
}

void
editor_msg (char *m, uint64_t cycles)
{
  if (edit != EDIT_NONE && cycles >= msg_cycles) {
    msg_cycles = cycles;
    msg = m;
    ui_msg (0, "%s", m);
  } else if (edit == EDIT_NONE) ui_msg (0, "%s", m);
}

void
ui_place_kid (struct actor *k, struct pos *p)
{
  if (! is_valid_pos (p)) {
    editor_msg ("SELECT PLACE", EDITOR_CYCLES_1);
    return;
  }
  if (! k) {
    editor_msg ("NO LIVE INSTANCE", EDITOR_CYCLES_1);
    return;
  }
  kid_resurrect (k);

  int dx = k->f.dir == LEFT ? +22 : +28;
  int dy = +15;
  place_actor (k, p, dx, dy, "KID", "NORMAL", 0);

  k->misstep = false;
  k->ignore_danger = 0;
  kid_normal (k);
  if (! is_game_paused ()) update_depressible_floor (k, -4, -10);
}

void
ui_place_guard (struct actor *g, struct pos *p)
{
  if (! is_guard_by_type (g->type)) {
    editor_msg ("DISABLED GUARD", EDITOR_CYCLES_1);
    return;
  }
  if (! is_valid_pos (p)) {
    editor_msg ("SELECT TILE", EDITOR_CYCLES_1);
    return;
  }
  if (! g) {
    editor_msg ("NO LIVE INSTANCE", EDITOR_CYCLES_1);
    return;
  }
  guard_resurrect (g);
  int dx = g->f.dir == LEFT ? +16 : +22;
  int dy = +14;
  place_actor (g, p, dx, dy, NULL, "NORMAL", 0);
  place_on_the_ground (&g->f, &g->f.c);
  guard_normal (g);
  if (! is_game_paused ()) update_depressible_floor (g, -7, -26);
}

void
editor_link (int room0, int room1, enum dir dir)
{
  *roomd_ptr (&global_level, room0, dir) = room1;
  if (reciprocal_links) make_reciprocal_link (&global_level, room0, room1, dir);

  if (locally_unique_links) {
    make_link_locally_unique (&global_level, room0, dir);
    if (reciprocal_links)
      make_link_locally_unique (&global_level, room1, opposite_dir (dir));
  }

  if (globally_unique_links) {
    make_link_globally_unique (&global_level, room0, dir);
    if (reciprocal_links)
      make_link_globally_unique (&global_level, room1, opposite_dir (dir));
  }
}

void
editor_mirror_link (int room, enum dir dir0, enum dir dir1)
{
  int r0 = roomd (&global_level, room, dir0);
  int r1 = roomd (&global_level, room, dir1);
  editor_link (room, r0, dir1);
  editor_link (room, r1, dir0);
}
