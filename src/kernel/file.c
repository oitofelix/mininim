/*
  file.c -- file module;

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

void *
load_resource (char *filename, load_resource_f lrf)
{
  void *resource = NULL;
  char *data_path_filename;
  char *user_data_path_filename;
  char *resources_path_filename;
  char *system_data_path_filename;

  /* user data path */
  if (! resource) {
    xasprintf (&user_data_path_filename, "%s/%s", user_data_dir, filename);
    resource = lrf (user_data_path_filename);
    al_free (user_data_path_filename);
  }

  /* data path */
  if (data_dir && ! resource) {
    xasprintf (&data_path_filename, "%s/%s", data_dir, filename);
    resource = lrf (data_path_filename);
    al_free (data_path_filename);
  }

  /* current working directory */
  if (! resource) resource = lrf (filename);

  /* resources path */
  if (! resource) {
    xasprintf (&resources_path_filename, "%s/%s", resources_dir, filename);
    resource = lrf (resources_path_filename);
    al_free (resources_path_filename);
  }

  /* system data path */
  if (! resource) {
    xasprintf (&system_data_path_filename, "%s/%s", system_data_dir, filename);
    resource = lrf (system_data_path_filename);
    al_free (system_data_path_filename);
  }

  return resource;
}

ALLEGRO_FILE *
xfopen_r (char *filename)
{
  return al_fopen (filename, "rb");
}

int8_t *
load_file (char *filename)
{
  ALLEGRO_FILE *f = xfopen_r (filename);
  if (! f) return NULL;
  int64_t s = al_fsize (f);
  if (s == -1) return NULL;
  int8_t *mf = xmalloc (s);
  al_fread (f, mf, s);
  al_fclose (f);
  return mf;
}
