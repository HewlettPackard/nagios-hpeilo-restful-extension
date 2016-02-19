AUTOMAKE_OPTIONS=foreign

AM_LDFLAGS = -L$(top_srcdir)/src/ilo_credit/.libs/ -lcurl -lm -lilocrst -ldl -lcrypto

AM_CPPFLAGS = -I$(top_srcdir)/src/include

AM_CFLAGS =

libexec_PROGRAMS = nagios_hpeilo_restful_engine

nagios_hpeilo_restful_engine_SOURCES= nagios_hpilo_restful_engine.c base64.c cJSON.c

