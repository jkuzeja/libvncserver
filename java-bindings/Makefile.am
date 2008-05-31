INTERFACE=nacro.h
SRCS=nacro.c
OBJS=nacro.o
ISRCS=nacro_wrap.c
IOBJS=nacro_wrap.o
TARGET=nacro
LIBPREFIX=lib
JAVAS=nacroConstants.java nacro.java nacroJNI.java
CLASSES=$(patsubst %.java,%.class,$(JAVAS))
LIBS= @LIBVNCSERVERLIBS@

nacro_CFLAGS= @LIBVNCSERVERCFLAGS@

SWIGOPT=

EXTRA_DIST=autogen.sh $(INTERFACE) $(SRCS) $(ISRCS) nacro.pm recorder.pl

all: $(LIBPREFIX)$(TARGET)$(SO) $(TARGET).jar test.class

# the following is borrowed from SWIG

SWIG= @SWIG@
JNI_INCLUDES= @JNI_INCLUDES@
JAVAC= @JAVAC@

# ----------------------------------------------------------------
# Build a Java dynamically loadable module (C)
# ----------------------------------------------------------------

$(ISRCS): $(INTERFACE)
	@test -n "$(SWIG)" || (echo "Need SWIG" && exit 1)
	$(SWIG) -java $(SWIGOPT) $(INTERFACE)

$(OBJS): $(SRCS) $(INTERFACE)
	$(CC) -c $(CCSHARED) $(CFLAGS) -o $@ $< $(LIBVNCSERVERCFLAGS) $(INCLUDES) $(JNI_INCLUDES)

$(IOBJS): $(ISRCS) $(INTERFACE)
	$(CC) -c $(CCSHARED) $(CFLAGS) -o $@ $< $(INCLUDES) $(JNI_INCLUDES)

$(LIBPREFIX)$(TARGET)$(SO): $(OBJS) $(IOBJS)
	$(LDSHARED) $(OBJS) $(IOBJS) $(LIBS) -o $(LIBPREFIX)$(TARGET)$(SO)

$(TARGET).jar: $(CLASSES)
	jar cvf $@ $^

%.class: %.java
	$(JAVAC) $^

PWD=$(shell pwd)
LD_LIBRARY_PATH=LD_LIBRARY_PATH="$(PWD)/../libvncserver/.libs:$(PWD)/../libvncclient/.libs"

test: all
	$(LD_LIBRARY_PATH) java -Djava.library.path=. test
