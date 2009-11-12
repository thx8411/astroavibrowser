# part of AstroAviBrowser, (c) Blaise-Florentin Collin 2009

CC=gcc -c
LD=gcc
MOC=/usr/share/qt4/bin/moc
CCFLAGS=-O3 -march=i686
LDFLAGS=-O3 -march=i686
INCS=/usr/include/qt4
LIBS=-lQtCore -lQtGui -lavformat -lavcodec -lswscale
BIN= AstroAviBrowser

all : $(BIN)

install : all

clean :
	rm -rf *.o
	rm -rf *.moc
	rm -f $(BIN)

$(BIN) : main.o Qmainwindow.o Qframedisplay.o Qframelist.o bayer.o
	$(LD) $(LDFLAGS) -o $(BIN) main.o Qmainwindow.o Qframedisplay.o Qframelist.o bayer.o $(LIBS)

main.o : main.cpp version.hpp
	$(CC) $(CCFLAGS) main.cpp -I $(INCS)

Qmainwindow.moc : Qmainwindow.hpp
	$(MOC) Qmainwindow.hpp -o Qmainwindow.moc

Qmainwindow.o : Qmainwindow.cpp Qmainwindow.moc Qframedisplay.hpp Qframelist.hpp version.hpp
	$(CC) $(CCFLAGS) Qmainwindow.cpp -I $(INCS)

Qframedisplay.moc : Qframedisplay.hpp
	$(MOC) Qframedisplay.hpp -o Qframedisplay.moc

Qframedisplay.o : Qframedisplay.cpp Qframedisplay.moc bayer.hpp
	$(CC) $(CCFLAGS) Qframedisplay.cpp -I $(INCS)

Qframelist.moc : Qframelist.hpp
	$(MOC) Qframelist.hpp -o Qframelist.moc

Qframelist.o : Qframelist.cpp Qframelist.moc
	$(CC) $(CCFLAGS) Qframelist.cpp -I $(INCS)

bayer.o : bayer.hpp bayer.cpp
	$(CC) $(CCFLAGS) bayer.cpp -I $(INCS)
