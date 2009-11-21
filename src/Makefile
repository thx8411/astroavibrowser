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

CC=gcc -c
LD=gcc
MOC=/usr/share/qt4/bin/moc
CCFLAGS=-O3 -march=i686
LDFLAGS=-O3 -march=i686
INCS=/usr/include/qt4
LIBS=-lQtCore -lQtGui -lavformat -lavcodec -lswscale -laviplay
BIN= AstroAviBrowser


# RULES

all : $(BIN)

install : all

clean :
	rm -rf *.o
	rm -rf *.moc
	rm -f $(BIN)

$(BIN) : main.o Qmainwindow.o Qframedisplay.o Qframelist.o bayer.o
	$(LD) $(LDFLAGS) -o $(BIN) main.o Qmainwindow.o Qframedisplay.o Qframelist.o bayer.o $(LIBS)

main.o : main.cpp version.hpp config.h
	$(CC) $(CCFLAGS) main.cpp -I $(INCS)

Qmainwindow.moc : Qmainwindow.hpp config.h
	$(MOC) Qmainwindow.hpp -o Qmainwindow.moc

Qmainwindow.o : Qmainwindow.cpp Qmainwindow.moc Qframedisplay.hpp Qframelist.hpp version.hpp
	$(CC) $(CCFLAGS) Qmainwindow.cpp -I $(INCS)

Qframedisplay.moc : Qframedisplay.hpp config.h
	$(MOC) Qframedisplay.hpp -o Qframedisplay.moc

Qframedisplay.o : Qframedisplay.cpp Qframedisplay.moc bayer.hpp
	$(CC) $(CCFLAGS) Qframedisplay.cpp -I $(INCS)

Qframelist.moc : Qframelist.hpp config.h
	$(MOC) Qframelist.hpp -o Qframelist.moc

Qframelist.o : Qframelist.cpp Qframelist.moc
	$(CC) $(CCFLAGS) Qframelist.cpp -I $(INCS)

bayer.o : bayer.hpp bayer.cpp config.h
	$(CC) $(CCFLAGS) bayer.cpp -I $(INCS)
