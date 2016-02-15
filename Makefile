CFLAGS += -Wall -O2
prefix=/usr/local
# NOTE: has to be /usr or snapcraft crashes
prefix=/usr
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
all: piglow
piglow: piglow.o sn3218.o main.o
piglow.o: piglow.c piglow.h sn3218.h
sn3218.o: sn3218.c sn3218.h
main.o: main.c piglow.h
clean:
	rm -f piglow.o sn3218.o main.o piglow
install: piglow
	install --directory $(DESTDIR)$(bindir)
	install -m 0755 piglow $(DESTDIR)$(bindir)
