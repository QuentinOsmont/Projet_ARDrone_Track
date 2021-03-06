# Don't set prefix, umake does.
DLMODEXT = .so
PACKAGE_BRAND = gostai
URBI_ENV = $(UMAKE_URBI_ENV)
URBI_HOST = i686-pc-linux-gnu
brandincludedir = ${includedir}/${PACKAGE_BRAND}
brandlibdir = ${libdir}/${PACKAGE_BRAND}
brandlibexecdir = ${libexecdir}/${PACKAGE_BRAND}
builddir = $(UMAKE_BUILD_DIR)
envdir = ${brandlibdir}/${URBI_ENV}
exec_prefix = ${prefix}
includedir = ${prefix}/include
# On Windows, libdir is defined as ${bindir}.
bindir = ${exec_prefix}/bin
libdir = ${exec_prefix}/lib
libexecdir = ${exec_prefix}/libexec

# The basename (no extension) of the library we should link against.
# This default value is for backward compatibility: find the
# (hopefully) unique lib here.
LIBNAME = $(UMAKE_LIBNAME)

# SDK flags.
SDK_CFLAGS =  -pthread
SDK_CXXFLAGS =  -pthread -isystem /home/build/local/boost-1.38/include/boost-1_38  -pthread
SDK_LDFLAGS = -L${envdir}
SDK_LIBS =   

# CPP.
CPP = gcc -E

# Upper case.
URBI_ENV_CPPFLAGS = -DURBI_ENV_$(shell echo "$(URBI_ENV)" | tr 'a-z' 'A-Z')

CPPFLAGS = -I/home/build/local/include -DNDEBUG -I$(includedir) $(URBI_ENV_CPPFLAGS)
ALL_CPPFLAGS = $(PRE_CPPFLAGS) $(CPPFLAGS) $(EXTRA_CPPFLAGS)

# C.
CC = gcc
CFLAGS = -g -O2 -O3 $(SDK_CFLAGS)

# C++.
WARNING_CXXFLAGS = -Wno-deprecated -Wall -W -Woverloaded-virtual -Wformat 
CXXFLAGS = $(WARNING_CXXFLAGS) -O3 $(SDK_CXXFLAGS)
CXX = g++

# Ld.
LD = gcc
LIBS = $(SDK_LIBS)
LDFLAGS = -L/home/build/local/lib -L/home/build/local/boost-1.38/lib -R/home/build/local/boost-1.38/lib $(SDK_LDFLAGS)

# Libtool.
LIBTOOL = $(brandlibexecdir)/libtool
LTOBJECTS = $(OBJECTS:.o=.lo)
OUTBIN = nothing
OUTLIB = nothing2

# Commands to compile and link.  Inspired from Automake's.
COMPILE = $(CC) $(ALL_CPPFLAGS) $(CFLAGS)
LTCOMPILE = $(LIBTOOL) --tag=C --mode=compile $(COMPILE)
CXXCOMPILE = $(CXX) $(ALL_CPPFLAGS) $(CXXFLAGS)
LTCXXCOMPILE = $(LIBTOOL) --tag=CXX --mode=compile $(CXXCOMPILE)
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link \
  $(CXX) -module $(CXXFLAGS) $(LDFLAGS) $(EXTRA_LDFLAGS)

# We use GNU Make pattern rules so that we can provide extra
# dependencies.  Before changing something, read the doc.
#
#    Suffix rules cannot have any prerequisites of their own.  If they
#    have any, they are treated as normal files with funny names, not as
#    suffix rules.  Thus, the rule:
#
#      .c.o: foo.h
#              $(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
#
#    tells how to make the file `.c.o' from the prerequisite file `foo.h',
#    and is not at all like the pattern rule:
#
#      %.o: %.c foo.h
#              $(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<
#
#    which tells how to make `.o' files from `.c' files, and makes all `.o'
#    files using this pattern rule also depend on `foo.h'.
$(builddir)/%.lo: %.c $(HEADERS)
	$(LTCOMPILE) -c -o $@ $<

$(builddir)/%.lo: %.cc $(HEADERS)
	$(LTCXXCOMPILE) -c -o $@ $<

$(builddir)/%.lo: %.cpp $(HEADERS)
	$(LTCXXCOMPILE) -c -o $@ $<

# UObject target.
$(OUTLIB): $(LTOBJECTS) $(ARGUMENT_LIBS)
	$(CXXLINK) $(LTOBJECTS) $(ARGUMENT_LIBS) $(LIBS) -o $@

lalib = $(OUTSHLIB:$(DLMODEXT)=.la)
ABS_OUTSHLIB = $(shell cd $(dir $(OUTSHLIB)) && pwd)/$(notdir $(OUTSHLIB))
$(OUTSHLIB): $(LTOBJECTS) $(ARGUMENT_LIBS)
	$(CXXLINK) -export-dynamic -o $(lalib)	\
	  $(LTOBJECTS) $(ARGUMENT_LIBS) $(LIBS)	\
	  -rpath $(envdir) $(LIBADD) -avoid-version
	$(LIBTOOL) --mode=install install -m 755 $(lalib) $(ABS_OUTSHLIB)


# The command to generate the library.
#
# We pass -whole-archive because there are global objects to be
# created that are not called by the main compilation units.  In that
# case the linker does not add these objects files in the result, and
# the final executable does not have the global objects contructed.
UMAKE_LINK = umake-link
LINK =						\
  $(UMAKE_LINK)					\
  --libtool='$(LIBTOOL)'			\
  --env='$(URBI_ENV)'				\
  --host='$(URBI_HOST)'				\
  --
$(OUTBIN): $(LTOBJECTS) $(ARGUMENT_LIBS)
	$(LINK) \
	    $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ \
	    $(ARGUMENT_LIBS) $(LIBS) $(LTOBJECTS) \
	    $(envdir)/$(LIBNAME)$(DLMODEXT) $(UMAKE_EXTRALIB) \
	    $(EXTRA_LDFLAGS)

clean:
	$(RM) *.lo *.o
