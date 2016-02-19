AM_CPPFLAGS = -I$(top_srcdir)/src/include

lib_LTLIBRARIES = libilocrst.la

libilocrst_la_SOURCES = base64.c ilo-credit-store.cpp 

libilocrst_la_LDFLAGS = -shared -fPIC -ltdb -lssl -lcrypto

AUTOMAKE_OPTIONS = foreign

install:
	LIBPATH=/usr/lib64; \
	if test -f "/etc/debian_version"; then LIBPATH=/usr/lib/x86_64-linux-gnu; fi; \
	install -m 755 .libs/libilocrst.so.0.0.0 $$LIBPATH; \
        ln -sf $$LIBPATH/libilocrst.so.0.0.0 $$LIBPATH/libilocrst.so.0

clean: clean-am
	LIBPATH=/usr/lib64; \
	if test -f "/etc/debian_version"; then LIBPATH=/usr/lib/x86_64-linux-gnu; fi; \
	unlink $$LIBPATH/libilocrst.so.0; \
	rm -f $$LIBPATH/libilocrst.so.0.0.0
