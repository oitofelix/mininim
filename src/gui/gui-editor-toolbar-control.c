/*
  gui-editor-toolbar-control.c -- editor GUI toolbar control module;

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

static int destroy_cb (Ihandle *ih);
static int _update_cb (Ihandle *ih);
static int button_action_cb (Ihandle *ih);


Ihandle *
gui_create_editor_toolbar_control (char *norm_group)
{
  Ihandle *ih;

  Ihandle *lock_button;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupGridBox
      (lock_button = IupSetCallbacks
       (IupSetAttributes
        (IupButton (NULL, NULL),
         "IMAGE = NOIMAGE,"
         "FLAT = YES,"),
        "ACTION", button_action_cb,
        NULL),
       NULL),
      "ORIENTATION = HORIZONTAL,"
      "NUMDIV = 1,"
      "SIZECOL = -1,"
      "SIZELIN = -1,"
      "NORMALIZESIZE = BOTH,"),
     "DESTROY_CB", destroy_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  ALLEGRO_BITMAP *lock_icon_bitmap = load_memory_bitmap (LOCK_ICON);
  Ihandle *lock_icon = bitmap_to_iup_image (lock_icon_bitmap, NULL);
  al_destroy_bitmap (lock_icon_bitmap);
  IupSetAttribute (ih, "_LOCK_ICON", (void *) lock_icon);

  ALLEGRO_BITMAP *unlock_icon_bitmap = load_memory_bitmap (UNLOCK_ICON);
  Ihandle *unlock_icon = bitmap_to_iup_image (unlock_icon_bitmap, NULL);
  al_destroy_bitmap (unlock_icon_bitmap);
  IupSetAttribute (ih, "_UNLOCK_ICON", (void *) unlock_icon);

  IupSetAttributeHandle (lock_button, "IMAGE", unlock_icon);

  IupSetAttribute (ih, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_LOCK_BUTTON", (void *) lock_button);

  return ih;
}

int
destroy_cb (Ihandle *ih)
{
  Ihandle *lock_icon = (void *) IupGetAttribute (ih, "_LOCK_ICON");
  IupDestroy (lock_icon);

  Ihandle *unlock_icon = (void *) IupGetAttribute (ih, "_UNLOCK_ICON");
  IupDestroy (unlock_icon);

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *lock_button = (void *) IupGetAttribute (ih, "_LOCK_BUTTON");
  Ihandle *lock_icon = (void *) IupGetAttribute (ih, "_LOCK_ICON");
  Ihandle *unlock_icon = (void *) IupGetAttribute (ih, "_UNLOCK_ICON");

  gui_control_image (lock_button, lock_icon, selection_locked);
  gui_control_image (lock_button, unlock_icon, ! selection_locked);

  return IUP_DEFAULT;
}

int
button_action_cb (Ihandle *ih)
{
  return IUP_DEFAULT;
}
