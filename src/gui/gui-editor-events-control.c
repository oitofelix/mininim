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

struct last {
  char *video_mode;
  enum em em;
  enum hue hue;
  int level_n;
};

enum {
  TARGET_DEPTH, EVENT_DEPTH, SOURCE_DEPTH,
};

static int destroy_cb (Ihandle *ih);
static int show_cb (Ihandle *ih, int state);

static struct level_event *selected_event (Ihandle *tree_ctrl);

static int _update_cb (Ihandle *ih);

/* tree build */
static bool is_event_active (struct level *l, int e);
static int count_inactive_events (struct level *l);
static int count_fragmented_events (struct level *l);
static int _update_tree_cb (Ihandle *ih);
static void update_tree_ctrl (Ihandle *ih, struct tree *tree);
static void update_target_icons (Ihandle *ih);
static void populate_event_tree (struct level *level, struct tree *tree,
                                 bool ignore_inactive);

/* auxiliary */
int new_event (Ihandle *ih, struct pos *p, bool next);

/* callbacks */
static int next_toggle_cb (Ihandle *toggle, int state);
static int next_button_cb (Ihandle *button);

static int source_button_cb (Ihandle *button);
static int target_button_cb (Ihandle *button);
static int add_button_cb (Ihandle *button);

static int defrag_button_cb (Ihandle *button);
static int clean_button_cb (Ihandle *button);

static int selection_cb (Ihandle *ih, int id, int status);

static int _select_target_cb (Ihandle *ih, struct pos *p);
static int _select_source_cb (Ihandle *ih, struct pos *p);
static int _add_event_cb (Ihandle *ih, struct pos *p);


Ihandle *
gui_create_editor_events_control (char *norm_group, struct level *level)
{
  Ihandle *ih, *tree_ctrl;

  Ihandle *refresh_button, *inactive_toggle;

  Ihandle *next_frame, *next_toggle, *prev_button, *next_button;

  Ihandle *selection_frame, *source_button, *target_button, *add_button;

  Ihandle *sanitation_frame, *defrag_button, *clean_button,
    *total_label, *inactive_label, *fragmented_label;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupHbox
      (tree_ctrl = IupSetCallbacks
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
             (inactive_toggle = IupSetCallbacks
              (IupSetAttributes
               (IupToggle ("Inactive", NULL),
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
          "TITLE = \"Trigger next\","
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

         sanitation_frame = IupSetAttributes
         (IupFrame
          (IupVbox
           (IupFill (),

            IupSetAttributes
             (IupGridBox
              (
               IupSetAttributes
               (IupLabel ("Total: "),
                "TIP = \"Total number of events\","
                "ALIGNMENT = ARIGHT"),

               total_label = IupSetAttributes
               (IupLabel ("000"),
                "ALIGNMENT = ARIGHT"),

               IupSetAttributes
               (IupLabel ("Inactive: "),
                "TIP = \"Number of inactive events\","
                "ALIGNMENT = ARIGHT"),

               inactive_label = IupSetAttributes
               (IupLabel ("000"),
                "ALIGNMENT = ARIGHT"),

               IupSetAttributes
               (IupLabel ("Fragmented: "),
                "TIP = \"Number of fragmented events\","
                "ALIGNMENT = ARIGHT"),

               fragmented_label = IupSetAttributes
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
                "IMAGE = DEFRAG_ICON,"
                "TIP = \"Defragment inactive events\","),
               "ACTION", defrag_button_cb,
               NULL),

              IupFill (),

              clean_button =
              IupSetCallbacks
              (IupSetAttributes
               (IupButton (NULL, NULL),
                "IMAGE = CLEAN_ICON,"
                "TIP = \"Delete trailing inactive events\","),
               "ACTION", clean_button_cb,
               NULL),

              IupFill (),

              NULL),
             "ALIGNMENT = ACENTER,"
             "NORMALIZERGROUP = EVENTS_NORM,"),

            IupFill (),

            NULL)),
          "TITLE = Sanitation,"),

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

  IupSetAttribute (tree_ctrl, "IMAGELEAF", "EVENT_ICON");
  IupSetAttribute (tree_ctrl, "IMAGEBRANCHCOLLAPSED", "EVENT_ICON");
  IupSetAttribute (tree_ctrl, "IMAGEBRANCHEXPANDED", "EVENT_ICON");

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_CONTROL", (void *) ih);

  IupSetAttribute (ih, "_LEVEL", (void *) level);

  struct tree *tree = xmalloc (sizeof (*tree));
  memset (tree, 0, sizeof (*tree));
  IupSetAttribute (ih, "_TREE", (void *) tree);
  IupSetAttribute (ih, "_TREE_CTRL", (void *) tree_ctrl);

  IupSetAttribute (ih, "_INACTIVE_TOGGLE", (void *) inactive_toggle);
  IupSetAttribute (ih, "_REFRESH_BUTTON", (void *) refresh_button);

  IupSetAttribute (ih, "_NEXT_FRAME", (void *) next_frame);
  IupSetAttribute (ih, "_NEXT_TOGGLE", (void *) next_toggle);
  IupSetAttribute (ih, "_PREV_BUTTON", (void *) prev_button);
  IupSetAttribute (ih, "_NEXT_BUTTON", (void *) next_button);

  IupSetAttribute (ih, "_SELECTION_FRAME", (void *) selection_frame);
  IupSetAttribute (ih, "_SOURCE_BUTTON", (void *) source_button);
  IupSetAttribute (ih, "_TARGET_BUTTON", (void *) target_button);
  IupSetAttribute (ih, "_ADD_BUTTON", (void *) add_button);

  IupSetAttribute (ih, "_SANITATION_FRAME", (void *) sanitation_frame);
  IupSetAttribute (ih, "_DEFRAG_BUTTON", (void *) defrag_button);
  IupSetAttribute (ih, "_CLEAN_BUTTON", (void *) clean_button);

  IupSetAttribute (ih, "_TOTAL_LABEL", (void *) total_label);
  IupSetAttribute (ih, "_INACTIVE_LABEL", (void *) inactive_label);
  IupSetAttribute (ih, "_FRAGMENTED_LABEL", (void *) fragmented_label);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  Ihandle *norm = IupGetHandle ("EVENTS_NORM");
  IupSetAttribute (norm, "NORMALIZE", "HORIZONTAL");

  update_target_icons (ih);

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

struct level_event *
selected_event (Ihandle *tree_ctrl)
{
  int id = IupGetInt (tree_ctrl, "VALUE");
  if (id < 0) return NULL;
  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  if (depth != EVENT_DEPTH) return NULL;
  struct tree *tree = (void *) IupGetAttribute (tree_ctrl, "_TREE");
  int *e = tree->node[id].data;
  struct level *level = (void *) IupGetAttribute (tree_ctrl, "_LEVEL");
  return event (level, *e);
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  Ihandle *inactive_toggle =
    (void *) IupGetAttribute (ih, "_INACTIVE_TOGGLE");
  bool inactive_toggle_value = IupGetInt (inactive_toggle, "VALUE");
  struct tree new_tree;
  populate_event_tree (level, &new_tree, ! inactive_toggle_value);
  if (! tree_eq (tree, &new_tree)) update_tree_ctrl (ih, &new_tree);
  else destroy_tree (&new_tree);

  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  struct level_event *e = selected_event (tree_ctrl);
  Ihandle *next_frame = (void *) IupGetAttribute (ih, "_NEXT_FRAME");
  gui_control_active (next_frame, e);
  if (e) {
    Ihandle *next_toggle = (void *) IupGetAttribute (ih, "_NEXT_TOGGLE");
    gui_control_int (next_toggle, "VALUE", e->next);
    gui_control_active
      (next_toggle, e - level->event < level->event_nmemb - 1);

    Ihandle *prev_button = (void *) IupGetAttribute (ih, "_PREV_BUTTON");
    gui_control_active (prev_button, e - level->event > 0);

    Ihandle *next_button = (void *) IupGetAttribute (ih, "_NEXT_BUTTON");
    gui_control_active
      (next_button, e - level->event < level->event_nmemb - 1);
  }

  Ihandle *selection_frame =
    (void *) IupGetAttribute (ih, "_SELECTION_FRAME");
  gui_control_active (selection_frame, is_valid_pos (&selection_pos));

  if (is_valid_pos (&selection_pos)) {
    Ihandle *source_button = (void *) IupGetAttribute (ih, "_SOURCE_BUTTON");
    gui_control_active (source_button, e && is_event_fg (&selection_pos));

    Ihandle *target_button = (void *) IupGetAttribute (ih, "_TARGET_BUTTON");
    gui_control_active (target_button, e);
  }

  Ihandle *total_label = (void *) IupGetAttribute (ih, "_TOTAL_LABEL");
  if (gui_control_attribute_strf
      (total_label, "TITLE", "%zu", level->event_nmemb))
    IupRefresh (total_label);

  Ihandle *inactive_label = (void *) IupGetAttribute (ih, "_INACTIVE_LABEL");
  if (gui_control_attribute_strf
      (inactive_label, "TITLE", "%i", count_inactive_events (level)))
    IupRefresh (inactive_label);

  Ihandle *fragmented_label = (void *) IupGetAttribute (ih, "_FRAGMENTED_LABEL");
  if (gui_control_attribute_strf
      (fragmented_label, "TITLE", "%i", count_fragmented_events (level)))
    IupRefresh (fragmented_label);

  if ((last->video_mode && strcmp (video_mode, last->video_mode))
      || last->em != em || last->hue != hue)
    update_target_icons (ih);

  if (last->level_n != level->n) _update_tree_cb (ih);

  return IUP_DEFAULT;
}

void
update_target_icons (Ihandle *ih)
{
  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  set_string_var (&last->video_mode, video_mode);
  last->em = em;
  last->hue = hue;

  gui_update ();

  for (int id = 0; id < tree->nmemb; id++) {
    if (tree->node[id].depth != TARGET_DEPTH) continue;
    struct pos *np = tree->node[id].data;
    IupSetAttributeHandleId (tree_ctrl, "IMAGE", id, tile_icon[fg (np)]);
    IupSetAttributeHandleId (tree_ctrl, "IMAGEEXPANDED", id,
                             tile_icon[fg (np)]);
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

int
count_inactive_events (struct level *l)
{
  int count = 0;
  for (int e = 0; e < l->event_nmemb; e++)
    if (! is_event_active (l, e)) count++;
  return count;
}

int
count_fragmented_events (struct level *l)
{
  int count = 0;
  bool within = false;
  for (int e = l->event_nmemb - 1; e >= 0; e--) {
    if (! within && is_event_active (l, e)) within = true;
    if (within && ! is_event_active (l, e)) count++;
  }
  return count;
}

void
populate_event_tree (struct level *level, struct tree *tree,
                     bool ignore_inactive)
{
  tree->node = NULL;
  tree->nmemb = 0;

  for (int e = 0; e < level->event_nmemb; e++) {
    if (ignore_inactive && ! is_event_active (level, e))
      continue;

    struct pos *p = &event (level, e)->p;
    if (p->room < 0) p->room = 0;

    int target_id = get_or_put_tree_branch_child
      (tree, -1, TREE_NODE_TYPE_BRANCH, p, sizeof (*p),
       (m_comparison_fn_t) peq);
    int event_id = get_or_put_tree_branch_child
      (tree, target_id, TREE_NODE_TYPE_BRANCH, &e, sizeof (e),
       (m_comparison_fn_t) int_eq);

    struct opener_floor *o = NULL;
    do {
      o = opener_floor_by_event (o ? &o->p : NULL, e, +1);
      if (o)
        get_or_put_tree_branch_child
          (tree, event_id, TREE_NODE_TYPE_LEAF, &o->p, sizeof (o->p),
           (m_comparison_fn_t) peq);
    } while (o);

    struct closer_floor *c = NULL;
    do {
      c = closer_floor_by_event (c ? &c->p : NULL, e, +1);
      if (c)
        get_or_put_tree_branch_child
          (tree, event_id, TREE_NODE_TYPE_LEAF, &c->p, sizeof (c->p),
           (m_comparison_fn_t) peq);
    } while (c);
  }
}

void
populate_event_tree_ctrl (Ihandle *tree_ctrl, struct tree *tree)
{
  populate_tree_ctrl (tree_ctrl, tree);
  for (size_t id = 0; id < tree->nmemb; id++) {
    switch (tree->node[id].depth) {
    case TARGET_DEPTH: {
      struct pos *p = tree->node[id].data;
      IupSetStrfId (tree_ctrl, "TITLE", id, "%s (%i,%i,%i)",
                    tile_fg_str [fg (p)], p->room, p->floor, p->place);
      IupSetAttributeHandleId (tree_ctrl, "IMAGE", id, tile_icon[fg (p)]);
      IupSetAttributeHandleId (tree_ctrl, "IMAGEEXPANDED", id,
                               tile_icon[fg (p)]);
      break;
    }
    case EVENT_DEPTH: {
      int *e = tree->node[id].data;
      IupSetStrfId (tree_ctrl, "TITLE", id, "Event %i", *e);
      break;
    }
    case SOURCE_DEPTH: {
      struct pos *p = tree->node[id].data;
      IupSetStrfId (tree_ctrl, "TITLE", id, "%s (%i,%i,%i)",
                    tile_fg_str [fg (p)],
                    p->room, p->floor, p->place);
      IupSetAttributeId (tree_ctrl, "IMAGE", id,
                         fg (p) == OPENER_FLOOR
                         ? "TOP_ICON" : "BOTTOM_ICON");
      break;
    }
    default: assert (false);
    }
  }
}

int
_update_tree_cb (Ihandle *ih)
{
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  Ihandle *inactive_toggle =
    (void *) IupGetAttribute (ih, "_INACTIVE_TOGGLE");
  bool inactive_toggle_value = IupGetInt (inactive_toggle, "VALUE");
  struct tree new_tree;
  populate_event_tree (level, &new_tree, ! inactive_toggle_value);
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
  last->level_n = level->n;

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
  populate_event_tree_ctrl (tree_ctrl, tree);
  IupRefresh (tree_ctrl);
  IupRedraw (tree_ctrl, true);
  IupSetAttribute (tree_ctrl, "AUTOREDRAW", "YES");

  /* recall previous "current" selection */
  select_node_by_title (tree_ctrl, selected_title, selected_depth);
  al_free (selected_title);
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
next_toggle_cb (Ihandle *toggle, int state)
{
  Ihandle *tree_ctrl = (void *) IupGetAttribute (toggle, "_TREE_CTRL");
  struct level_event *e = selected_event (tree_ctrl);
  if (e) {
    struct level *level = (void *) IupGetAttribute (toggle, "_LEVEL");
    int n = e - level->event;

    /* try to economize on undo registration */
    bool should_register_undo = true;
    if (undo.pass[undo.current].f == (undo_f) event_undo) {
      struct event_undo *d = undo.pass[undo.current].data;
      if (d->b_event_nmemb == d->f_event_nmemb
          && d->f_event[n].next != d->b_event[n].next) {
        bool old_next = d->f_event[n].next;
        d->f_event[n].next = d->b_event[n].next;
        if (! memcmp (d->f_event, d->b_event, d->f_event_nmemb
                      * sizeof (*d->f_event))) {
          d->f_event[n].next = old_next;
          undo_pass (&undo, -1, NULL);
          should_register_undo = false;
        } else d->f_event[n].next = old_next;
      }
    }

    if (should_register_undo) {
      struct level_event *event;
      size_t event_nmemb;
      event = copy_array (level->event, level->event_nmemb,
                          &event_nmemb, sizeof (*event));

      event[n].p = e->p;
      event[n].next = state;

      char *desc = xasprintf ("NEXT EVENT %i", n);
      register_event_undo (&undo, event, event_nmemb, desc);
      destroy_array ((void **) &event, &event_nmemb);
      al_free (desc);
    }

    _update_tree_cb (tree_ctrl);
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
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");
  struct level_event *e = selected_event (tree_ctrl);
  if (! e) return IUP_DEFAULT;
  int n = e - level->event;
  struct tree *tree = (void *) IupGetAttribute (button, "_TREE");
  int next = n + d;
  int id = get_tree_node_id_by_data (tree, EVENT_DEPTH, &next);
  if (id < 0) return IUP_DEFAULT;
  select_node_by_id (tree_ctrl, id);
  return IUP_DEFAULT;
}

int
source_button_cb (Ihandle *button)
{
  if (! is_valid_pos (&selection_pos)
      || ! is_event_fg (&selection_pos)) return IUP_DEFAULT;

  /* get event selection */
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");
  struct level_event *e = selected_event (tree_ctrl);
  if (! e) return IUP_DEFAULT;

  /* change floor extension */
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");
  int n = e - level->event;
  change_tile_ext (&selection_pos, n);

  /* update */
  _update_tree_cb (tree_ctrl);
  IupSetAttribute (tree_ctrl, "STATE", "EXPANDED");

  return IUP_DEFAULT;
}

int
target_button_cb (Ihandle *button)
{
  if (! is_valid_pos (&selection_pos)) return IUP_DEFAULT;

  /* get event selection */
  Ihandle *tree_ctrl = (void *) IupGetAttribute (button, "_TREE_CTRL");
  struct level_event *e = selected_event (tree_ctrl);
  if (! e) return IUP_DEFAULT;

  /* change event target */
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");

  struct level_event *event;
  size_t event_nmemb;
  event = copy_array (level->event, level->event_nmemb,
                      &event_nmemb, sizeof (*event));

  int n = e - level->event;

  event[n].p = selection_pos;
  event[n].next = e->next;

  char *desc = xasprintf ("EVENT %i TARGET", n);
  register_event_undo (&undo, event, event_nmemb, desc);
  destroy_array ((void **) &event, &event_nmemb);
  al_free (desc);

  _update_tree_cb (tree_ctrl);

  return IUP_DEFAULT;
}

int
new_event (Ihandle *ih, struct pos *p, bool next)
{
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");

  struct level_event *event;
  size_t event_nmemb;
  event = copy_array (level->event, level->event_nmemb,
                      &event_nmemb, sizeof (*event));

  struct level_event event_n;
  event_n.p = *p;
  event_n.next = next;

  event = add_to_array (&event_n, 1, event, &event_nmemb,
                        event_nmemb, sizeof (*event));

  int e = event_nmemb - 1;
  char *desc = xasprintf ("NEW EVENT %i", e);
  register_event_undo (&undo, event, event_nmemb, desc);
  destroy_array ((void **) &event, &event_nmemb);
  al_free (desc);

  if (! is_event_active (p->l, e)) {
    Ihandle *inactive_toggle =
      (void *) IupGetAttribute (ih, "_INACTIVE_TOGGLE");
    IupSetInt (inactive_toggle, "VALUE", true);
  }

  _update_tree_cb (ih);

  select_pos (&global_mr, p);

  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  int id = get_tree_node_id_by_data (tree, EVENT_DEPTH, &e);
  select_node_by_id (tree_ctrl, id);

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
defrag_button_cb (Ihandle *button)
{
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");

  int count = count_fragmented_events (level);
  if (! count) return IUP_DEFAULT;

  char *desc = xasprintf ("DEFRAGMENT %i INACTIVE EVENTS", count);

  struct level_event *event;
  size_t event_nmemb;
  event = copy_array (level->event, level->event_nmemb,
                      &event_nmemb, sizeof (*event));

  int e0, e1;

  for (e0 = 0;
       e0 < event_nmemb && is_event_active (level, e0);
       e0++);

  for (e1 = e0 + 1; e1 < event_nmemb; e1++)
    if (is_event_active (level, e1))
      do {
        struct opener_floor *o = NULL;
        do {
          o = opener_floor_by_event (o ? &o->p : NULL, e1, +1);
          if (o)
            register_tile_undo (&undo, &o->p,
                                MIGNORE, MIGNORE, e0, MIGNORE,
                                NULL, (intptr_t) NULL, NULL);
        } while (o);

        struct closer_floor *c = NULL;
        do {
          c = closer_floor_by_event (c ? &c->p : NULL, e1, +1);
          if (c)
            register_tile_undo (&undo, &c->p,
                                MIGNORE, MIGNORE, e0, MIGNORE,
                                NULL, (intptr_t) NULL, NULL);
        } while (c);

        struct level_event event_bkp = event[e0];
        event[e0] = event[e1];
        event[e1] = event_bkp;
        e0++;
      } while (e1 < event_nmemb - 1 && event[e0 - 1].next && e1++);

  register_event_undo (&undo, event, event_nmemb, NULL);
  end_undo_set (&undo, desc);
  destroy_array ((void **) &event, &event_nmemb);
  al_free (desc);

  _update_tree_cb (button);

  return IUP_DEFAULT;
}

int
clean_button_cb (Ihandle *button)
{
  struct level *level = (void *) IupGetAttribute (button, "_LEVEL");

  int e;
  for (e = level->event_nmemb - 1;
       e >= 0 && ! is_event_active (level, e);
       e--);

  struct level_event *event;
  size_t event_nmemb;
  event = copy_array (level->event, e + 1,
                      &event_nmemb, sizeof (*event));

  char *desc = xasprintf ("DELETE %zi INACTIVE EVENTS",
                          level->event_nmemb - e - 1);
  register_event_undo (&undo, event, event_nmemb, desc);
  destroy_array ((void **) &event, &event_nmemb);
  al_free (desc);

  return IUP_DEFAULT;
}

int
selection_cb (Ihandle *tree_ctrl, int id, int status)
{
  if (! status) return IUP_DEFAULT;

  int depth = IupGetIntId (tree_ctrl, "DEPTH", id);
  if (depth != EVENT_DEPTH) {
    struct tree *tree = (void *) IupGetAttribute (tree_ctrl, "_TREE");
    struct pos *p = tree->node[id].data;
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
    Ihandle *inactive_toggle =
      (void *) IupGetAttribute (ih, "_INACTIVE_TOGGLE");
    IupSetInt (inactive_toggle, "VALUE", true);
  }

  _update_tree_cb (ih);

  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  int id = get_tree_node_id_by_data (tree, TARGET_DEPTH, p);
  select_node_by_id (tree_ctrl, id);
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

  struct tree *tree = (void *) IupGetAttribute (ih, "_TREE");
  Ihandle *tree_ctrl = (void *) IupGetAttribute (ih, "_TREE_CTRL");
  int id = get_tree_node_id_by_data (tree, SOURCE_DEPTH, p);
  select_node_by_id (tree_ctrl, id);
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
