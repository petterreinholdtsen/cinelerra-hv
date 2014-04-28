/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if on AIX 3.
   System headers sometimes define this.
   We just want to avoid a redefinition error message.  */
#ifndef _ALL_SOURCE
/* #undef _ALL_SOURCE */
#endif

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to empty if the keyword does not work.  */
/* #undef const */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
#define HAVE_ALLOCA_H 1

/* Define if the `long double' type works.  */
#define HAVE_LONG_DOUBLE 1

/* Define as __inline if that's what the C compiler calls it.  */
/* #undef inline */

/* Define if on MINIX.  */
/* #undef _MINIX */

/* Define if the system does not provide POSIX.1 features except
   with this defined.  */
/* #undef _POSIX_1_SOURCE */

/* Define if you need to in order for stat and other things to work.  */
/* #undef _POSIX_SOURCE */

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if you can safely include both <sys/time.h> and <time.h>.  */
#define TIME_WITH_SYS_TIME 1

/* Define if your processor stores words with the most significant
   byte first (like Motorola and SPARC, unlike Intel and VAX).  */
/* #undef WORDS_BIGENDIAN */

/* #undef  int8_t */
#define uint8_t unsigned char
/* #undef  int18_t */
#define uint16_t unsigned short
/* #undef  int32_t */
#define uint32_t unsigned int
/* #undef  int64_t */
#define uint64_t unsigned long long
/* #undef ieee854_float80_t */
#define ieee754_float64_t double
#define ieee754_float32_t float

/* The number of bytes in a double.  */
#define SIZEOF_DOUBLE 8

/* The number of bytes in a float.  */
#define SIZEOF_FLOAT 4

/* The number of bytes in a int.  */
#define SIZEOF_INT 4

/* The number of bytes in a long.  */
#define SIZEOF_LONG 4

/* The number of bytes in a long double.  */
/* #undef SIZEOF_LONG_DOUBLE */

/* The number of bytes in a long long.  */
#define SIZEOF_LONG_LONG 8

/* The number of bytes in a short.  */
#define SIZEOF_SHORT 2

/* The number of bytes in a unsigned int.  */
#define SIZEOF_UNSIGNED_INT 4

/* The number of bytes in a unsigned long.  */
#define SIZEOF_UNSIGNED_LONG 4

/* The number of bytes in a unsigned long long.  */
#define SIZEOF_UNSIGNED_LONG_LONG 8

/* The number of bytes in a unsigned short.  */
#define SIZEOF_UNSIGNED_SHORT 2

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the socket function.  */
#define HAVE_SOCKET 1

/* Define if you have the strtol function.  */
#define HAVE_STRTOL 1

/* Define if you have the <errno.h> header file.  */
#define HAVE_ERRNO_H 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <limits.h> header file.  */
#define HAVE_LIMITS_H 1

/* Define if you have the <linux/soundcard.h> header file.  */
#define HAVE_LINUX_SOUNDCARD_H 1

/* Define if you have the <ncurses/termcap.h> header file.  */
#define HAVE_NCURSES_TERMCAP_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/soundcard.h> header file.  */
#define HAVE_SYS_SOUNDCARD_H 1

/* Define if you have the <sys/time.h> header file.  */
#define HAVE_SYS_TIME_H 1

/* Define if you have the <termcap.h> header file.  */
#define HAVE_TERMCAP_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

/* Name of package */
#define PACKAGE "lame"

/* Version number of package */
#define VERSION "3.92"

/* Define if compiler has function prototypes */
#define PROTOTYPES 1

/* Define if using the dmalloc debugging malloc package */
/* #undef WITH_DMALLOC */

/* system has 80 bit floats */
/* #undef HAVE_IEEE854_FLOAT80 */

/* requested by Frank, seems to be temporary needed for a smooth transition */
#define LAME_LIBRARY_BUILD 1

/* have working GTK */
#define HAVE_GTK 1

/* we link against libefence */
/* #undef HAVE_EFENCE */

/* build with libsndfile support */
/* #undef LIBSNDFILE */

/* build without hooks for analyzer */
/* #undef NOANALYSIS */

/* build with mpglib support */
#define HAVE_MPGLIB 1

/* build with layer 1 decoding */
#define USE_LAYER_1 1

/* build with layer 2 decoding */
#define USE_LAYER_2 1

/* build with vorbis support */
/* #undef HAVE_VORBIS */

/* work around a glibc bug */
/* #undef __NO_MATH_INLINES */

/* enable VBR bitrate histogram */
#define BRHIST 1

/* have termcap */
#define HAVE_TERMCAP 1

/* IEEE754 compatible machine */
#define TAKEHIRO_IEEE754_HACK 1

/* float instead of double */
/* #undef FLOAT8 */

/* have nasm */
/* #undef HAVE_NASM */

/* use MMX version of choose_table */
/* #undef MMX_choose_table */

/* double is faster than float on Alpha */
/* #undef FLOAT */

/* no debug build */
#define NDEBUG 1

/* debug define */
/* #undef ABORTFP */

/* debug define */
/* #undef ABORTFP */

/* debug define */
/* #undef ABORTFP */

/* alot of debug output */
/* #undef DEBUG */

/* we're on DEC Alpha */
/* #undef __DECALPHA__ */

