/*
  xstring.h -- string module;

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

#ifndef MININIM_XSTRING_H
#define MININIM_XSTRING_H

#define SHORTEN_STRING_ELLIPSIS L"..."

void tolower_str (char *str);
void toupper_str (char *str);
void repl_str_char (char *str, char a, char b);
char *trim_string (char *s);
char *repeat_char (char c, size_t n);
bool str_end_matches (const char *s, const char *m);
void set_string_var (char **var, const char *value);
int str2enum (char *enum2str[], char *str);





char *hline (char c);
void fmt_begin (int ncols);
char *fmt_row (const char *fmt, ...);
char *fmt_end (void);
char *fmt_manual (const char *sep, ...);





wchar_t *m2w_str (const char *s);
char *w2m_str (const wchar_t *s);
char *shorten_str (char *s, size_t max);

#endif	/* MININIM_XSTRING_H */
