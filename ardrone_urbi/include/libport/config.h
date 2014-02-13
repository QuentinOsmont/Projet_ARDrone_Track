/* include/libport/config.h.tmp.  Generated from include/config.h by ../../../source/sdk-remote/libport/include/libport/generate-libport-config-h.  */

#ifndef LIBPORT_CONFIG_H
# define LIBPORT_CONFIG_H

/* include/config.h.  Generated from config.h.in by configure.  */
/* include/config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to enable Urbi debugging tools. */
/* # undef LIBPORT_COMPILATION_MODE_DEBUG */

/* Define to 1 to optimize for space. */
/* # undef LIBPORT_COMPILATION_MODE_SPACE */

/* Define to 1 to optimize for speed the kernel at the detriment of
   compilation time. */
# define LIBPORT_COMPILATION_MODE_SPEED  1

/* Define to 1 if serialization is enabled. */
# define LIBPORT_ENABLE_SERIALIZATION  1

/* Define to 1 to enable SLL support. */
/* # undef LIBPORT_ENABLE_SSL */

/* Define to 1 if you have the `argz_add' function. */
# define LIBPORT_HAVE_ARGZ_ADD  1

/* Define to 1 if you have the `argz_append' function. */
# define LIBPORT_HAVE_ARGZ_APPEND  1

/* Define to 1 if you have the `argz_count' function. */
# define LIBPORT_HAVE_ARGZ_COUNT  1

/* Define to 1 if you have the `argz_create_sep' function. */
# define LIBPORT_HAVE_ARGZ_CREATE_SEP  1

/* Define to 1 if you have the <argz.h> header file. */
# define LIBPORT_HAVE_ARGZ_H  1

/* Define to 1 if you have the `argz_insert' function. */
# define LIBPORT_HAVE_ARGZ_INSERT  1

/* Define to 1 if you have the `argz_next' function. */
# define LIBPORT_HAVE_ARGZ_NEXT  1

/* Define to 1 if you have the `argz_stringify' function. */
# define LIBPORT_HAVE_ARGZ_STRINGIFY  1

/* Defined if the requested minimum BOOST version is satisfied */
# define LIBPORT_HAVE_BOOST  1

/* Define to 1 if you have <boost/archive/text_oarchive.hpp> */
/* # undef LIBPORT_HAVE_BOOST_ARCHIVE_TEXT_OARCHIVE_HPP */

/* Define to 1 if you have <boost/filesystem/path.hpp> */
# define LIBPORT_HAVE_BOOST_FILESYSTEM_PATH_HPP  1

/* Define to 1 if you have <boost/signal.hpp> */
# define LIBPORT_HAVE_BOOST_SIGNAL_HPP  1

/* Define to 1 if you have <boost/system/error_code.hpp> */
# define LIBPORT_HAVE_BOOST_SYSTEM_ERROR_CODE_HPP  1

/* Define to 1 if you have <boost/test/unit_test.hpp> */
# define LIBPORT_HAVE_BOOST_TEST_UNIT_TEST_HPP  1

/* Define to 1 if you have <boost/thread.hpp> */
# define LIBPORT_HAVE_BOOST_THREAD_HPP  1

/* Define to 1 if you have the `closedir' function. */
# define LIBPORT_HAVE_CLOSEDIR  1

/* Define to 1 if you have the declaration of `cygwin_conv_path', and to 0 if
   you don't. */
/* # undef LIBPORT_HAVE_DECL_CYGWIN_CONV_PATH */

/* Define to 1 if you have the <direct.h> header file. */
/* # undef LIBPORT_HAVE_DIRECT_H */

/* Define to 1 if you have the <dirent.h> header file. */
# define LIBPORT_HAVE_DIRENT_H  1

/* Define if you have the GNU dld library. */
/* # undef LIBPORT_HAVE_DLD */

/* Define to 1 if you have the <dld.h> header file. */
/* # undef LIBPORT_HAVE_DLD_H */

/* Define to 1 if you have the `dlerror' function. */
# define LIBPORT_HAVE_DLERROR  1

/* Define to 1 if you have the <dlfcn.h> header file. */
# define LIBPORT_HAVE_DLFCN_H  1

/* Define to 1 if you have the <dl.h> header file. */
/* # undef LIBPORT_HAVE_DL_H */

/* Define if you have the _dyld_func_lookup function. */
/* # undef LIBPORT_HAVE_DYLD */

/* Define to 1 if the system has the type `error_t'. */
# define LIBPORT_HAVE_ERROR_T  1

/* Define to 1 if you have the <execinfo.h> header file. */
# define LIBPORT_HAVE_EXECINFO_H  1

/* Define to 1 if you have the <inttypes.h> header file. */
# define LIBPORT_HAVE_INTTYPES_H  1

/* Define if you have the libdl library or equivalent. */
# define LIBPORT_HAVE_LIBDL  1

/* Define if libdlloader will be built on this platform */
# define LIBPORT_HAVE_LIBDLLOADER  1

/* Define this if a modern libltdl is already installed */
/* # undef LIBPORT_HAVE_LTDL */

/* Define to 1 if you have the <mach-o/dyld.h> header file. */
/* # undef LIBPORT_HAVE_MACH_O_DYLD_H */

/* Define to 1 if you have the <memory.h> header file. */
# define LIBPORT_HAVE_MEMORY_H  1

/* Define to 1 if you have the `mlockall' function. */
# define LIBPORT_HAVE_MLOCKALL  1

/* Define to 1 if you have the `opendir' function. */
# define LIBPORT_HAVE_OPENDIR  1

/* Define to 1 if the compiler supports `#pragma GCC diagnostic'. */
/* # undef LIBPORT_HAVE_PRAGMA_GCC_DIAGNOSTIC */

/* Define if libtool can extract symbol lists from object files. */
# define LIBPORT_HAVE_PRELOADED_SYMBOLS  1

/* Define to 1 if you have the <pthread.h> header file. */
# define LIBPORT_HAVE_PTHREAD_H  1

/* Define to 1 if you have pthread sources. */
/* # undef LIBPORT_HAVE_PTHREAD_SOURCES */

/* Define to 1 if you have the `readdir' function. */
# define LIBPORT_HAVE_READDIR  1

/* Define to 1 if you have the `round' function. */
# define LIBPORT_HAVE_ROUND  1

/* Define to 1 if you have the <sched.h> header file. */
# define LIBPORT_HAVE_SCHED_H  1

/* Define to 1 if you have the `sched_setscheduler' function. */
# define LIBPORT_HAVE_SCHED_SETSCHEDULER  1

/* Define to 1 if you have the `semget' function. */
# define LIBPORT_HAVE_SEMGET  1

/* Define to 1 if you have the `setpriority' function. */
# define LIBPORT_HAVE_SETPRIORITY  1

/* Define if you have the shl_load function. */
/* # undef LIBPORT_HAVE_SHL_LOAD */

/* Define to 1 if you have the <stdint.h> header file. */
# define LIBPORT_HAVE_STDINT_H  1

/* Define to 1 if you have the <stdlib.h> header file. */
# define LIBPORT_HAVE_STDLIB_H  1

/* Define to 1 if you have the <strings.h> header file. */
# define LIBPORT_HAVE_STRINGS_H  1

/* Define to 1 if you have the <string.h> header file. */
# define LIBPORT_HAVE_STRING_H  1

/* Define to 1 if you have the `strlcat' function. */
/* # undef LIBPORT_HAVE_STRLCAT */

/* Define to 1 if you have the `strlcpy' function. */
/* # undef LIBPORT_HAVE_STRLCPY */

/* Define to 1 if you have the <sys/dl.h> header file. */
/* # undef LIBPORT_HAVE_SYS_DL_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
# define LIBPORT_HAVE_SYS_MMAN_H  1

/* Define to 1 if you have the <sys/param.h> header file. */
# define LIBPORT_HAVE_SYS_PARAM_H  1

/* Define to 1 if you have the <sys/prctl.h> header file. */
# define LIBPORT_HAVE_SYS_PRCTL_H  1

/* Define to 1 if you have the <sys/resource.h> header file. */
# define LIBPORT_HAVE_SYS_RESOURCE_H  1

/* Define to 1 if you have the <sys/stat.h> header file. */
# define LIBPORT_HAVE_SYS_STAT_H  1

/* Define to 1 if you have the <sys/times.h> header file. */
# define LIBPORT_HAVE_SYS_TIMES_H  1

/* Define to 1 if you have the <sys/types.h> header file. */
# define LIBPORT_HAVE_SYS_TYPES_H  1

/* Define to 1 if you have the <sys/utsname.h> header file. */
# define LIBPORT_HAVE_SYS_UTSNAME_H  1

/* Define to 1 if you have the <sys/wait.h> header file. */
# define LIBPORT_HAVE_SYS_WAIT_H  1

/* Define to 1 if you have the `times' function. */
# define LIBPORT_HAVE_TIMES  1

/* Define to 1 if you have the `trunc' function. */
# define LIBPORT_HAVE_TRUNC  1

/* Define to 1 if you have the <unistd.h> header file. */
# define LIBPORT_HAVE_UNISTD_H  1

/* Define to 1 if you have the <valgrind/valgrind.h> header file. */
# define LIBPORT_HAVE_VALGRIND_VALGRIND_H  1

/* Define to 1 if you have the <windows.h> header file. */
/* # undef LIBPORT_HAVE_WINDOWS_H */

/* This value is set to 1 to indicate that the system argz facility works */
# define LIBPORT_HAVE_WORKING_ARGZ  1

/* Defined to libdir's basename */
# define LIBPORT_LIBDIRNAME  "lib"

/* Define to the library suffix. */
# define LIBPORT_LIBSFX  ""

/* Define if the OS needs help to load dependent libraries for dlopen(). */
/* # undef LIBPORT_LTDL_DLOPEN_DEPLIBS */

/* Define to have libltdl support log messages. */
# define LIBPORT_LT_DEBUG_LOADERS  1

/* Define to the system default library search path. */
# define LIBPORT_LT_DLSEARCH_PATH  "/lib:/usr/lib:/lib/i486-linux-gnu:/usr/lib/i486-linux-gnu:/usr/lib/atlas"

/* The archive extension */
# define LIBPORT_LT_LIBEXT  "a"

/* Define to the extension used for runtime loadable modules, say, ".so". */
# define LIBPORT_LT_MODULE_EXT  ".so"

/* Define to the name of the environment variable that determines the run-time
   module search path. */
# define LIBPORT_LT_MODULE_PATH_VAR  "LD_LIBRARY_PATH"

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
# define LIBPORT_LT_OBJDIR  ".libs/"

/* Define if dlsym() requires a leading underscore in symbol names. */
/* # undef LIBPORT_NEED_USCORE */

/* Name of package */
# define LIBPORT_PACKAGE  "libport"

/* Define to the address where bug reports for this package should be sent. */
# define LIBPORT_PACKAGE_BUGREPORT  "libport-bugs@lists.gostai.com"

/* Define to the copyright line of this package, including the period. */
# define LIBPORT_PACKAGE_COPYRIGHT  "Copyright (C) 2005-2010, Gostai S.A.S."

/* Define to the copyright holder of this package. */
# define LIBPORT_PACKAGE_COPYRIGHT_HOLDER  "Gostai S.A.S."

/* Define to the copyright years of this package. */
# define LIBPORT_PACKAGE_COPYRIGHT_YEARS  "2005-2010"

/* Define to the full name of this package. */
# define LIBPORT_PACKAGE_NAME  "Libport"

/* Define to the full name and version of this package. */
# define LIBPORT_PACKAGE_STRING  "Libport UNDEFINED"

/* Define to the one symbol short name of this package. */
# define LIBPORT_PACKAGE_TARNAME  "libport"

/* Define to the home page for this package. */
# define LIBPORT_PACKAGE_URL  ""

/* Define to the version of this package. */
# define LIBPORT_PACKAGE_VERSION  "UNDEFINED"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* # undef LIBPORT_PTHREAD_CREATE_JOINABLE */

/* Define to use the OS-thread implementation of coroutines. */
/* # undef LIBPORT_SCHED_CORO_OSTHREAD */

/* Define to 1 if you have the ANSI C header files. */
# define LIBPORT_STDC_HEADERS  1

/* Define if Urbi symbols should be precompiled. */
# define LIBPORT_SYMBOLS_PRECOMPILED  1

/* Define if compiling for Aibo. */
/* # undef LIBPORT_URBI_ENV_AIBO */

/* Define if compiling generic engine. */
/* # undef LIBPORT_URBI_ENV_ENGINE */

/* Define if compiling for Korebot-based robots. */
/* # undef LIBPORT_URBI_ENV_KOREBOT */

/* Define if compiling for Webots. */
/* # undef LIBPORT_URBI_ENV_WEBOTS */

/* Define as the Urbi host architecture name. */
# define LIBPORT_URBI_HOST  "i686-pc-linux-gnu"

/* Define as the install prefix. */
# define LIBPORT_URBI_ROOT  "/prefix"

/* Define if ufloats should be doubles. */
# define LIBPORT_URBI_UFLOAT_DOUBLE  1

/* Define if ufloats should be floats. */
/* # undef LIBPORT_URBI_UFLOAT_FLOAT */

/* Define if ufloats should be long mantissa, long exponent. */
/* # undef LIBPORT_URBI_UFLOAT_FLOATING */

/* Define if ufloats should be long. */
/* # undef LIBPORT_URBI_UFLOAT_LONG */

/* Define if ufloats should be long long. */
/* # undef LIBPORT_URBI_UFLOAT_LONG_LONG */

/* Define if ufloats should be tabulated. */
/* # undef LIBPORT_URBI_UFLOAT_TABULATED */

/* Version number of package */
# define LIBPORT_VERSION  "UNDEFINED"

/* Whether the host is Windows using VC++. */
/* # undef LIBPORT_WIN32 */

/* Define to enable boost::serialization support. */
/* # undef LIBPORT_WITH_BOOST_SERIALIZATION */

/* Define so that glibc/gnulib argp.h does not typedef error_t. */
/* # undef LIBPORT___error_t_defined */

/* Define to a type to use for `error_t' if it is not otherwise available. */
/* # undef _libport_error_t */

// Needed for std::min and std::max on Windows.
# ifndef NOMINMAX
#  define NOMINMAX
# endif

#endif // LIBPORT_CONFIG_H
