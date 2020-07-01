###############################################################
#
# Purpose: Makefile for motorControl
# Author.: robotis, Eli Jelesko
# Version: 0.1
# License: GPL
#
###############################################################

TARGET = motorControl

CXX = g++
INCLUDE_DIRS = -I../../include -I../../../Framework/include
CXXFLAGS += -O2 -DLINUX -g -Wall -fmessage-length=0 $(INCLUDE_DIRS)
LIBS += -lpthread -lrt
LDFLAGS= -m32

OBJS =	motorControl.o ColorOutput.o

$(TARGET): $(OBJS) ../../lib/darwin.a
	$(CXX) -o $(TARGET) $(OBJS) -m32 ../../lib/darwin.a $(LIBS) $(LDFLAGS)

all: darwin.a $(TARGET)

darwin.a:
	make -C ../../build

clean:
	rm -f *.a *.o $(TARGET) core *~ *.so *.lo

libclean:
	make -C ../../build clean

distclean: clean libclean

# useful to make a backup "make tgz"
tgz: clean
	mkdir -p backups
	tar czvf ./backups/dxl_monitor_`date +"%Y_%m_%d_%H.%M.%S"`.tgz --exclude backups *
