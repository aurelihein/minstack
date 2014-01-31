## Process this file with automake to produce Makefile.in

AM_CPPFLAGS = -I$(top_srcdir)/include

if BUILD_TEST

bin_PROGRAMS =  \
	minstack_tcp_server_level \
	minstack_tcp_server_test \
	minstack_tcp_server_load_test \
	minstack_tcp_client_test \
	minstack_tcp_client_send \
	minstack_tcp_client_load_test \
	minstack_tcp_client_multiple_test \
	minstack_udp_server_test \
	minstack_udp_server_level \
	minstack_udp_server_load_test \
	minstack_udp_client_test \
	minstack_udp_client_load_test \
	minstack_udp_client_multiple_test \
	mini_rtsp_server \
	mini_rtsp_client \
	minstack_udp_proxy \
	test_tools
	
minstack_tcp_server_level_SOURCES= minstack_tcp_server_level.c
	
minstack_tcp_server_test_SOURCES= minstack_tcp_server_test.c
	
minstack_tcp_server_load_test_SOURCES= minstack_tcp_server_load_test.c

mini_rtsp_server_SOURCES= mini_rtsp_server.c

mini_rtsp_client_SOURCES= mini_rtsp_client.c

minstack_tcp_client_test_SOURCES= minstack_tcp_client_test.c

minstack_tcp_client_send_SOURCES= minstack_tcp_client_send.c

minstack_tcp_client_load_test_SOURCES= minstack_tcp_client_load_test.c

minstack_tcp_client_multiple_test_SOURCES= minstack_tcp_client_multiple_test.c

minstack_udp_server_test_SOURCES= minstack_udp_server_test.c
	
minstack_udp_server_load_test_SOURCES= minstack_udp_server_load_test.c

minstack_udp_server_level_SOURCES= minstack_udp_server_level.c

minstack_udp_client_test_SOURCES= minstack_udp_client_test.c

minstack_udp_client_load_test_SOURCES= minstack_udp_client_load_test.c

minstack_udp_client_multiple_test_SOURCES= minstack_udp_client_multiple_test.c

minstack_udp_proxy_SOURCES= minstack_udp_proxy.c

AM_LDFLAGS=	$(top_builddir)/src/libminstack.la -lpthread -pthread
endif
