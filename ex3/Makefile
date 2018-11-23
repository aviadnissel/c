CC = gcc
CCFLAGS = -c -Wall -Wvla
LDFLAGS = -lm


# add your .c files here  (no file suffixes)
CLASSES = stack calculator calculatorUtils

# Prepare object and source file list using pattern substitution func.
OBJS = $(patsubst %, %.o,  $(CLASSES))
SRCS = $(patsubst %, %.c, $(CLASSES))

all: $(OBJS) libstack.a
	$(CC) $(OBJS) $(LDFLAGS) -L. -lstack -o calc

%.o: %.c
	$(CC) $(CCFLAGS) $*.c

LIBOBJECTS = stack.o

libstack.a: ${LIBOBJECTS}
	ar rcs libstack.a ${LIBOBJECTS}


depend:
	makedepend -- $(CCFLAGS) -- $(SRCS)
# DO NOT DELETE
