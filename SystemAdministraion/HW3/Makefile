ifeq ($(PREFIX),)
PREFIX := /usr/local
endif

all:
	@echo "Use this with 'make install'"

.PHONY: install

install:
	install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 zbackup $(DESTDIR)$(PREFIX)/bin/
	install -d $(DESTDIR)$(PREFIX)/etc/rc.d/
	install -m 755 ftp-watchd $(DESTDIR)$(PREFIX)/etc/rc.d

