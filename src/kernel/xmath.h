/*
  xmath.h -- math module;

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

#ifndef MININIM_XMATH_H
#define MININIM_XMATH_H

double max_double (double a, double b);
double min_double (double a, double b);
bool equiv (bool a, bool b);
int round2multiple (int n, int m);
int next_multiple (int n, int m);
int precise_unit (int n, int m);
int max_int (int a, int b);
int min_int (int a, int b);
int cint (int *x, int *y);
unsigned char add_char (unsigned char a, signed char b);
int int_to_range (int i, int min, int max);
float dist_cart (float x0, float y0, float x1, float y1);
int next_remainder (int n, int divisor, int offset, int direction);
bool invalid (int i);

#endif	/* MININIM_XMATH_H */
