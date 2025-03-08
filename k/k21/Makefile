# Makefile for Joe's Own Editor, modified to be K

###############
# Configuration
###############

# Set where you want joe to go, where you
# want joe's initialization file (krc)
# to go and where you want the man page
# to go:

WHEREBIN = /usr/local/bin
WHERERC = /usr/local/lib
WHEREMAN = /usr/man/man1

# If you want to be able to edit '-', which causes joe to read in or write out
# to the stdin/stdout, change the '1' below to '0'.  Be warned however: this
# makes joe use /dev/tty to open the tty, which means that the modification
# times on the real tty don't get updated.  Idle session killers and screen
# blankers will think that no one is using the terminal and log you out or
# blank the screen.

IDLEOUT = 1

# You may also have to add some additional
# defines to get the include files to work
# right on some systems.
#
# for some HPUX systems, you need to add:  -D_HPUX_SOURCE

#CFLAGS = -O
CFLAGS = -O2 -DFILE_LOCKING -DUNIQUE_BACKUPS

# You may have to include some extra libraries
# for some systems
#
# for Xenix, add (in this order!!): -ldir -lx
#
# For some systems you might have to add: -lbsd
# to get access to the timer system calls.
#
# If you wish to use terminfo, you have to
# add '-ltinfo', '-lcurses' or '-ltermlib',
# depending on the system.

EXTRALIBS =

# Object files
#
# If you wish to use terminfo instead of
# termcap, replace 'termcap.o' below with 'terminfo.o'

OBJS = b.o blocks.o bw.o cmd.o hash.o help.o kbd.o macro.o main.o menu.o \
 path.o poshist.o pw.o queue.o qw.o random.o rc.o regex.o scrn.o tab.o \
 termcap.o tty.o tw.o ublock.o uedit.o uerror.o ufile.o uformat.o uisrch.o \
 umath.o undo.o usearch.o ushell.o utag.o va.o vfile.o vs.o w.o zstr.o

CC = cc

# That's it!

k: $(OBJS)
	$(CC) $(CFLAGS) -o k $(EXTRALIBS) $(OBJS)

$(OBJS): config.h

config.h:
	$(CC) conf.c -o conf
	./conf $(WHERERC) $(IDLEOUT)

install: k
	strip k
	if [ ! -d $(WHEREBIN) ]; then mkdir $(WHEREBIN); chmod a+rx $(WHEREBIN); fi
	mv k $(WHEREBIN)
	if [ ! -d $(WHERERC) ]; then mkdir $(WHERERC); chmod a+rx $(WHERERC); fi
	rm -f $(WHERERC)/krc $(WHEREMAN)/k.1
	cp krc $(WHERERC)
#	cp k.1 $(WHEREMAN)
	chmod a+x $(WHEREBIN)/k
	chmod a+r $(WHERERC)/krc

clean:
	rm -f $(OBJS) termidx.o conf conf.o config.h
