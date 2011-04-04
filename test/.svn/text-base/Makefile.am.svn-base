## Process this file with automake to produce Makefile.in

INCLUDES = -I$(top_srcdir)/include

if BUILD_TEST

bin_PROGRAMS =  \
	minstack_tcp_server_test \
	minstack_tcp_client_test \
	minstack_tcp_client_load_test \
	minstack_tcp_client_multiple_test \
	minstack_udp_server_test \
	minstack_udp_client_test \
	minstack_udp_client_load_test \
	minstack_udp_client_multiple_test
	
minstack_tcp_server_test_SOURCES= minstack_tcp_server_test.c
minstack_tcp_server_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_tcp_client_test_SOURCES= minstack_tcp_client_test.c
minstack_tcp_client_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_tcp_client_load_test_SOURCES= minstack_tcp_client_load_test.c
minstack_tcp_client_load_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_tcp_client_multiple_test_SOURCES= minstack_tcp_client_multiple_test.c
minstack_tcp_client_multiple_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_udp_server_test_SOURCES= minstack_udp_server_test.c
minstack_udp_server_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_udp_client_test_SOURCES= minstack_udp_client_test.c
minstack_udp_client_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_udp_client_load_test_SOURCES= minstack_udp_client_load_test.c
minstack_udp_client_load_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

minstack_udp_client_multiple_test_SOURCES= minstack_udp_client_multiple_test.c
minstack_udp_client_multiple_test_LDADD=	$(top_builddir)/src/libminstack.la -lpthread

endif
