# Makefile
# Author: Zach Kissel
# Date: 2/9/15
# Description: A generic makefile that can be used for any
# project.

#Program Name (Modify as needed.)
PROG=shell

# Name of compiler. You should not modify this.
CC=gcc

# Option needed for the linker for example (-lm or -lpthread)
LDOPTS=

# Flags to the C compiler. You may add to this but do NOT
# remove from it. The base required flags are
# -Wall and -pedantic. The -g flag compiles your code with
# symbols so you can use the debugger without assembly.
# The -I option tells the compiler additional places to
# look for header files.
CFLAGS=-g -Wall -pedantic -I head

# The name of the object files (modify as needed)
# Basically this is just all our C files with a .o
# extension.
OBJECTS= history.o \
	 joblist.o \
	 shell.o

# How to link together all the object files
# Do NOT modify
LINK.c= $(CC) $(CFLAGS) $(LDFLAGS)

# How to compile all the source files
# Do NOT modify.
%.o:%.c
	$(CC) $(CFLAGS) -c $<

# Options to make.
# Do NOT modify
all: $(PROG)
clean:
	rm -f $(OBJECTS)
clobber: clean
	rm -f $(PROG)

#
# Create the program. Do NOT modify
#
$(PROG): $(OBJECTS)
	$(LINK.c) $(OBJECTS) -o $(PROG)
