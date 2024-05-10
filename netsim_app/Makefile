all: netsim_helper netsim

CC ?= $(CROSS_COMPILE)gcc
CFLAGS ?= -g -Wall -Werror
TARGET ?= netsim
LDFLAGS ?= -lpthread -lrt

.PHONY: clean
clean:
	rm -rf netsim *.o

netsim: netsim.c netsim_status.c netsim_profiles.c
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) -lcivetweb -lcjson
