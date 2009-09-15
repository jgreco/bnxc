CC = gcc
#modify the CFLAGS variable as needed.  specifically you will probably need to modify the include directory arguments
CFLAGS = -O2 -Wall -Wextra -Wno-unused-parameter -pedantic -pipe -I/usr/local/include/xmms2 -I/usr/include/glib-2.0/ -I/usr/lib64/glib-2.0/include/ -I/usr/include/glib-2.0/glib/
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

clean:
	rm -f $(OBJECTS) bnxc test
