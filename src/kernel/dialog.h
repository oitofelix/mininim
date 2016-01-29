/*
  dialog.h -- dialog module;

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

#ifndef MININIM_DIALOG_H
#define MININIM_DIALOG_H

void init_dialog (void);
void finalize_dialog (void);
ALLEGRO_FILECHOOSER *
create_native_file_dialog (char const *initial_path,
                           char const *title,
                           char const *patterns,
                           int mode);
void show_native_file_dialog (ALLEGRO_DISPLAY *display,
                              ALLEGRO_FILECHOOSER *dialog);
ALLEGRO_TEXTLOG *open_native_text_log (char const *title, int flags);
ALLEGRO_EVENT_SOURCE *get_native_text_log_event_source (ALLEGRO_TEXTLOG *textlog);

#endif	/* MININIM_DIALOG_H */
