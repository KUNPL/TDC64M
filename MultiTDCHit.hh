// --------------
// MultiTDCHit.hh
// --------------

#ifndef MULTITDCHIT_H
#define MULTITDCHIT_H 1

class TDCHit {
	public:
		Int_t tdc;		// TDC data (unit: ns)
		Int_t edge;		// edge flag, 0 for trailing edge, 1 for leading edge
		Int_t hitnum;	// hit number
		Int_t evtnum;	// event number
		Int_t ch;			// TDC channel number
};

class TDCHead {
	public:
		Int_t gmode;		// gate mode, 0 for common start, 1 for common stop
		Int_t emode;		// edge mode, 0 for leading edge, 1 for both leading and trailing edge
		Int_t rmode;		// run mode, 0 for L1 buffer mode, 1 for L2 buffer mode
		Int_t range;		// dynamic range (unit: us)
		Int_t delay;		// delay (unit: ns)
		Int_t mask[64];	// channel mask, 0 for unmask, 1 for mask
};

#endif
