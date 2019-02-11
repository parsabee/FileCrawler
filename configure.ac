# Auto configure initialization
AC_INIT(file_crawler, [1.0], [parsab@uoregon.edu])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_HEADERS([config.h])

# Identify host platform
AC_CANONICAL_HOST

##########################################################################################
# Tests

# Check for pthreads
AX_PTHREAD([have_pthreads="yes"],[have_pthreads="no"])
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"
CC="$PTHREAD_CC"

# Check for the math library
AC_SEARCH_LIBS([floor],[m],[have_m="yes"],[have_m="no"])

case $host_os in
    darwin* )
        have_darwin="yes"
        AC_DEFINE(HAVE_SOCKADDR_LEN, 1, "socket definition for Darwin based OSes")
        ;;
    * )
        have_darwin="no"
        ;;
esac

##########################################################################################
# End of configure summary output
DE_STAT("pthreads found", $have_pthreads)
DE_STAT("libmath found", $have_m)
DE_STAT("Apple sockets", $have_darwin)

##########################################################################################
# Generate files
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
