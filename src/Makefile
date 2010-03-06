# copyright (c) 2009-2010 Blaise-Florentin Collin

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

# release
CCFLAGS=-O3
LDFLAGS=-O3
# debug
#CCFLAGS= -O0 -g
#LDFLAGS= -O0 -g

INCS=/usr/include/qt4
LIBS=-lQtCore -lQtGui -lavformat -lavcodec -lswscale -laviplay
BIN= AstroAviBrowser


# STANDARD RULES

all : $(BIN)

clean :
	rm -rf *.o
	rm -rf *.moc
	rm -f $(BIN)

# FILES RULES

$(BIN) : main.o Qmainwindow.o Qframedisplay.o Qframelist.o processing.o aviwriter.o Qhistogram.o filewriter.o bmpwriter.o
	$(LD) $(LDFLAGS) -o $(BIN) main.o Qmainwindow.o Qframedisplay.o Qframelist.o processing.o aviwriter.o Qhistogram.o filewriter.o bmpwriter.o $(LIBS)

main.o : main.cpp version.hpp config.h Qmainwindow.hpp
	$(CC) $(CCFLAGS) main.cpp -I $(INCS)

Qmainwindow.moc : Qmainwindow.hpp config.h
	$(MOC) Qmainwindow.hpp -o Qmainwindow.moc

Qmainwindow.o : Qmainwindow.cpp Qmainwindow.moc Qframedisplay.hpp Qframelist.hpp Qhistogram.hpp version.hpp processing.hpp aviwriter.hpp
	$(CC) $(CCFLAGS) Qmainwindow.cpp -I $(INCS)

Qframedisplay.moc : Qframedisplay.hpp config.h
	$(MOC) Qframedisplay.hpp -o Qframedisplay.moc

Qframedisplay.o : Qframedisplay.cpp Qframedisplay.moc processing.hpp Qhistogram.hpp
	$(CC) $(CCFLAGS) Qframedisplay.cpp -I $(INCS)

Qframelist.moc : Qframelist.hpp Qframedisplay.hpp Qhistogram.hpp config.h processing.hpp aviwriter.hpp
	$(MOC) Qframelist.hpp -o Qframelist.moc

Qframelist.o : Qframelist.cpp Qframelist.moc Qhistogram.hpp
	$(CC) $(CCFLAGS) Qframelist.cpp -I $(INCS)

Qhistogram.moc : Qhistogram.hpp config.h
	$(MOC) Qhistogram.hpp -o Qhistogram.moc

Qhistogram.o : Qhistogram.cpp Qhistogram.moc
	$(CC) $(CCFLAGS) Qhistogram.cpp -I $(INCS)

processing.o : processing.hpp processing.cpp config.h
	$(CC) $(CCFLAGS) processing.cpp -I $(INCS)

filewriter.o : filewriter.cpp filewriter.hpp
	$(CC) $(CCFLAGS) filewriter.cpp -I $(INCS)

aviwriter.o : aviwriter.cpp aviwriter.hpp filewriter.hpp processing.hpp config.h
	$(CC) $(CCFLAGS) aviwriter.cpp -I $(INCS)

bmpwriter.o : bmpwriter.cpp bmpwriter.hpp filewriter.hpp processing.hpp config.h
	$(CC) $(CCFLAGS) bmpwriter.cpp -I $(INCS)
