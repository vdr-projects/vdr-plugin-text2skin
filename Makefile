# exchange the comments on the following to lines if you would like to use
# Imlib2 for loading images. BEWARE that you can not use GraphTFT together with
# Text2Skin if you use Imlib2! (That's why I actually implemented ImageMagick)

HAVE_IMAGEMAGICK=1
#HAVE_IMLIB2=1


# DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU'RE DOING
# -------------------------------------------------------------
#
# $Id: Makefile,v 1.11 2004/06/16 18:46:50 lordjaxom Exp $
#

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
#
PLUGIN = text2skin

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'const char \*cText2SkinPlugin::VERSION *=' $(PLUGIN).c | awk '{ print $$5 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -O2 -Wall -Woverloaded-virtual

### The directory environment:

DVBDIR = ../../../../DVB
VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR (taken from VDR's "config.h"):

VDRVERSION = $(shell grep 'define VDRVERSION ' $(VDRDIR)/config.h | awk '{ print $$3 }' | sed -e 's/"//g')

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### Includes and Defines (add further entries here):

ifdef HAVE_IMLIB2
	DEFINES += -DHAVE_IMLIB2
	LIBS += -lImlib2
endif

ifdef HAVE_IMAGEMAGICK
	DEFINES += -DHAVE_IMAGEMAGICK
	LIBS += -lMagick++
endif

ifdef DEBUG
	DEFINES += -DDEBUG
endif

INCLUDES += -I$(VDRDIR)/include -I$(DVBDIR)/include

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The object files (add further files here):

OBJS = $(PLUGIN).o loader.o data.o display.o render.o common.o bitmap.o \
       file.o i18n.o theme.o cache.o setup.o status.o

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) $<

# Dependencies:

MAKEDEP = g++ -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Targets:

all: libvdr-$(PLUGIN).so

libvdr-$(PLUGIN).so: $(OBJS) SKINS SKINS.de
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp $@ $(LIBDIR)/$@.$(VDRVERSION)

SKINS: contrib/items.doc
	@contrib/list_items.pl >$@

SKINS.de: contrib/items.doc
	@contrib/list_items.pl de >$@

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so *.tgz core* *~ SKINS SKINS.de
