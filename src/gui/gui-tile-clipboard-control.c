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
gui_create_tile_clipboard_control (struct pos *p)
{
  Ihandle *ih, *button, *radio;

  ih = IupSetCallbacks
    (IupSetAttributes
     (IupFrame
      (IupSetAttributes
       (IupVbox
        (IupFill (),
         button = IupSetCallbacks
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "IMAGE = NOIMAGE,"
            "TIP = \"Click to copy\""),
           "ACTION", copy_button_action_cb,
           NULL),
         IupSetCallbacks
         (IupSetAttributes
          (IupButton ("&Paste", NULL),
           "PADDING = 16,"
           "TIP = \"Paste tile at selected scope\""),
          "ACTION", paste_button_action_cb,
          NULL),
         radio = IupRadio
         (IupHbox
          (IupSetAttributes
           (IupToggle ("Tile", NULL),
            "TIP = \"Tile pasting scope\""),
           IupSetAttributes
           (IupToggle ("Room", NULL),
            "TIP = \"Room pasting scope\""),
           IupSetAttributes
           (IupToggle ("Level", NULL),
            "TIP = \"Level pasting scope\""),
           NULL)),
         IupFill (),
         NULL),
        "ALIGNMENT = ACENTER")),
      "TITLE = Clipboard"),
     "DESTROY_CB", destroy_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  Ihandle *norm = IupGetHandle ("CLIPBOARD_BUTTON_NORM");
  IupSetAttribute (norm, "NORMALIZE", "BOTH");

  IupSetAttribute (ih, "_BUTTON", (void *) button);
  IupSetAttribute (ih, "_RADIO", (void *) radio);

  IupSetAttribute (ih, "_POS", (void *) p);

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
  gui_control_active (ih, is_valid_pos (p));

  if (! is_valid_pos (p)) return IUP_DEFAULT;

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

  Ihandle *button = (void *) IupGetAttribute (ih, "_BUTTON");
  ALLEGRO_BITMAP *b = get_tile_bitmap
    (&last->tile_copy.c, &last->tile_copy.cs, 2, TILE_ALL);
  gui_set_image (button, b, transp_to_black);
  al_destroy_bitmap (b);
}

int
copy_button_action_cb (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  copy_tile (&last->tile_copy, p);
  update (ih);
  return IUP_DEFAULT;
}

int
paste_button_action_cb (Ihandle *ih)
{
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  Ihandle *radio = (void *) IupGetAttribute (ih, "_RADIO");
  Ihandle *toggle = (void *) IupGetAttribute (radio, "VALUE_HANDLE");
  char *title = IupGetAttribute (toggle, "TITLE");
  if (! strcmp (title, "Tile"))
    apply_to_pos (p, (pos_trans) paste_tile,
                  &last->tile_copy, "PASTE TILE");
  else if (! strcmp (title, "Room"))
    apply_to_room (&global_level, mr.room, (pos_trans) paste_tile,
                   &last->tile_copy, "PASTE TILE (ROOM)");
  else if (! strcmp (title, "Level"))
    apply_to_level (&global_level, (pos_trans) paste_tile,
                    &last->tile_copy, "PASTE TILE (LEVEL)");
  else assert (NULL);

  return IUP_DEFAULT;
}
