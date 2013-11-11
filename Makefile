ROOTCFLAGS := $(shell root-config --cflags)
ROOTGLIBS := $(shell root-config --glibs)

COPTS = -c -fPIC -O -Wall -Wno-deprecated -DNKROOT -I/$(NKHOME)/include $(ROOTCFLAGS)

DEPLIBS = -L/$(NKHOME)/lib  -lNotice6UVME_root -lNoticeTDC64M_root

TARGET = MultiTDC
HEADERS = MultiTDC.hh MultiTDCMask.hh MultiTDCPlot.hh MultiTDCProcess.hh

$(TARGET): $(HEADERS:.hh=.o) $(TARGET)Dict.o
	g++ -o $@ $(DEPLIBS) $(ROOTGLIBS) $^

$(TARGET)Dict.cc: $(HEADERS)
	rootcint -f $@ -c $^ $(TARGET)LinkDef.hh

%.o: %.cc
	g++ $(COPTS) $<

clean:
	rm -rf *.o
	rm -rf *Dict.*
	rm $(TARGET)
