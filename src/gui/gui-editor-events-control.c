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
static int _update_cb (Ihandle *ih);
static int _update_tree_cb (Ihandle *ih);
static void update_target_icons (Ihandle *ih);

static int last_node_id (Ihandle *tree, int parent_id);

static int get_target (Ihandle *tree, struct pos *p);
static int add_target (Ihandle *tree, struct pos *p);

static int get_event (Ihandle *tree, int target_id, int e);
static int add_event (Ihandle *tree, int target_id, int e);

static int get_source (Ihandle *tree, int event_id, struct pos *p);
static int add_source (Ihandle *tree, int event_id, struct pos *p);

static int selection_cb (Ihandle *ih, int id, int status);


Ihandle *
gui_create_editor_events_control (char *norm_group, struct level *level)
{
  Ihandle *ih;

  Ihandle *tree, *refresh_button;

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

       refresh_button =
       IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = RELOAD_ICON,"
         "TIP = \"Refresh\","),
        "ACTION", _update_tree_cb,
        NULL),

       NULL),
      "ALIGNMENT = ACENTER,"),
     "DESTROY_CB", destroy_cb,
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     "_UPDATE_TREE_CB", _update_tree_cb,
     NULL);

  IupSetAttribute (tree, "IMAGELEAF", "EVENT_ICON");
  IupSetAttribute (tree, "IMAGEBRANCHCOLLAPSED", "EVENT_ICON");
  IupSetAttribute (tree, "IMAGEBRANCHEXPANDED", "EVENT_ICON");

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_LEVEL", (void *) level);
  IupSetAttribute (ih, "_TREE", (void *) tree);
  IupSetAttribute (ih, "_REFRESH_BUTTON", (void *) refresh_button);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

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
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

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

int
_update_tree_cb (Ihandle *ih)
{
  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  last->level_n = level->n;

  int value = IupGetInt (tree, "VALUE");

  IupSetAttribute (tree, "AUTOREDRAW", "NO");

  destroy_userdata (ih);
  IupSetAttribute (tree, "DELNODE0", "ALL");

  for (int e = 0; e < level->event_nmemb; e++) {
    struct pos *p = &event (level, e)->p;
    if (p->room < 0) p->room = 0;
    int target_id = add_target (tree, p);
    int event_id = add_event (tree, target_id, e);

    for (int i = 0; i < opener_floor_nmemb; i++) {
      struct opener_floor *o = &opener_floor[i];
      if (ext (&o->p) == e) add_source (tree, event_id, &o->p);
    }

    for (int i = 0; i < closer_floor_nmemb; i++) {
      struct closer_floor *c = &closer_floor[i];
      if (ext (&c->p) == e) add_source (tree, event_id, &c->p);
    }
  }

  IupSetAttribute (tree, "AUTOREDRAW", "YES");

  IupSetInt (tree, "VALUE", value);

  return IUP_DEFAULT;
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
  } else command = "INSERTLEAFT";
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

int selection_cb (Ihandle *tree, int id, int status)
{
  return IUP_DEFAULT;
}
