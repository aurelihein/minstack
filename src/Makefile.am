## Process this file with automake to produce Makefile.in

lib_LTLIBRARIES = libminstack.la

libminstack_la_SOURCES= \
	private.c \
	private.h \
	tcp.c \
	minstack_debug.c \
	udp.c

libminstack_la_CPPFLAGS= -D_GNU_SOURCE
libminstack_la_CFLAGS= -Wall -I$(top_srcdir)/include $(CONFIG_CFLAGS)
#libminstack_la_LDFLAGS= -version-info $(VERSION_INFO)

INCLUDE = -I$(top_srcdir)/include
AM_CFLAGS=$(STRICT_OPTIONS) $(CONFIG_CFLAGS)