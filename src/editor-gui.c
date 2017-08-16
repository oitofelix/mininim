/*
  editor-dialog.c -- editor dialog module;

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

static Ihandle *editor_dialog (void);
static void update_editor_dialog (struct pos *p);

static Ihandle *tile_fg_button (void);
static int tile_fg_button_action (Ihandle *ih);
static void update_tile_fg_button (struct tile *t);

static Ihandle *tile_fg_dialog (void);
void update_tile_fg_dialog (void);


/*********************************************************************
 * external interface
 *********************************************************************/

void
show_editor_gui (void)
{
  IupShow (editor_dialog ());
  bool was_visible = IupGetInt (tile_fg_dialog (), "WAS_VISIBLE");
  IupSetInt (tile_fg_dialog (), "VISIBLE", was_visible);
}

void
hide_editor_gui (void)
{
  IupHide (editor_dialog ());
  bool was_visible = IupGetInt (tile_fg_dialog (), "VISIBLE");
  IupSetInt (tile_fg_dialog (), "WAS_VISIBLE", was_visible);
  IupHide (tile_fg_dialog ());
}

void
update_editor_gui (void)
{
  if (IupGetInt (editor_dialog (), "VISIBLE"))
    update_editor_dialog (&mouse_pos);
  if (IupGetInt (tile_fg_dialog (), "VISIBLE"))
    update_tile_fg_dialog ();
}



/*********************************************************************
 * editor_dialog
 *********************************************************************/

Ihandle *
editor_dialog (void)
{
  static Ihandle *ih;
  if (ih) return ih;

  ih = IupSetAttributes
    (IupDialog
     (IupSetAttributes
      (IupTabs
       (IupVbox
        (IupFrame
         (IupHbox
          (tile_fg_button (),
           IupGridBox
           (IupLabel ("Name:"),
            IupLabel ("<name>"),
            NULL),
           NULL)),
         IupFrame (NULL),
         IupFrame (NULL),
         NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        IupVbox (NULL),
        NULL),
       "TABTITLE0 = Tile,"
       "TABTITLE1 = Event,"
       "TABTITLE2 = Room,"
       "TABTITLE3 = Kid,"
       "TABTITLE4 = Guard,"
       "TABTITLE5 = Level")),
     "TITLE = \"MININIM : Editor\","
     "ICON = logo_icon");

  struct pos p; new_pos (&p, &global_level, room_view, 0, 0);
  update_editor_dialog (&p);

  dialog_fit_natural_size (ih);

  return ih;
}

void
update_editor_dialog (struct pos *p)
{
  if (is_valid_pos (p)) update_tile_fg_button (tile (p));

  /* update tile_fg_name label */
  /* .... */
}



/*********************************************************************
 * tile_fg_button
 *********************************************************************/

Ihandle *
tile_fg_button (void)
{
  static Ihandle *ih;
  if (ih) return ih;
  return ih = IupSetCallbacks
    (IupSetAttributes
     (IupButton (NULL, NULL),
      "IMAGE = NOIMAGE"),
     "ACTION", tile_fg_button_action,
     NULL);
}

int
tile_fg_button_action (Ihandle *ih)
{
  bool visible = IupGetInt (tile_fg_dialog (), "VISIBLE");
  if (visible) IupHide (tile_fg_dialog ());
  else IupShow (tile_fg_dialog ());
  return IUP_DEFAULT;
}

void
update_tile_fg_button (struct tile *t)
{
  static struct {
    char *video_mode;
    enum em em;
    enum hue hue;
    struct tile tile;
  } last;

  if (last.video_mode
      && ! strcmp (video_mode, last.video_mode)
      && last.em == em
      && last.hue == hue
      && last.tile.fg == t->fg
      && (last.tile.ext == t->ext
          || ! carpet_cs (last.tile.fg))) return;
  else {
    set_string_var (&last.video_mode, video_mode);
    last.em = em;
    last.hue = hue;
    last.tile = *t;
  }

  ALLEGRO_BITMAP *b = get_tile_fg_bitmap (t, 2);
  set_button_bitmap (tile_fg_button (), b);
  al_destroy_bitmap (b);
}


/*********************************************************************
 * tile_fg_dialog
 *********************************************************************/

Ihandle *tile_fg_image[TILE_FGS];

Ihandle *
tile_fg_dialog (void)
{
  static Ihandle *ih;
  if (ih) return ih;
  ih = IupSetAttributes
    (IupDialog
     (IupSetAttributes
      (IupVbox
       (IupSetAttributes (IupFill (), "RASTERSIZE = 0x5"),
        IupSetAttributes
        (IupGridBox
         (IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = NO_FLOOR"),
           IupLabel ("NO_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = FLOOR"),
           IupLabel ("FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = BROKEN_FLOOR"),
           IupLabel ("BROKEN_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = SKELETON_FLOOR"),
           IupLabel ("SKELETON_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = LOOSE_FLOOR"),
           IupLabel ("LOOSE_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = SPIKES_FLOOR"),
           IupLabel ("SPIKES_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = OPENER_FLOOR"),
           IupLabel ("OPENER_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = CLOSER_FLOOR"),
           IupLabel ("CLOSER_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = STUCK_FLOOR"),
           IupLabel ("STUCK_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = HIDDEN_FLOOR"),
           IupLabel ("HIDDEN_FLOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = PILLAR"),
           IupLabel ("PILLAR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = BIG_PILLAR_TOP"),
           IupLabel ("BIG_PILLAR_TOP"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = BIG_PILLAR_BOTTOM"),
           IupLabel ("BIG_PILLAR_BOTTOM"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_BOTTOM"),
           IupLabel ("ARCH_BOTTOM"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = LEVEL_DOOR"),
           IupLabel ("LEVEL_DOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = DOOR"),
           IupLabel ("DOOR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = CARPET"),
           IupLabel ("CARPET"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = TCARPET"),
           IupLabel ("TCARPET"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = CHOMPER"),
           IupLabel ("CHOMPER"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = MIRROR"),
           IupLabel ("MIRROR"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = WALL"),
           IupLabel ("WALL"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_MID"),
           IupLabel ("ARCH_TOP_MID"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_SMALL"),
           IupLabel ("ARCH_TOP_SMALL"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_LEFT"),
           IupLabel ("ARCH_TOP_LEFT"),
           NULL),
          IupVbox
          (IupSetAttributes
           (IupButton (NULL, NULL),
            "NAME = ARCH_TOP_RIGHT"),
           IupLabel ("ARCH_TOP_RIGHT"),
           NULL),
          NULL),
         "ORIENTATION = HORIZONTAL,"
         "NUMDIV = 5,"
         "SIZECOL = -1,"
         "SIZELIN = -1,"
         "NORMALIZESIZE = BOTH,"
         "NMARGIN = 5,"
         "NGAPCOL = 5,"
         "NGAPLIN = 5"),
        IupSetAttributes (IupFill (), "RASTERSIZE = 0x15"),
        IupSetCallbacks
        (IupSetAttributes
         (IupButton ("&Close", NULL), "PADDING = 32"),
         "ACTION", hide_dialog, NULL),
        IupSetAttributes (IupFill (), "RASTERSIZE = 0x10"),
        NULL),
       "ALIGNMENT = ACENTER")),
     "TITLE = \"MININIM : Editor/Foreground\","
     "ICON = logo_icon");

  update_tile_fg_dialog ();

  dialog_fit_natural_size (ih);

  return ih;
}

void
update_tile_fg_dialog (void)
{
  static struct {
    char *video_mode;
    enum em em;
    enum hue hue;
  } last;

  if (last.video_mode
      && ! strcmp (video_mode, last.video_mode)
      && last.em == em
      && last.hue == hue) return;
  else {
    set_string_var (&last.video_mode, video_mode);
    last.em = em;
    last.hue = hue;
  }

  for (int i = 0; i < TILE_FGS; i++) {
    Ihandle *button =
      IupGetDialogChild (tile_fg_dialog (), tile_fg_str [i]);

    /* Make the button black so its selection is white (at least in
       Motif) */
    IupSetAttribute (button, "BGCOLOR", "0 0 0");

    /* update image */
    struct tile t = {.fg = i, .ext = 0};
    ALLEGRO_BITMAP *b = get_tile_fg_bitmap (&t, 1);
    set_button_bitmap (button, b);
    al_destroy_bitmap (b);

    /* centralize label */
    Ihandle *vbox = IupGetParent (button);
    IupSetAttribute (vbox, "ALIGNMENT", "ACENTER");
  }
}
