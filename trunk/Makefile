# copyright (c) 2009 Blaise-Florentin Collin

# This file is part of AstroAviBrowser.

# AstroAviBrowser is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License v3
# as published by the Free Software Foundation        

# AstroAviBrowser is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.

# You should have received a copy of the GNU General Public License
# v3 along with AstroAviBrowser; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, 
# MA 02110-1301 USA


# VALUES

SHELL=/bin/bash

# STANDARD RULES

all :
	$(MAKE) -C src all

install : all
	install src/AstroAviBrowser /usr/bin
	install -d /usr/share/icons; install icons/astroavibrowser-icon.png /usr/share/icons
	install -d /usr/share/astroavibrowser/icons; install icons/*.png /usr/share/astroavibrowser/icons
	install -d /usr/share/applications; install astroavibrowser.desktop /usr/share/applications

uninstall :
	rm -rf /usr/share/icons/astroavibrowser-icon.png
	rm -rf /usr/bin/AstroAviBrowser
	rm -rf /usr/share/astroavibrowser
	rm -rf /usr/share/applications/astroavibrowser.desktop

clean :
	$(MAKE) -C src clean

distclean : clean

mostlyclean : clean

maintainer-clean : clean

info :

dvi :

dist :

check :

