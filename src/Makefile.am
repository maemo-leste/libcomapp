comappincludedir=$(includedir)/$(PACKAGE_INC)
comappinclude_HEADERS = \
	comapp_recent.h \
	comapp_sendvia.h \
	comapp_details.h \
	comapp_opensave.h \
	comapp_common.h \
	comapp_system.h

comappdir = $(libdir)
comapp_LTLIBRARIES = libcomapp.la

libcomapp_la_SOURCES = \
	comapp_recent.c \
	comapp_recent.h \
	comapp_recent_private.h \
	comapp_sendvia.c \
	comapp_sendvia.h \
	comapp_details.c \
	comapp_details.h \
	comapp_common.c \
	comapp_common.h \
	comapp_opensave.c \
	comapp_opensave.h \
	comapp_system.c \
	comapp_system.h

libcomapp_la_CPPFLAGS = $(PACKAGE_CFLAGS)
libcomapp_la_LIBADD = $(PACKAGE_LIBS)

