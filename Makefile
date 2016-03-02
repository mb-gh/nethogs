VERSION      := 0
SUBVERSION   := 8
MINORVERSION := 2-SNAPSHOT

#prefix := /usr
prefix := /usr/local

sbin := $(prefix)/sbin
man8 := $(prefix)/share/man/man8

all: nethogs decpcap_test

runtests: test
	./test
	
	
# nethogs_testsum

ifeq ($(DEBUG),1)
  $(info debug mode)
  CFLAGS?=-Wall -Wextra -O0 -g
  CXXFLAGS?=-Wall -Wextra -O0 -g --std=c++11
else
  $(info release mode)
  CFLAGS?=-Wall -Wextra -O3
  CXXFLAGS?=-Wall -Wextra -O3 --std=c++11
endif

OBJS=packet.o \
	 connection.o \
	 process.o \
	 refresh.o \
	 decpcap.o \
	 cui.o \
	 inode2prog.o \
	 conninode.o \
	 devices.o \
	 ipc_server.o

NCURSES_LIBS?=-lncurses

.PHONY: tgz

tgz: clean
	cd .. ; tar czvf nethogs-$(VERSION).$(SUBVERSION).$(MINORVERSION).tar.gz --exclude-vcs nethogs/*

.PHONY: check uninstall
check:
	@echo "Not implemented"

install: nethogs nethogs.8
	install -d -m 755 $(DESTDIR)$(sbin)
	install -m 755 nethogs $(DESTDIR)$(sbin)
	install -d -m 755 $(DESTDIR)$(man8)
	install -m 644 nethogs.8 $(DESTDIR)$(man8)
	@echo
	@echo "Installed nethogs to $(DESTDIR)$(sbin)"
	@echo
	@echo "You might have to add this directory to your PATH and/or refresh your shells' path cache with a command like 'hash -r'."

uninstall:
	rm $(DESTDIR)$(sbin)/nethogs
	rm $(DESTDIR)$(man8)/nethogs.8

test: test.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) test.cpp -o test -lpcap -lm ${NCURSES_LIBS} -DVERSION=\"$(VERSION)\" -DSUBVERSION=\"$(SUBVERSION)\" -DMINORVERSION=\"$(MINORVERSION)\"

nethogs: main.cpp nethogs.cpp $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) main.cpp $(OBJS) -o nethogs -lpcap -lpthread -lm ${NCURSES_LIBS} -DVERSION=\"$(VERSION)\" -DSUBVERSION=\"$(SUBVERSION)\" -DMINORVERSION=\"$(MINORVERSION)\"
nethogs_testsum: nethogs_testsum.cpp $(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) nethogs_testsum.cpp $(OBJS) -o nethogs_testsum -lpcap -lm ${NCURSES_LIBS} -DVERSION=\"$(VERSION)\" -DSUBVERSION=\"$(SUBVERSION)\" -DMINORVERSION=\"$(MINORVERSION)\"

decpcap_test: decpcap_test.cpp decpcap.o
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) decpcap_test.cpp decpcap.o -o decpcap_test -lpcap -lm

#-lefence

refresh.o: refresh.cpp refresh.h nethogs.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c refresh.cpp
process.o: process.cpp process.h nethogs.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c process.cpp
packet.o: packet.cpp packet.h nethogs.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c packet.cpp
connection.o: connection.cpp connection.h nethogs.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c connection.cpp
decpcap.o: decpcap.c decpcap.h
	$(CC) $(CPPFLAGS) $(CFLAGS) -c decpcap.c
inode2prog.o: inode2prog.cpp inode2prog.h nethogs.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c inode2prog.cpp
conninode.o: conninode.cpp nethogs.h conninode.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c conninode.cpp
ipc_server.o: ipc_server.cpp ipc_server.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c ipc_server.cpp
#devices.o: devices.cpp devices.h
#	$(CXX) $(CXXFLAGS) -c devices.cpp
cui.o: cui.cpp cui.h nethogs.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c cui.cpp -DVERSION=\"$(VERSION)\" -DSUBVERSION=\"$(SUBVERSION)\" -DMINORVERSION=\"$(MINORVERSION)\"

.PHONY: clean
clean:
	rm -f $(OBJS)
	rm -f nethogs
	rm -f test
	rm -f decpcap_test
