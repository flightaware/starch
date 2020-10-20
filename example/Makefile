CC ?= gcc
CFLAGS = -O3 -Wall -g -fopt-info-vec

STARCH_COMPILE := $(CC) $(CFLAGS) -c

ARCH := $(shell uname -m)

all: testit

ifneq (,$(findstring arm,$(ARCH)))
  include src/makefile.arm
else ifneq (,$(findstring x86_64,$(ARCH)))
  include src/makefile.x86_64
else
  include src/makefile.generic
endif

testit: $(STARCH_OBJS)

generate:
	./generate.py

clean:
	rm -f $(STARCH_OBJS)
