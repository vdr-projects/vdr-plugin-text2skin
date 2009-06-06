# exchange the comments on the following to lines if you would like to use
# Imlib2 for loading images. BEWARE that you can not use GraphTFT together with
# Text2Skin if you use Imlib2! (That's why I actually implemented ImageMagick)

#DEBUG=1
#BENCH=1

HAVE_IMAGEMAGICK=1
#HAVE_IMLIB2=1
# not recommended

# comment this out if you don't want to use FreeType font rendering

HAVE_FREETYPE=1


# DO NOT EDIT BELOW THIS LINE UNLESS YOU KNOW WHAT YOU'RE DOING
# -------------------------------------------------------------
#
# $Id: Makefile,v 1.4 2005/01/27 17:32:07 lordjaxom Exp $
#

# The official name of this plugin.
# This name will be used in the '-P...' option of VDR to load the plugin.
# By default the main source file also carries this name.
# IMPORTANT: the presence of this macro is important for the Make.config
# file. So it must be defined, even if it is not used here!
#
PLUGIN = text2skin

### The version number of this plugin (taken from the main source file):

VERSION = $(shell grep 'const char \*cText2SkinPlugin::VERSION *=' $(PLUGIN).c | awk '{ print $$5 }' | sed -e 's/[";]//g')

### The C++ compiler and options:

CXX      ?= g++
CXXFLAGS ?= -fPIC -Wall -Woverloaded-virtual

### The directory environment:

VDRDIR = ../../..
LIBDIR = ../../lib
TMPDIR = /tmp

### Allow user defined options to overwrite defaults:

-include $(VDRDIR)/Make.config

### The version number of VDR's plugin API (taken from VDR's "config.h"):

APIVERSION = $(shell sed -ne '/define APIVERSION/s/^.*"\(.*\)".*$$/\1/p' \
                         $(VDRDIR)/config.h)

### The name of the distribution archive:

ARCHIVE = $(PLUGIN)-$(VERSION)
PACKAGE = vdr-$(ARCHIVE)

### The object files (add further files here):

OBJS = $(PLUGIN).o loader.o display.o render.o common.o bitmap.o \
       file.o i18n.o theme.o cache.o setup.o status.o scroller.o screen.o \
       menu.o font.o quantize.o \
       \
       xml/skin.o xml/parser.o xml/string.o xml/object.o xml/function.o \
       xml/type.o xml/display.o xml/xml.o

### Includes and Defines (add further entries here):

ifdef DEVELOPMENT_FEATURES
	DEFINES += -DDEVELOPMENT_FEATURES
endif

ifdef HAVE_IMLIB2
	DEFINES += -DHAVE_IMLIB2
	LIBS += -lImlib2
endif

ifdef HAVE_IMAGEMAGICK
	DEFINES += -DHAVE_IMAGEMAGICK
	INCLUDES += `pkg-config --cflags ImageMagick++`
	LIBS += `pkg-config --libs ImageMagick++`
#	LIBS += $(shell Magick++-config --ldflags --libs)
endif

ifdef HAVE_FREETYPE
	ifneq ($(shell which freetype-config),)
		INCLUDES += $(shell freetype-config --cflags)
		LIBS += $(shell freetype-config --libs)
	else
		INCLUDES += -I/usr/include/freetype -I/usr/local/include/freetype
		LIBS += -lfreetype
	endif
	DEFINES += -DHAVE_FREETYPE
	OBJS += graphtft/font.o
endif

ifdef DEBUG
	CXXFLAGS += -g -fno-inline
	DEFINES += -DDEBUG
else
	CXXFLAGS += -O3
ifdef BENCH
	DEFINES += -DBENCH
endif
endif

INCLUDES += -I$(VDRDIR)/include -I.

DEFINES += -D_GNU_SOURCE -DPLUGIN_NAME_I18N='"$(PLUGIN)"'

### The main target:

all: libvdr-$(PLUGIN).so i18n

### Implicit rules:

%.o: %.c
	$(CXX) $(CXXFLAGS) -c $(DEFINES) $(INCLUDES) -o $@ $<

### Dependencies:

MAKEDEP = $(CXX) -MM -MG
DEPFILE = .dependencies
$(DEPFILE): Makefile
	@$(MAKEDEP) $(DEFINES) $(INCLUDES) $(OBJS:%.o=%.c) > $@

-include $(DEPFILE)

### Internationalization (I18N):

PODIR     = po
LOCALEDIR = $(VDRDIR)/locale
I18Npo    = $(notdir $(wildcard $(PODIR)/*.po))
I18Npot   = $(PODIR)/$(PLUGIN).pot

$(I18Npot): $(wildcard *.c)
	xgettext -C -cTRANSLATORS --no-wrap --no-location -k -ktr -ktrNOOP \
	         --msgid-bugs-address='<sascha@akv-soft.de>' -o $@ $^

%.po: $(I18Npot)
	msgmerge -U --no-wrap --no-location --backup=none -q $@ $<
	@touch $@

$(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo: $(PODIR)/%.po
	@mkdir -p $(dir $@)
	msgfmt -c -o $@ $<

.PHONY: i18n
i18n: $(I18Npo:%.po=$(LOCALEDIR)/%/LC_MESSAGES/vdr-$(PLUGIN).mo)

### Targets:

libvdr-$(PLUGIN).so: $(OBJS)
	$(CXX) $(CXXFLAGS) -shared $(OBJS) $(LIBS) -o $@
	@cp --remove-destination $@ $(LIBDIR)/$@.$(APIVERSION)
ifndef DEBUG
	strip $(LIBDIR)/$@.$(APIVERSION)
endif

dist: clean
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@mkdir $(TMPDIR)/$(ARCHIVE)
	@cp -a * $(TMPDIR)/$(ARCHIVE)
	@tar czf $(PACKAGE).tgz -C $(TMPDIR) \
	     --exclude debian --exclude CVS --exclude .svn $(ARCHIVE)
	@-rm -rf $(TMPDIR)/$(ARCHIVE)
	@echo Distribution package created as $(PACKAGE).tgz

clean:
	@-rm -f $(OBJS) $(DEPFILE) *.so $(I18Npot) *.tgz core* *~ SKINS SKINS.de
