INTERFACE=rfb/rfb_java.h
SRCS=rfb/rfb_java.c
OBJS=rfb/rfb_java.o
ISRCS=rfb/rfb_java_wrap.c
IOBJS=rfb/rfb_java_wrap.o
TARGET=rfb_java
LIBPREFIX=lib
JAVAS=rfb/rfb.java rfb/rfbJNI.java \
	rfb/RFBClientMessage.java rfb/ServerTest.java
CLASSES=$(patsubst %.java,%.class,$(JAVAS))
LIBS= @LIBVNCSERVERLIBS@

rfb_java_CFLAGS= @LIBVNCSERVERCFLAGS@

SWIGOPT=-package rfb -outdir rfb

EXTRA_DIST=autogen.sh $(INTERFACE) $(SRCS) $(ISRCS)

all: $(LIBPREFIX)$(TARGET)$(SO) $(TARGET).jar

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

rfb/%.class: rfb/%.java
	$(JAVAC) $^

PWD=$(shell pwd)
LD_LIBRARY_PATH=LD_LIBRARY_PATH="$(PWD)/../libvncserver/.libs:$(PWD)/../libvncclient/.libs"

test: all
	$(LD_LIBRARY_PATH) java -Djava.library.path=. rfb/ServerTest
