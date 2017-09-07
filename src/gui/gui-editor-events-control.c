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
};

static void destroy_userdata (Ihandle *ih);
static int destroy_cb (Ihandle *ih);
static int show_cb (Ihandle *ih, int state);
static int _update_cb (Ihandle *ih);
static void update (Ihandle *ih);
static void update_target_icons (Ihandle *ih);

static int get_target (Ihandle *ih, struct pos *p);
static int last_target_id (Ihandle *ih);
static int add_target (Ihandle *ih, struct pos *p);

static int get_event (Ihandle *ih, int target_id, int e);
static int last_event_id (Ihandle *ih, int target_id);
static int add_event (Ihandle *ih, int target_id, int e);

static int get_source (Ihandle *ih, int event_id, struct pos *p);
static int last_source_id (Ihandle *ih, int event_id);
static int add_source (Ihandle *ih, int event_id, struct pos *p);


Ihandle *
gui_create_editor_events_control (char *norm_group, struct level *level)
{
  Ihandle *ih;

  Ihandle *tree;

  ih = IupSetCallbacks
    (IupSetAttributes
     (tree = IupTree (),
      "ADDROOT = NO,"
      "ADDEXPANDED = NO,"),
     "DESTROY_CB", destroy_cb,
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "IMAGELEAF", "EVENT_ICON");
  IupSetAttribute (ih, "IMAGEBRANCHCOLLAPSED", "EVENT_ICON");
  IupSetAttribute (ih, "IMAGEBRANCHEXPANDED", "EVENT_ICON");

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_LEVEL", (void *) level);
  IupSetAttribute (ih, "_TREE", (void *) tree);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  update_target_icons (ih);

  return ih;
}

void
destroy_userdata (Ihandle *ih)
{
  int count = IupGetInt (ih, "COUNT");
  for (int id = 0; id < count ; id++) {
    void *userdata = (void *) IupGetAttributeId (ih, "USERDATA", id);
    al_free (userdata);
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
    if (! count) update (tree);
  }
  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  if ((last->video_mode && strcmp (video_mode, last->video_mode))
      || last->em != em || last->hue != hue)
    update_target_icons (tree);

  return IUP_DEFAULT;
}

void
update_target_icons (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  set_string_var (&last->video_mode, video_mode);
  last->em = em;
  last->hue = hue;

  gui_update ();

  int count = IupGetInt (ih, "COUNT");
  for (int id = 0; id < count; id++) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth != TARGET_DEPTH) continue;
    struct pos *np = (void *) IupGetAttributeId (ih, "USERDATA", id);
    IupSetAttributeHandleId (ih, "IMAGE", id, tile_icon[fg (np)]);
    IupSetAttributeHandleId (ih, "IMAGEEXPANDED", id, tile_icon[fg (np)]);
  }
}

void
update (Ihandle *ih)
{
  struct level *level = (void *) IupGetAttribute (ih, "_LEVEL");
  Ihandle *tree = (void *) IupGetAttribute (ih, "_TREE");

  destroy_userdata (ih);
  IupSetAttribute (tree, "DELNODE0", "ALL");

  for (int e = 0; e < EVENTS; e++) {
    struct pos *p = &event (level, e)->p;
    if (p->room > 0) {
      int target_id = add_target (ih, p);

      IupSetAttributeHandleId (ih, "IMAGE", target_id, tile_icon[fg (p)]);
      IupSetAttributeHandleId (ih, "IMAGEEXPANDED", target_id, tile_icon[fg (p)]);

      int event_id = add_event (ih, target_id, e);

      for (int i = 0; i < opener_floor_nmemb; i++) {
        struct opener_floor *o = &opener_floor[i];
        if (ext (&o->p) == e) {
          int source_id = add_source (ih, event_id, &o->p);
          IupSetAttributeId (ih, "IMAGE", source_id, "TOP_ICON");
        }
      }

      for (int i = 0; i < closer_floor_nmemb; i++) {
        struct closer_floor *c = &closer_floor[i];
        if (ext (&c->p) == e) {
          int source_id = add_source (ih, event_id, &c->p);
          IupSetAttributeId (ih, "IMAGE", source_id, "BOTTOM_ICON");
        }
      }
    }
  }
}

int
get_target (Ihandle *ih, struct pos *p)
{
  int count = IupGetInt (ih, "COUNT");
  for (int id = 0; id < count; id++) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth != TARGET_DEPTH) continue;
    struct pos *np = (void *) IupGetAttributeId (ih, "USERDATA", id);
    if (peq (p, np)) return id;
  }
  return -1;
}

int
last_target_id (Ihandle *ih)
{
  IupSetAttribute (ih, "VALUE", "LAST");
  for (int id = IupGetInt (ih, "VALUE"); id >= 0; id--) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth == TARGET_DEPTH) return id;
  }
  return -1;
}

int
add_target (Ihandle *ih, struct pos *p)
{
  int id = get_target (ih, p);
  if (id >= 0) return id;
  id = last_target_id (ih);
  IupSetStrfId (ih, "INSERTBRANCH", id,
                "%s (%i,%i,%i)",
                tile_fg_str [fg (p)],
                p->room, p->floor, p->place);
  struct pos *np = xmalloc (sizeof (*np));
  *np = *p;
  id = IupGetInt (ih, "LASTADDNODE");
  IupSetAttributeId (ih, "USERDATA", id, (void *) np);
  return id;
}

int
get_event (Ihandle *ih, int target_id, int e)
{
  int count = IupGetInt (ih, "COUNT");
  for (int id = target_id + 1; id < count; id++) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth != EVENT_DEPTH) break;
    int *ne = (void *) IupGetAttributeId (ih, "USERDATA", id);
    if (e == *ne) return id;
  }
  return -1;
}

int
last_event_id (Ihandle *ih, int target_id)
{
  int count = IupGetInt (ih, "COUNT");
  for (int id = target_id + 1; id < count; id++) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth != EVENT_DEPTH)
      return (id - 1 > target_id) ? id - 1 : -1;
  }
  return -1;
}

int
add_event (Ihandle *ih, int target_id, int e)
{
  int id = get_event (ih, target_id, e);
  if (id >= 0) return id;
  id = last_event_id (ih, target_id);
  const char *command;
  if (id < 0) {
    command = "ADDBRANCH";
    id = target_id;
  } else command = "INSERTBRANCH";
  IupSetStrfId (ih, command, id, "Event %i", e);
  int *ne = xmalloc (sizeof (*ne));
  *ne = e;
  id = IupGetInt (ih, "LASTADDNODE");
  IupSetAttributeId (ih, "USERDATA", id, (void *) ne);
  return id;
}

int
get_source (Ihandle *ih, int event_id, struct pos *p)
{
  int count = IupGetInt (ih, "COUNT");
  for (int id = event_id + 1; id < count; id++) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth != SOURCE_DEPTH) break;
    struct pos *np = (void *) IupGetAttributeId (ih, "USERDATA", id);
    if (peq (p, np)) return id;
  }
  return -1;
}

int
last_source_id (Ihandle *ih, int event_id)
{
  int count = IupGetInt (ih, "COUNT");
  for (int id = event_id + 1; id < count; id++) {
    int depth = IupGetIntId (ih, "DEPTH", id);
    if (depth != SOURCE_DEPTH)
      return (id - 1 > event_id) ? id - 1 : -1;
  }
  return -1;
}

int
add_source (Ihandle *ih, int event_id, struct pos *p)
{
  int id = get_source (ih, event_id, p);
  if (id >= 0) return id;
  id = last_source_id (ih, event_id);
  const char *command;
  if (id < 0) {
    command = "ADDLEAF";
    id = event_id;
  } else command = "INSERTLEAFT";
  IupSetStrfId (ih, command, id,
                "%s (%i,%i,%i)",
                tile_fg_str [fg (p)],
                p->room, p->floor, p->place);
  struct pos *np = xmalloc (sizeof (*np));
  *np = *p;
  id = IupGetInt (ih, "LASTADDNODE");
  IupSetAttributeId (ih, "USERDATA", id, (void *) np);
  return id;
}
