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

char *
trim_string (char *s)
{
  ALLEGRO_USTR *us = al_ustr_new (s);
  al_ustr_trim_ws (us);
  char *rs = al_cstr_dup (us);
  al_ustr_free (us);
  return rs;
}

char *
repeat_char (char c, size_t n)
{
  char *s = xcalloc (n + 1, sizeof (*s));
  size_t i;
  for (i = 0; i < n; i++) s[i] = c;
  s[n] = '\0';
  return s;
}

bool
str_end_matches (const char *s, const char *m)
{
  size_t ls = strlen (s), lm = strlen (m);
  return ls >= lm && ! strcasecmp (s + ls - lm, m);
}

void
set_string_var (char **var, const char *value)
{
  char *old_str = *var;
  if (value) *var = xasprintf ("%s", value);
  else *var = NULL;
  al_free (old_str);
}

int
str2enum (char *enum2str[], char *str)
{
  for (size_t i = 0; enum2str[i]; i++)
    if (! strcmp (str, enum2str[i])) return i;
  return -1;
}





static size_t *fmt_width;
static size_t fmt_width_nmemb;

void
fmt_begin (int ncols)
{
  assert (! fmt_width && fmt_width_nmemb == 0);
  fmt_width = xcalloc (ncols, sizeof (*fmt_width));
  memset (fmt_width, 0, ncols * sizeof (*fmt_width));
  fmt_width_nmemb = ncols;
}

char *
fmt_row (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);

  char *r = NULL;

  if (fmt) r = xvasprintf (fmt, ap);
  else {
    va_arg (ap, char *); /* ignore first */
    size_t i;
    for (i = 0; i < fmt_width_nmemb; i++) {
      char *s = va_arg (ap, char *);
      size_t l = strlen (s);
      if (fmt_width[i] < l) fmt_width[i] = l;
    }
  }

  va_end (ap);

  return r;
}

unsigned int
term_cols (void)
{
  static const unsigned int MAX_TERM_COLS = 320, DEFAULT_TERM_COLS = 80;
#if WINDOWS_PORT
  CONSOLE_SCREEN_BUFFER_INFO csbi;
  GetConsoleScreenBufferInfo (GetStdHandle (STD_OUTPUT_HANDLE), &csbi);
  unsigned int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
  return cols <= MAX_TERM_COLS ? cols : DEFAULT_TERM_COLS;
#else
  struct winsize win;
  if (ioctl (STDOUT_FILENO, TIOCGWINSZ, &win) < 0) return DEFAULT_TERM_COLS;
  else return win.ws_col <= MAX_TERM_COLS ? win.ws_col : DEFAULT_TERM_COLS;
#endif
}

char *
fmt_end (void)
{
  const char *sep = " ";

  int total_width = 0;
  size_t i;
  for (i = 0; i < fmt_width_nmemb; i++)
    total_width += fmt_width[i];

  int avail_width =
    max_int (0, term_cols () - 1 - (fmt_width_nmemb - 1) * strlen (sep));

  if (total_width == 0) total_width = 1;

  char *h = xasprintf ("%%s");

  for (i = 0; i < fmt_width_nmemb; i++) {
    int w = (fmt_width[i] * avail_width) / total_width;
    if (total_width < avail_width) {
      w = fmt_width[i] + (avail_width - total_width) / fmt_width_nmemb;
    } else if (w < fmt_width[i] / 2.0 && avail_width >= fmt_width[i] + 1) {
      w = fmt_width[i];
      total_width -= w;
      avail_width -= w + 1;
    }

    assert (avail_width >= 0);

    char *old_h = h;
    h = xasprintf ("%1$s%%-%2$i.%2$is%3$s", old_h, w,
                   i + 1 < fmt_width_nmemb ? sep : "");
    al_free (old_h);
  }

  char *old_h = h;
  h = xasprintf ("%s%%s", old_h);
  al_free (old_h);

  al_free (fmt_width);
  fmt_width = NULL;
  fmt_width_nmemb = 0;

  return h;
}

char *
fmt_manual (const char *sep, ...)
{
  int *width = NULL;
  size_t width_nmemb = 0;

  va_list ap;
  va_start (ap, sep);

  size_t i;
  int w;
  for (i = 0; (w = va_arg (ap, int)); i++)
    width = add_to_array (&w, 1, width, &width_nmemb,
                          width_nmemb, sizeof (w));

  int fixed_width = (width_nmemb - 1) * strlen (sep);
  int var_parts = 0;
  for (i = 0; i < width_nmemb; i++)
    if (width[i] > 0) fixed_width += width[i];
    else var_parts += abs (width[i]);

  int avail_width = max_int (0, term_cols () - 1 - fixed_width);
  char *h = xasprintf ("%%s");
  for (i = 0; i < width_nmemb; i++) {
    int w;
    if (width[i] > 0) w = width[i];
    else w = (abs (width[i]) * avail_width) / var_parts;
    char *old_h = h;
    h = xasprintf ("%1$s%%-%2$i.%2$is%3$s", old_h, w,
                   i + 1 < width_nmemb ? sep : "");
    al_free (old_h);
  }

  char *old_h = h;
  h = xasprintf ("%s%%s", old_h);
  al_free (old_h);

  va_end (ap);

  return h;
}

char *
hline (char c)
{
  return repeat_char (c, term_cols () - 1);
}





wchar_t *
m2w_str (const char *s)
{
  size_t ls = strlen (s);
  wchar_t *r = xcalloc (ls, sizeof (wchar_t));
  mbsrtowcs (r, &s, ls, NULL);
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
