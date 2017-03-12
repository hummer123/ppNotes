#!/bin/bash 
OUTPUT = ../output
ifeq ($(x86),y)
TOOL_PREFIX = 
BIN = libthread_base.so
else
TOOL_PREFIX = 
BIN = libthread_base.so
endif

CPP = $(TOOL_PREFIX)g++
CC = $(TOOL_PREFIX)gcc
CFLAGS = $(CFLAG) -fPIC -c -Wall -D_GNU_SOURCE
STRIP = $(TOOL_PREFIX)strip
MKSHLIB = $(TOOL_PREFIX)g++ -shared

CPPOBJ := $(patsubst %.cpp,%.o,$(wildcard *.cpp))
COBJ := $(patsubst %.c,%.o,$(wildcard *.c))
LIBOBJ = $(CPPOBJ) $(COBJ)
%.o:%.cpp
	$(CPP) -lpthread $(CFLAGS) $<
%.o:%.c
	$(CC) -lpthread $(CFLAGS) $<
$(BIN):$(LIBOBJ)
	$(MKSHLIB) -o $(BIN) $(LIBOBJ)
	$(STRIP) $(BIN)
	cp $(BIN) ../../lib/
clean:
	rm -rf $(BIN) *.o
			
			
