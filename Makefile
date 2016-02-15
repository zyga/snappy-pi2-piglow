CFLAGS += -Wall -O2
prefix=/usr/local
exec_prefix=$(prefix)
bindir=$(exec_prefix)/bin
all: piglow
clean:
	rm -f piglow
install: piglow
	install --directory $(DESTDIR)$(bindir)
	install -m 0755 piglow $(DESTDIR)$(bindir)
