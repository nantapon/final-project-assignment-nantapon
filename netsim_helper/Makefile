all: netsim_helper

CC ?= $(CROSS_COMPILE)gcc
CFLAGS ?= -g -Wall -Werror
TARGET ?= netsim_helper 
LDFLAGS ?= -lpthread -lrt

.PHONY: clean
clean:
	rm -rf netsim_helper *.o

netsim_helper: netsim_helper.c
	$(CC) $(CFLAGS) $^ -o $@ $(INCLUDES) $(LDFLAGS) 
