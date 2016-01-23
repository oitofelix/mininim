#
# Makefile.mk -- Auxiliary pure Makefile
#
# Copyright (C) 2016 Bruno Félix Rezende Ribeiro <oitofelix@gnu.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

uninstall-man : uninstall-man-local

uninstall-man-local :
	${AM_V_at} for f in ${l10n_man}; do \
	  LINGUA=$$(echo $$f | $(SED) 's/mininim\.\(.*\)\.1/\1/'); \
	  $(RM) -f ${DESTDIR}${mandir}/$${LINGUA}.UTF-8/man1/mininim.1; done;
