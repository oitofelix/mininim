/*
  gui-tile-clipboard-control.c -- editor GUI tile clipboard control module;

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
  struct tile_copy tile_copy;
};

static int destroy_cb (Ihandle *ih);
static int _update_cb (Ihandle *ih);
static void update (Ihandle *ih);
static int copy_button_action_cb (Ihandle *ih);
static int paste_button_action_cb (Ihandle *ih);



Ihandle *
gui_create_tile_clipboard_control (struct pos *p,
                                   struct sel_ring *sr,
                                   char *norm_group)
{
  Ihandle *ih, *vbox, *copy_button, *paste_button, *sel_ring_toggle,
    *radio, *place_toggle, *room_toggle, *level_toggle;

  copy_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = NOIMAGE,"
      "TIP = \"Click to copy\""),
     "ACTION", copy_button_action_cb,
     "DESTROY_CB", gui_destroy_image_cb,
     NULL);

  paste_button = IupSetCallbacks
    (IupSetAttributes
     (IupButton ("&Paste", NULL),
      "PADDING = 8,"
      "TIP = \"Paste tile at selected scope\""),
     "ACTION", paste_button_action_cb,
     NULL);

  sel_ring_toggle = IupSetAttributes
    (IupToggle ("S", NULL),
     "VALUE = YES,"
     "TIP = \"Apply to selection set\",");

  place_toggle = IupSetAttributes
    (IupToggle ("P", NULL),
     "TIP = \"Place pasting scope\"");

  room_toggle = IupSetAttributes
    (IupToggle ("R", NULL),
     "TIP = \"Room pasting scope\"");

  level_toggle = IupSetAttributes
    (IupToggle ("L", NULL),
     "TIP = \"Level pasting scope\"");

  radio = IupRadio
    (IupHbox
     (place_toggle,
      room_toggle,
      level_toggle,
      NULL));

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (vbox = IupSetAttributes
       (IupVbox
        (IupFill (),
         copy_button,
         paste_button,
         sel_ring_toggle,
         radio,
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER,"
        "MARGIN = 0,"
        "GAṔ = 0,")),
      "TITLE = Clipboard"),
     "DESTROY_CB", destroy_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "_COPY_BUTTON", (void *) copy_button);
  IupSetAttribute (ih, "_SEL_RING_TOGGLE", (void *) sel_ring_toggle);
  IupSetAttribute (ih, "_RADIO", (void *) radio);
  IupSetAttribute (ih, "_PLACE_TOGGLE", (void *) place_toggle);
  IupSetAttribute (ih, "_ROOM_TOGGLE", (void *) room_toggle);
  IupSetAttribute (ih, "_LEVEL_TOGGLE", (void *) level_toggle);

  IupSetAttribute (ih, "_POS", (void *) p);
  IupSetAttribute (ih, "_SEL_RING", (void *) sr);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  update (ih);

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

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  struct sel_ring *sr = (void *) IupGetAttribute (ih, "_SEL_RING");
  gui_control_active
    (ih, is_valid_pos (p) || sel_ring_ss_c_nmemb (sr) > 0);

  Ihandle *copy_button = (void *) IupGetAttribute (ih, "_COPY_BUTTON");
  gui_control_active (copy_button, is_valid_pos (p));

  if (! is_valid_pos (p) && ! sel_ring_ss_c_nmemb (sr))
    return IUP_DEFAULT;

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  if (last->video_mode
      && ! strcmp (video_mode, last->video_mode)
      && last->em == em
      && last->hue == hue)
    return IUP_DEFAULT;
  else update (ih);

  return IUP_DEFAULT;
}

void
update (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  set_string_var (&last->video_mode, video_mode);
  last->em = em;
  last->hue = hue;

  Ihandle *copy_button = (void *) IupGetAttribute (ih, "_COPY_BUTTON");
  ALLEGRO_BITMAP *b = get_tile_bitmap
    (&last->tile_copy.c, &last->tile_copy.cs, 1, TILE_ALL);
  gui_set_image (copy_button, b, transp_to_black);
  al_destroy_bitmap (b);
}

int
copy_button_action_cb (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  if (! is_valid_pos (p)) return IUP_DEFAULT;
  copy_tile (&last->tile_copy, p);
  update (ih);
  return IUP_DEFAULT;
}

int
paste_button_action_cb (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  struct sel_ring *sr = (void *) IupGetAttribute (ih, "_SEL_RING");

  Ihandle *place_toggle = (void *) IupGetAttribute (ih, "_PLACE_TOGGLE");
  Ihandle *room_toggle = (void *) IupGetAttribute (ih, "_ROOM_TOGGLE");
  Ihandle *level_toggle = (void *) IupGetAttribute (ih, "_LEVEL_TOGGLE");
  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *selected = (void *) IupGetAttribute (radio, "VALUE_HANDLE");

  enum scope scope;
  if (selected == place_toggle)
    scope = sel_ring_ss_c_nmemb (sr) > 0
      ? PLACE_SEL_RING_SCOPE : PLACE_SCOPE;
  else if (selected == room_toggle)
    scope = sel_ring_ss_c_nmemb (sr) > 0
      ? ROOM_SEL_RING_SCOPE : ROOM_SCOPE;
  else if (selected == level_toggle) scope = LEVEL_SCOPE;
  else assert (false);

  apply_to_scope (p, sr,(pos_trans) paste_tile, &last->tile_copy,
                  "PASTE", scope);

  return IUP_DEFAULT;
}
