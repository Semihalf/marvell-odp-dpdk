include $(top_srcdir)/platform/@with_platform@/Makefile.inc
LIB   = $(top_builddir)/lib

#in the following line, the libs using the symbols should come before
#the libs containing them! The includer is given a chance to add things
#before libodp by setting PRE_LDADD before the inclusion.
LDADD = $(PRE_LDADD) $(LIB)/libodphelper.la $(LIB)/libodp-dpdk.la

INCFLAGS = \
	-I$(top_builddir)/platform/@with_platform@/include \
	-I$(top_srcdir)/helper/include \
	-I$(top_srcdir)/include \
	-I$(top_srcdir)/include/odp/arch/@ARCH_ABI@ \
	-I$(top_srcdir)/platform/@with_platform@/include \
	-I$(top_srcdir)/test \
	-I$(top_builddir)/include

AM_CFLAGS += $(INCFLAGS)
AM_CXXFLAGS = $(INCFLAGS)

AM_LDFLAGS += -L$(LIB)

@VALGRIND_CHECK_RULES@
valgrind_tools = memcheck

TESTS_ENVIRONMENT= ODP_PLATFORM=${with_platform} EXEEXT=${EXEEXT}
