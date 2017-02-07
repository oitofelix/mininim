/*
  dialog.c -- dialog module;

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

ALLEGRO_TEXTLOG *text_log;

void
init_dialog (void)
{
  if (! al_init_native_dialog_addon())
    error (0, 0, "%s (void): failed to initialize dialog addon",
           __func__);

  /* Gtk inadvertently sets the application locale */
  if (GNU_PORT) setlocale (LC_ALL, "C");
}

void
finalize_dialog (void)
{
  if (text_log) {
    al_close_native_text_log (text_log);
    text_log = NULL;
  }
  al_shutdown_native_dialog_addon ();
}

void
print_text_log (char const *template, ...)
{
  if (! text_log)
    text_log = al_open_native_text_log
      ("MININIM log", ALLEGRO_TEXTLOG_MONOSPACE);

  al_register_event_source
    (event_queue, al_get_native_text_log_event_source (text_log));

  va_list ap;
  va_start (ap, template);

  char *text;
  vasprintf (&text, template, ap);
  al_append_native_text_log (text_log, text);
  al_free (text);

  va_end (ap);
}

void
close_text_log (ALLEGRO_EVENT *event)
{
  al_close_native_text_log ((ALLEGRO_TEXTLOG *) event->user.data1);
  text_log = NULL;
}

bool
file_overwrite_dialog (char *filename)
{
  /* not needed currently by any port, but should be kept in case the
     need arise */
  return true;

  if (al_filename_exists (filename)) {
    int r = al_show_native_message_box
      (display, "Warning",
       "File already exists!",
       "Do you want to overwrite it?",
       NULL,
       ALLEGRO_MESSAGEBOX_WARN | ALLEGRO_MESSAGEBOX_YES_NO);
    return r == 1;
  }
  else return true;
}
