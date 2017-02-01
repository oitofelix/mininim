/*
  xstring.c -- string module;

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
tolower_str (char *str)
{
  size_t i;
  for (i = 0; str[i] != 0; i++) str[i] = tolower (str[i]);
}

void
toupper_str (char *str)
{
  size_t i;
  for (i = 0; str[i] != 0; i++) str[i] = toupper (str[i]);
}

void
repl_str_char (char *str, char a, char b)
{
  size_t i;
  for (i = 0; str[i] != 0; i++) if (str[i] == a) str[i] = b;
}

wchar_t *
m2w_str (const char *s)
{
  size_t ls = strlen (s);
  wchar_t *r = xcalloc (ls, sizeof (wchar_t));
  mbsrtowcs (r, &s, ls * sizeof (wchar_t), NULL);
  r = xrealloc (r, (wcslen (r) + 1) * sizeof (*r));
  return r;
}

char *
w2m_str (const wchar_t *s)
{
  size_t ls = wcslen (s);
  char *r = xcalloc (ls, sizeof (wchar_t));
  wcsrtombs (r, &s, ls * sizeof (wchar_t), NULL);
  r = xrealloc (r, (strlen (r) + 1) * sizeof (*r));
  return r;
}

char *
shorten_str (char *s, size_t max)
{
  wchar_t *ws = m2w_str (s);
  size_t lws = wcslen (ws);
  size_t le = wcslen (SHORTEN_STRING_ELLIPSIS);
  assert (max >= le + 2);
  if (lws > max) {
    wchar_t *wr = xcalloc (max + 1, sizeof (wchar_t));
    size_t lp = (max - le) / 2;
    size_t a = (max - le) % 2;
    wmemcpy (wr, ws, lp);;
    wmemcpy (wr + lp, SHORTEN_STRING_ELLIPSIS, le);
    wmemcpy (wr + lp + le, ws + lws - lp - a, lp + a);
    *(wr + lp + le + lp + a) = L'\0';
    char *r = w2m_str (wr);
    al_free (ws);
    al_free (wr);
    return r;
  } else {
    al_free (ws);
    return strdup (s);
  }
}
