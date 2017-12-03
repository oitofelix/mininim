/*
  gui-editor-tabs.c -- GUI editor tabs module;

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

static int show_cb (Ihandle *ih, int state);
static int _update_cb (Ihandle *ih);
static int tabchange_cb (Ihandle *ih, Ihandle *new_tab, Ihandle *old_tab);


Ihandle *
gui_create_editor_tabs (char *norm_group, struct level *level)
{
  Ihandle *tile_ctrl = gui_create_editor_tile_control
    ("EDITOR_TAB_NORM", level);

  Ihandle *events_ctrl = IupSetAttributes
    (gui_create_editor_events_control
     ("EDITOR_TAB_NORM", level),
     "NAME = _EVENTS_CTRL,");

  Ihandle *room_ctrl = IupSetAttributes
    (gui_create_editor_room_control
     ("EDITOR_TAB_NORM", level),
     "NAME = _ROOM_CTRL,");

  Ihandle *ih = IupSetCallbacks
    (IupSetAttributes
     (IupTabs
      (tile_ctrl,
       events_ctrl,
       room_ctrl,
       IupVbox (NULL),
       IupVbox (NULL),
       IupVbox (NULL),
       NULL),
      "TABTITLE0 = Tile,"
      "TABTITLE1 = Event,"
      "TABTITLE2 = Room,"
      "TABTITLE3 = Kid,"
      "TABTITLE4 = Guard,"
      "TABTITLE5 = Level,"),
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     "TABCHANGE_CB", (Icallback) tabchange_cb,
     NULL);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_TILE_CTRL", (void *) tile_ctrl);
  IupSetAttribute (ih, "_EVENTS_CTRL", (void *) events_ctrl);
  IupSetAttribute (ih, "_ROOM_CTRL", (void *) room_ctrl);

  Ihandle *tab_norm = IupGetHandle ("EDITOR_TAB_NORM");
  IupSetAttribute (tab_norm, "NORMALIZE", "BOTH");

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  Ihandle *tile_ctrl = (void *) IupGetAttribute (ih, "_TILE_CTRL");
  gui_run_callback_IFni ("SHOW_CB", tile_ctrl, state);

  Ihandle *events_ctrl = (void *) IupGetAttribute (ih, "_EVENTS_CTRL");
  gui_run_callback_IFni ("SHOW_CB", events_ctrl, state);

  Ihandle *room_ctrl = (void *) IupGetAttribute (ih, "_ROOM_CTRL");
  gui_run_callback_IFni ("SHOW_CB", room_ctrl, state);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *tile_ctrl = (void *) IupGetAttribute (ih, "_TILE_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", tile_ctrl);

  Ihandle *events_ctrl = (void *) IupGetAttribute (ih, "_EVENTS_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", events_ctrl);

  Ihandle *room_ctrl = (void *) IupGetAttribute (ih, "_ROOM_CTRL");
  gui_run_callback_IFn ("_UPDATE_CB", room_ctrl);

  return IUP_DEFAULT;
}

int
tabchange_cb (Ihandle *ih, Ihandle *new_tab, Ihandle *old_tab)
{
  return IUP_DEFAULT;
}
