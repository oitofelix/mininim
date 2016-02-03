/*
  xconfig.c -- configuration module;

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

ALLEGRO_CONFIG *
create_config (void)
{
  ALLEGRO_CONFIG *config = al_create_config ();

  if (! config)
    error (-1, 0, "%s (void): failed to create configuration",
           __func__);

  return config;
}

ALLEGRO_CONFIG *
load_config_file (const char *filename)
{
  ALLEGRO_CONFIG *config = al_load_config_file (filename);

  if (! config)
    error (-1, 0, "%s (%s): failed to load configuration file",
           __func__, filename);

  return config;
}

ALLEGRO_CONFIG *
load_config_file_f (ALLEGRO_FILE *file)
{
  ALLEGRO_CONFIG *config = al_load_config_file_f (file);

  if (! config)
    error (-1, 0, "%s (%p): failed to load configuration file",
           __func__, file);

  return config;
}

void
save_config_file (const char *filename, const ALLEGRO_CONFIG *config)
{
  if (! al_save_config_file (filename, config))
    error (-1, 0, "%s (%s, %p): failed to save configuration file",
           __func__, filename, config);
}

void
save_config_file_f (ALLEGRO_FILE *file, const ALLEGRO_CONFIG *config)
{
  if (! al_save_config_file_f (file, config))
    error (-1, 0, "%s (%p, %p): failed to save configuration file",
           __func__, file, config);
}

ALLEGRO_CONFIG *
merge_config (const ALLEGRO_CONFIG *cfg1,
              const ALLEGRO_CONFIG *cfg2)
{
  ALLEGRO_CONFIG *config = al_merge_config (cfg1, cfg2);

  if (! config)
    error (-1, 0, "%s (%p, %p): failed to merge configurations",
           __func__, cfg1, cfg2);

  return config;
}
