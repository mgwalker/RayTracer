#    file: makefile for libgm; created because original used imakefile.
# created: 1-17-2005
#  author: J. Edward Swan II
###############################################################################
# Source file information

MAIN	=	libgm.a

SRCS    =	gmMat3.cc gmMat4.cc
OBJS	=	$(SRCS:.cc=.o)

###############################################################################
# General compilation settings.  
DESTDIR		= .
INCLUDE 	= -I.
DEFINES 	= 

# For an optimized version, put this definition last.  Note that according to
# the gcc/g++ web page, inlined functions are not actually inlined unless the
# "-O3" flag is specified.  "-DNDEBUG" removes assertions.
OPTIM		= -DNDEBUG -O3
# For a debug version, put this definition last.  "-Wall" prints copious,
# lint-type error messages.  "-g" produces a symbol table suitable for a
# debugger such as GDB.
OPTIM		= -Wall -g

# Collect all the compilation settings
CPPFLAGS	= $(OPTIM) $(INCLUDE) $(DEFINES)

###############################################################################
# Programs to run
RM		= /bin/rm -f
MAKE		= /bin/make
MKDEPEND	= makedepend $(DEFINES) $(INCLUDE) $(SRCS)
AR		= ar qcs

###############################################################################
# Explicit rules
.PRECIOUS: $(SRCS)

$(MAIN): $(OBJS)
	$(AR) $(DESTDIR)/$@ $(OBJS)

all:
	$(MAKE) clean
	$(MAKE) depend
	$(MAKE) $(MAIN)

depend:	
	$(MKDEPEND)	

clean:
	$(RM) *.o $(MAIN)

###############################################################################
# Dependency info
# Do not put stuff below this line; makedepend will clobber it!
# DO NOT DELETE THIS LINE -- make depend depends on it.

gmMat3.o: gmMat3.h ./gmUtils.h ./gmConst.h gmVec2.h gmVec3.h
gmMat4.o: gmMat4.h ./gmUtils.h ./gmConst.h gmVec3.h gmVec4.h
