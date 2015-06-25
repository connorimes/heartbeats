CXX = /usr/bin/gcc
CXXFLAGS = -fPIC -Wall -Wno-unknown-pragmas -Iinc -Llib -O6
DBG = -g
DEFINES ?=
LDFLAGS = -shared -lpthread -lrt -lm

DOCDIR = doc
LIBDIR = lib
INCDIR = ./inc
SRCDIR = ./src

all: $(LIBDIR) $(LIBDIR)/libhbt-acc-pow.so

$(LIBDIR):
	-mkdir -p $(LIBDIR)

$(LIBDIR)/libhbt-acc-pow.so: $(SRCDIR)/heartbeat-tree-accuracy-power.c $(SRCDIR)/heartbeat-tree-util.c
	$(CXX) $(CXXFLAGS) -DHEARTBEAT_MODE_ACC_POW $(LDFLAGS) -Wl,-soname,$(@F) -o $@ $^

# Installation
install: all
	install -m 0644 $(LIBDIR)/*.so /usr/local/lib/
	mkdir -p /usr/local/include/heartbeats
	install -m 0644 $(INCDIR)/* /usr/local/include/heartbeats/

uninstall:
	rm -f /usr/local/lib/libhbt-*.so
	rm -rf /usr/local/include/heartbeats/

## cleaning
clean:
	-rm -rf $(LIBDIR) *.log *~ $(SRCDIR)/*~
