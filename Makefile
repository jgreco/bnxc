CC = gcc
#modify the CFLAGS variable as needed.  specifically you will probably need to modify the include directory arguments
CFLAGS = -O2 -Wall -Wextra -Wno-unused-parameter -pedantic -pipe `pkg-config --cflags xmms2-client` `pkg-config --cflags  glib-2.0`
LIBS = -lxmmsclient -lncursesw -lxmmsclient-glib -lglib-2.0 -lpthread
OBJDIR = .build
OBJECTS = main.o db.o utils.o frontend.o player.o now_playing.o
OBJECTS :=  $(addprefix ${OBJDIR}/,${OBJECTS})

bnxc: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o bnxc

test: $(OBJECTS)
	$(CC) $(CFLAGS) $(LIBS) $(OBJECTS) -o test

${OBJDIR}/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi #create directory if it doesn't exist
	$(CC) $(CFLAGS) -c -o $@ $<

install: bnxc
	cp ./bnxc /usr/bin/

clean:
	rm -f $(OBJECTS) bnxc test
