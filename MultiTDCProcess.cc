// ------------------
// MultiTDCProcess.cc
// ------------------

#include "MultiTDCConfig.hh"
#include "MultiTDCProcess.hh"
#include "MultiTDC.hh"
#include "MultiTDCHit.hh"
#include "NoticeTDC64M.h"

#include <Riostream.h>
#include <TGClient.h>
#include <TSystem.h>
#include <TFile.h>
#include <TTree.h>
#include <TSystem.h>
#include <fstream>

ClassImp(MultiTDCProcess)

MultiTDCProcess::MultiTDCProcess(MultiTDC *main, const Char_t *filenameWithPath, UInt_t gmode, UInt_t emode, UInt_t rmode, UInt_t dynamicRange, Int_t delay, Int_t numEvents, Bool_t isTDC1On, Bool_t isTDC2On, ULong_t *TDC1Mask, ULong_t *TDC2Mask)
{
	// Get NKHome environment
	TString myPath = gSystem -> Getenv("NKHOME");
	TString myVME = myPath + TString("/lib/libNotice6UVME_root.so");
	TString myTDC = myPath + TString("/lib/libNoticeTDC64M_root.so");

	// Load VME & TDC library
	gSystem -> Load(myVME);
	gSystem -> Load(myTDC);

	// Local Variables
	Int_t ch[0x40000];  // TDC channel number array
	Int_t nevt[0x40000];  // Event number array
	Int_t nhit[0x40000];  // Hit number array
	Int_t edge[0x40000];  // Edge flag array, 0 for trailing edge, 1 for leading edge
	Int_t data[0x40000];  // TDC data array, in ns unit
	ULong_t value;
	Int_t i, j;
	Int_t bufnumTDC1 = 0; 
	Int_t bufnumTDC2 = 0; 
	Int_t evtTDC1 = 0; 
	Int_t evtTDC2 = 0; 
	Int_t flagTDC1 = 1; 
	Int_t flagTDC2 = 1; 
	Int_t IsFillTDC1 = 1; 
	Int_t IsFillTDC2 = 1; 
	Int_t dnum;
	Int_t sevtTDC1 = 0;
	Int_t eevtTDC1 = 0;
	Int_t aevtTDC1 = 0;
	Int_t sevtTDC2 = 0;
	Int_t eevtTDC2 = 0;
	Int_t aevtTDC2 = 0;
	Int_t ich, ievt;

	// Flag setting
	if (!isTDC1On)
		flagTDC1 = 0;
	if (!isTDC2On)
		flagTDC2 = 0;

	// Class
	NKTDC64M kTDC;

	// VME Open
	kTDC.VMEopen(nkusb);

	// Tree class
	TDCHit TDC1;
	TDCHead head1;

	TDCHit TDC2;
	TDCHead head2;

  TString asciiFilenameWithPath(filenameWithPath);
  asciiFilenameWithPath.ReplaceAll(".root", ".txt");
  std::ofstream writeFileascii(asciiFilenameWithPath.Data());

	TFile *writeFile = 0;
	TTree *treeTDC1 = 0;
	TTree *headTDC1 = 0;
	TTree *treeTDC2 = 0;
	TTree *headTDC2 = 0;
	TString branchTDC = TString("tdc/I:edge/I:hitnum/I:evtnum/I:ch/I");
	TString branchHead = TString("gmode/I:emode/I:rmode/I:range/I:delay/I:mask[64]/I");

  writeFileascii << "# gmode: 0 - common start  1 - common stop" << std::endl;
  writeFileascii << "# emode: 0 - leding        1 - both" << std::endl;
  writeFileascii << "# rmode: 0 - L1            1 - L2" << std::endl;

  writeFileascii << "#";
  writeFileascii << std::setw(15) << "gmode";
  writeFileascii << std::setw(15) << "emode";
  writeFileascii << std::setw(15) << "rmode";
  writeFileascii << std::setw(15) << "range";
  writeFileascii << std::setw(15) << "delay";
  writeFileascii << std::setw(130) << std::left << " mask";
  writeFileascii << std::endl;

	writeFile = new TFile(filenameWithPath, "RECREATE");
	if (isTDC1On) {
		treeTDC1 = new TTree("tree_TDC1", "TDC1 data");
		headTDC1 = new TTree("head_TDC1", "Head of Run - TDC1");
		treeTDC1 -> Branch("TDC1", &TDC1, branchTDC);
		headTDC1 -> Branch("head1", &head1, branchHead);

		kTDC.TDC64Mwrite_MODE(nkusb, moduleID1, gmode, emode, rmode);

		value = kTDC.TDC64Mread_MODE(nkusb, moduleID1);
		head1.gmode = value & 0x1;
		head1.emode = (value >> 1) & 0x1;
		head1.rmode = (value >> 2) & 0x1;

		// Set dynamic range
		kTDC.TDC64Mwrite_RANGE(nkusb, moduleID1, dynamicRange);
		head1.range = kTDC.TDC64Mread_RANGE(nkusb, moduleID1);

		// Set TDC input delay
		kTDC.TDC64Mwrite_DLY(nkusb, moduleID1, delay);
		head1.delay = kTDC.TDC64Mread_DLY(nkusb, moduleID1);

    writeFileascii << std::setw(16) << std::right << head1.gmode;
    writeFileascii << std::setw(15) << std::right << head1.emode;
    writeFileascii << std::setw(15) << std::right << head1.rmode;
    writeFileascii << std::setw(15) << std::right << head1.range;
    writeFileascii << std::setw(15) << std::right << head1.delay;

		// Set TDC input mask
		for (i = 0; i < 4; i++) {
			kTDC.TDC64Mwrite_TMASK(nkusb, moduleID1, i, TDC1Mask[i]);
			value = kTDC.TDC64Mread_TMASK(nkusb, moduleID1, i);

			for (j = 0 + 16*i; j < 16 + 16*i; j++) {
				head1.mask[j] = (value >> j) & 0x1;
        writeFileascii << std::setw(2) << head1.mask[j];
      }
		}

    writeFileascii << std::endl;

		headTDC1 -> Fill();

		// Reset TDC
		kTDC.TDC64Mreset(nkusb, moduleID1);

		// Start TDC DAQ ( Start buffer page 0 and 1 )
		kTDC.TDC64MstartL(nkusb, moduleID1);
		kTDC.TDC64MstartH(nkusb, moduleID1);
	}

	if (isTDC2On) {
		treeTDC2 = new TTree("tree_TDC2", "TDC2 data");
		headTDC2 = new TTree("head_TDC2", "Head of Run - TDC2");
		treeTDC2 -> Branch("TDC2", &TDC2, branchTDC);
		headTDC2 -> Branch("head2", &head2, branchHead);
    
		kTDC.TDC64Mwrite_MODE(nkusb, moduleID2, gmode, emode, rmode);
    
		value = kTDC.TDC64Mread_MODE(nkusb, moduleID2);
		head2.gmode = value & 0x1; 
		head2.emode = (value >> 1) & 0x1;
		head2.rmode = (value >> 2) & 0x1;
    
		// Set dynamic range
		kTDC.TDC64Mwrite_RANGE(nkusb, moduleID2, dynamicRange);
		head2.range = kTDC.TDC64Mread_RANGE(nkusb, moduleID2);
    
		// Set TDC input delay
		kTDC.TDC64Mwrite_DLY(nkusb, moduleID2, delay);
		head2.delay = kTDC.TDC64Mread_DLY(nkusb, moduleID2);

    writeFileascii << std::setw(16) << std::right << head2.gmode;
    writeFileascii << std::setw(15) << std::right << head2.emode;
    writeFileascii << std::setw(15) << std::right << head2.rmode;
    writeFileascii << std::setw(15) << std::right << head2.range;
    writeFileascii << std::setw(15) << std::right << head2.delay;
    
		// Set TDC input mask
		for (i = 0; i < 4; i++) {
			kTDC.TDC64Mwrite_TMASK(nkusb, moduleID2, i, TDC2Mask[i]);
			value = kTDC.TDC64Mread_TMASK(nkusb, moduleID2, i);
      
			for (j = 0 + 16*i; j < 16 + 16*i; j++) {
				head2.mask[j] = (value >> j) & 0x1;
        writeFileascii << std::setw(2) << head2.mask[j];
      }
		}

    writeFileascii << std::endl;
    
		headTDC2 -> Fill();
    
		// Reset TDC
		kTDC.TDC64Mreset(nkusb, moduleID2);
    
		// Start TDC DAQ ( Start buffer page 0 and 1 )
		kTDC.TDC64MstartL(nkusb, moduleID2);
		kTDC.TDC64MstartH(nkusb, moduleID2);
	}
  
  writeFileascii << std::endl;
  writeFileascii << "# edge: 0 - trailing, 1 - leading" << std::endl;

  writeFileascii << "#";
  writeFileascii << std::setw(15) << "moduleID";
  writeFileascii << std::setw(15) << "evtNum";
  writeFileascii << std::setw(15) << "chNum";
  writeFileascii << std::setw(15) << "hitNum";
  writeFileascii << std::setw(15) << "edge";
  writeFileascii << std::setw(15) << "tdc";
  writeFileascii << std::endl;

	while (flagTDC1 | flagTDC2) {
		while (1 & isTDC1On & flagTDC1) {
			if (bufnumTDC1 == 0)
				IsFillTDC1 = !(kTDC.TDC64Mread_RunL(nkusb, moduleID1));
			else if (bufnumTDC1 == 1) 
				IsFillTDC1 = !(kTDC.TDC64Mread_RunH(nkusb, moduleID1));

			if (IsFillTDC1)
				break;
		}

		while (1 & isTDC2On & flagTDC2) {
			if (bufnumTDC2 == 0)
				IsFillTDC2 = !(kTDC.TDC64Mread_RunL(nkusb, moduleID2));
			else if (bufnumTDC2 == 1)
				IsFillTDC2 = !(kTDC.TDC64Mread_RunH(nkusb, moduleID2));
      
			if (IsFillTDC2)
				break;
		}

		// Read data
		// in case of L2 buffer mode
		if (rmode) {
			if (isTDC1On & flagTDC1) {
				// Read data
				dnum = kTDC.TDC64Mread_TDCL2(nkusb, moduleID1, bufnumTDC1, data, edge, nhit, nevt, ch);

				// Save data
				for (i = 0; i < dnum; i++) {
					TDC1.tdc = data[i];
					TDC1.edge = edge[i];
					TDC1.hitnum = nhit[i];
					TDC1.evtnum = nevt[i];
					TDC1.ch = ch[i];

					// Get event number taken
					if (i == 0)
						sevtTDC1 = TDC1.evtnum;
					if (i == (dnum - i))
						eevtTDC1 = TDC1.evtnum;

					if (TDC1.evtnum >= numEvents) {
						eevtTDC1 = TDC1.evtnum;
						aevtTDC1 = eevtTDC1 - sevtTDC1;
						break;
					}

					aevtTDC1 = eevtTDC1 - sevtTDC1 + 1;
					if (aevtTDC1 < 0)
						aevtTDC1 += 0x1000000;

          writeFileascii << std::setw(16) << std::right << "0";
          writeFileascii << std::setw(15) << std::right << TDC1.evtnum;
          writeFileascii << std::setw(15) << std::right << TDC1.ch;
          writeFileascii << std::setw(15) << std::right << TDC1.hitnum;
          writeFileascii << std::setw(15) << std::right << TDC1.edge;
          writeFileascii << std::setw(15) << std::right << TDC1.tdc;
          writeFileascii << std::endl;

					treeTDC1 -> Fill();
				}
			}

			if (isTDC2On & flagTDC2) {
				// Read data
				dnum = kTDC.TDC64Mread_TDCL2(nkusb, moduleID2, bufnumTDC2, data, edge, nhit, nevt, ch);

				// Save data
				for (i = 0; i < dnum; i++) {
					TDC2.tdc = data[i];
					TDC2.edge = edge[i];
					TDC2.hitnum = nhit[i];
					TDC2.evtnum = nevt[i];
					TDC2.ch = ch[i];

					// Get event number taken
					if (i == 0)
						sevtTDC2 = TDC2.evtnum;
					if (i == (dnum - i))
						eevtTDC2 = TDC2.evtnum;
				
					if (TDC2.evtnum >= numEvents) {
						eevtTDC2 = TDC2.evtnum;
						aevtTDC2 = eevtTDC2 - sevtTDC2;
						break;
					}

					aevtTDC2 = eevtTDC2 - sevtTDC2 + 1;
					if (aevtTDC2 < 0)
						aevtTDC2 += 0x1000000;

          writeFileascii << std::setw(16) << std::right << "1";
          writeFileascii << std::setw(15) << std::right << TDC2.evtnum;
          writeFileascii << std::setw(15) << std::right << TDC2.ch;
          writeFileascii << std::setw(15) << std::right << TDC2.hitnum;
          writeFileascii << std::setw(15) << std::right << TDC2.edge;
          writeFileascii << std::setw(15) << std::right << TDC2.tdc;
          writeFileascii << std::endl;

					treeTDC2 -> Fill();
				}
			}
		}

		// in case of L1 buffer mode
		else {
			if (isTDC1On & flagTDC1) {
				// Read data
				kTDC.TDC64Mread_TDCL1(nkusb, moduleID1, bufnumTDC1, emode, nhit, data, edge);

				// Save data
				for (ievt = 0; ievt < 16; ievt++) {
					if (evtTDC1 + ievt >= numEvents)
						break;

					for (ich = 0; ich < 64; ich++) {
						for (i = 0; i < nhit[ich * 16 + ievt]; i++) {
							TDC1.tdc = data[ich * 512 + ievt * 32 + i];
							TDC1.edge = edge[ich * 512 + ievt * 32 + i];
							TDC1.hitnum = i + 1;
							TDC1.evtnum = evtTDC1 + ievt;
							TDC1.ch = ich + 1;

              writeFileascii << std::setw(16) << std::right << "0";
              writeFileascii << std::setw(15) << std::right << TDC1.evtnum;
              writeFileascii << std::setw(15) << std::right << TDC1.ch;
              writeFileascii << std::setw(15) << std::right << TDC1.hitnum;
              writeFileascii << std::setw(15) << std::right << TDC1.edge;
              writeFileascii << std::setw(15) << std::right << TDC1.tdc;
              writeFileascii << std::endl;

							treeTDC1 -> Fill();
						}
					}
				}

				aevtTDC1 = ievt;
			}

			if (isTDC2On & flagTDC2) {
				// Read data
				kTDC.TDC64Mread_TDCL1(nkusb, moduleID2, bufnumTDC2, emode, nhit, data, edge);

				// Save data
				for (ievt = 0; ievt < 16; ievt++) {
					if (evtTDC2 + ievt >= numEvents)
						break;

					for (ich = 0; ich < 64; ich++) {
						for (i = 0; i < nhit[ich * 16 + ievt]; i++) {
							TDC2.tdc = data[ich * 512 + ievt * 32 + i];
							TDC2.edge = edge[ich * 512 + ievt * 32 + i];
							TDC2.hitnum = i + 1;
							TDC2.evtnum = evtTDC2 + ievt;
							TDC2.ch = ich + 1;

              writeFileascii << std::setw(16) << std::right << "1";
              writeFileascii << std::setw(15) << std::right << TDC2.evtnum;
              writeFileascii << std::setw(15) << std::right << TDC2.ch;
              writeFileascii << std::setw(15) << std::right << TDC2.hitnum;
              writeFileascii << std::setw(15) << std::right << TDC2.edge;
              writeFileascii << std::setw(15) << std::right << TDC2.tdc;
              writeFileascii << std::endl;

							treeTDC2 -> Fill();
						}
					}
				}

				aevtTDC2 = ievt;
			}
		}

		if (flagTDC1)
			evtTDC1 += aevtTDC1;
		if (flagTDC2)
			evtTDC2 += aevtTDC2;

		if (isTDC1On & isTDC2On)
			main -> SetCurrentEvent((evtTDC1 + evtTDC2)/2);
		else if (isTDC1On & !isTDC2On)
			main -> SetCurrentEvent(evtTDC1);
		else if (!isTDC1On & isTDC2On)
			main -> SetCurrentEvent(evtTDC2);
		
		gClient -> NeedRedraw(main, kTRUE); // This line is test

		if (evtTDC1 >= numEvents)
			flagTDC1 = 0;
		if (evtTDC2 >= numEvents)
			flagTDC2 = 0;

		if (flagTDC1) {
			if (isTDC1On && bufnumTDC1 == 0) {
				bufnumTDC1 = 1;
				kTDC.TDC64MstartL(nkusb, moduleID1);
      } else if (isTDC1On && bufnumTDC1 == 1) {
				bufnumTDC1 = 0;
				kTDC.TDC64MstartH(nkusb, moduleID1);
			}
		}
		if (flagTDC2) {
			if (isTDC2On && bufnumTDC2 == 0) {
				bufnumTDC2 = 1;
				kTDC.TDC64MstartL(nkusb, moduleID2);
			} else if (isTDC2On && bufnumTDC2 == 1) {
				bufnumTDC2 = 0;
				kTDC.TDC64MstartH(nkusb, moduleID2);
			}
		}

		if (isTDC1On)
			treeTDC1 -> AutoSave();
		if (isTDC2On)
			treeTDC2 -> AutoSave();

		FILE *fp = fopen("KILLME", "r");
		if (fp) {
		fclose(fp);
			printf("KILLME file exist BYE \n");
			gSystem -> Exec("rm -f KILLME");
			break;
		}
	}

	writeFile -> Write();

	kTDC.VMEclose(nkusb);

	delete this;
}

MultiTDCProcess::~MultiTDCProcess()
{
}
