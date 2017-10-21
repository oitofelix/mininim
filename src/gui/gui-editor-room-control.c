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

enum {
  ROOM_DEPTH, DIR_DEPTH, ADJACENT_DEPTH,
};

static int destroy_cb (Ihandle *ih);
static int show_cb (Ihandle *ih, int state);

static int selected_room (Ihandle *tree_ctrl);

static int _update_cb (Ihandle *ih);

/* tree build */
static int count_room_broken_links (struct level *l, int room);
static int count_level_broken_links (struct level *l);
static int _update_tree_cb (Ihandle *ih);
static void update_tree_ctrl (Ihandle *ih, struct tree *tree);
static void populate_room_tree (struct room_linking *rlink,
                                size_t room_nmemb, struct tree *tree);

/* auxiliary */
int new_room (Ihandle *ih, int l, int r, int a, int b);

/* callbacks */
static int next_button_cb (Ihandle *button);

static int delete_button_cb (Ihandle *button);
static int target_button_cb (Ihandle *button);
static int add_button_cb (Ihandle *button);

static int defrag_button_cb (Ihandle *button);
static int clean_button_cb (Ihandle *button);

static int selection_cb (Ihandle *ih, int id, int status);

static int _select_target_cb (Ihandle *ih, struct pos *p);
static int _select_source_cb (Ihandle *ih, struct pos *p);
static int _add_room_cb (Ihandle *ih, struct pos *p);


Ihandle *
gui_create_editor_room_control (char *norm_group, struct level *level)
{
  Ihandle *ih, *tree_ctrl;

  Ihandle *view_frame, *refresh_button;

  Ihandle *next_frame, *prev_button, *next_button;

  Ihandle *selection_frame, *delete_button, *target_button, *add_button;

  Ihandle *sanitation_frame, *defrag_button, *clean_button,
    *rooms_label, *broken_links_label;

  tree_ctrl = IupSetCallbacks
    (IupSetAttributes
     (IupTree (),
      "ADDROOT = NO,"
      "ADDEXPANDED = NO,"),
     "SELECTION_CB", (Icallback) selection_cb,
     NULL);

  refresh_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = RELOAD_ICON,"
      "TIP = \"Refresh\","),
     "ACTION", _update_tree_cb,
     NULL);

  view_frame = IupSetAttributes
    (IupFrame
     (IupVbox
      (IupFill (),
       IupSetAttributes
       (IupHbox
        (IupFill (),
         refresh_button,
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER,"
        "NORMALIZERGROUP = ROOM_NORM,"),
       IupFill (),
       NULL)),
     "TITLE = View,"
     "NORMALIZERGROUP = ROOM_NORM,");

  prev_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = PREVIOUS_ICON,"
      "TIP = \"Select previous room\","),
     "ACTION", next_button_cb,
     NULL);

  next_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = NEXT_ICON,"
      "TIP = \"Select next room\","),
     "ACTION", next_button_cb,
     NULL);

  next_frame = IupSetAttributes
    (IupFrame
     (IupVbox
      (IupFill (),
       IupSetAttributes
       (IupHbox
        (IupFill (),
         prev_button,
         IupFill (),
         next_button,
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER,"
        "NORMALIZERGROUP = ROOM_NORM,"),
       IupFill (),
       NULL)),
     "TITLE = Next,"
     "NORMALIZERGROUP = ROOM_NORM,");

  delete_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = VIEW_RING_DEL_ICON,"
      "TIP = \"Isolate\","),
     "ACTION", delete_button_cb,
     NULL);

  target_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = ROOM_TARGET_ICON,"
      "TIP = \"Set selection as target\","),
     "ACTION", target_button_cb,
     NULL);

  add_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = ROOM_ADD_ICON,"
      "TIP = \"Add selection as target\","),
     "ACTION", add_button_cb,
     NULL);

  selection_frame = IupSetAttributes
    (IupFrame
     (IupVbox
      (IupFill (),
       IupSetAttributes
       (IupHbox
        (delete_button,
         IupFill (),
         target_button,
         IupFill (),
         add_button,
         NULL),
        "ALIGNMENT = ACENTER,"
        "NORMALIZERGROUP = ROOM_NORM,"),
       IupFill (),
       NULL)),
     "TITLE = Selection,");

  sanitation_frame = IupSetAttributes
    (IupFrame
     (IupVbox
      (IupFill (),

       IupSetAttributes
       (IupGridBox
        (
         IupSetAttributes
         (IupLabel ("Rooms: "),
          "TIP = \"Total number of rooms\","
          "ALIGNMENT = ARIGHT"),

         rooms_label = IupSetAttributes
         (IupLabel ("000"),
          "ALIGNMENT = ARIGHT"),

         IupSetAttributes
         (IupLabel ("Broken links: "),
          "TIP = \"Number of broken links\","
          "ALIGNMENT = ARIGHT"),

         broken_links_label = IupSetAttributes
         (IupLabel ("000"),
          "ALIGNMENT = ARIGHT"),

         NULL),
        "ORIENTATION = HORIZONTAL,"
        "NUMDIV = 2,"
        "SIZECOL = -1,"
        "SIZELIN = -1,"
        /* "NORMALIZESIZE = BOTH," */
        /* "HOMOGENEOUSLIN = YES," */
        /* "HOMOGENEOUSCOL = YES," */
        /* "ALIGNMENTLIN = ACENTER," */
        /* "ALIGNMENTCOL = ACENTER," */
        /* "FITTOCHILDREN = YES," */
        ),

       IupFill (),

       IupSetAttributes
       (IupHbox
        (IupFill (),

         defrag_button =
         IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = ROOM_DEFRAG_ICON,"
           "TIP = \"Defragment rooms\","),
          "ACTION", defrag_button_cb,
          NULL),

         IupFill (),

         clean_button =
         IupSetCallbacks
         (IupSetAttributes
          (IupButton (NULL, NULL),
           "IMAGE = ROOM_CLEAN_ICON,"
           "TIP = \"Delete trailing inactive rooms\","),
          "ACTION", clean_button_cb,
          NULL),

         IupFill (),

         NULL),
        "ALIGNMENT = ACENTER,"
        "NORMALIZERGROUP = ROOM_NORM,"),

       IupFill (),

       NULL)),
     "TITLE = Sanitation,");

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupHbox
      (tree_ctrl,
       IupSetAttributes
       (IupVbox
        (view_frame,
         next_frame,
         selection_frame,
         sanitation_frame,
         NULL),
        "ALIGNMENT = ACENTER"),
       NULL),
      "ALIGNMENT = ACENTER,"),
     "DESTROY_CB", destroy_cb,
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     "_UPDATE_TREE_CB", _update_tree_cb,
     "_SELECT_TARGET_CB", (Icallback) _select_target_cb,
     "_SELECT_SOURCE_CB", (Icallback) _select_source_cb,
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

  IupSetAttribute (ih, "_REFRESH_BUTTON", (void *) refresh_button);

  IupSetAttribute (ih, "_NEXT_FRAME", (void *) next_frame);
  IupSetAttribute (ih, "_PREV_BUTTON", (void *) prev_button);
  IupSetAttribute (ih, "_NEXT_BUTTON", (void *) next_button);

  IupSetAttribute (ih, "_SELECTION_FRAME", (void *) selection_frame);
  IupSetAttribute (ih, "_DELETE_BUTTON", (void *) delete_button);
  IupSetAttribute (ih, "_TARGET_BUTTON", (void *) target_button);
  IupSetAttribute (ih, "_ADD_BUTTON", (void *) add_button);

  IupSetAttribute (ih, "_SANITATION_FRAME", (void *) sanitation_frame);
  IupSetAttribute (ih, "_DEFRAG_BUTTON", (void *) defrag_button);
  IupSetAttribute (ih, "_CLEAN_BUTTON", (void *) clean_button);

  IupSetAttribute (ih, "_ROOMS_LABEL", (void *) rooms_label);
  IupSetAttribute (ih, "_BROKEN_LINKS_LABEL", (void *) broken_links_label);

  Ihandle *norm = IupGetHandle ("ROOM_NORM");
  IupSetAttribute (norm, "NORMALIZE", "HORIZONTAL");

  return ih;
}

int
destroy_cb (Ihandle *ih)
{
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

  while (IupGetIntId (tree_ctrl, "DEPTH", id) != 0)
    id = IupGetIntId (tree_ctrl, "PARENT", id);

  struct tree *tree = (void *) IupGetAttribute (tree_ctrl, "_TREE");
  int *room = tree->node[id].data;
  return *room;
}

int
_update_cb (Ihandle *ih)
{
  /* if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT; */

  /* struct level *level = (void *) IupGetAttribute (ih, "_LEVEL"); */

  /* struct tree *tree = (void *) IupGetAttribute (ih, "_TREE"); */
  /* struct tree new_tree; */
  /* populate_room_tree (level, &new_tree); */
  /* if (! tree_eq (tree, &new_tree)) update_tree_ctrl (ih, &new_tree); */
  /* else destroy_tree (&new_tree); */

  /* Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL"); */
  /* int *room = selected_room (tree_ctrl); */
  /* Ihandle *next_frame = (void *) IupGetAttribute (ih, "_NEXT_FRAME"); */
  /* gui_control_active (next_frame, e); */
  /* if (room) { */
  /*   Ihandle *prev_button = (void *) IupGetAttribute (ih, "_PREV_BUTTON"); */
  /*   gui_control_active (prev_button, e - level->room > 0); */

  /*   Ihandle *next_button = (void *) IupGetAttribute (ih, "_NEXT_BUTTON"); */
  /*   gui_control_active */
  /*     (next_button, e - level->room < level->room_nmemb - 1); */
  /* } */

  /* Ihandle *selection_frame = */
  /*   (void *) IupGetAttribute (ih, "_SELECTION_FRAME"); */
  /* gui_control_active (selection_frame, is_valid_pos (&selection_pos)); */

  /* if (is_valid_pos (&selection_pos)) { */
  /*   Ihandle *delete_button = (void *) IupGetAttribute (ih, "_DELETE_BUTTON"); */
  /*   gui_control_active (delete_button, e && is_room_fg (&selection_pos)); */

  /*   Ihandle *target_button = (void *) IupGetAttribute (ih, "_TARGET_BUTTON"); */
  /*   gui_control_active (target_button, e); */
  /* } */

  /* Ihandle *rooms_label = (void *) IupGetAttribute (ih, "_ROOMS_LABEL"); */
  /* if (gui_control_attribute_strf */
  /*     (rooms_label, "TITLE", "%zu", level->room_nmemb)) */
  /*   IupRefresh (rooms_label); */

  /* Ihandle *broken_links_label = */
  /*   (void *) IupGetAttribute (ih, "_BROKEN_LINKS_LABEL"); */
  /* if (gui_control_attribute_strf */
  /*     (broken_links_label, "TITLE", "%i", count_level_broken_links (level))) */
  /*   IupRefresh (broken_links_label); */

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

int
count_room_broken_links (struct level *l, int room)
{
  return 0;
}

int
count_level_broken_links (struct level *l)
{
  return 0;
}

void
populate_room_tree (struct room_linking *rlink, size_t room_nmemb,
                    struct tree *tree)
{
  tree->node = NULL;
  tree->nmemb = 0;

  for (int room = 1; room < room_nmemb; room++) {
    int room_id = get_or_put_tree_branch_child
      (tree, -1, TREE_NODE_TYPE_BRANCH, &room, sizeof (room),
       (m_comparison_fn_t) int_eq);

    for (enum dir d = FIRST_DIR; d <= LAST_DIR; d++) {
      int dir_id = get_or_put_tree_branch_child
        (tree, room_id, TREE_NODE_TYPE_BRANCH, &d, sizeof (d),
         (m_comparison_fn_t) int_eq);

      int room_out = roomd (rlink, room_nmemb, room, d);
      get_or_put_tree_branch_child
        (tree, dir_id, TREE_NODE_TYPE_LEAF, &room_out, sizeof (room_out),
         (m_comparison_fn_t) int_eq);

      for (int room_in = 1; room_in < room_nmemb; room_in++)
        if (roomd (rlink, room_nmemb, room_in, opposite_dir (d)) == room)
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
      break;
    }
    case DIR_DEPTH: {
      int parent_id = IupGetIntId (tree_ctrl, "PARENT", id);
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
      int parent_id = IupGetIntId (tree_ctrl, "PARENT", id);
      int parent_parent_id = IupGetIntId (tree_ctrl, "PARENT", parent_id);
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
                           ? "255 0 0"
                           : "180 0 0");

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
  struct tree new_tree;
  populate_room_tree (level->link, level->room_nmemb, &new_tree);
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
}

int
next_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
delete_button_cb (Ihandle *button)
{
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");

  int id = IupGetInt (tree_ctrl, "VALUE");
  if (id < 0) return IUP_DEFAULT;

  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  int room = selected_room (tree_ctrl);

  switch (depth) {
  case ROOM_DEPTH: {
    struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
    struct room_linking *rlink =
      copy_array (level->link, level->room_nmemb, NULL, sizeof (*rlink));
    for (enum dir d = FIRST_DIR; d <= LAST_DIR; d++)
      link_room (rlink, level->room_nmemb, room, 0, d);
    char *desc = xasprintf ("ISOLATE ROOM %i", room);
    register_link_undo (&undo, rlink, desc);
    al_free (desc);
    destroy_array ((void **) &rlink, NULL);
    break;
  }
  case DIR_DEPTH: {
    struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
    struct room_linking *rlink =
      copy_array (level->link, level->room_nmemb, NULL, sizeof (*rlink));
    struct tree *tree = (void *) IupGetAttribute (button, "_TREE");
    enum dir *d = tree->node[id].data;
    link_room (rlink, level->room_nmemb, room, 0, *d);
    char *desc = xasprintf ("ISOLATE ROOM %i %s", room,
                            direction_string (*d));
    register_link_undo (&undo, rlink, desc);
    al_free (desc);
    destroy_array ((void **) &rlink, NULL);
    break;
  }
  case ADJACENT_DEPTH:
    break;
  default: assert (false);
  }

  return IUP_DEFAULT;
}

int
target_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
new_room (Ihandle *ih, int l, int r, int a, int b)
{
  return 0;
}

int
add_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
defrag_button_cb (Ihandle *button)
{
  return IUP_DEFAULT;
}

int
clean_button_cb (Ihandle *button)
{
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
    int parent_id = IupGetIntId (tree_ctrl, "PARENT", id);
    int *room = tree->node[parent_id].data;
    mr_simple_center_room (&global_mr, *room);
    break;
  }
  case ADJACENT_DEPTH: {
    int *room = tree->node[id].data;
    int id = get_tree_node_id_by_data (tree, ROOM_DEPTH, room);
    if (id < 0) return IUP_DEFAULT;
    select_node_by_id (tree_ctrl, id);
    break;
  }
  default: assert (false); break;
  }

  return IUP_DEFAULT;
}

int
_select_target_cb (Ihandle *ih, struct pos *p)
{
  return IUP_DEFAULT;
}

int
_select_source_cb (Ihandle *ih, struct pos *p)
{
  return IUP_DEFAULT;
}

int
_add_room_cb (Ihandle *ih, struct pos *p)
{
  return IUP_DEFAULT;
}
