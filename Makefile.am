## Process this file with automake to produce Makefile.in

EXTRA_DIST = README autogen.sh minstack.pc.in

SUBDIRS = src include test

indent:
	indent */*.c
	indent */*.h
	rm -rf */*.c~
	rm -rf */*.h~
	
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = minstack.pc