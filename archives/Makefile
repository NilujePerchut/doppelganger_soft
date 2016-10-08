#Build toolchain
TOOLSDIR = $(shell dirname `pwd`)/toolchain
export PATH:=$(TOOLSDIR)/bin:$(PATH)
all: gputils sdcc

sdcc: gputils
	if [ ! -x "$(TOOLSDIR)/bin/sdcc" ]; then \
	 tar xvjf sdcc-src-3.6.0.tar.bz2;\
	 mkdir -p $(TOOLSDIR);\
	 (cd sdcc-3.6.0 && \
	 ./configure --disable-z80-port --disable-mcs51-port \
	             --disable-gbz80-port --disable-avr-port \
	             --disable-ds390-port --disable-ds400-port \
	             --disable-pic-port --disable-xa51-port \
	             --disable-hc08-port --disable-ucsim \
	             --prefix=$(TOOLSDIR) && \
	 make && \
	 make install);\
	fi

gputils:
	if [ ! -x "$(TOOLSDIR)/bin/gpasm" ]; then \
	 tar xvjf gputils-1.5.0-1.tar.bz2;\
	 patch -p1 < fix-negated-config-word-bug-for-18f45k50.patch; \
	 mkdir -p $(TOOLSDIR);\
	 (cd gputils-1.5.0 && \
	 ./configure --prefix=$(TOOLSDIR) && \
	 make && \
	 make install);\
	fi

clean:
	rm -fr $(TOOLSDIR)
	rm -fr gputils-1.5.0
	rm -fr sdcc-3.6.0
