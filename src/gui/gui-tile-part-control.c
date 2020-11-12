/*
  gui-tile-part-control.c -- editor GUI tile part control module;

  Copyright (C) Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

#if WINDOWS_EDITION
#define LIST_VISIBLECOLUMNS 9
#else
#define LIST_VISIBLECOLUMNS 14
#endif

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
static int change_tile_part_from_val_cb (Ihandle *ih);
static int toggle_action_cb (Ihandle *ih, int state);
static int fall_toggle_action_cb (Ihandle *ih, int state);
static int view_button_action_cb (Ihandle *ih);


Ihandle *
gui_create_tile_part_control (struct pos *p, enum tile_part tile_part,
                              char *norm_group)
{
  Ihandle *ih, *button = NULL, *val = NULL, *toggle = NULL,
    *fall_toggle = NULL, *list = NULL, *spin = NULL, *view_button = NULL,
    *insertion_point;

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
    title = "Item";
    break;
  case TILE_EXT_DESIGN:
    tile_part_str = NULL;
    tile_parts = CARPET_DESIGNS;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Design";
    break;
  case TILE_EXT_STEP_SPIKES_FLOOR:
    tile_part_str = NULL;
    tile_parts = SPIKES_TOTAL;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Step";
    break;
  case TILE_EXT_STEP_CHOMPER:
    tile_part_str = NULL;
    tile_parts = CHOMPER_TOTAL;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Step";
    break;
  case TILE_EXT_STEP_DOOR:
    tile_part_str = NULL;
    tile_parts = DOOR_TOTAL;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Step";
    break;
  case TILE_EXT_STEP_LEVEL_DOOR:
    tile_part_str = NULL;
    tile_parts = LEVEL_DOOR_STEPS;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Step";
    break;
  case TILE_EXT_EVENT:
    tile_part_str = NULL;
    tile_parts = 0;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Event";
    break;
  case TILE_EXT_FALL:
    tile_part_str = NULL;
    tile_parts = 0;
    change_tile_part = (change_tile_part_t) change_tile_ext;
    title = "Fall";
    break;
  case TILE_FAKE:
    tile_part_str = tile_fg_str;
    tile_parts = TILE_FGS;
    change_tile_part = (change_tile_part_t) change_tile_fake;
    title = "Fake";
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
        insertion_point = IupFill (),
        NULL),
       "ALIGNMENT = ACENTER")),
     "DESTROY_CB", destroy_cb,
     "SHOW_CB", (Icallback) show_cb,
     "_UPDATE_CB", _update_cb,
     NULL);

  switch (tile_part) {
  case TILE_FG:
  case TILE_BG:
  case TILE_EXT_ITEM:
  case TILE_EXT_DESIGN:
  case TILE_EXT_STEP_SPIKES_FLOOR:
  case TILE_EXT_STEP_CHOMPER:
  case TILE_FAKE:
    spin = IupSetCallbacks
      (IupSetAttributes
       (IupText (NULL),
        "SPIN = YES,"
        "SPINWRAP = YES,"
        "MASK = /d/d"),
       "VALUECHANGED_CB", change_tile_part_from_spin_cb,
       "KILLFOCUS_CB", gui_empty_value_to_0,
       NULL);
    IupInsert (vbox, insertion_point, spin);

    list = IupSetCallbacks
      (IupSetAttributes
       (IupList (NULL),
        "DROPDOWN = YES,"
        "VISIBLECOLUMNS = " STR (LIST_VISIBLECOLUMNS) ","),
       "VALUECHANGED_CB", change_tile_part_from_list_cb,
       NULL);
    IupInsert (vbox, spin, list);

    button = IupSetCallbacks
      (IupSetAttributes
       (IupButton (NULL, NULL),
        "IMAGE = NOIMAGE,"
        "TIP = \"Click to open selection dialog\""),
       "ACTION", button_action_cb,
       "DESTROY_CB", gui_destroy_image_cb,
       NULL);
    IupInsert (vbox, list, button);
    break;
  case TILE_EXT_STEP_DOOR:
  case TILE_EXT_STEP_LEVEL_DOOR:
    spin = IupSetCallbacks
      (IupSetAttributes
       (IupText (NULL),
        "SPIN = YES,"
        "SPINWRAP = YES,"
        "MASK = /d/d"),
       "VALUECHANGED_CB", change_tile_part_from_spin_cb,
       "KILLFOCUS_CB", gui_empty_value_to_0,
       NULL);
    IupInsert (vbox, insertion_point, spin);

    toggle = IupSetCallbacks
      (IupSetAttributes
       (IupToggle (NULL, NULL),
        "ACTIVE = NO"),
       "ACTION", (Icallback) toggle_action_cb,
       NULL);
    IupInsert (vbox, spin, toggle);

    val = IupSetCallbacks
      (IupSetAttributes
       (IupVal ("VERTICAL"),
        "TICKPOS = BOTH,"
        "INVERTED = NO"),
       "VALUECHANGED_CB", change_tile_part_from_val_cb,
       NULL);
    int w = IupGetInt (val, "RASTERSIZE");
    int h = 98;
    IupSetStrf (val, "RASTERSIZE", "%ix%i", w, h);
    IupSetDouble (val, "PAGESTEP", 10.0 / tile_parts);
    IupSetDouble (val, "SHOWTICKS", tile_parts);
    IupSetDouble (val, "STEP", 1.0 / tile_parts);
    IupSetDouble (val, "MAX", tile_parts - 1);
    IupInsert (vbox, toggle, val);

    if (tile_part == TILE_EXT_STEP_LEVEL_DOOR)
      IupSetAttribute (toggle, "ACTIVE", "YES");

    break;
  case TILE_EXT_EVENT:
    spin = IupSetCallbacks
      (IupSetAttributes
       (IupText (NULL),
        "SPIN = NO,"
        "MASK = /d*,"
        "VISIBLECOLUMNS = 6"),
       "VALUECHANGED_CB", change_tile_part_from_spin_cb,
       "KILLFOCUS_CB", gui_empty_value_to_0,
       NULL);
    IupInsert (vbox, insertion_point, spin);

    view_button = IupSetCallbacks
      (IupSetAttributes
       (IupButton ("View", NULL),
        "TIP = \"View in event tab\","
        "PADDING = 16,"),
       "ACTION", view_button_action_cb,
       NULL);
    IupInsert (vbox, insertion_point, view_button);
    break;
  case TILE_EXT_FALL:
    fall_toggle = IupSetCallbacks
      (IupToggle ("Can't fall", NULL),
       "ACTION", (Icallback) fall_toggle_action_cb,
       NULL);
    IupInsert (vbox, insertion_point, fall_toggle);
    break;
  default: break;
  }

  IupSetAttribute (vbox, "NORMALIZERGROUP", norm_group);

  IupSetAttribute (ih, "TITLE", title);

  IupSetAttribute (ih, "_POS", (void *) p);
  IupSetAttribute (ih, "_TILE_PART_STR", (void *) tile_part_str);
  IupSetAttribute (ih, "_CHANGE_TILE_PART", (void *) change_tile_part);
  IupSetInt (ih, "_TILE_PART", tile_part);
  IupSetInt (ih, "_TILE_PARTS", tile_parts);

  IupSetAttribute (ih, "_BUTTON", (void *) button);
  IupSetAttribute (ih, "_VAL", (void *) val);
  IupSetAttribute (ih, "_FALL_TOGGLE", (void *) fall_toggle);
  IupSetAttribute (ih, "_LIST", (void *) list);
  IupSetAttribute (ih, "_SPIN", (void *) spin);
  IupSetAttribute (ih, "_VIEW_BUTTON", (void *) view_button);

  struct last *last = xmalloc (sizeof (*last));
  memset (last, 0, sizeof (*last));
  IupSetAttribute (ih, "_LAST", (void *) last);

  for (int i = 0; i < tile_parts; i++)
    if (tile_part_str)
      IupSetAttributeId (list, "", i + 1, tile_part_str[i]);
    else IupSetStrfId (list, "", i + 1, "%i", i);

  IupSetInt (list, "VISIBLEITEMS", tile_parts);

  IupSetInt (spin, "SPINMAX", tile_parts - 1);

  if (button) {
    Ihandle *tile_part_dialog = gui_create_tile_part_dialog (ih);
    IupSetAttribute (ih, "_TILE_PART_DIALOG", (void *) tile_part_dialog);
  }

  update (ih, &last->tile);

  return ih;
}

int
show_cb (Ihandle *ih, int state)
{
  Ihandle *tile_part_dialog =
    (void *) IupGetAttribute (ih, "_TILE_PART_DIALOG");

  if (tile_part_dialog) {
    if (state == IUP_SHOW) {
      bool _was_visible = IupGetInt (tile_part_dialog, "_WAS_VISIBLE");
      IupSetInt (tile_part_dialog, "VISIBLE", _was_visible);
    } else if (state == IUP_HIDE) {
      bool visible = IupGetInt (tile_part_dialog, "VISIBLE");
      IupSetInt (tile_part_dialog, "_WAS_VISIBLE", visible);
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
view_button_action_cb (Ihandle *ih)
{
  Ihandle *events =
    (void *) IupGetAttribute (ih, "_EVENTS_CTRL");
  gui_run_callback_IFns
    ("_SELECT_SOURCE_CB", events, (void *) &selection_pos);
  return IUP_DEFAULT;
}

int
_update_cb (Ihandle *ih)
{
  Ihandle *tile_part_dialog =
    (void *) IupGetAttribute (ih, "_TILE_PART_DIALOG");
  if (tile_part_dialog)
    gui_run_callback_IFn ("_UPDATE_CB", tile_part_dialog);

  /* This must come after the tile part dialog update call, otherwise
     that dialog is prevented from updating in case this control is
     not visible */
  if (! IupGetInt (ih, "VISIBLE")) return IUP_DEFAULT;

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
          || fg_tile (&last->tile) == fg_tile (t))
      && (tile_part != TILE_BG
          || bg_tile (&last->tile) == bg_tile (t))
      && ((tile_part != TILE_EXT_ITEM
           && tile_part != TILE_EXT_FALL
           && tile_part != TILE_EXT_STEP_SPIKES_FLOOR
           && tile_part != TILE_EXT_STEP_DOOR
           && tile_part != TILE_EXT_STEP_LEVEL_DOOR
           && tile_part != TILE_EXT_STEP_CHOMPER
           && tile_part != TILE_EXT_DESIGN)
          || (fg_tile (&last->tile) == fg_tile (t)
              && ext_tile (p->l, &last->tile) == ext_tile (p->l, t)))
      && (tile_part != TILE_EXT_EVENT
          || (fg_tile (&last->tile) == fg_tile (t)
              && last->tile.ext == t->ext))
      && (tile_part != TILE_FAKE
          || fake_tile (&last->tile) == fake_tile (t)))
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

  Ihandle *button = (void *) IupGetAttribute (ih, "_BUTTON");
  if (button) {
    ALLEGRO_BITMAP *b = get_tile_bitmap (t, NULL, 1, tile_part);
    gui_set_image (button, b, transp_to_black);
    al_destroy_bitmap (b);
  }

  struct pos *p = (void *) IupGetAttribute (ih, "_POS");

  int i;
  switch (tile_part) {
  case TILE_FG: i = fg_tile (t); break;
  case TILE_BG: i = bg_tile (t); break;
  case TILE_EXT_ITEM:
  case TILE_EXT_FALL:
  case TILE_EXT_STEP_SPIKES_FLOOR:
  case TILE_EXT_STEP_DOOR:
  case TILE_EXT_STEP_LEVEL_DOOR:
  case TILE_EXT_STEP_CHOMPER:
  case TILE_EXT_DESIGN:
    i = ext_tile (p->l, t);
    break;
  case TILE_EXT_EVENT:
    i = t->ext;
    break;
  case TILE_FAKE: i = fake_tile (t); break;
  default: assert (false); return;
  }

  Ihandle *val = (void *) IupGetAttribute (ih, "_VAL");
  if (val) IupSetInt (val, "VALUE", i);

  Ihandle *list = (void *) IupGetAttribute (ih, "_LIST");
  if (list) IupSetInt (list, "VALUE", i + 1);

  char *list_value_str = IupGetAttribute (list, "VALUESTRING");
  if (list_value_str && strlen (list_value_str) > LIST_VISIBLECOLUMNS)
    IupSetStrAttribute (list, "TIP", list_value_str);

  Ihandle *spin = (void *) IupGetAttribute (ih, "_SPIN");
  if (spin && IupGetInt (spin, "SPIN")) IupSetInt (spin, "SPINVALUE", i);
  else if (spin) {
    /* This verification step is necessary in order for the text
       control to behave correctly under Windows */
    int j = IupGetInt (spin, "VALUE");
    if (i != j) IupSetInt (spin, "VALUE", i);
  }

  Ihandle *fall_toggle = (void *) IupGetAttribute (ih, "_FALL_TOGGLE");
  if (fall_toggle) IupSetInt (fall_toggle, "VALUE", i);
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

  int i;
  if (IupGetInt (ih, "SPIN")) i = IupGetInt (ih, "SPINVALUE");
  else i = IupGetInt (ih, "VALUE");

  change_tile_part (p, i);

  return IUP_DEFAULT;
}

int
change_tile_part_from_val_cb (Ihandle *ih)
{
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");

  if (! is_valid_pos (p)) return IUP_DEFAULT;

  change_tile_part_t change_tile_part =
    (void *) IupGetAttribute (ih, "_CHANGE_TILE_PART");

  int i = IupGetInt (ih, "VALUE");
  change_tile_part (p, i);

  return IUP_DEFAULT;
}

int
toggle_action_cb (Ihandle *ih, int state)
{
  Ihandle *val = (void *) IupGetAttribute (ih, "_VAL");
  Ihandle *spin = (void *) IupGetAttribute (ih, "_SPIN");
  int tile_parts = IupGetInt (ih, "_TILE_PARTS");

  int cv = IupGetInt (spin, "SPINVALUE");
  int nv;

  IupUnmap (spin);

  if (state) {
    nv = cv + tile_parts;

    IupSetDouble (val, "MIN", tile_parts);
    IupSetDouble (val, "MAX", 2 * tile_parts - 1);
    IupSetDouble (val, "VALUE", nv);

    IupSetInt (spin, "SPINMIN", tile_parts);
    IupSetInt (spin, "SPINMAX", 2 * tile_parts - 1);
    IupSetInt (spin, "SPINVALUE", nv);
  } else {
    nv = cv - tile_parts;

    IupSetDouble (val, "MIN", 0);
    IupSetDouble (val, "MAX", tile_parts - 1);
    IupSetDouble (val, "VALUE", nv);

    IupSetInt (spin, "SPINMIN", 0);
    IupSetInt (spin, "SPINMAX", tile_parts - 1);
    IupSetInt (spin, "SPINVALUE", nv);
  }

  change_tile_part_t change_tile_part =
    (void *) IupGetAttribute (ih, "_CHANGE_TILE_PART");
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  change_tile_part (p, nv);

  IupMap (spin);
  IupRefreshChildren (IupGetParent (spin));
  IupFlush ();

  return IUP_DEFAULT;
}

int
fall_toggle_action_cb (Ihandle *ih, int state)
{
  change_tile_part_t change_tile_part =
    (void *) IupGetAttribute (ih, "_CHANGE_TILE_PART");
  struct pos *p = (void *) IupGetAttribute (ih, "_POS");
  change_tile_part (p, state ? 1 : 0);
  return IUP_DEFAULT;
}
