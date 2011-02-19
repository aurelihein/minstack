## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/include

bin_PROGRAMS =  minstack_server_test minstack_client_test minstack_load_test minstack_multiple_clients_test

minstack_server_test_SOURCES= minstack_server_test.c
minstack_server_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_client_test_SOURCES= minstack_client_test.c
minstack_client_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_load_test_SOURCES= minstack_load_test.c
minstack_load_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_multiple_clients_test_SOURCES= minstack_multiple_clients_test.c
minstack_multiple_clients_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread
