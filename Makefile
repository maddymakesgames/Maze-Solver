#
# Created by gmakemake (Ubuntu Oct  3 2011) on Fri Apr 15 20:59:07 2022
#

#
# Definitions
#

.SUFFIXES:
.SUFFIXES:	.a .o .c .C .cpp .s .S
.c.o:
		$(COMPILE.c) $<
.C.o:
		$(COMPILE.cc) $<
.cpp.o:
		$(COMPILE.cc) $<
.S.s:
		$(CPP) -o $*.s $<
.s.o:
		$(COMPILE.s) -o $@ $<
.c.a:
		$(COMPILE.c) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.C.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%
.cpp.a:
		$(COMPILE.cc) -o $% $<
		$(AR) $(ARFLAGS) $@ $%
		$(RM) $%

AS =		as
CC =		gcc
CXX =		g++

RM = rm -f
AR = ar
LINK.c = $(CC) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
LINK.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS)
COMPILE.s = $(AS) $(ASFLAGS)
COMPILE.c = $(CC) $(CFLAGS) $(CPPFLAGS) -c
COMPILE.cc = $(CXX) $(CXXFLAGS) $(CPPFLAGS) -c
CPP = $(CPP) $(CPPFLAGS)
########## Flags from header.mak

CFLAGS =        -std=c99 -ggdb -Wall -Wextra -Werror -ggdb -pedantic
CLIBFLAGS =     -lm
########## End of flags from header.mak


CPP_FILES =	
C_FILES =	a_star.c mopsolver.c test.c vec.c
PS_FILES =	
S_FILES =	
H_FILES =	path_finding.h vec.h
SOURCEFILES =	$(H_FILES) $(CPP_FILES) $(C_FILES) $(S_FILES)
.PRECIOUS:	$(SOURCEFILES)
OBJFILES =	a_star.o vec.o 

#
# Main targets
#

all:	mopsolver test 

mopsolver:	mopsolver.o $(OBJFILES)
	$(CC) $(CFLAGS) -o mopsolver mopsolver.o $(OBJFILES) $(CLIBFLAGS)

test:	test.o $(OBJFILES)
	$(CC) $(CFLAGS) -o test test.o $(OBJFILES) $(CLIBFLAGS)

#
# Dependencies
#

a_star.o:	path_finding.h vec.h
mopsolver.o:	path_finding.h vec.h
test.o:	
vec.o:	vec.h

#
# Housekeeping
#

Archive:	archive.tgz

archive.tgz:	$(SOURCEFILES) Makefile
	tar cf - $(SOURCEFILES) Makefile | gzip > archive.tgz

clean:
	-/bin/rm -f $(OBJFILES) mopsolver.o test.o core

realclean:        clean
	-/bin/rm -f mopsolver test 
