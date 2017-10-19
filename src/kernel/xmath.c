/*
  xmath.c -- math module;

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

bool
equiv (bool a, bool b)
{
  return (a && b) || (! a && ! b);
}

int
round2multiple (int n, int m)
{
  assert (m != 0);
  if (n % abs (m) == 0) return n;
  else if (m < 0) return (n / -m) * -m;
  else if (m > 0) return ((n / m) + 1) * m;
  return 0;
}

int
next_multiple (int n, int m)
{
  assert (m != 0);
  if (n % abs (m) == 0) return n + m;
  else return round2multiple (n, m);
}

int
precise_unit (int n, int m)
{
  return round2multiple (n, m) / m;
}

int
max_int (int a, int b)
{
  return (a > b) ? a : b;
}

int
min_int (int a, int b)
{
  return (a < b) ? a : b;
}

int
cint (int *x, int *y)
{
  return *x - *y;
}

int
int_eq (int *a, int *b)
{
  return *a == *b;
}

unsigned char
add_char (unsigned char a, signed char b)
{
  if (b < 0 && -b > a) return 0;
  if (b > 0 && b > UCHAR_MAX - a) return UCHAR_MAX;
  return a + b;
}

int
int_to_range (int i, int min, int max)
{
  if (i >= max) return max;
  else if (i <= min) return min;
  else return i;
}

float
dist_cart (float x0, float y0, float x1, float y1)
{
  int dx = x0 - x1;
  int dy = y0 - y1;
  return sqrt (dx * dx + dy * dy);
}

int
next_remainder (int n, int divisor, int offset, int direction)
{
  int c = n - offset + direction;
  if (c < 0) c += divisor - offset;
  return c % (divisor - offset) + offset;
}

bool
invalid (int i)
{
  return i == INT_MIN;
}
