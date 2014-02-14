# Don't set prefix, umake does.
DLMODEXT = .so
PACKAGE_BRAND = gostai
URBI_ENV = $(UMAKE_URBI_ENV)
URBI_HOST = arm-none-linux-gnueabi
brandincludedir = ${includedir}/${PACKAGE_BRAND}
brandlibdir = ${libdir}/${PACKAGE_BRAND}
brandlibexecdir = ${libexecdir}/${PACKAGE_BRAND}
builddir = $(UMAKE_BUILD_DIR)
remotedir = ${brandlibdir}/remote
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
SDK_CFLAGS =  -pthread -pthread
SDK_CXXFLAGS =  -pthread -pthread -isystem /home/gostai/ardrone/lib/boost_1_46/__install//include -pthread 
SDK_LDFLAGS = 
SDK_LIBS =    

# CPP.
CPP = arm-none-linux-gnueabi-gcc -E

# Upper case.
URBI_ENV_CPPFLAGS = -DURBI_ENV_$(shell echo "$(URBI_ENV)" | tr 'a-z' 'A-Z')

CPPFLAGS = -DNDEBUG -I$(includedir) $(URBI_ENV_CPPFLAGS)
ALL_CPPFLAGS = $(PRE_CPPFLAGS) $(CPPFLAGS) $(EXTRA_CPPFLAGS)

# C.
CC = arm-none-linux-gnueabi-gcc
CFLAGS = -msoft-float -fpromote-loop-indices -funroll-loops -O3 $(SDK_CFLAGS)

# C++.
WARNING_CXXFLAGS = -Wno-deprecated -Wall -W -Woverloaded-virtual -Wformat -Wcast-align -Wcast-qual -Wwrite-strings -Wmissing-declarations
CXXFLAGS = $(WARNING_CXXFLAGS) -msoft-float -fpromote-loop-indices -funroll-loops -O3 $(SDK_CXXFLAGS)
CXX = arm-none-linux-gnueabi-g++

# Ld.
LD = arm-none-linux-gnueabi-gcc
LIBS = $(SDK_LIBS)
LDFLAGS =  -L/home/gostai/ardrone/lib/boost_1_46/__install//lib -R/home/gostai/ardrone/lib/boost_1_46/__install//lib $(SDK_LDFLAGS)

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

# Install.
INSTALL_LIB = install -m 755
LTINSTALL_LIB = $(LIBTOOL) --mode=install $(INSTALL_LIB)

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

# Library to create without its extension.
lib_noext = $(OUTSHLIB:$(DLMODEXT)=)
# Its directory.
lib_dir = $(dir $(lib_noext))
# Its base name (no directory, no extension).
lib_base = $(notdir $(lib_noext))
# Libtool library.
lib_la = $(lib_noext).la

# Absolute path to OUTSHLIB.
OUTSHLIB_ABS = $(shell cd $(dir $(OUTSHLIB)) && pwd)/$(notdir $(OUTSHLIB))
$(OUTSHLIB): $(LTOBJECTS) $(ARGUMENT_LIBS)
	$(CXXLINK) -export-dynamic -o $(lib_la)	\
	  $(LTOBJECTS) $(ARGUMENT_LIBS) $(LIBS)	\
	  -rpath $(remotedir) $(LIBADD) -avoid-version
	mkdir -p $(lib_dir)
# Sometimes people build libraries with dependencies that are not
# sufficiently exposed to Libtool.  It then believes that it's going
# to do something wrong, and refuses to do it.  It might indeed go
# wrong, but more often than never, it works.  So let's proceed
# anyway, keeping fingers crossed.
	if $(LTINSTALL_LIB) $(lib_la) $(OUTSHLIB_ABS) 2>$@.log; then	\
	  sed >&2 -n							\
	    -e '/warning: remember to run `libtool --finish/!p' $@.log;	\
	  rm -f $@.log;							\
	else								\
	  echo >&2 "umake: Libtool failed to install the library:";	\
	  sed >&2 -e 's/^/umake: /g' $@.log;				\
	  echo >&2 "umake: proceeding by hand.";			\
	  rm -f $@.log;							\
	  lib_so=$$(ls $(lib_dir).libs/$(lib_base)*$(DLMODEXT)		\
	            | head -n 1);					\
	  $(INSTALL_LIB) $$lib_so $(OUTSHLIB);				\
	fi


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
	    $(remotedir)/$(LIBNAME)$(DLMODEXT) $(UMAKE_EXTRALIB) \
	    $(EXTRA_LDFLAGS)

clean:
	$(RM) *.lo *.o
