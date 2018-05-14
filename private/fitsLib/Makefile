# Makefile utility for libfits library. Configured for Linux OS.
#
# modification history
# --------------------
# 01a 23jan2004, rcr - Created.
# 02a 16jun2004, rcr - Moved to Labview 7.1 under RH8.0
# 03a 28sep2010, rcr - Moved to Labview 8.6 under FC12
# 04a 12jul2012, rcr - Moved to LabVIEW 21010 under CentOS 5.8
# 04b 14mar2017, rcr - Mod. Configure LabView 2016 under CentOS 6.8
#                      Mod. Use distribution cfitsio lib.

BASENAME = fits
MACHINE = centos
V = 4
MV = 0
STATICLIB = lib$(BASENAME)_$(MACHINE).a
SHAREDLIB = lib$(BASENAME)_$(MACHINE).so.$(V).$(MV)
LVDIR = /usr/local/natinst/LabVIEW-2016-64

#
# Compiler and compiler flags
#

CC = gcc
CFLAGS = -Wall -fPIC -I$(LVDIR)/cintools `pkg-config --cflags cfitsio` -DLINUX
LD = ld
LDFLAGS = `pkg-config --libs cfitsio`

#
# Source files and object files
#

CFILES = fitsLib.c
OFILES = $(CFILES:%c=%o)

#
# Target rules
#

default	: shared

all	: clean shared

static	: $(OFILES)
	ar rv $(STATICLIB) $(OFILES)
	-ranlib $(STATICLIB)

shared	: $(OFILES)
	$(LD) -G -o $(SHAREDLIB) -h $(SHAREDLIB) $(OFILES) $(LDFLAGS)

clean	:
	-rm -f *.a *.o *.bak *~ core $(SHAREDLIB)

install	:
	-cp $(SHAREDLIB) /usr/local/lib
	-rm /usr/local/lib/lib$(BASENAME).so
	-ln -s /usr/local/lib/$(SHAREDLIB) /usr/local/lib/lib$(BASENAME).so

# auto dependency make

depend	:
	$(CC) -MM $(CFLAGS) $(CFILES) > makedep
	echo '/^# DO NOT DELETE THIS LINE/+1,$$d' > eddep
	echo '$$r makedep' >> eddep
	echo 'w' >> eddep
	cp Makefile Makefile.bak
	ed - Makefile < eddep
	rm eddep makedep

# DO NOT DELETE THIS LINE -- make depend uses it
fitsLib.o: fitsLib.c /usr/include/cfitsio/fitsio.h \
 /usr/include/cfitsio/longnam.h \
 /usr/local/natinst/LabVIEW-2016-64/cintools/extcode.h \
 /usr/local/natinst/LabVIEW-2016-64/cintools/platdefines.h \
 /usr/local/natinst/LabVIEW-2016-64/cintools/fundtypes.h \
 /usr/local/natinst/LabVIEW-2016-64/cintools/lv_prolog.h \
 /usr/local/natinst/LabVIEW-2016-64/cintools/lv_epilog.h
