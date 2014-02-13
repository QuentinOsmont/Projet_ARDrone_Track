#ifndef _JCONFIG_H
#define _JCONFIG_H 1
 
/* jconfig.h. Generated automatically at end of configure. */
/* config.h.  Generated from config.hin by configure.  */
/* config.hin.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef LIBJPEG_HAVE_DLFCN_H 
#define LIBJPEG_HAVE_DLFCN_H  1 
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef LIBJPEG_HAVE_INTTYPES_H 
#define LIBJPEG_HAVE_INTTYPES_H  1 
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef LIBJPEG_HAVE_MEMORY_H 
#define LIBJPEG_HAVE_MEMORY_H  1 
#endif

/* Define to 1 if you have the <stddef.h> header file. */
#ifndef LIBJPEG_HAVE_STDDEF_H 
#define LIBJPEG_HAVE_STDDEF_H  1 
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef LIBJPEG_HAVE_STDINT_H 
#define LIBJPEG_HAVE_STDINT_H  1 
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef LIBJPEG_HAVE_STDLIB_H 
#define LIBJPEG_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef LIBJPEG_HAVE_STRINGS_H 
#define LIBJPEG_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef LIBJPEG_HAVE_STRING_H 
#define LIBJPEG_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef LIBJPEG_HAVE_SYS_STAT_H 
#define LIBJPEG_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef LIBJPEG_HAVE_SYS_TYPES_H 
#define LIBJPEG_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef LIBJPEG_HAVE_UNISTD_H 
#define LIBJPEG_HAVE_UNISTD_H  1 
#endif

/* Define to 1 if the system has the type `unsigned char'. */
#ifndef LIBJPEG_HAVE_UNSIGNED_CHAR 
#define LIBJPEG_HAVE_UNSIGNED_CHAR  1 
#endif

/* Define to 1 if the system has the type `unsigned short'. */
#ifndef LIBJPEG_HAVE_UNSIGNED_SHORT 
#define LIBJPEG_HAVE_UNSIGNED_SHORT  1 
#endif

/* Define to 1 if you have the <windows.h> header file. */
/* #undef LIBJPEG_HAVE_WINDOWS_H */

/* Although a real ANSI C compiler can deal perfectly well with pointers to
   unspecified structures (see "incomplete types" in the spec), a few pre-ANSI
   and pseudo-ANSI compilers get confused. To keep one of these bozos happy,
   define INCOMPLETE_TYPES_BROKEN. This is not recommended unless you actually
   get "missing structure definition" warnings or errors while compiling the
   JPEG code. */
/* #undef LIBJPEG_INCOMPLETE_TYPES_BROKEN */

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef LIBJPEG_LT_OBJDIR 
#define LIBJPEG_LT_OBJDIR  ".libs/" 
#endif

/* For 80x86 machines, you need to define NEED_FAR_POINTERS, unless you are
   using a large-data memory model or 80386 flat-memory mode. On less
   brain-damaged CPUs this symbol must not be defined. (Defining this symbol
   causes large data structures to be referenced through "far" pointers and to
   be allocated with a special version of malloc.) */
/* #undef LIBJPEG_NEED_FAR_POINTERS */

/* Define this if your linker needs global names to be unique in less than the
   first 15 characters. */
/* #undef LIBJPEG_NEED_SHORT_EXTERNAL_NAMES */

/* Define this if your system does not provide typedef size_t in any of the
   ANSI-standard places (stddef.h, stdlib.h, or stdio.h), but places it in
   <sys/types.h> instead. */
/* #undef LIBJPEG_NEED_SYS_TYPES_H */

/* Name of package */
#ifndef LIBJPEG_PACKAGE 
#define LIBJPEG_PACKAGE  "libjpeg" 
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef LIBJPEG_PACKAGE_BUGREPORT 
#define LIBJPEG_PACKAGE_BUGREPORT  "" 
#endif

/* Define to the full name of this package. */
#ifndef LIBJPEG_PACKAGE_NAME 
#define LIBJPEG_PACKAGE_NAME  "libjpeg" 
#endif

/* Define to the full name and version of this package. */
#ifndef LIBJPEG_PACKAGE_STRING 
#define LIBJPEG_PACKAGE_STRING  "libjpeg 6b" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef LIBJPEG_PACKAGE_TARNAME 
#define LIBJPEG_PACKAGE_TARNAME  "libjpeg" 
#endif

/* Define to the home page for this package. */
#ifndef LIBJPEG_PACKAGE_URL 
#define LIBJPEG_PACKAGE_URL  "" 
#endif

/* Define to the version of this package. */
#ifndef LIBJPEG_PACKAGE_VERSION 
#define LIBJPEG_PACKAGE_VERSION  "6b" 
#endif

/* Define to 1 if the C compiler supports function prototypes. */
#ifndef LIBJPEG_PROTOTYPES 
#define LIBJPEG_PROTOTYPES  1 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef LIBJPEG_STDC_HEADERS 
#define LIBJPEG_STDC_HEADERS  1 
#endif

/* Version number of package */
#ifndef LIBJPEG_VERSION 
#define LIBJPEG_VERSION  "6b" 
#endif

/* Define like PROTOTYPES; this can be used by system headers. */
#ifndef LIBJPEG___PROTOTYPES 
#define LIBJPEG___PROTOTYPES  1 
#endif


/*
 * The following options affect code selection within the JPEG library,
 * but they don't need to be visible to applications using the library.
 * To minimize application namespace pollution, the symbols won't be
 * defined unless JPEG_INTERNALS has been defined.
 */

#ifdef JPEG_INTERNALS

/* Define this if your compiler implements ">>" on signed values as a logical
 * (unsigned) shift; leave it undefined if ">>" is a signed (arithmetic) shift,
 * which is the normal and rational definition.
 */
/* # undef RIGHT_SHIFT_IS_UNSIGNED */


#endif /* JPEG_INTERNALS */


/*
 * The remaining options do not affect the JPEG library proper,
 * but only the sample applications cjpeg/djpeg (see cjpeg.c, djpeg.c).
 * Other applications can ignore these.
 */

#ifdef JPEG_CJPEG_DJPEG

/* These defines indicate which image (non-JPEG) file formats are allowed. */

/* Define if BMP image file format is supported. */
/* # undef BMP_SUPPORTED */

/* Define if GIF image file format is supported. */
/* # undef GIF_SUPPORTED */

/* Define if PBMPLUS PPM/PGM image file format is supported. */
/* # undef PPM_SUPPORTED */

/* Define if Utah RLE image file format is supported. */
/* # undef RLE_SUPPORTED */

/* Define if Targa image file format is supported. */
/* # undef TARGA_SUPPORTED */



/* Define this if you want to name both input and output files on the command
 * line, rather than using stdout and optionally stdin.  You MUST do this if
 * your system can't cope with binary I/O to stdin/stdout.  See comments at
 * head of cjpeg.c or djpeg.c.
 */
/* # undef TWO_FILE_COMMANDLINE */

/* Define this if your system needs explicit cleanup of temporary files.
 * This is crucial under MS-DOS, where the temporary "files" may be areas
 * of extended memory; on most other systems it's not as important.
 */
/* # undef NEED_SIGNAL_CATCHER */

/* By default, we open image files with fopen(...,"rb") or fopen(...,"wb").
 * This is necessary on systems that distinguish text files from binary files,
 * and is harmless on most systems that don't.  If you have one of the rare
 * systems that complains about the "b" spec, define this symbol.
 */
/* # undef DONT_USE_B_MODE */

/* Define this if you want percent-done progress reports from cjpeg/djpeg.
 */
/* # undef PROGRESS_REPORT */

#endif /* JPEG_CJPEG_DJPEG */

/*
 * Local Variables:
 * mode: C
 * End:
 */

 
/* once: _JCONFIG_H */
#endif
