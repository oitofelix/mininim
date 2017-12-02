/*
  gui-editor-room-control.c -- GUI editor room control module;

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

#define ACTIVELY_BROKEN_COLOR "255 0 0"
#define PASSIVELY_BROKEN_COLOR "0 128 0"
#define ISOLATED_COLOR "0 0 255"

struct last {
  size_t total_rooms, isolated_rooms, broken_rooms, fragmented_rooms,
    start_room, level_n;
  int first_actively_broken_id, last_actively_broken_id;
  int first_passively_broken_id, last_passively_broken_id;
  int first_isolated_id, last_isolated_id;
};

enum {
  ROOM_DEPTH, DIR_DEPTH, ADJACENT_DEPTH,
};

static int destroy_cb (Ihandle *ih);
static int show_cb (Ihandle *ih, int state);
static int toggle_action_cb (Ihandle *ih, int state);

static int selected_room (Ihandle *tree_ctrl);

static int _update_cb (Ihandle *ih);

/* tree build */
static bool is_broken_link (struct room_linking *rlink, size_t room_nmemb,
                            int room, enum dir d);
static bool is_passively_broken_link (struct room_linking *rlink,
                                      size_t room_nmemb, int room, enum dir d);

static int count_isolated_rooms (struct level *level);
static int count_broken_rooms (struct room_linking *rlink, size_t room_nmemb);
static int count_fragmented_rooms (struct level *level);

static int _update_tree_cb (Ihandle *ih);
static void update_tree_ctrl (Ihandle *ih, struct tree *tree);
void populate_room_tree (struct level *level, struct tree *tree,
                         bool ignore_isolated);

/* auxiliary */
int new_room (Ihandle *ih, int l, int r, int a, int b);

/* callbacks */
static int next_broken_link_id (struct level *level, struct tree *tree,
                                int start_id, int d, bool passive);
static int next_isolated_room_id (struct level *level, struct tree *tree,
                                  int start_id, int d);
static int go_button_cb (Ihandle *button);

static int isolate_button_cb (Ihandle *button);
static int new_button_cb (Ihandle *button);
static int in_button_cb (Ihandle *button);
static int out_button_cb (Ihandle *button);
static int exchange_links_button_cb (Ihandle *button);
static int exchange_tiles_button_cb (Ihandle *button);

static int fix_button_cb (Ihandle *button);
static int defrag_button_cb (Ihandle *button);
static int clean_button_cb (Ihandle *button);

static void select_room_node (Ihandle *ih, int room);

static int selection_cb (Ihandle *ih, int id, int status);

static int _select_origin_cb (Ihandle *ih, struct mouse_coord *mc,
                              struct pos *p);
static int _swap_origin_selection_cb (Ihandle *ih);
static int _add_room_cb (Ihandle *ih, struct pos *p);


Ihandle *
gui_create_editor_room_control (char *norm_group, struct level *level)
{
  Ihandle *ih, *tree_ctrl;

  Ihandle *tree_frame, *isolated_toggle, *refresh_button;

  Ihandle *go_frame, *go_actively_broken_toggle, *go_passively_broken_toggle,
    *go_isolated_toggle, *go_radio, *prev_button, *next_button, *start_button;

  Ihandle *link_frame, *reciprocal_toggle, *locally_unique_toggle,
    *globally_unique_toggle, *isolate_button, *new_button, *in_button,
    *out_button, *exchange_links_button, *exchange_tiles_button;

  Ihandle *sanitation_frame, *fix_button, *defrag_button, *clean_button,
    *total_label, *isolated_label, *broken_label, *start_label,
    *fragmented_label;

  tree_ctrl = IupSetCallbacks
    (IupSetAttributes
     (IupTree (),
      "ADDROOT = NO,"
      "ADDEXPANDED = NO,"),
     "SELECTION_CB", (Icallback) selection_cb,
     NULL);

  isolated_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("Isolated", NULL),
      "VALUE = NO,"
      "TIP = \"Show isolated rooms\","),
     "ACTION", _update_tree_cb,
     NULL);

  refresh_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = RELOAD_ICON,"
      "TIP = \"Refresh\","),
     "ACTION", _update_tree_cb,
     NULL);

  tree_frame = IupSetAttributes
    (IupFrame
     (IupSetAttributes
      (IupVbox
       (IupFill (),
        IupSetAttributes
        (IupHbox
         (IupFill (),
          isolated_toggle,
          IupFill (),
          refresh_button,
          IupFill (),
          NULL),
         "ALIGNMENT = ACENTER,"),
        IupFill (),
        NULL),
       "NORMALIZERGROUP = ROOM_NORM,")),
     "TITLE = Tree,");

  prev_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = PREVIOUS_ICON,"
      "TIP = \"Select previous room\","),
     "ACTION", go_button_cb,
     NULL);

  next_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = NEXT_ICON,"
      "TIP = \"Select next room\","),
     "ACTION", go_button_cb,
     NULL);

  start_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = HOME_ICON,"
      "TIP = \"Select kid's starting room\","),
     "ACTION", go_button_cb,
     NULL);

  go_actively_broken_toggle = IupSetAttributes
    (IupToggle ("A", NULL),
     "FGCOLOR = \"" ACTIVELY_BROKEN_COLOR "\","
     "VALUE = NO,"
     "TIP = \"Go through actively broken links\",");

  go_passively_broken_toggle = IupSetAttributes
    (IupToggle ("P", NULL),
     "FGCOLOR = \"" PASSIVELY_BROKEN_COLOR "\","
     "VALUE = NO,"
     "TIP = \"Go through passively broken links\",");

  go_isolated_toggle = IupSetAttributes
    (IupToggle ("I", NULL),
     "FGCOLOR = \"" ISOLATED_COLOR "\","
     "VALUE = NO,"
     "TIP = \"Go through isolated rooms\",");

  go_radio = IupRadio
    (IupSetAttributes
     (IupHbox
      (IupFill (),
       go_actively_broken_toggle,
       go_passively_broken_toggle,
       go_isolated_toggle,
       IupFill (),
       NULL),
      "ALIGNMENT = ACENTER,"));

  go_frame = IupSetAttributes
    (IupFrame
     (IupSetAttributes
      (IupVbox
       (IupFill (),
        go_radio,
        IupFill (),
        IupSetAttributes
        (IupHbox
         (IupFill (),
          prev_button,
          IupFill (),
          next_button,
          IupFill (),
          start_button,
          IupFill (),
          NULL),
         "ALIGNMENT = ACENTER,"),
        IupFill (),
        NULL),
       "NORMALIZERGROUP = ROOM_NORM,")),
     "TITLE = \"Go\",");

  reciprocal_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("R", NULL),
      "TIP = \"Make links reciprocal\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);
  IupSetInt (reciprocal_toggle, "VALUE", editor_reciprocal_links);

  locally_unique_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("L", NULL),
      "TIP = \"Make links locally unique\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);
  IupSetInt (locally_unique_toggle, "VALUE", editor_locally_unique_links);

  globally_unique_toggle = IupSetCallbacks
    (IupSetAttributes
     (IupToggle ("G", NULL),
      "TIP = \"Make links globally unique\""),
     "ACTION", (Icallback) toggle_action_cb,
     NULL);
  IupSetInt (globally_unique_toggle, "VALUE", editor_globally_unique_links);

  isolate_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = VIEW_RING_DEL_ICON,"
      "TIP = \"Isolate MR origin room\","),
     "ACTION", isolate_button_cb,
     NULL);

  new_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = VIEW_RING_NEW_ICON,"
      "TIP = \"Create new room\","),
     "ACTION", new_button_cb,
     NULL);

  in_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = IN_ICON,"
      "TIP = \"Link selected room inwards at given MR origin room direction\","),
     "ACTION", in_button_cb,
     NULL);

  out_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = OUT_ICON,"
      "TIP = \"Link selected room outwards at given MR origin room direction\","),
     "ACTION", out_button_cb,
     NULL);

  exchange_links_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = EXCHANGE_LINKS_ICON,"
      "TIP = \"Exchange links between selected room and MR origin room\","),
     "ACTION", exchange_links_button_cb,
     NULL);

  exchange_tiles_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = EXCHANGE_TILES_ICON,"
      "TIP = \"Exchange tiles between selected room and MR origin room\","),
     "ACTION", exchange_tiles_button_cb,
     NULL);

  link_frame = IupSetAttributes
    (IupFrame
     (IupSetAttributes
      (IupHbox
       (IupFill (),
        IupSetAttributes
        (IupVbox
         (IupFill (),
          reciprocal_toggle,
          locally_unique_toggle,
          globally_unique_toggle,
          IupFill (),
          NULL),
         "ALIGNMENT = ALEFT,"),
        IupFill (),
        IupSetAttributes
        (IupLabel (NULL),
         "SEPARATOR = VERTICAL,"),
        IupFill (),
        IupSetAttributes
        (IupGridBox
         (isolate_button,
          new_button,
          out_button,
          in_button,
          exchange_links_button,
          exchange_tiles_button,
          NULL),
         "ORIENTATION = HORIZONTAL,"
         "NUMDIV = 2,"
         "SIZECOL = -1,"
         "SIZELIN = -1,"
         "NORMALIZESIZE = BOTH,"),
        IupFill (),
        NULL),
       "ALIGNMENT = ACENTER,"
       "NORMALIZERGROUP = ROOM_NORM,")),
     "TITLE = Link,");

  fix_button =
    IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = FIX_ICON,"
      "TIP = \"Isolate broken links\","),
     "ACTION", fix_button_cb,
     NULL);

  defrag_button =
    IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = DEFRAG_ICON,"
      "TIP = \"Defragment isolated rooms\","),
     "ACTION", defrag_button_cb,
     NULL);

  clean_button =
    IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = CLEAN_ICON,"
      "TIP = \"Delete trailing isolated rooms\","),
     "ACTION", clean_button_cb,
     NULL);

  total_label = IupSetAttributes
    (IupLabel ("000"),
     "ALIGNMENT = ARIGHT");

  broken_label = IupSetAttributes
    (IupLabel ("000"),
     "ALIGNMENT = ARIGHT");

  isolated_label = IupSetAttributes
    (IupLabel ("000"),
     "ALIGNMENT = ARIGHT");

  fragmented_label = IupSetAttributes
    (IupLabel ("000"),
     "ALIGNMENT = ARIGHT");

  start_label = IupSetAttributes
    (IupLabel ("000"),
     "ALIGNMENT = ARIGHT");

  sanitation_frame = IupSetAttributes
    (IupFrame
     (IupSetAttributes
      (IupVbox
       (IupFill (),
        IupSetAttributes
        (IupGridBox
         (IupSetAttributes
          (IupLabel ("Start: "),
           "TIP = \"Kid's starting room\","
           "ALIGNMENT = ALEFT"),
          start_label,
          IupSetAttributes
          (IupLabel ("Total: "),
           "TIP = \"Total number of rooms\","
           "ALIGNMENT = ALEFT"),
          total_label,
          IupSetAttributes
          (IupLabel ("Broken: "),
           "TIP = \"Number of broken rooms\","
           "ALIGNMENT = ALEFT"),
          broken_label,
          IupSetAttributes
          (IupLabel ("Isolated: "),
           "TIP = \"Number of isolated rooms\","
           "ALIGNMENT = ALEFT"),
          isolated_label,
          IupSetAttributes
          (IupLabel ("Fragmented: "),
           "TIP = \"Number of fragmented isolated rooms\","
           "ALIGNMENT = ALEFT"),
          fragmented_label,
          NULL),
         "ORIENTATION = HORIZONTAL,"
         "NUMDIV = 2,"
         "SIZECOL = -1,"
         "SIZELIN = -1,"
         "EXPANDCHILDREN = HORIZONTAL,"),
        IupFill (),
        IupSetAttributes
        (IupHbox
         (IupFill (),
          fix_button,
          IupFill (),
          defrag_button,
          IupFill (),
          clean_button,
          IupFill (),
          NULL),
         "ALIGNMENT = ACENTER,"),
        IupFill (),
        NULL),
       "NORMALIZERGROUP = ROOM_NORM,")),
     "TITLE = Sanitation,");

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupHbox
      (tree_ctrl,
       IupSetAttributes
       (IupVbox
        (tree_frame,
         go_frame,
         link_frame,
         sanitation_frame,
         NULL),
        "ALIGNMENT = ACENTER"),
       NULL),
      "ALIGNMENT = ACENTER,"),
     "DESTROY_CB", destroy_cb,
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     "_UPDATE_TREE_CB", _update_tree_cb,
     "_SELECT_ORIGIN_CB", (Icallback) _select_origin_cb,
     "_SWAP_ORIGIN_SELECTION_CB", _swap_origin_selection_cb,
     "_ADD_ROOM_CB", (Icallback) _add_room_cb,
     NULL);

  IupSetAttribute (tree_ctrl, "IMAGEBRANCHCOLLAPSED", "VIEW_RING_ICON");
  IupSetAttribute (tree_ctrl, "IMAGEBRANCHEXPANDED", "VIEW_RING_ICON");

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_CONTROL", (void *) ih);

  IupSetAttribute (ih, "_LEVEL", (void *) level);

  struct tree *tree = xmalloc (sizeof (*tree));
  memset (tree, 0, sizeof (*tree));
  IupSetAttribute (ih, "_TREE", (void *) tree);
  IupSetAttribute (ih, "_TREE_CTRL", (void *) tree_ctrl);

  IupSetAttribute (ih, "_TREE_FRAME", (void *) tree_frame);
  IupSetAttribute (ih, "_ISOLATED_TOGGLE", (void *) isolated_toggle);
  IupSetAttribute (ih, "_REFRESH_BUTTON", (void *) refresh_button);

  IupSetAttribute (ih, "_GO_FRAME", (void *) go_frame);
  IupSetAttribute (ih, "_GO_ACTIVELY_BROKEN_TOGGLE",
                   (void *) go_actively_broken_toggle);
  IupSetAttribute (ih, "_GO_PASSIVELY_BROKEN_TOGGLE",
                   (void *) go_passively_broken_toggle);
  IupSetAttribute (ih, "_GO_ISOLATED_TOGGLE",
                   (void *) go_isolated_toggle);
  IupSetAttribute (ih, "_GO_RADIO", (void *) go_radio);
  IupSetAttribute (ih, "_PREV_BUTTON", (void *) prev_button);
  IupSetAttribute (ih, "_NEXT_BUTTON", (void *) next_button);
  IupSetAttribute (ih, "_START_BUTTON", (void *) start_button);

  IupSetAttribute (ih, "_LINK_FRAME", (void *) link_frame);
  IupSetAttribute (ih, "_RECIPROCAL_TOGGLE", (void *) reciprocal_toggle);
  IupSetAttribute
    (ih, "_LOCALLY_UNIQUE_TOGGLE", (void *) locally_unique_toggle);
  IupSetAttribute
    (ih, "_GLOBALLY_UNIQUE_TOGGLE", (void *) globally_unique_toggle);
  IupSetAttribute (ih, "_ISOLATE_BUTTON", (void *) isolate_button);
  IupSetAttribute (ih, "_NEW_BUTTON", (void *) new_button);
  IupSetAttribute (ih, "_OUT_BUTTON", (void *) out_button);
  IupSetAttribute (ih, "_IN_BUTTON", (void *) in_button);
  IupSetAttribute (ih, "_EXCHANGE_LINKS_BUTTON", (void *) exchange_links_button);
  IupSetAttribute (ih, "_EXCHANGE_TILES_BUTTON", (void *) exchange_tiles_button);

  IupSetAttribute (ih, "_SANITATION_FRAME", (void *) sanitation_frame);
  IupSetAttribute (ih, "_FIX_BUTTON", (void *) fix_button);
  IupSetAttribute (ih, "_DEFRAG_BUTTON", (void *) defrag_button);
  IupSetAttribute (ih, "_CLEAN_BUTTON", (void *) clean_button);

  IupSetAttribute (ih, "_TOTAL_LABEL", (void *) total_label);
  IupSetAttribute (ih, "_BROKEN_LABEL", (void *) broken_label);
  IupSetAttribute (ih, "_ISOLATED_LABEL", (void *) isolated_label);
  IupSetAttribute (ih, "_FRAGMENTED_LABEL", (void *) fragmented_label);
  IupSetAttribute (ih, "_START_LABEL", (void *) start_label);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  Ihandle *norm = IupGetHandle ("ROOM_NORM");
  IupSetAttribute (norm, "NORMALIZE", "HORIZONTAL");

  return ih;
}

int
destroy_cb (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  al_free (last);
  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  destroy_tree (tree);
  al_free (tree);
  return IUP_DEFAULT;
}

int
show_cb (Ihandle *ih, int state)
{
  if (state == IUP_SHOW) {
    Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
    int count = IupGetInt (tree_ctrl, "COUNT");
    if (! count) _update_tree_cb (ih);
  }
  return IUP_DEFAULT;
}

int
selected_room (Ihandle *tree_ctrl)
{
  int id = IupGetInt (tree_ctrl, "VALUE");
  if (id < 0) return 0;
  struct tree *tree = (void *) IupGetAttribute (tree_ctrl, "_TREE");
  while (tree->node[id].depth != 0) id = tree_node_parent_id (tree, id);
  int *room = tree->node[id].data;
  return *room;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  /* Update tree (if necessary) */
  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  Ihandle *isolated_toggle =
    (void *) IupGetAttribute (ih, "_ISOLATED_TOGGLE");
  bool isolated_toggle_value = IupGetInt (isolated_toggle, "VALUE");
  struct tree new_tree;
  populate_room_tree (level, &new_tree, ! isolated_toggle_value);
  if (! tree_eq (tree, &new_tree)) update_tree_ctrl (ih, &new_tree);
  else destroy_tree (&new_tree);

  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  int id = IupGetInt (tree_ctrl, "VALUE");
  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  int room = selected_room (tree_ctrl);
  struct link_survey ls;
  if (room) link_survey (&ls, level->link, level->room_nmemb, room);

  /* Go frame */
  Ihandle *go_radio = (void *) IupGetAttribute (ih, "_GO_RADIO");
  Ihandle *go_selected = (void *) IupGetAttribute (go_radio, "VALUE_HANDLE");

  Ihandle *go_actively_broken_toggle =
    (void *) IupGetAttribute (ih, "_GO_ACTIVELY_BROKEN_TOGGLE");
  Ihandle *go_passively_broken_toggle =
    (void *) IupGetAttribute (ih, "_GO_PASSIVELY_BROKEN_TOGGLE");
  Ihandle *go_isolated_toggle =
    (void *) IupGetAttribute (ih, "_GO_ISOLATED_TOGGLE");

  Ihandle *next_button = (void *) IupGetAttribute (ih, "_NEXT_BUTTON");
  Ihandle *prev_button = (void *) IupGetAttribute (ih, "_PREV_BUTTON");

  int first_id, last_id;
  if (go_selected == go_actively_broken_toggle) {
    first_id = last->first_actively_broken_id;
    last_id = last->last_actively_broken_id;
  } else if (go_selected == go_passively_broken_toggle) {
    first_id = last->first_passively_broken_id;
    last_id = last->last_passively_broken_id;
  } else if (go_selected == go_isolated_toggle) {
    first_id = last->first_isolated_id;
    last_id = last->last_isolated_id;
  } else assert (false);

  gui_control_active (prev_button, first_id >= 0 && id > first_id);
  gui_control_active (next_button, last_id >= 0 && id < last_id);

  /* Link frame */
  Ihandle *reciprocal_toggle =
    (void *) IupGetAttribute (ih, "_RECIPROCAL_TOGGLE");
  gui_control_int (reciprocal_toggle, "VALUE", editor_reciprocal_links);

  Ihandle *locally_unique_toggle =
    (void *) IupGetAttribute (ih, "_LOCALLY_UNIQUE_TOGGLE");
  gui_control_int (locally_unique_toggle, "VALUE", editor_locally_unique_links);

  Ihandle *globally_unique_toggle =
    (void *) IupGetAttribute (ih, "_GLOBALLY_UNIQUE_TOGGLE");
  gui_control_int (globally_unique_toggle, "VALUE", editor_globally_unique_links);

  Ihandle *isolate_button = (void *) IupGetAttribute (ih, "_ISOLATE_BUTTON");
  if (id >= 0) {
    bool c;
    switch (depth) {
    case ROOM_DEPTH:
      c = ls.l || ls.r || ls.a || ls.b;
      break;
    case DIR_DEPTH: {
      enum dir *d = tree->node[id].data;
      c = roomd (level->link, level->room_nmemb, room, *d);
      break;
    }
    case ADJACENT_DEPTH:
      c = false;
      break;
    default: assert (false);
    }
    gui_control_active (isolate_button, c);
  } else gui_control_active (isolate_button, false);

  Ihandle *new_button = (void *) IupGetAttribute (ih, "_NEW_BUTTON");
  if (id >= 0) {
    bool c;
    switch (depth) {
    case ROOM_DEPTH:
      c = (! ls.l && ls.r && ls.a && ls.b)
        || (ls.l && ! ls.r && ls.a && ls.b)
        || (ls.l && ls.r && ! ls.a && ls.b)
        || (ls.l && ls.r && ls.a && ! ls.b);
      break;
    case DIR_DEPTH: {
      enum dir *d = tree->node[id].data;
      c = ! roomd (level->link, level->room_nmemb, room, *d);
      break;
    }
    case ADJACENT_DEPTH:
      c = false;
      break;
    default: assert (false);
    }
    gui_control_active (new_button, c);
  } else gui_control_active (new_button, false);

  Ihandle *out_button =
    (void *) IupGetAttribute (ih, "_OUT_BUTTON");
  gui_control_active (out_button, is_valid_pos (&selection_pos));

  Ihandle *in_button =
    (void *) IupGetAttribute (ih, "_IN_BUTTON");
  gui_control_active (in_button, is_valid_pos (&selection_pos));

  Ihandle *exchange_links_button =
    (void *) IupGetAttribute (ih, "_EXCHANGE_LINKS_BUTTON");
  gui_control_active (exchange_links_button, is_valid_pos (&selection_pos)
                      && selection_pos.room != global_mr.room);

  Ihandle *exchange_tiles_button =
    (void *) IupGetAttribute (ih, "_EXCHANGE_TILES_BUTTON");
  gui_control_active (exchange_tiles_button, is_valid_pos (&selection_pos)
                      && selection_pos.room != global_mr.room);

  /* Sanitation frame */
  Ihandle *total_label = (void *) IupGetAttribute (ih, "_TOTAL_LABEL");
  if (gui_control_attribute_strf
      (total_label, "TITLE", "%zu", last->total_rooms))
    IupRefresh (total_label);

  Ihandle *broken_label =
    (void *) IupGetAttribute (ih, "_BROKEN_LABEL");
  if (gui_control_attribute_strf
      (broken_label, "TITLE", "%zu", last->broken_rooms))
    IupRefresh (broken_label);

  Ihandle *isolated_label =
    (void *) IupGetAttribute (ih, "_ISOLATED_LABEL");
  if (gui_control_attribute_strf
      (isolated_label, "TITLE", "%zu", last->isolated_rooms))
    IupRefresh (isolated_label);

  Ihandle *fragmented_label =
    (void *) IupGetAttribute (ih, "_FRAGMENTED_LABEL");
  if (gui_control_attribute_strf
      (fragmented_label, "TITLE", "%zu", last->fragmented_rooms))
    IupRefresh (fragmented_label);

  Ihandle *start_label =
    (void *) IupGetAttribute (ih, "_START_LABEL");
  if (gui_control_attribute_strf
      (start_label, "TITLE", "%02zu", last->start_room))
    IupRefresh (start_label);

  Ihandle *fix_button =
    (void *) IupGetAttribute (ih, "_FIX_BUTTON");
  gui_control_active (fix_button, last->broken_rooms > 0);

  Ihandle *defrag_button =
    (void *) IupGetAttribute (ih, "_DEFRAG_BUTTON");
  gui_control_active (defrag_button, last->fragmented_rooms > 0);

  Ihandle *clean_button =
    (void *) IupGetAttribute (ih, "_CLEAN_BUTTON");
  gui_control_active
    (clean_button, last->isolated_rooms > last->fragmented_rooms);

  struct pos p; npos (&level->start_pos, &p);
  if (last->level_n != level->n || last->start_room != p.room)
    _update_tree_cb (ih);

  return IUP_DEFAULT;
}

bool
is_broken_link (struct room_linking *rlink, size_t room_nmemb,
                int room, enum dir d)
{
  int rd = roomd (rlink, room_nmemb, room, d);
  return room != 0 && rd != 0 &&
    room != roomd (rlink, room_nmemb, rd, opposite_dir (d));
}

bool
is_passively_broken_link (struct room_linking *rlink, size_t room_nmemb,
                          int room, enum dir d)
{
  enum dir od = opposite_dir (d);

  for (int i = 1; i < room_nmemb; i++)
    if (roomd (rlink, room_nmemb, i, od) == room
        && is_broken_link (rlink, room_nmemb, i, od))
      return true;

  return false;
}

int
count_isolated_rooms (struct level *level)
{
  size_t count = 0;
  for (size_t room = 1; room < level->room_nmemb; room++)
    if (! is_room_accessible_from_kid_start (level, room))
      count++;
  return count;
}

int
count_broken_rooms (struct room_linking *rlink, size_t room_nmemb)
{
  size_t count = 0;
  for (size_t room = 1; room < room_nmemb; room++)
    for (enum dir d = FIRST_DIR; d <= LAST_DIR; d++)
      if (is_broken_link (rlink, room_nmemb, room, d)) {
        count++;
        break;
      }
  return count;
}

int
count_fragmented_rooms (struct level *level)
{
  int count = 0;
  bool within = false;
  for (int room = level->room_nmemb - 1; room > 0; room--) {
    if (! within && is_room_accessible_from_kid_start (level, room))
      within = true;
    if (within && ! is_room_accessible_from_kid_start (level, room))
      count++;
  }
  return count;
}

void
populate_room_tree (struct level *level, struct tree *tree,
                    bool ignore_isolated)
{
  tree->node = NULL;
  tree->nmemb = 0;

  for (int room = 1; room < level->room_nmemb; room++) {
    if (ignore_isolated && ! is_room_accessible_from_kid_start (level, room))
      continue;

    int room_id = get_or_put_tree_branch_child
      (tree, -1, TREE_NODE_TYPE_BRANCH, &room, sizeof (room),
       (m_comparison_fn_t) int_eq);

    for (enum dir d = FIRST_DIR; d <= LAST_DIR; d++) {
      int dir_id = get_or_put_tree_branch_child
        (tree, room_id, TREE_NODE_TYPE_BRANCH, &d, sizeof (d),
         (m_comparison_fn_t) int_eq);

      int room_out = roomd (level->link, level->room_nmemb, room, d);
      get_or_put_tree_branch_child
        (tree, dir_id, TREE_NODE_TYPE_LEAF, &room_out, sizeof (room_out),
         (m_comparison_fn_t) int_eq);

      for (int room_in = 1; room_in < level->room_nmemb; room_in++)
        if (roomd (level->link, level->room_nmemb, room_in, opposite_dir (d))
            == room)
          get_or_put_tree_branch_child
            (tree, dir_id, TREE_NODE_TYPE_LEAF, &room_in, sizeof (room_in),
             (m_comparison_fn_t) int_eq);
    }
  }
}

char *
dir_icon_name (enum dir d)
{
  switch (d) {
  case LEFT: return "L_ICON";
  case RIGHT: return "R_ICON";
  case ABOVE: return "A_ICON";
  case BELOW: return "B_ICON";
  default: assert (false); return NULL;
  }
}

char
direction_char (enum dir d)
{
  switch (d) {
  case LEFT: return '<';
  case RIGHT: return '>';
  case ABOVE: return '^';
  case BELOW: return 'v';
  default: assert (false); return '?';
  }
}

void
populate_room_tree_ctrl (Ihandle *tree_ctrl, struct tree *tree)
{
  struct level *level = (void *) IupGetAttribute (tree_ctrl, "_LEVEL");
  populate_tree_ctrl (tree_ctrl, tree);
  for (size_t id = 0; id < tree->nmemb; id++) {
    switch (tree->node[id].depth) {
    case ROOM_DEPTH: {
      int *room = tree->node[id].data;
      IupSetStrfId (tree_ctrl, "TITLE", id, "Room %02i", *room);
      if (! is_room_accessible_from_kid_start (level, *room))
        IupSetAttributeId (tree_ctrl, "COLOR", id, ISOLATED_COLOR);
      break;
    }
    case DIR_DEPTH: {
      int parent_id = tree_node_parent_id (tree, id);
      int *room = tree->node[parent_id].data;
      enum dir *d = tree->node[id].data;
      IupSetStrfId (tree_ctrl, "TITLE", id, "Room %02i: %s",
                    *room, direction_string (*d));
      const char *icon_name = dir_icon_name (*d);
      IupSetAttributeId (tree_ctrl, "IMAGE", id, icon_name);
      IupSetAttributeId (tree_ctrl, "IMAGEEXPANDED", id, icon_name);
      break;
    }
    case ADJACENT_DEPTH: {
      int parent_id = tree_node_parent_id (tree, id);
      int parent_parent_id = tree_node_parent_id (tree, parent_id);
      int *room = tree->node[id].data;
      int *parent_room = tree->node[parent_parent_id].data;

      bool first = id == parent_id + 1;
      int src_room = first ? *parent_room : *room;
      int dst_room = first ? *room : *parent_room;

      enum dir d = * (enum dir *) tree->node[parent_id].data;
      d = first ? d : opposite_dir (d);

      IupSetStrfId (tree_ctrl, "TITLE", id, "Room %02i %c Room %02i",
                    src_room, direction_char (d), dst_room);
      IupSetAttributeId
        (tree_ctrl, "IMAGE", id, dir_icon_name (d));

      if (is_broken_link (level->link, level->room_nmemb, src_room, d))
        IupSetAttributeId (tree_ctrl, "COLOR", id, first
                           ? ACTIVELY_BROKEN_COLOR
                           : PASSIVELY_BROKEN_COLOR);

      break;
    }
    default: assert (false);
    }
  }

  propagate_children_nodes_color (tree_ctrl);
}

int
_update_tree_cb (Ihandle *ih)
{
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  Ihandle *isolated_toggle =
    (void *) IupGetAttribute (ih, "_ISOLATED_TOGGLE");
  bool isolated_toggle_value = IupGetInt (isolated_toggle, "VALUE");
  struct tree new_tree;
  populate_room_tree (level, &new_tree, ! isolated_toggle_value);
  update_tree_ctrl (ih, &new_tree);
  return IUP_DEFAULT;
}

void
update_tree_ctrl (Ihandle *ih, struct tree *new_tree)
{
  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  destroy_tree (tree);
  *tree = *new_tree;

  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  last->total_rooms = level->room_nmemb - 1;
  last->isolated_rooms = count_isolated_rooms (level);
  last->broken_rooms = count_broken_rooms (level->link, level->room_nmemb);
  last->fragmented_rooms = count_fragmented_rooms (level);
  struct pos p; npos (&level->start_pos, &p);
  last->start_room = p.room;
  last->level_n = level->n;

  last->first_actively_broken_id =
    next_broken_link_id (level, tree, -1, +1, false);
  last->last_actively_broken_id =
    next_broken_link_id (level, tree, tree->nmemb, -1, false);
  last->first_passively_broken_id =
    next_broken_link_id (level, tree, -1, +1, true);
  last->last_passively_broken_id =
    next_broken_link_id (level, tree, tree->nmemb, -1, true);
  last->first_isolated_id =
    next_isolated_room_id (level, tree, -1, +1);
  last->last_isolated_id =
    next_isolated_room_id (level, tree, tree->nmemb, -1);

  /* record current selection */
  int id = IupGetInt (tree_ctrl, "VALUE");
  char *title_buffer = IupGetAttributeId (tree_ctrl, "TITLE", id);
  char *selected_title = xasprintf ("%s", title_buffer ? title_buffer : "");
  int selected_depth = IupGetIntId (tree_ctrl, "DEPTH", id);

  /* build tree */
  IupSetAttribute (tree_ctrl, "AUTOREDRAW", "NO");
  IupUnmap (tree_ctrl);
  IupSetAttribute (tree_ctrl, "DELNODE", "ALL");
  IupMap (tree_ctrl);
  populate_room_tree_ctrl (tree_ctrl, tree);
  IupRefresh (tree_ctrl);
  IupRedraw (tree_ctrl, true);
  IupSetAttribute (tree_ctrl, "AUTOREDRAW", "YES");

  /* recall previous "current" selection */
  select_node_by_title (tree_ctrl, selected_title, selected_depth);
  al_free (selected_title);

  /* Sync selection with MR origin */
  int room = selected_room (tree_ctrl);
  if (room != global_mr.room) select_room_node (ih, global_mr.room);
}

int
next_broken_link_id (struct level *level, struct tree *tree,
                     int start_id, int d, bool passive)
{
  int inc = d >= 0 ? +1 : -1;
  for (int id = start_id + inc;
       d >= 0 ? id < tree->nmemb : id >= 0;
       id += inc) {
      if (tree->node[id].depth != DIR_DEPTH) continue;
      int parent_id = tree_node_parent_id (tree, id);
      int *room = tree->node[parent_id].data;
      enum dir *dir = tree->node[id].data;
      if ((passive &&
           is_passively_broken_link (level->link, level->room_nmemb, *room, *dir))
          || (! passive
              && is_broken_link (level->link, level->room_nmemb, *room, *dir)))
        return id;
  }
  return -1;
}

int
next_isolated_room_id (struct level *level, struct tree *tree,
                       int start_id, int d)
{
  int inc = d >= 0 ? +1 : -1;
  for (int id = start_id + inc;
       d >= 0 ? id < tree->nmemb : id >= 0;
       id += inc) {
      if (tree->node[id].depth != ROOM_DEPTH) continue;
      int *room = tree->node[id].data;
      if (! is_room_accessible_from_kid_start (level, *room))
        return id;
  }
  return -1;
}

int
go_button_cb (Ihandle *button)
{
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
  struct tree *tree = (void *) IupGetAttribute (button, "_TREE");
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");

  Ihandle *start_button = (void *) IupGetAttribute (button, "_START_BUTTON");
  if (button == start_button) {
    struct pos p; npos (&level->start_pos, &p);
    int id = get_tree_node_id_by_data (tree, ROOM_DEPTH, &p.room);
    if (id < 0) return IUP_DEFAULT;
    select_node_by_id (tree_ctrl, id);
    gui_run_callback_IFnii ("SELECTION_CB", tree_ctrl, id, true);
    return IUP_DEFAULT;
  }

  Ihandle *next_button = (void *) IupGetAttribute (button, "_NEXT_BUTTON");
  Ihandle *prev_button = (void *) IupGetAttribute (button, "_PREV_BUTTON");

  int d;
  if (button == next_button) d = +1;
  else if (button == prev_button) d = -1;
  else assert (false);

  Ihandle *go_radio = (void *) IupGetAttribute (button, "_GO_RADIO");
  Ihandle *go_selected = (void *) IupGetAttribute (go_radio, "VALUE_HANDLE");

  Ihandle *go_actively_broken_toggle =
    (void *) IupGetAttribute (button, "_GO_ACTIVELY_BROKEN_TOGGLE");
  Ihandle *go_passively_broken_toggle =
    (void *) IupGetAttribute (button, "_GO_PASSIVELY_BROKEN_TOGGLE");
  Ihandle *go_isolated_toggle =
    (void *) IupGetAttribute (button, "_GO_ISOLATED_TOGGLE");

  int start_id = IupGetInt (tree_ctrl, "VALUE");
  if (start_id < 0) return IUP_DEFAULT;

  int id;
  if (go_selected == go_actively_broken_toggle)
    id = next_broken_link_id (level, tree, start_id, d, false);
  else if (go_selected == go_passively_broken_toggle)
    id = next_broken_link_id (level, tree, start_id, d, true);
  else if (go_selected == go_isolated_toggle)
    id = next_isolated_room_id (level, tree, start_id, d);
  else assert (false);

  select_node_by_id (tree_ctrl, id);
  gui_run_callback_IFnii ("SELECTION_CB", tree_ctrl, id, true);

  return IUP_DEFAULT;
}

int
isolate_button_cb (Ihandle *button)
{
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");
  int id = IupGetInt (tree_ctrl, "VALUE");
  if (id < 0) return IUP_DEFAULT;
  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  int room = selected_room (tree_ctrl);
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");

  switch (depth) {
  case ROOM_DEPTH: {
    struct room_linking *rlink =
      copy_array (level->link, level->room_nmemb, NULL, sizeof (*rlink));
    for (enum dir d = FIRST_DIR; d <= LAST_DIR; d++)
      editor_link (rlink, level->room_nmemb, room, 0, d);
    char *desc = xasprintf ("ISOLATE ROOM %i", room);
    register_link_undo (&undo, rlink, desc);
    al_free (desc);
    destroy_array ((void **) &rlink, NULL);
    break;
  }
  case DIR_DEPTH: {
    struct room_linking *rlink =
      copy_array (level->link, level->room_nmemb, NULL, sizeof (*rlink));
    struct tree *tree = (void *) IupGetAttribute (button, "_TREE");
    enum dir *d = tree->node[id].data;
    editor_link (rlink, level->room_nmemb, room, 0, *d);
    char *desc = xasprintf ("ISOLATE ROOM %i %s", room,
                            direction_string (*d));
    register_link_undo (&undo, rlink, desc);
    al_free (desc);
    destroy_array ((void **) &rlink, NULL);
    break;
  }
  case ADJACENT_DEPTH:
    return IUP_DEFAULT;
  default: assert (false);
  }

  if (! is_room_accessible_from_kid_start (level, room)) {
    Ihandle *isolated_toggle =
      (void *) IupGetAttribute (button, "_ISOLATED_TOGGLE");
    IupSetInt (isolated_toggle, "VALUE", true);
  }

  _update_tree_cb (button);

  return IUP_DEFAULT;
}

int
new_button_cb (Ihandle *button)
{
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");
  int id = IupGetInt (tree_ctrl, "VALUE");
  if (id < 0) return IUP_DEFAULT;
  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  int room = selected_room (tree_ctrl);
  enum dir d;

  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");

  switch (depth) {
  case ROOM_DEPTH: {
    for (d = FIRST_DIR; d <= LAST_DIR; d++)
      if (! roomd (level->link, level->room_nmemb, room, d)) break;
    break;
  }
  case DIR_DEPTH: {
    struct tree *tree = (void *) IupGetAttribute (button, "_TREE");
    d = * (enum dir *) tree->node[id].data;
    break;
  }
  case ADJACENT_DEPTH:
    return IUP_DEFAULT;
  default: assert (false);
  }

  register_new_room_undo (&undo, NULL);
  int new_room = level->room_nmemb - 1;
  struct room_linking *rlink =
    copy_array (level->link, level->room_nmemb, NULL, sizeof (*rlink));

  closure_link_room (rlink, level->room_nmemb, room, new_room, d);

  char *desc = xasprintf ("NEW ROOM %i %s %i", new_room,
                          direction_string (d), room);
  register_link_undo (&undo, rlink, desc);
  al_free (desc);
  destroy_array ((void **) &rlink, NULL);

  _update_tree_cb (button);

  select_room_node (button, new_room);

  return IUP_DEFAULT;
}

int
new_room (Ihandle *ih, int l, int r, int a, int b)
{
  return 0;
}

int
in_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
out_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
exchange_links_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
exchange_tiles_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
toggle_action_cb (Ihandle *ih, int state)
{
  Ihandle *reciprocal_toggle =
    (void *) IupGetAttribute (ih, "_RECIPROCAL_TOGGLE");
  Ihandle *locally_unique_toggle =
    (void *) IupGetAttribute (ih, "_LOCALLY_UNIQUE_TOGGLE");
  Ihandle *globally_unique_toggle =
    (void *) IupGetAttribute (ih, "_GLOBALLY_UNIQUE_TOGGLE");

  if (ih == reciprocal_toggle)
    editor_reciprocal_links = IupGetInt (ih, "VALUE");
  else if (ih == locally_unique_toggle)
    editor_locally_unique_links = IupGetInt (ih, "VALUE");
  else if (ih == globally_unique_toggle)
    editor_globally_unique_links = IupGetInt (ih, "VALUE");

  return IUP_DEFAULT;
}

int
fix_button_cb (Ihandle *button)
{
  _update_tree_cb (button);
  return IUP_DEFAULT;
}

int
defrag_button_cb (Ihandle *button)
{
  _update_tree_cb (button);
  return IUP_DEFAULT;
}

int
clean_button_cb (Ihandle *button)
{
  _update_tree_cb (button);
  return IUP_DEFAULT;
}

int
selection_cb (Ihandle *tree_ctrl, int id, int status)
{
  if (! status) return IUP_DEFAULT;

  struct tree *tree = (void *) IupGetAttribute (tree_ctrl, "_TREE");

  switch (tree->node[id].depth) {
  case ROOM_DEPTH: {
    int *room = tree->node[id].data;
    mr_simple_center_room (&global_mr, *room);
    break;
  }
  case DIR_DEPTH: {
    int parent_id = tree_node_parent_id (tree, id);
    int *room = tree->node[parent_id].data;
    mr_simple_center_room (&global_mr, *room);
    break;
  }
  case ADJACENT_DEPTH: {
    int *room = tree->node[id].data;
    if (*room) select_room_node (tree_ctrl, *room);
    else {
      int pid = tree_node_parent_id (tree, id);
      select_node_by_id (tree_ctrl, pid);
      gui_run_callback_IFnii ("SELECTION_CB", tree_ctrl, id, true);
    }
    break;
  }
  default: assert (false); break;
  }

  return IUP_DEFAULT;
}
void
select_room_node (Ihandle *ih, int room)
{
  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  int id = get_tree_node_id_by_data (tree, ROOM_DEPTH, &room);
  if (id < 0) {
    Ihandle *isolated_toggle =
      (void *) IupGetAttribute (ih, "_ISOLATED_TOGGLE");
    IupSetInt (isolated_toggle, "VALUE", true);
    _update_tree_cb (ih);
    struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
    id = get_tree_node_id_by_data (tree, ROOM_DEPTH, &room);
  }

  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  select_node_by_id (tree_ctrl, id);
  gui_run_callback_IFnii ("SELECTION_CB", tree_ctrl, id, true);
}

int
_select_origin_cb (Ihandle *ih, struct mouse_coord *mc, struct pos *p)
{
  Ihandle *tabs = (void *) IupGetAttribute (ih, "_EDITOR_TABS");
  IupSetAttribute (tabs, "VALUE_HANDLE", (void *) ih);

  struct pos np; npos (p, &np);
  select_room_node (ih, np.room);
  mr_set_origin (&global_mr, np.room, mc->x, mc->y);

  return IUP_DEFAULT;
}

int
_swap_origin_selection_cb (Ihandle *ih)
{
  Ihandle *tabs = (void *) IupGetAttribute (ih, "_EDITOR_TABS");
  IupSetAttribute (tabs, "VALUE_HANDLE", (void *) ih);

  int room = global_mr.room;
  int x = global_mr.x, y = global_mr.y;
  mr_coord (&global_mr, selection_pos.room, -1, &x, &y);
  select_room_node (ih, selection_pos.room);
  mr_set_origin (&global_mr, selection_pos.room, x, y);
  selection_pos.room = room;

  return IUP_DEFAULT;
}

int
_add_room_cb (Ihandle *ih, struct pos *p)
{
  return IUP_DEFAULT;
}
