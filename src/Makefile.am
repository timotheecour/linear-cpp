SUBDIRS = .

lib_LTLIBRARIES = liblinear.la

liblinear_la_CXXFLAGS = \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/deps/libtv/include \
	-I$(top_srcdir)/deps/libtv/deps/http-parser \
	-I$(top_srcdir)/deps/libtv/deps/libuv/include \
	-I$(top_srcdir)/deps/msgpack/include

liblinear_la_LDFLAGS = -version-info 0:0:0

liblinear_la_SOURCES = \
	addrinfo.cpp \
	auth_context.cpp \
	auth_context_impl.cpp \
	condition_variable.cpp \
	error.cpp \
	event_loop.cpp \
	event_loop_impl.cpp \
	group.cpp \
	handler_delegate.cpp \
	log.cpp \
	log_file.cpp \
	log_function.cpp \
	log_stderr.cpp \
	message.cpp \
	mutex.cpp \
	server.cpp \
	socket.cpp \
	socket_impl.cpp \
	tcp_client.cpp \
	tcp_server.cpp \
	tcp_server_impl.cpp \
	tcp_socket.cpp \
	tcp_socket_impl.cpp \
	timer.cpp \
	timer_impl.cpp \
	ws_client.cpp \
	ws_server.cpp \
	ws_server_impl.cpp \
	ws_socket.cpp \
	ws_socket_impl.cpp

if WITH_SSL
liblinear_la_SOURCES += \
	ssl_client.cpp \
	ssl_context.cpp \
	ssl_server.cpp \
	ssl_server_impl.cpp \
	ssl_socket.cpp \
	ssl_socket_impl.cpp \
	wss_client.cpp \
	wss_server.cpp \
	wss_server_impl.cpp \
	wss_socket.cpp \
	wss_socket_impl.cpp \
	x509_certificate.cpp
endif

clean-local:
	rm -f *~
