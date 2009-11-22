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
	install -d $(DESTDIR)/bin; install src/AstroAviBrowser $(DESTDIR)/bin
	install -d $(DESTDIR)/share/icons; install -m 644 icons/astroavibrowser-icon.png $(DESTDIR)/share/icons
	install -d $(DESTDIR)/share/astroavibrowser/icons; install -m 644 icons/*.png $(DESTDIR)/share/astroavibrowser/icons
	install -d $(DESTDIR)/share/applications; install -m 644 astroavibrowser-entry.desktop $(DESTDIR)/share/applications

clean :
	$(MAKE) -C src clean

distclean : clean
	rm -rf debian/*.log
	rm -rf debian/files
	rm -rf debian/astroavibrowser.substvars
	rm -rf debian/astroavibrowser
