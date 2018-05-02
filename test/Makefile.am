## Process this file with automake to produce Makefile.in
     
TESTS = libcomapp_test
check_PROGRAMS = libcomapp_test
libcomapp_test_SOURCES = check_sendvia.c check_recent.c check_libcomapp_master.c check_main.c 
libcomapp_test_CFLAGS = @PACKAGE_CFLAGS@ @CHECK_CFLAGS@ -I$(top_builddir)/src/
libcomapp_test_LDADD = @PACKAGE_LIBS@ @CHECK_LIBS@ $(top_builddir)/src/libcomapp.la
