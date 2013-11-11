// -----------------
// MultiTDCConfig.hh
// -----------------

#ifndef MULTITDCCONFIG_H
#define MULTITDCCONFIG_H 1

#include <TROOT.h>

// - Device configuration ----------------------------------------------------------
static Int_t nkusb = 0;	// nkvmeusb driver ID

static Int_t moduleID1 = 0x0;	// The module ID can be set with DIP switches on each
static Int_t moduleID2 = 0x1; // electronics panel. You should match them correctly.
// ---------------------------------------------------------------------------------

#endif
