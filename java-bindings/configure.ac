dnl Process this file with autoconf to produce a configure script.
dnl The macros which aren't shipped with the autotools are stored in the 
dnl Tools/config directory in .m4 files.

AC_INIT([LibVNCServer-java],[0.1],[http://libvncserver.sourceforge.net])
AC_PREREQ(2.54)
AC_CANONICAL_HOST
AM_INIT_AUTOMAKE

dnl Checks for programs.
AC_CHECK_PROG(SWIG,swig,swig)
AC_CHECK_PROG(LIBVNCSERVERCONFIG,libvncserver-config,yes,no)
if test "$LIBVNCSERVERCONFIG" != "yes"; then
	AC_MSG_ERROR([Need to have libvncserver-config in PATH])
	exit 1
fi
AC_PROG_CC
AC_PROG_RANLIB
AC_EXEEXT
AC_OBJEXT

LIBVNCSERVERCFLAGS=`libvncserver-config --cflags`
LIBVNCSERVERLIBS=`libvncserver-config --libs`
AC_SUBST(LIBVNCSERVERCFLAGS)
AC_SUBST(LIBVNCSERVERLIBS)

dnl Checks for header files.
AC_HEADER_STDC

dnl How to specify include directories that may be system directories.
# -I should not be used on system directories (GCC)
if test "$GCC" = yes; then
    ISYSTEM="-isystem "
else
    ISYSTEM="-I"
fi


# Set info about shared libraries.
AC_SUBST(SO)
AC_SUBST(LDSHARED)
AC_SUBST(CCSHARED)
AC_SUBST(LINKFORSHARED)

# SO is the extension of shared libraries `(including the dot!)
AC_MSG_CHECKING(SO)
if test -z "$SO"
then
	case $host in
	*-*-hp*) SO=.sl;;
	*-*-darwin*) SO=.bundle;;
	*-*-cygwin* | *-*-mingw*) SO=.dll;;
	*) SO=.so;;
	esac
fi
AC_MSG_RESULT($SO)

# LDSHARED is the ld *command* used to create shared library
# -- "ld" on SunOS 4.x.x, "ld -G" on SunOS 5.x, "ld -shared" on IRIX 5
# (Shared libraries in this instance are shared modules to be loaded into
# Python, as opposed to building Python itself as a shared library.)
AC_MSG_CHECKING(LDSHARED)
if test -z "$LDSHARED"
then
	case $host in
	*-*-aix*) LDSHARED="\$(srcdir)/ld_so_aix \$(CC)";;
	*-*-cygwin* | *-*-mingw*)
            if test "$GCC" = yes; then
                LDSHARED="dllwrap --driver-name gcc --dlltool dlltool --export-all-symbols --as as --dllname \$(LIBPREFIX)\$(TARGET)\$(SO)"
            else
                if test "cl" = $CC ;  then
                    # Microsoft Visual C++ (MSVC)
                    LDSHARED="$CC -nologo -LD"
                else
                    # Unknown compiler try gcc approach
                    LDSHARED="$CC -shared"
                fi
            fi ;;
	*-*-irix5*) LDSHARED="ld -shared";;
	*-*-irix6*) LDSHARED="ld ${SGI_ABI} -shared -all";;
	*-*-sunos4*) LDSHARED="ld";;
	*-*-solaris*) LDSHARED="ld -G";;
	*-*-hp*) LDSHARED="ld -b";;
	*-*-osf*) LDSHARED="ld -shared -expect_unresolved \"*\"";;
	*-sequent-sysv4) LDSHARED="ld -G";;
	*-*-next*)
		if test "$ns_dyld"
		then LDSHARED='$(CC) $(LDFLAGS) -bundle -prebind'
		else LDSHARED='$(CC) $(CFLAGS) -nostdlib -r';
		fi
                if test "$with_next_framework" ; then
		    LDSHARED="$LDSHARED \$(LDLIBRARY)"
		fi ;;
	*-*-linux*) LDSHARED="gcc -shared";;
	*-*-dgux*) LDSHARED="ld -G";;
	*-*-freebsd3*) LDSHARED="gcc -shared";;
	*-*-freebsd* | *-*-openbsd*) LDSHARED="ld -Bshareable";;
	*-*-netbsd*)
		if [[ "`$CC -dM -E - </dev/null | grep __ELF__`" != "" ]]
		then
			LDSHARED="cc -shared"
		else
			LDSHARED="ld -Bshareable"
		fi;;
	*-sco-sysv*) LDSHARED="cc -G -KPIC -Ki486 -belf -Wl,-Bexport";;
	*-*-darwin*) LDSHARED="cc -bundle -undefined suppress -flat_namespace";;
	*)	LDSHARED="ld";;
	esac
fi
AC_MSG_RESULT($LDSHARED)
# CCSHARED are the C *flags* used to create objects to go into a shared
# library (module) -- this is only needed for a few systems
AC_MSG_CHECKING(CCSHARED)
if test -z "$CCSHARED"
then
	case $host in
	*-*-hp*) if test "$GCC" = yes;
		 then CCSHARED="-fpic";
		 else CCSHARED="+z";
		 fi;;
	*-*-linux*) CCSHARED="-fpic";;
	*-*-freebsd* | *-*-openbsd*) CCSHARED="-fpic";;
	*-*-netbsd*) CCSHARED="-fPIC";;
	*-sco-sysv*) CCSHARED="-KPIC -dy -Bdynamic";;
	*-*-irix6*)  case $CC in
		   *gcc*) CCSHARED="-shared";;
		   *) CCSHARED="";;
		   esac;;
	esac
fi
AC_MSG_RESULT($CCSHARED)

# RPATH is the path used to look for shared library files.
AC_MSG_CHECKING(RPATH)
if test -z "$RPATH"
then
	case $host in
	*-*-solaris*) RPATH='-R. -R$(exec_prefix)/lib';;
        *-*-irix*) RPATH='-rpath .:$(exec_prefix)/lib';;
	*-*-linux*) RPATH='-Xlinker -rpath $(exec_prefix)/lib -Xlinker -rpath .';;
	*)	RPATH='';;
	esac
fi
AC_MSG_RESULT($RPATH)
AC_SUBST(RPATH)

# LINKFORSHARED are the flags passed to the $(CC) command that links
# the a few executables -- this is only needed for a few systems

AC_MSG_CHECKING(LINKFORSHARED)
if test -z "$LINKFORSHARED"
then
	case $host in
	*-*-aix*)	LINKFORSHARED='-Wl,-bE:$(srcdir)/python.exp -lld';;
	*-*-hp*)
	    LINKFORSHARED="-Wl,-E -Wl,+s -Wl,+b\$(BINLIBDEST)/lib-dynload";;
	*-*-linux*) LINKFORSHARED="-Xlinker -export-dynamic";;
	*-*-next*) LINKFORSHARED="-u libsys_s";;
	*-sco-sysv*) LINKFORSHARED="-Bdynamic -dy -Wl,-Bexport";;
	*-*-irix6*) LINKFORSHARED="-all";;
	esac
fi
AC_MSG_RESULT($LINKFORSHARED)

# This variation is needed on OS-X because there is no (apparent) consistency in shared libary naming.
# Sometimes .bundle works, but sometimes .so is needed.  It depends on the target language

# Optional CFLAGS used to silence compiler warnings on some platforms.

AC_SUBST(PLATFLAGS)
case $host in
   *-*-darwin*) PLATFLAGS="-Wno-long-double";;
   *) PLATFLAGS="";;
esac

#----------------------------------------------------------------
# Look for javac
#----------------------------------------------------------------

JAVACBIN=

AC_ARG_WITH(perl5,[  --with-javac=path       Set location of javac executable],[ JAVACBIN="$withval"], [JAVACBIN=])

# First figure out what the name of Perl5 is

if test -z "$JAVACBIN"; then
AC_PATH_PROG(JAVAC, javac)
else
JAVAC="$JAVACBIN"
fi


AC_MSG_CHECKING(for jni.h header file)
if test -n "$JAVAC"; then
	JNI_H_PATH="`echo "$JAVAC" | sed "s/bin\/javac$/include/"`"
	if test -f "$JNI_H_PATH/jni.h"; then
		JNI_INCLUDES="`find "$JNI_H_PATH" -type d -exec \
			echo -n \ \"-I{}\" \;`"
		AC_MSG_RESULT(using $JNI_INCLUDES)
	else
		AC_MSG_RESULT(could not find jni.h)
	fi
else
	AC_MSG_RESULT(not found)
fi
		
AC_SUBST(JAVAC)
AC_SUBST(JNI_INCLUDES)

#----------------------------------------------------------------
# Miscellaneous
#----------------------------------------------------------------

AC_CONFIG_FILES([Makefile])
AC_OUTPUT

dnl configure.in ends here
