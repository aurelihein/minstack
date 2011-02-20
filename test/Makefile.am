## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/include

if BUILD_TEST

bin_PROGRAMS =  minstack_tcp_server_test minstack_tcp_client_test minstack_tcp_load_test minstack_tcp_multiple_clients_test

minstack_tcp_server_test_SOURCES= minstack_tcp_server_test.c
minstack_tcp_server_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_tcp_client_test_SOURCES= minstack_tcp_client_test.c
minstack_tcp_client_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_tcp_load_test_SOURCES= minstack_tcp_load_test.c
minstack_tcp_load_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_tcp_multiple_clients_test_SOURCES= minstack_tcp_multiple_clients_test.c
minstack_tcp_multiple_clients_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

endif
