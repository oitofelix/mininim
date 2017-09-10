/*
  gui.c -- GUI module;

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
} last;

Ihandle *gui_editor_dialog;

Ihandle *tile_icon[TILE_FGS];

#define LOAD_ICON(token) load_icon (token, #token)
#define UNLOAD_ICON(token) unload_icon (#token)

static void globalctrlfunc_cb (int c);
static void load_icon (char *filename, char *handle_name);
static void unload_icon (char *handle_name);
static void load_icons (void);
static void unload_icons (void);
static void load_tile_icon_array (void);
static void unload_tile_icon_array (void);

void
init_gui (int argc, char **argv)
{
  /* Initialize IUP */
  IupOpen (&argc, &argv);

  IupSetGlobal ("AUTOREPEAT", "YES");

#if DEBUG
  IupSetFunction ("GLOBALCTRLFUNC_CB", (Icallback) globalctrlfunc_cb);
#endif

  load_icons ();

  load_tile_icon_array ();

  Ihandle *logo_icon_image = bitmap_to_iup_image (logo_icon, NULL);
  IupSetHandle ("LOGO_ICON", logo_icon_image);

  gui_editor_dialog = gui_create_editor_dialog ();
}

void
finalize_gui (void)
{
  IupDestroy (gui_editor_dialog);

  unload_icons ();

  unload_tile_icon_array ();

  IupExitLoop ();
  IupFlush ();
  IupClose ();
}

void
load_icon (char *filename, char *handle_name)
{
  ALLEGRO_BITMAP *icon_bitmap = load_memory_bitmap (filename);
  Ihandle *icon = bitmap_to_iup_image (icon_bitmap, NULL);
  al_destroy_bitmap (icon_bitmap);
  IupSetHandle (handle_name, icon);
}

void
unload_icon (char *handle_name)
{
  Ihandle *icon = IupGetHandle (handle_name);
  IupDestroy (icon);
}

void
load_icons (void)
{
  LOAD_ICON (ZOOM_ICON);
  LOAD_ICON (ZOOM_NONE_ICON);
  LOAD_ICON (ZOOM_STRETCH_ICON);
  LOAD_ICON (ZOOM_RATIO_ICON);
  LOAD_ICON (ZOOM_IN_ICON);
  LOAD_ICON (ZOOM_OUT_ICON);
  LOAD_ICON (VH_ICON);
  LOAD_ICON (V_ICON);
  LOAD_ICON (H_ICON);
  LOAD_ICON (PLACE_ICON);
  LOAD_ICON (ROOM_ICON);
  LOAD_ICON (ROW_ICON);
  LOAD_ICON (PAGE_ICON);
  LOAD_ICON (L_ICON);
  LOAD_ICON (R_ICON);
  LOAD_ICON (A_ICON);
  LOAD_ICON (B_ICON);
  LOAD_ICON (AL_ICON);
  LOAD_ICON (AR_ICON);
  LOAD_ICON (BL_ICON);
  LOAD_ICON (BR_ICON);
  LOAD_ICON (TOOLS_ICON);
  LOAD_ICON (CLEAR_ICON);
  LOAD_ICON (RANDOM_ICON);
  LOAD_ICON (DECORATION_ICON);
  LOAD_ICON (FIX_ICON);
  LOAD_ICON (MIRROR_ICON);
  LOAD_ICON (SHUFFLE_ICON);
  LOAD_ICON (LOCK_ICON);
  LOAD_ICON (UNLOCK_ICON);
  LOAD_ICON (PAUSE_ICON);
  LOAD_ICON (PLAY_ICON);
  LOAD_ICON (UNMASK_ICON);
  LOAD_ICON (MASK_EXCHANGE_ICON);
  LOAD_ICON (UNDO_ICON);
  LOAD_ICON (REDO_ICON);
  LOAD_ICON (FULL_SCREEN_ICON);
  LOAD_ICON (WINDOWS_ICON);
  LOAD_ICON (HOME_ICON);
  LOAD_ICON (EVENT_ICON);
  LOAD_ICON (TOP_ICON);
  LOAD_ICON (BOTTOM_ICON);
  LOAD_ICON (RELOAD_ICON);
  LOAD_ICON (NEXT_ICON);
  LOAD_ICON (PREVIOUS_ICON);
  LOAD_ICON (EVENT_SOURCE_ICON);
  LOAD_ICON (EVENT_TARGET_ICON);
}

void
unload_icons (void)
{
  UNLOAD_ICON (ZOOM_ICON);
  UNLOAD_ICON (ZOOM_NONE_ICON);
  UNLOAD_ICON (ZOOM_STRETCH_ICON);
  UNLOAD_ICON (ZOOM_RATIO_ICON);
  UNLOAD_ICON (ZOOM_IN_ICON);
  UNLOAD_ICON (ZOOM_OUT_ICON);
  UNLOAD_ICON (VH_ICON);
  UNLOAD_ICON (V_ICON);
  UNLOAD_ICON (H_ICON);
  UNLOAD_ICON (PLACE_ICON);
  UNLOAD_ICON (ROOM_ICON);
  UNLOAD_ICON (ROW_ICON);
  UNLOAD_ICON (PAGE_ICON);
  UNLOAD_ICON (L_ICON);
  UNLOAD_ICON (R_ICON);
  UNLOAD_ICON (A_ICON);
  UNLOAD_ICON (B_ICON);
  UNLOAD_ICON (AL_ICON);
  UNLOAD_ICON (AR_ICON);
  UNLOAD_ICON (BL_ICON);
  UNLOAD_ICON (BR_ICON);
  UNLOAD_ICON (TOOLS_ICON);
  UNLOAD_ICON (CLEAR_ICON);
  UNLOAD_ICON (RANDOM_ICON);
  UNLOAD_ICON (DECORATION_ICON);
  UNLOAD_ICON (FIX_ICON);
  UNLOAD_ICON (MIRROR_ICON);
  UNLOAD_ICON (SHUFFLE_ICON);
  UNLOAD_ICON (LOCK_ICON);
  UNLOAD_ICON (UNLOCK_ICON);
  UNLOAD_ICON (PAUSE_ICON);
  UNLOAD_ICON (PLAY_ICON);
  UNLOAD_ICON (UNMASK_ICON);
  UNLOAD_ICON (MASK_EXCHANGE_ICON);
  UNLOAD_ICON (UNDO_ICON);
  UNLOAD_ICON (REDO_ICON);
  UNLOAD_ICON (FULLSCREEN_ICON);
  UNLOAD_ICON (WINDOWS_ICON);
  UNLOAD_ICON (HOME_ICON);
  UNLOAD_ICON (EVENT_ICON);
  UNLOAD_ICON (TOP_ICON);
  UNLOAD_ICON (BOTTOM_ICON);
  UNLOAD_ICON (RELOAD_ICON);
  UNLOAD_ICON (NEXT_ICON);
  UNLOAD_ICON (PREVIOUS_ICON);
  UNLOAD_ICON (EVENT_SOURCE_ICON);
  UNLOAD_ICON (EVENT_TARGET_ICON);
}

void
load_tile_icon_array (void)
{
  set_string_var (&last.video_mode, video_mode);
  last.em = em;
  last.hue = hue;

  struct tile t; memset (&t, 0, sizeof (t));
  for (t.fg = 0; t.fg < TILE_FGS; t.fg++) {
    if (tile_icon[t.fg]) IupDestroy (tile_icon[t.fg]);
    ALLEGRO_BITMAP *b = get_tile_bitmap (&t, NULL, REAL_WIDTH / ORIGINAL_WIDTH,
                                         TILE_FG);
    ALLEGRO_BITMAP *b_scaled = clone_scaled_bitmap (b, 16, 16, 0);
    tile_icon[t.fg] = bitmap_to_iup_image (b_scaled, NULL);
    al_destroy_bitmap (b_scaled);
    al_destroy_bitmap (b);
  }
}

void
unload_tile_icon_array (void)
{
  for (enum tile_fg f = 0; f < TILE_FGS; f++)
    if (tile_icon[f]) IupDestroy (tile_icon[f]);
}

void
gui_update (void)
{
  if ((last.video_mode && strcmp (video_mode, last.video_mode))
      || last.em != em || last.hue != hue)
    load_tile_icon_array ();
}

void
globalctrlfunc_cb (int c)
{
  IupShow (IupLayoutDialog (NULL));
}

const char *load_led_error_msg;

intptr_t
load_led_sub (const char *filename)
{
  if (! al_filename_exists (filename)) return false;
  const char *msg = IupLoad (filename);
  if (msg) {
    load_led_error_msg = msg;
    return false;
  }
  return true;
}

bool
load_led (const char *filename)
{
  load_led_error_msg = NULL;
  bool success = load_resource (filename, (load_resource_f) load_led_sub,
                                true);
  if (success) return true;
  if (load_led_error_msg) error (0, 0, "%s", load_led_error_msg);
  else error (0, 0, "LED file \"%s\" not found", filename);
  return false;
}

Ihandle *
bitmap_to_iup_image (ALLEGRO_BITMAP *b, palette p)
{
  if (! b) return NULL;

  int x, y;
  int w = get_bitmap_width (b);
  int h = get_bitmap_height (b);
  int s = al_get_pixel_size (ALLEGRO_PIXEL_FORMAT_RGBA_8888);
  unsigned char *bs = xcalloc (w * h, s);

  al_lock_bitmap (b, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READONLY);
  for (y = 0; y < h; y++)
    for (x = 0; x < w; x++) {
      ALLEGRO_COLOR c = al_get_pixel (b, x, y);
      if (p) c = p (c, NULL);
      unsigned char r, g, b, a;
      al_unmap_rgba (c, &r, &g, &b, &a);
      int pos = (y * w + x) * s;
      bs[pos + 0] = r;
      bs[pos + 1] = g;
      bs[pos + 2] = b;
      bs[pos + 3] = a;
    }
  al_unlock_bitmap (b);

  Ihandle *iup_image = IupImageRGBA (w, h, bs);
  al_free (bs);
  return iup_image;
}

void
gui_set_image (Ihandle *ih, ALLEGRO_BITMAP *b, palette p)
{
  Ihandle *old_image = IupGetAttributeHandle (ih, "IMAGE");
  Ihandle *new_image = bitmap_to_iup_image (b, p);
  IupSetAttributeHandle (ih, "IMAGE", new_image);
  IupSetAttribute (ih, "RASTERSIZE", NULL);
  /* ATTENTION: the old bitmap is destroyed! */
  IupDestroy (old_image);
}

void
gui_set_stock_image (Ihandle *ih, char *new_image_hname)
{
  Ihandle *old_image = IupGetAttributeHandle (ih, "IMAGE");
  Ihandle *new_image = IupGetHandle (new_image_hname);
  if (old_image == new_image) return;
  IupSetAttribute (ih, "IMAGE", new_image_hname);
}

ALLEGRO_COLOR
transp_to_black (ALLEGRO_COLOR c, void *data)
{
  if (color_eq (c, TRANSPARENT_COLOR)) return BLACK;
  else return c;
}

ALLEGRO_COLOR
white_to_transp (ALLEGRO_COLOR c, void *data)
{
  if (color_eq (c, WHITE)) return TRANSPARENT_COLOR;
  else return c;
}

int
hide_dialog (Ihandle *ih)
{
  IupHide (IupGetDialog (ih));
  return IUP_DEFAULT;
}

/* Update active status.  This function based on the method of first
   assessing the current control state is necessary because (specially
   for Windows) the IUP driver may insist in redrawing the control
   even when there is no change in state, resulting in random
   graphical glitches if done recurrently. */
bool
gui_control_active (Ihandle *ih, bool a)
{
  /* Only consider changing the active status of the control if its
     already mapped, otherwise a bug in Motif might cause the
     application to hang up. */
  if (! IupGetAttribute (ih, "WID")) return false;

  bool b = IupGetInt (ih, "ACTIVE");
  if (! equiv (a, b)) {
    IupSetInt (ih, "ACTIVE", a);
    return true;
  } else return false;
}

bool
gui_control_int (Ihandle *ih, char *name, int value)
{
  int old_value = IupGetInt (ih, name);
  if (old_value == value) return false;
  IupSetInt (ih, name, value);
  return true;
}

bool
gui_control_attribute (Ihandle *ih, char *name, char *value)
{
  char *old_value = IupGetAttribute (ih, name);
  if (old_value == value) return false;
  IupSetAttribute (ih, name, value);
  return true;
}

bool
gui_control_attribute_str (Ihandle *ih, char *name, char *value)
{
  char *old_value = IupGetAttribute (ih, name);
  if (! strcmp (old_value, value)) return false;
  IupSetAttribute (ih, name, value);
  return true;
}

bool
gui_control_attribute_strf (Ihandle *ih, char *name, char *format, ...)
{
  bool r = false;
  va_list ap;
  va_start (ap, format);
  char *value = NULL;
  vasprintf (&value, format, ap);
  char *old_value = IupGetAttribute (ih, name);
  if (! equiv (old_value, value)
      || (old_value && value && strcmp (old_value, value))) {
    IupSetStrAttribute (ih, name, value);
    r = true;
  }
  al_free (value);
  va_end (ap);
  return r;
}

int
gui_default_key_cb (Ihandle *ih, int c)
{
  return IUP_DEFAULT;
}

int
gui_debug_print_key_cb (Ihandle *ih, int c)
{
  fprintf (stderr, "Key pressed: %x\n", c);
  return IUP_CONTINUE;
}

int
gui_empty_value_to_0 (Ihandle *ih)
{
  /* This works for spins as well (no need to modify "SPINVALUE"
     attribute) */
  char *v = IupGetAttribute (ih, "VALUE");
  if (! strlen (v)) IupSetAttribute (ih, "VALUE", "0");
  return IUP_DEFAULT;
}

int
gui_run_callback_IFn (const char *name, Ihandle *ih)
{
  if (! ih) return IUP_DEFAULT;
  IFn cb = (IFn) IupGetCallback (ih, name);
  if (cb) return cb (ih);
  else return IUP_DEFAULT;
}

int
gui_run_callback_IFni (const char *name, Ihandle *ih, int i)
{
  if (! ih) return IUP_DEFAULT;
  IFni cb = (IFni) IupGetCallback (ih, name);
  if (cb) return cb (ih, i);
  else return IUP_DEFAULT;
}

void
dialog_fit_natural_size (Ihandle *ih)
{
  IupSetAttribute (ih, "RASTERSIZE", NULL);
  IupRefresh (ih);
  const char *size = IupGetAttribute (ih, "NATURALSIZE");
  IupSetAttribute (ih, "MINSIZE", size);
#if WINDOWS_PORT
  IupSetAttribute (ih, "RESIZE", "NO");
#else
  /* IupSetAttribute (ih, "MAXSIZE", size); */
#endif
}

void
gui_resize_cb (Ihandle *ih, int width, int height)
{
  IupSetAttribute (ih, "RASTERSIZE", NULL);
  IupRefresh (ih);
}

int
gui_destroy_image_cb (Ihandle *ih)
{
  Ihandle *image = IupGetAttributeHandle (ih, "IMAGE");
  if (image) IupDestroy (image);
  return IUP_DEFAULT;
}
