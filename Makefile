CC = gcc
CFLAGS = -O3 -Wall -Wextra -Wno-unused-parameter -pedantic -pipe -I/usr/local/include/xmms2
LIBS = -lxmmsclient -lncursesw
OBJDIR = build
OBJECTS = main.o db.o utils.o frontend.o player.o
OBJECTS :=  $(addprefix ${OBJDIR}/,${OBJECTS})

bnxc: $(OBJECTS)
	$(CC) $(LIBS) $(OBJECTS) -o bnxc

${OBJDIR}/%.o : %.c
	@if [ ! -d $(OBJDIR) ]; then mkdir $(OBJDIR); fi #create directory if it doesn't exist
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJECTS) bnxc
