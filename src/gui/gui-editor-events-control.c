/*
  gui-editor-events-control.c -- GUI editor events control module;

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
  TARGET_DEPTH, EVENT_DEPTH, SOURCE_DEPTH,
};

struct last {
  char *video_mode;
  enum em em;
  enum hue hue;
  int level_n;
};

static void destroy_userdata (Ihandle *ih);
static int destroy_cb (Ihandle *ih);
static int show_cb (Ihandle *ih, int state);

static int selected_event_node_id (Ihandle *tree);
static struct level_event *selected_event (Ihandle *tree);

static int _update_cb (Ihandle *ih);

static void select_node_by_id (Ihandle *tree, int id);
static void select_node_by_title (Ihandle *tree, char *title, int depth);

static int get_event_node_id (Ihandle *tree, int e);
static int get_source_node_id (Ihandle *tree, struct pos *p);

/* tree build */
static bool is_event_active (struct level *l, int e);
static void add_nodes (Ihandle *tree, bool remove_inactive);
static int _update_tree_cb (Ihandle *ih);
static void update_target_icons (Ihandle *ih);

static int last_node_id (Ihandle *tree, int parent_id);

static int get_target (Ihandle *tree, struct pos *p);
static int add_target (Ihandle *tree, struct pos *p);

/* restricted to target_id branch */
static int get_event (Ihandle *tree, int target_id, int e);
static int add_event (Ihandle *tree, int target_id, int e);

/* restricted to event_id branch */
static int get_source (Ihandle *tree, int event_id, struct pos *p);
static int add_source (Ihandle *tree, int event_id, struct pos *p);

/* auxiliary */
int new_event (Ihandle *ih, struct pos *p, bool next);

/* callbacks */
static int next_toggle_cb (Ihandle *toggle, int state);
static int next_button_cb (Ihandle *button);

static int source_button_cb (Ihandle *button);
static int target_button_cb (Ihandle *button);
static int add_button_cb (Ihandle *button);

static int selection_cb (Ihandle *ih, int id, int status);

static int _select_target_cb (Ihandle *ih, struct pos *p);
static int _select_source_cb (Ihandle *ih, struct pos *p);
static int _add_event_cb (Ihandle *ih, struct pos *p);


Ihandle *
gui_create_editor_events_control (char *norm_group, struct level *level)
{
  Ihandle *ih, *tree;

  Ihandle *refresh_button, *all_toggle;

  Ihandle *next_frame, *next_toggle, *prev_button, *next_button;

  Ihandle *selection_frame, *source_button, *target_button, *add_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupHbox
      (tree = IupSetCallbacks
       (IupSetAttributes
        (IupTree (),
         "ADDROOT = NO,"
         "ADDEXPANDED = NO,"),
        "SELECTION_CB", (Icallback) selection_cb,
        NULL),

       IupSetAttributes
       (IupVbox
        (IupSetAttributes
         (IupFrame
          (IupVbox
           (IupFill (),

            IupSetAttributes
            (IupHbox
             (all_toggle = IupSetCallbacks
              (IupSetAttributes
               (IupToggle ("All", NULL),
                "VALUE = NO,"
                "TIP = \"Show inactive events\","),
               "ACTION", _update_tree_cb,
               NULL),

              IupFill (),

              refresh_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = RELOAD_ICON,"
                "TIP = \"Refresh\","),
               "ACTION", _update_tree_cb,
               NULL),

              NULL),
             "ALIGNMENT = ACENTER,"
             "NORMALIZERGROUP = EVENTS_NORM,"),

            IupFill (),

            NULL)),
          "TITLE = View,"
          "NORMALIZERGROUP = EVENTS_NORM,"),

         next_frame = IupSetAttributes
         (IupFrame
          (IupVbox
           (IupFill (),

            IupSetAttributes
            (IupHbox
             (next_toggle = IupSetCallbacks
              (IupSetAttributes
               (IupToggle (NULL, NULL),
                "TIP = \"Activate next event\","),
               "ACTION", (Icallback) next_toggle_cb,
               NULL),

              IupFill (),

              prev_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = PREVIOUS_ICON,"
                "TIP = \"Select previous event\","),
               "ACTION", next_button_cb,
               NULL),

              IupFill (),

              next_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = NEXT_ICON,"
                "TIP = \"Select next event\","),
               "ACTION", next_button_cb,
               NULL),

              NULL),
             "ALIGNMENT = ACENTER,"
             "NORMALIZERGROUP = EVENTS_NORM,"),

            IupFill (),

            NULL)),
          "TITLE = Next,"
          "NORMALIZERGROUP = EVENTS_NORM,"),

         selection_frame = IupSetAttributes
         (IupFrame
          (IupVbox
           (IupFill (),

            IupSetAttributes
            (IupHbox
             (source_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = EVENT_SOURCE_ICON,"
                "TIP = \"Set selection as source\","),
               "ACTION", source_button_cb,
               NULL),

              IupFill (),

              target_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = EVENT_TARGET_ICON,"
                "TIP = \"Set selection as target\","),
               "ACTION", target_button_cb,
               NULL),

              IupFill (),

              add_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = EVENT_ADD_ICON,"
                "TIP = \"Add selection as target\","),
               "ACTION", add_button_cb,
               NULL),

              NULL),
             "ALIGNMENT = ACENTER,"
             "NORMALIZERGROUP = EVENTS_NORM,"),

            IupFill (),

            NULL)),
          "TITLE = Selection,"),

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
     "_ADD_EVENT_CB", (Icallback) _add_event_cb,
     NULL);

  IupSetAttribute (tree, "IMAGELEAF", "EVENT_ICON");
  IupSetAttribute (tree, "IMAGEBRANCHCOLLAPSED", "EVENT_ICON");
  IupSetAttribute (tree, "IMAGEBRANCHEXPANDED", "EVENT_ICON");

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_CONTROL", (void *) ih);

  IupSetAttribute (ih, "_LEVEL", (void *) level);
  IupSetAttribute (ih, "_TREE", (void *) tree);
  IupSetAttribute (ih, "_ALL_TOGGLE", (void *) all_toggle);
  IupSetAttribute (ih, "_REFRESH_BUTTON", (void *) refresh_button);

  IupSetAttribute (ih, "_NEXT_FRAME", (void *) next_frame);
  IupSetAttribute (ih, "_NEXT_TOGGLE", (void *) next_toggle);
  IupSetAttribute (ih, "_PREV_BUTTON", (void *) prev_button);
  IupSetAttribute (ih, "_NEXT_BUTTON", (void *) next_button);

  IupSetAttribute (ih, "_SELECTION_FRAME", (void *) selection_frame);
  IupSetAttribute (ih, "_SOURCE_BUTTON", (void *) source_button);
  IupSetAttribute (ih, "_TARGET_BUTTON", (void *) target_button);
  IupSetAttribute (ih, "_ADD_BUTTON", (void *) add_button);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  Ihandle *norm = IupGetHandle ("EVENTS_NORM");
  IupSetAttribute (norm, "NORMALIZE", "BOTH");

  update_target_icons (ih);

  return ih;
}

void
destroy_userdata (Ihandle *ih)
{
  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");

  int count = IupGetInt (tree, "COUNT");
  for (int id = 0; id < count ; id++) {
    void *userdata = IupTreeGetUserId (tree, id);
    al_free (userdata);
    IupTreeSetUserId (tree, id, NULL);
  }
}

int
destroy_cb (Ihandle *ih)
{
  destroy_userdata (ih);

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  al_free (last);

  return IUP_DEFAULT;
}

int
show_cb (Ihandle *ih, int state)
{
  if (state == IUP_SHOW) {
    Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");
    int count = IupGetInt (tree, "COUNT");
    if (! count) _update_tree_cb (ih);
  }
  return IUP_DEFAULT;
}

int
selected_event_node_id (Ihandle *tree)
{
  int id = IupGetInt (tree, "VALUE");
  if (id < 0) return -1;

  int depth = IupGetIntId (tree, "DEPTH", id);
  if (depth < EVENT_DEPTH) return -1;

  if (depth == SOURCE_DEPTH) {
    /* id = IupGetIntId (tree, "PARENT", id); */
    return -1;
  }

  return id;
}

struct level_event *
selected_event (Ihandle *tree)
{
  struct level *level = (void *) IupGetAttribute (tree, "_LEVEL");
  int id = selected_event_node_id (tree);
  if (id < 0) return NULL;
  int *e = IupTreeGetUserId (tree, id);
  return event (level, *e);
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");
  struct level_event *e = selected_event (tree);
  Ihandle *next_frame = (void *) IupGetAttribute (ih, "_NEXT_FRAME");
  gui_control_active (next_frame, e);
  if (e) {
    Ihandle *next_toggle = (void *) IupGetAttribute (ih, "_NEXT_TOGGLE");
    gui_control_int (next_toggle, "VALUE", e->next);

    Ihandle *prev_button = (void *) IupGetAttribute (ih, "_PREV_BUTTON");
    gui_control_active (prev_button, e - level->event > 0);

    Ihandle *next_button = (void *) IupGetAttribute (ih, "_NEXT_BUTTON");
    gui_control_active (next_button, e - level->event < level->event_nmemb - 1);
  }

  Ihandle *selection_frame = (void *) IupGetAttribute (ih, "_SELECTION_FRAME");
  gui_control_active (selection_frame, is_valid_pos (&selection_pos));

  if (is_valid_pos (&selection_pos)) {
    Ihandle *source_button = (void *) IupGetAttribute (ih, "_SOURCE_BUTTON");
    gui_control_active (source_button, e && is_event_fg (&selection_pos));

    Ihandle *target_button = (void *) IupGetAttribute (ih, "_TARGET_BUTTON");
    gui_control_active (target_button, e);
  }

  if ((last->video_mode && strcmp (video_mode, last->video_mode))
      || last->em != em || last->hue != hue)
    update_target_icons (ih);

  if (last->level_n != level->n) _update_tree_cb (ih);

  return IUP_DEFAULT;
}

void
update_target_icons (Ihandle *ih)
{
  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  set_string_var (&last->video_mode, video_mode);
  last->em = em;
  last->hue = hue;

  gui_update ();

  int count = IupGetInt (tree, "COUNT");
  for (int id = 0; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth != TARGET_DEPTH) continue;
    struct pos *np = IupTreeGetUserId (tree, id);
    IupSetAttributeHandleId (tree, "IMAGE", id, tile_icon[fg (np)]);
    IupSetAttributeHandleId (tree, "IMAGEEXPANDED", id, tile_icon[fg (np)]);
  }
}

bool
is_event_active (struct level *l, int e)
{
  return opener_floor_by_event (NULL, e, +1)
    || closer_floor_by_event (NULL, e, +1)
    || (e > 0 && event (l, e - 1)->next
        && is_event_active (l, e - 1));
}

void
add_nodes (Ihandle *tree, bool ignore_inactive)
{
  struct level *level = (void *) IupGetAttribute (tree, "_LEVEL");

  for (int e = 0; e < level->event_nmemb; e++) {
    if (ignore_inactive && ! is_event_active (level, e))
      continue;

    struct pos *p = &event (level, e)->p;
    if (p->room < 0) p->room = 0;

    int target_id = add_target (tree, p);
    int event_id = add_event (tree, target_id, e);

    struct opener_floor *o = NULL;
    do {
      o = opener_floor_by_event (o ? &o->p : NULL, e, +1);
      if (o) add_source (tree, event_id, &o->p);
    } while (o);

    struct closer_floor *c = NULL;
    do {
      c = closer_floor_by_event (c ? &c->p : NULL, e, +1);
      if (c) add_source (tree, event_id, &c->p);
    } while (c);
  }
}

int
_update_tree_cb (Ihandle *ih)
{
  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  last->level_n = level->n;

  /* record current selection */
  int id = IupGetInt (tree, "VALUE");
  char *title_buffer = IupGetAttributeId (tree, "TITLE", id);
  char *selected_title = xasprintf ("%s", title_buffer ? title_buffer : "");
  int selected_depth = IupGetIntId (tree, "DEPTH", id);

  /* build tree */
  IupSetAttribute (tree, "AUTOREDRAW", "NO");

  destroy_userdata (ih);
  IupUnmap (tree);
  IupSetAttribute (tree, "DELNODE", "ALL");
  IupMap (tree);
  IupRefresh (tree);

  Ihandle *all_toggle = (void *) IupGetAttribute (ih, "_ALL_TOGGLE");
  bool all_toggle_value = IupGetInt (all_toggle, "VALUE");
  add_nodes (tree, ! all_toggle_value);

  IupSetAttribute (tree, "AUTOREDRAW", "YES");

  /* recall previous "current" selection */
  select_node_by_title (tree, selected_title, selected_depth);
  al_free (selected_title);

  return IUP_DEFAULT;
}

void
select_node_by_id (Ihandle *tree, int id)
{
  IupSetInt (tree, "VALUE", id);

  /* Motif bug workaround */
  int depth = IupGetIntId (tree, "DEPTH", id);
  if (depth > 0) {
    int parent_id = IupGetIntId (tree, "PARENT", id);
    IupSetAttributeId (tree, "STATE", parent_id, "COLLAPSED");
    IupSetAttributeId (tree, "STATE", parent_id, "EXPANDED");
    IupSetInt (tree, "VALUE", id);
  }
}

void
select_node_by_title (Ihandle *tree, char *s_title, int s_depth)
{
  bool selected = false;

  int count = IupGetInt (tree, "COUNT");
  for (int id = 0; id < count; id++) {
    char *title = IupGetAttributeId (tree, "TITLE", id);
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (s_depth >= 0 && s_depth != depth) continue;
    if (! strcmp (s_title, title)) {
      select_node_by_id (tree, id);
      selected = true;
      break;
    }
  }

  if (! selected) IupSetAttribute (tree, "VALUE", "FIRST");
}

int
target_event (struct pos *p)
{
  if (cutscene) return -1;
  for (size_t e = 0; e < p->l->event_nmemb; e++)
    if (peq (p, &p->l->event[e].p)) return e;
  return -1;
}

int
get_event_node_id (Ihandle *tree, int e)
{
  int count = IupGetInt (tree, "COUNT");
  for (int id = 0; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth != EVENT_DEPTH) continue;
    int *ne = IupTreeGetUserId (tree, id);
    if (e == *ne) return id;
  }
  return -1;
}

int
get_source_node_id (Ihandle *tree, struct pos *p)
{
  int count = IupGetInt (tree, "COUNT");
  for (int id = 0; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth != SOURCE_DEPTH) continue;
    struct pos *np = IupTreeGetUserId (tree, id);
    if (peq (p, np)) return id;
  }
  return -1;
}

int
last_node_id (Ihandle *tree, int parent_id)
{
  int last_id = -1;
  int parent_depth = IupGetIntId (tree, "DEPTH", parent_id);
  int count = IupGetInt (tree, "COUNT");
  for (int id = parent_id + 1; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth == parent_depth + 1) last_id = id;
    if (depth <= parent_depth || id == count - 1)
      return last_id;
  }
  return -1;
}

int
get_target (Ihandle *tree, struct pos *p)
{
  int count = IupGetInt (tree, "COUNT");
  for (int id = 0; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth > TARGET_DEPTH) continue;
    struct pos *np = IupTreeGetUserId (tree, id);
    if (peq (p, np)) return id;
  }
  return -1;
}

int
add_target (Ihandle *tree, struct pos *p)
{
  int id = get_target (tree, p);
  if (id >= 0) return id;
  id = last_node_id (tree, -1);
  IupSetStrfId (tree, "INSERTBRANCH", id,
                "%s (%i,%i,%i)",
                tile_fg_str [fg (p)],
                p->room, p->floor, p->place);
  struct pos *np = xmalloc (sizeof (*np));
  *np = *p;
  id = IupGetInt (tree, "LASTADDNODE");
  IupTreeSetUserId (tree, id, np);
  IupSetAttributeHandleId (tree, "IMAGE", id, tile_icon[fg (p)]);
  IupSetAttributeHandleId (tree, "IMAGEEXPANDED", id, tile_icon[fg (p)]);
  return id;
}

int
get_event (Ihandle *tree, int target_id, int e)
{
  int count = IupGetInt (tree, "COUNT");
  for (int id = target_id + 1; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth > EVENT_DEPTH) continue;
    else if (depth < EVENT_DEPTH) break;
    int *ne = IupTreeGetUserId (tree, id);
    if (e == *ne) return id;
  }
  return -1;
}

int
add_event (Ihandle *tree, int target_id, int e)
{
  int id = get_event (tree, target_id, e);
  if (id >= 0) return id;
  id = last_node_id (tree, target_id);
  const char *command;
  if (id < 0) {
    command = "ADDBRANCH";
    id = target_id;
  } else command = "INSERTBRANCH";
  IupSetStrfId (tree, command, id, "Event %i", e);
  int *ne = xmalloc (sizeof (*ne));
  *ne = e;
  id = IupGetInt (tree, "LASTADDNODE");
  IupTreeSetUserId (tree, id, ne);
  return id;
}

int
get_source (Ihandle *tree, int event_id, struct pos *p)
{
  int count = IupGetInt (tree, "COUNT");
  for (int id = event_id + 1; id < count; id++) {
    int depth = IupGetIntId (tree, "DEPTH", id);
    if (depth < SOURCE_DEPTH) break;
    struct pos *np = IupTreeGetUserId (tree, id);
    if (peq (p, np)) return id;
  }
  return -1;
}

int
add_source (Ihandle *tree, int event_id, struct pos *p)
{
  int id = get_source (tree, event_id, p);
  if (id >= 0) return id;
  id = last_node_id (tree, event_id);
  const char *command;
  if (id < 0) {
    command = "ADDLEAF";
    id = event_id;
  } else command = "INSERTLEAF";
  IupSetStrfId (tree, command, id,
                "%s (%i,%i,%i)",
                tile_fg_str [fg (p)],
                p->room, p->floor, p->place);
  struct pos *np = xmalloc (sizeof (*np));
  *np = *p;
  id = IupGetInt (tree, "LASTADDNODE");
  IupTreeSetUserId (tree, id, np);
  IupSetAttributeId (tree, "IMAGE", id,
                     fg (p) == OPENER_FLOOR
                     ? "TOP_ICON" : "BOTTOM_ICON");
  return id;
}

int
next_toggle_cb (Ihandle *toggle, int state)
{
  Ihandle *tree = (void *) IupGetAttribute (toggle, "_TREE");
  struct level_event *e = selected_event (tree);
  if (e) {
    struct level *level = (void *) IupGetAttribute (toggle, "_LEVEL");
    int n = e - level->event;
    register_event_undo (&undo, n, &e->p, state, "NEXT EVENT");
    _update_tree_cb (tree);
  }
  else return IUP_DEFAULT;
  return IUP_DEFAULT;
}

int
next_button_cb (Ihandle *button)
{
  Ihandle *next_button = (void *) IupGetAttribute (button, "_NEXT_BUTTON");
  int d = button == next_button ? +1 : - 1;
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
  Ihandle *tree = (void *) IupGetAttribute (button, "_TREE");
  struct level_event *e = selected_event (tree);
  int n = e - level->event;
  int id = get_event_node_id (tree, n + d);
  if (id < 0) return IUP_DEFAULT;
  select_node_by_id (tree, id);
  return IUP_DEFAULT;
}

int
source_button_cb (Ihandle *button)
{
  if (! is_valid_pos (&selection_pos)
      || ! is_event_fg (&selection_pos)) return IUP_DEFAULT;

  /* get event selection */
  Ihandle *tree = (void *) IupGetAttribute (button, "_TREE");
  struct level_event *e = selected_event (tree);

  /* change floor extension */
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
  int n = e - level->event;
  change_tile_ext (&selection_pos, n);

  /* update */
  _update_tree_cb (tree);
  IupSetAttribute (tree, "STATE", "EXPANDED");

  return IUP_DEFAULT;
}

int
target_button_cb (Ihandle *button)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;

  /* get event selection */
  Ihandle *tree = (void *) IupGetAttribute (button, "_TREE");
  struct level_event *e = selected_event (tree);

  /* change event target */
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
  int n = e - level->event;
  register_event_undo (&undo, n, &selection_pos, e->next, "EVENT TARGET");

  /* update */
  _update_tree_cb (tree);

  return IUP_DEFAULT;
}

int
new_event (Ihandle *ih, struct pos *p, bool next)
{
  struct level_event event;
  event.p = *p;
  event.next = next;

  p->l->event =
    add_to_array (&event, 1, p->l->event, &p->l->event_nmemb,
                  p->l->event_nmemb, sizeof (*p->l->event));

  int e = p->l->event_nmemb - 1;

  if (! is_event_active (p->l, e)) {
    Ihandle *all_toggle = (void *) IupGetAttribute (ih, "_ALL_TOGGLE");
    IupSetInt (all_toggle, "VALUE", true);
  }

  _update_tree_cb (ih);

  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");

  int id = get_event_node_id (tree, e);
  select_node_by_id (tree, id);
  select_pos (&global_mr, p);

  return e;
}

int
add_button_cb (Ihandle *button)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;
  new_event (button, &selection_pos, false);
  return IUP_DEFAULT;
}

int
selection_cb (Ihandle *tree, int id, int status)
{
  if (! status) return IUP_DEFAULT;

  int depth = IupGetIntId (tree, "DEPTH", id);
  if (depth != EVENT_DEPTH) {
    struct pos *p = IupTreeGetUserId (tree, id);
    select_pos (&global_mr, p);
  }

  return IUP_DEFAULT;
}

int
_select_target_cb (Ihandle *ih, struct pos *p)
{
  int e = target_event (p);
  if (e < 0) return IUP_DEFAULT;

  Ihandle *tabs = IupGetDialogChild (ih, "TABS_CONTROL");
  IupSetAttribute (tabs, "VALUE_HANDLE", (void *) ih);

  if (! is_event_active (p->l, e)) {
    Ihandle *all_toggle = (void *) IupGetAttribute (ih, "_ALL_TOGGLE");
    IupSetInt (all_toggle, "VALUE", true);
  }

  _update_tree_cb (ih);

  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");

  int id = get_target (tree, p);
  select_node_by_id (tree, id);
  select_pos (&global_mr, p);

  return IUP_DEFAULT;
}

int
_select_source_cb (Ihandle *ih, struct pos *p)
{
  if (! is_event_fg (p)) return IUP_DEFAULT;

  Ihandle *tabs = IupGetDialogChild (ih, "TABS_CONTROL");
  IupSetAttribute (tabs, "VALUE_HANDLE", (void *) ih);

  _update_tree_cb (ih);

  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");

  int id = get_source_node_id (tree, p);
  select_node_by_id (tree, id);
  select_pos (&global_mr, p);

  return IUP_DEFAULT;
}

int
_add_event_cb (Ihandle *ih, struct pos *p)
{
  Ihandle *tabs = IupGetDialogChild (ih, "TABS_CONTROL");
  IupSetAttribute (tabs, "VALUE_HANDLE", (void *) ih);
  new_event (ih, p, false);
  return IUP_DEFAULT;
}
