# Archivo Makefile simple

PROYECTO = serverrtcc
DESTDIR=/usr/local/bin
CFLAGS=-Wall
LFLAGS=-lm -lpthread -lwiringPi
CC=gcc
																																																																											

all: $(PROYECTO)

%.o:%.c
	$(CC) -c $< $(CFLAGS)


$(PROYECTO): $(patsubst %.c, %.o, $(wildcard *.c))
	$(CC) $^ -o $@ $(LFLAGS)

.PHONY: clean

install:all
	if [ ! -d $(DESTDIR) ] ; then \
		sudo mkdir $(DESTDIR) ; \
	fi; \
	sudo cp $(PROYECTO) $(DESTDIR)

uninstall:
	sudo rm $(DESTDIR)/$(PROYECTO)


clean:
	rm -f *.o $(PROYECTO)
	rm -f *.txt $(PROYECTO)


