// ------------------
// MultiTDCProcess.hh
// ------------------

#ifndef MULTITDCPROCESS_H
#define MULTITDCPROCESS_H 1

#include "MultiTDC.hh"

class MultiTDCProcess {
	public:
		MultiTDCProcess(MultiTDC *main, const Char_t *filenameWithPath, UInt_t gmode, UInt_t emode, UInt_t rmode, UInt_t dynamicRange, Int_t delay, Int_t numEvents, Bool_t isTDC1On, Bool_t isTDC2On, ULong_t *TDC1Mask, ULong_t *TDC2Mask);
		virtual ~MultiTDCProcess();

	ClassDef(MultiTDCProcess, 1);
};

#endif
