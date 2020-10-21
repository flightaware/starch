CC ?= gcc
CFLAGS = -O3 -Wall -g

STARCH_COMPILE := $(CC) $(CFLAGS) -c

ARCH := $(shell uname -m)

all: benchmark testit

ifneq (,$(findstring arm,$(ARCH)))
  include src/makefile.arm
else ifneq (,$(findstring x86_64,$(ARCH)))
  include src/makefile.x86_64
else
  include src/makefile.generic
endif

support.o: support.c
	$(CC) $(CFLAGS) -c -o $@ $^

testit: $(STARCH_OBJS) support.o

benchmark: $(STARCH_OBJS) $(STARCH_BENCHMARK_OBJ) support.o
	$(CC) $(CFLAGS) -o $@ $^

generate:
	./generate.py

clean:
	rm -f $(STARCH_OBJS) $(STARCH_BENCHMARK_OBJ) support.o benchmark

