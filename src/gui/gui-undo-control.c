/*
  gui-undo-control.c -- GUI undo control module;

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

#if WINDOWS_PORT
#define LIST_VISIBLECOLUMNS 15
#else
#define LIST_VISIBLECOLUMNS 25
#endif

struct last {
  size_t count;
  int current;
  char *desc;
};

static int destroy_cb (Ihandle *ih);
static int _update_cb (Ihandle *ih);
static void update (Ihandle *ih);
static int button_action_cb (Ihandle *ih);
static int valuechanged_cb (Ihandle *ih);


Ihandle *
gui_create_undo_control (struct undo *undo, char *norm_group)
{
  Ihandle *ih;

  Ihandle *undo_button, *redo_button, *list;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupGridBox
      (
       undo_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = UNDO_ICON,"
         "FLAT = YES,"
         "CANFOCUS = NO,"
         "TIP = \"Undo (Ctrl+Z)\""),
        "ACTION", button_action_cb,
        NULL),

       redo_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = REDO_ICON,"
         "FLAT = YES,"
         "CANFOCUS = NO,"
         "TIP = \"Redo (Ctrl+Y)\""),
        "ACTION", button_action_cb,
        NULL),

       list = IupSetCallbacks
       (IupSetAttributes
        (IupList (NULL),
         "DROPDOWN = YES,"
         "VISIBLECOLUMNS = " STR (LIST_VISIBLECOLUMNS) ","
         "VISIBLEITEMS = 15,"
         /* "CANFOCUS = NO," */
         ),
        "VALUECHANGED_CB", valuechanged_cb,
        NULL),

       NULL),
      "ORIENTATION = HORIZONTAL,"
      "NUMDIV = 3,"
      "NGAPCOL = 3,"
      "SIZECOL = -1,"
      "SIZELIN = -1,"
      "NORMALIZESIZE = VERTICAL,"
      "ALIGNMENTLIN = ACENTER,"
      "ALIGNMENTCOL = ACENTER,"
      ),
     "DESTROY_CB", destroy_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_UNDO", (void *) undo);

  IupSetAttributeId (list, "", 1, "");

  struct last *last = xmalloc (sizeof (*last));
  last->count = 0;
  last->current = -1;
  last->desc = NULL;
  IupSetAttribute (ih, "_LAST", (void *) last);

  IupSetAttribute (ih, "_UNDO_BUTTON", (void *) undo_button);
  IupSetAttribute (ih, "_REDO_BUTTON", (void *) redo_button);
  IupSetAttribute (ih, "_LIST", (void *) list);

  return ih;
}

int
destroy_cb (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  al_free (last);
  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  struct undo *undo = (void *) IupGetAttribute (ih, "_UNDO");

  Ihandle *undo_button = (void *) IupGetAttribute (ih, "_UNDO_BUTTON");
  gui_control_active (undo_button, can_undo (undo, -1));

  Ihandle *redo_button = (void *) IupGetAttribute (ih, "_REDO_BUTTON");
  gui_control_active (redo_button, can_undo (undo, +1));

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  if (last->count != undo->count
      || last->current != undo->current
      || (undo->current >= 0
          && last->desc != undo->pass[undo->current].desc)
      || (undo->current >= 0 && last->desc && undo->pass[undo->current].desc
          && strcmp (last->desc, undo->pass[undo->current].desc)))
    update (ih);

 return IUP_DEFAULT;
}

void
update (Ihandle *ih)
{
  struct undo *undo = (void *) IupGetAttribute (ih, "_UNDO");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  last->current = undo->current;
  last->count = undo->count;
  if (undo->current >= 0) last->desc = undo->pass[undo->current].desc;
  else last->desc = NULL;

  Ihandle *list = (void *) IupGetAttribute (ih, "_LIST");

  IupSetAttributeId (list, "", 2, NULL);
  for (size_t i = 0; i < undo->count; i++)
    if (undo->pass[i].desc) {
      IupSetAttribute (list, "APPENDITEM", undo->pass[i].desc);
      if (i == undo->current)
        IupSetInt (list, "VALUE", IupGetInt (list, "COUNT"));
    }
}

int
button_action_cb (Ihandle *ih)
{
  struct undo *undo = (void *) IupGetAttribute (ih, "_UNDO");

  Ihandle *undo_button = (void *) IupGetAttribute (ih, "_UNDO_BUTTON");
  Ihandle *redo_button = (void *) IupGetAttribute (ih, "_REDO_BUTTON");

  if (ih == undo_button) ui_undo_pass (undo, -1, NULL);
  else if (ih == redo_button) ui_undo_pass (undo, +1, NULL);

  return IUP_DEFAULT;
}

int
valuechanged_cb (Ihandle *ih)
{
  struct undo *undo = (void *) IupGetAttribute (ih, "_UNDO");

  Ihandle *list = (void *) IupGetAttribute (ih, "_LIST");

  int current = current_macro_undo (undo) + 2;
  int new = IupGetInt (list, "VALUE");

  for (; new < current; current--) ui_undo_pass (undo, -1, NULL);
  for (; new > current; current++) ui_undo_pass (undo, +1, NULL);

  return IUP_DEFAULT;
}
