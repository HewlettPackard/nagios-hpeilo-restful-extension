#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(nagios-hpeilo-restful-extension, 1.0, ying.yang2@hpe.com)
AC_CONFIG_SRCDIR([src/restful/nagios_hpilo_restful_engine.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE
AC_CONFIG_MACRO_DIR([m4])

m4_include([m4/m4-ax_compare_version.m4])

AC_PREFIX_DEFAULT(/usr/libexec/nagios)

# Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

LT_INIT

# Checks for libraries.
AC_CHECK_LIB(netsnmp, snmp_sess_init, [LIBS="$LIBS -lnetsnmp"], [AC_MSG_ERROR(Could not find net-snmp library.)])
AC_CHECK_LIB(curl, curl_easy_init, , AC_MSG_ERROR([curl library is required.]))
AC_CHECK_LIB(tdb, tdb_open, , AC_MSG_ERROR([tdb library is required.]))

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Check the nmap utility 
NMAP_MIN_VERSION=5.20
AC_SUBST(NMAP_MIN_VERSION)

AC_PATH_PROG(NMAP_PATH, nmap, no)

if test "x$NMAP_PATH" = "xno"; then
    AC_MSG_ERROR(Cannot find nmap.)
else
    AC_MSG_CHECKING(for namp version >= $NMAP_MIN_VERSION)
    NMAP_VERSION=`$NMAP_PATH --version | grep version | cut -d ' ' -f3`
    AX_COMPARE_VERSION([$NMAP_VERSION], [ge], [$NMAP_MIN_VERSION], 
			AC_MSG_RESULT(yes), 
			AC_MSG_ERROR(your namp version is '$NMAP_VERSION'. Must be >= $NMAP_MIN_VERSION))
fi

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memset strdup])
AC_CONFIG_FILES([
	Makefile 
	src/ilo_credit/Makefile 
	src/credit_save/Makefile 
	src/restful/Makefile 
	scripts/Makefile
	scripts/nagios_hpeilo_cfg_generator
	scripts/nagios_hpeilo_traps
	scripts/hpeilo_nagios_config
	scripts/hpeilo_snmp_service_set_tool
])

AC_OUTPUT
