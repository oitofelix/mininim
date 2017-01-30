/*
  xconfig.c -- configuration module;

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

void
config_entry_file_type (const char *key, const char *value,
                        enum file_type *file_type)
{
  if (strcasecmp (key, "FILE TYPE")) return;
  else if (! strcasecmp (value, "MININIM CONFIGURATION"))
    *file_type = CONFIG_FILE_TYPE;
  else if (! strcasecmp (value, "MININIM GAME SAVE"))
    *file_type = GAME_SAVE_FILE_TYPE;
  else if (! strcasecmp (value, "MININIM LEVEL"))
    *file_type = LEVEL_FILE_TYPE;
  else *file_type = UNKNOWN_FILE_TYPE;
}

char *
file_type2str (enum file_type file_type)
{
  switch (file_type) {
  case UNKNOWN_FILE_TYPE: default: return "unknown type file";
  case CONFIG_FILE_TYPE: return "configuration file";
  case GAME_SAVE_FILE_TYPE: return "game save file";
  case LEVEL_FILE_TYPE: return "level file";
  }
}

void
remove_config_entry (ALLEGRO_CONFIG *config, char *section, char *key)
{
  ALLEGRO_CONFIG_ENTRY *iterator;
  char const *entry = al_get_first_config_entry (config, section, &iterator);

  while (entry) {
    if (! strcasecmp (entry, key)) {
      al_remove_config_key (config, section, entry);
      entry = al_get_first_config_entry (config, section, &iterator);
    } else entry = al_get_next_config_entry (&iterator);
  }
}
