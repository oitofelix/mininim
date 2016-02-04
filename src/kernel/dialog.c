/*
  dialog.c -- dialog module;

  Copyright (C) 2015, 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>

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

void
init_dialog (void)
{
  if (! al_init_native_dialog_addon())
    error (-1, 0, "%s (void): failed to initialize dialog addon",
           __func__);
}

void
finalize_dialog (void)
{
  al_shutdown_native_dialog_addon ();
}

ALLEGRO_FILECHOOSER *
create_native_file_dialog (char const *initial_path,
                           char const *title,
                           char const *patterns,
                           int mode)
{
  ALLEGRO_FILECHOOSER *dialog =
    al_create_native_file_dialog (initial_path, title, patterns, mode);

  if (! dialog)
    error (-1, 0, "%s (%s, %s, %s, %i): failed to create native file dialog",
           __func__, initial_path, title, patterns, mode);

  return dialog;
}

void
show_native_file_dialog (ALLEGRO_DISPLAY *display,
                         ALLEGRO_FILECHOOSER *dialog)
{
  /* It seems that on MinGW al_show_native_file_dialog returns false
     when the dialog is cancelled, thus you may want to call
     al_show_native_file_dialog directly. */

  if (! al_show_native_file_dialog (display, dialog))
    error (-1, 0, "%s (%p, %p): failed to show native file dialog",
           __func__, display, dialog);
}

ALLEGRO_TEXTLOG *
open_native_text_log (char const *title, int flags)
{
  ALLEGRO_TEXTLOG *textlog = al_open_native_text_log (title, flags);

  /* if (! textlog) */
  /*   error (-1, 0, "%s (%s, %i): failed to open native text log dialog", */
  /*          __func__, title, flags); */

  return textlog;
}

ALLEGRO_EVENT_SOURCE *
get_native_text_log_event_source (ALLEGRO_TEXTLOG *textlog)
{
  ALLEGRO_EVENT_SOURCE *event_source =
    al_get_native_text_log_event_source (textlog);

  if (! event_source)
    error (-1, 0, "%s (%p): failed to get native text log dialog event source",
           __func__, textlog);

  return event_source;
}
