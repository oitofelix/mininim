/*
  gui-tile-part-control.c -- editor GUI tile part control module;

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
  struct tile tile;
};

static int show_cb (Ihandle *ih, int state);
static int destroy_cb (Ihandle *ih);
static int _update_cb (Ihandle *ih);
static void update (Ihandle *ih, struct tile *t);
static int button_action_cb (Ihandle *ih);
static int change_tile_part_from_list_cb (Ihandle *ih);
static int change_tile_part_from_spin_cb (Ihandle *ih);


Ihandle *
gui_create_tile_part_control (struct pos *p, enum tile_part tile_part)
{
  Ihandle *ih, *button, *list, *spin;

  char **tile_part_str;
  int tile_parts;
  change_tile_part_t change_tile_part;
  char *title;

  switch (tile_part) {
  case TILE_FG:
    tile_part_str = tile_fg_str;
    tile_parts = TILE_FGS;
    change_tile_part = (change_tile_part_t) change_tile_fg;
    title = "Foreground";
    break;
  case TILE_BG:
    tile_part_str = tile_bg_str;
    tile_parts = TILE_BGS;
    change_tile_part = (change_tile_part_t) change_tile_bg;
    title = "Background";
    break;
  case TILE_EXT_ITEM:
    tile_part_str = tile_item_str;
    tile_parts = TILE_ITEMS;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Item Extension";
    break;
  case TILE_EXT_DESIGN:
    tile_part_str = NULL;
    tile_parts = TILE_DESIGNS;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Design Extension";
    break;
  default: assert (false);
    return NULL;
  }

  Ihandle *vbox;
  ih = IupSetCallbacks
    (IupFrame
     (vbox = IupSetAttributes
      (IupVbox
       (IupFill (),
        button = IupSetCallbacks
        (IupSetAttributes
         (IupButton (NULL, NULL),
          "IMAGE = NOIMAGE"),
         "ACTION", button_action_cb,
         NULL),
        list = IupSetCallbacks
        (IupSetAttributes
         (IupList (NULL),
          "DROPDOWN = YES"),
         "VALUECHANGED_CB", change_tile_part_from_list_cb,
         NULL),
        spin = IupSetCallbacks
        (IupSetAttributes
         (IupText (NULL),
          "SPIN = YES,"
          "SPINWRAP = YES,"
          "MASK = /d/d"),
         "VALUECHANGED_CB", change_tile_part_from_spin_cb,
         "KILLFOCUS_CB", gui_empty_value_to_0,
         NULL),
        IupFill (),
        NULL),
       "ALIGNMENT = ACENTER")),
     "DESTROY_CB", destroy_cb,
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  switch (tile_part) {
  case TILE_EXT_ITEM:
  case TILE_EXT_FALL:
  case TILE_EXT_EVENT:
  case TILE_EXT_STEP:
  case TILE_EXT_DESIGN:
    IupSetAttribute (vbox, "NORMALIZERGROUP", "TILE_EXT_NORM");
    break;
  default: break;
  }

  IupSetAttribute (ih, "TITLE", title);

  IupSetAttribute (ih, "_POS", (void *) p);
  IupSetAttribute (ih, "_TILE_PART_STR", (void *) tile_part_str);
  IupSetAttribute (ih, "_CHANGE_TILE_PART", (void *) change_tile_part);
  IupSetInt (ih, "_TILE_PART", tile_part);
  IupSetInt (ih, "_TILE_PARTS", tile_parts);

  IupSetAttribute (ih, "_BUTTON", (void *) button);
  IupSetAttribute (ih, "_LIST", (void *) list);
  IupSetAttribute (ih, "_SPIN", (void *) spin);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  for (int i = 0; i < tile_parts; i++)
    if (tile_part_str)
      IupSetAttributeId (list, "", i + 1, tile_part_str[i]);
    else IupSetStrfId (list, "", i + 1, "%i", i);

  IupSetInt (list, "VISIBLEITEMS", tile_parts);

  IupSetInt (spin, "SPINMAX", tile_parts - 1);

  Ihandle *tile_part_dialog = gui_create_tile_part_dialog (ih);
  IupSetAttribute (ih, "_TILE_PART_DIALOG", (void *) tile_part_dialog);

  struct tile t;
  memset (&t, 0, sizeof (t));
  update (ih, &t);

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  Ihandle *tile_part_dialog =
    (void *) IupGetAttribute (ih, "_TILE_PART_DIALOG");

  if (tile_part_dialog) {
    if (state == IUP_SHOW) {
      bool was_visible = IupGetInt (tile_part_dialog, "WAS_VISIBLE");
      IupSetInt (tile_part_dialog, "VISIBLE", was_visible);
    } else if (state == IUP_HIDE) {
      bool visible = IupGetInt (tile_part_dialog, "VISIBLE");
      IupSetInt (tile_part_dialog, "WAS_VISIBLE", visible);
      IupHide (tile_part_dialog);
    }
  }

  return IUP_DEFAULT;
}

int
destroy_cb (Ihandle *ih)
{
  Ihandle *tile_part_dialog =
    (void *) IupGetAttribute (ih, "_TILE_PART_DIALOG");

  if (tile_part_dialog) IupDestroy (tile_part_dialog);

  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  al_free (last);
  return IUP_DEFAULT;
}

int
button_action_cb (Ihandle *ih)
{
  Ihandle *tile_part_dialog =
    (void *) IupGetAttribute (ih, "_TILE_PART_DIALOG");

  if (tile_part_dialog) {
    bool visible = IupGetInt (tile_part_dialog, "VISIBLE");
    if (visible) IupHide (tile_part_dialog);
    else IupShow (tile_part_dialog);
  }

  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

  Ihandle *tile_part_dialog =
    (void *) IupGetAttribute (ih, "_TILE_PART_DIALOG");
  if (tile_part_dialog)
    gui_run_callback_IFn ("_UPDATE_CB", tile_part_dialog);

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  gui_control_active (ih, is_valid_pos (p));

  if (! is_valid_pos (p)) return IUP_DEFAULT;

  enum tile_part tile_part = IupGetInt (ih, "_TILE_PART");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");
  struct tile *t = tile (p);

  if (last->video_mode
      && ! strcmp (video_mode, last->video_mode)
      && last->em == em
      && last->hue == hue
      && (tile_part != TILE_FG
          || fg_val (last->tile.fg) == fg_val (t->fg))
      && (tile_part != TILE_BG
          || bg_val (last->tile.bg) == bg_val (t->bg))
      && ((tile_part != TILE_EXT_ITEM
           && tile_part != TILE_EXT_FALL
           && tile_part != TILE_EXT_EVENT
           && tile_part != TILE_EXT_STEP
           && tile_part != TILE_EXT_DESIGN)
          || ext_val (last->tile.fg, last->tile.ext)
          == ext_val (t->fg, t->ext)))
    return IUP_DEFAULT;
  else update (ih, t);

  return IUP_DEFAULT;
}

void
update (Ihandle *ih, struct tile *t)
{
  enum tile_part tile_part = IupGetInt (ih, "_TILE_PART");
  struct last *last = (void *) IupGetAttribute (ih, "_LAST");

  set_string_var (&last->video_mode, video_mode);
  last->em = em;
  last->hue = hue;
  last->tile = *t;

  ALLEGRO_BITMAP *b = get_tile_bitmap (t, 2, tile_part);
  Ihandle *button = (void *) IupGetAttribute (ih, "_BUTTON");
  gui_set_image (button, b, transp_to_black);
  al_destroy_bitmap (b);

  int i;
  switch (tile_part) {
  case TILE_FG: i = fg_val (t->fg); break;
  case TILE_BG: i = bg_val (t->bg); break;
  case TILE_EXT_ITEM:
  case TILE_EXT_FALL:
  case TILE_EXT_EVENT:
  case TILE_EXT_STEP:
  case TILE_EXT_DESIGN:
    i = ext_val (t->fg, t->ext);
    break;
  default: assert (false); return;
  }

  Ihandle *list = (void *) IupGetAttribute (ih, "_LIST");
  IupSetInt (list, "VALUE", i + 1);

  Ihandle *spin = (void *) IupGetAttribute (ih, "_SPIN");
  IupSetInt (spin, "SPINVALUE", i);
}




int
change_tile_part_from_list_cb (Ihandle *ih)
{
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");

  if (! is_valid_pos (p)) return IUP_DEFAULT;

  change_tile_part_t change_tile_part =
    (void *) IupGetAttribute (ih, "_CHANGE_TILE_PART");

  int i = IupGetInt (ih, "VALUE") - 1;
  change_tile_part (p, i);

  return IUP_DEFAULT;
}

int
change_tile_part_from_spin_cb (Ihandle *ih)
{
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");

  if (! is_valid_pos (p)) return IUP_DEFAULT;

  change_tile_part_t change_tile_part =
    (void *) IupGetAttribute (ih, "_CHANGE_TILE_PART");

  int i = IupGetInt (ih, "SPINVALUE");
  change_tile_part (p, i);

  return IUP_DEFAULT;
}
