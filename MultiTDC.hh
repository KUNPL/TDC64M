// -----------
// MultiTDC.hh
// -----------

#ifndef MULTITDC_H
#define MULTITDC_H 1

#include <TGButton.h>
#include <TGFrame.h>

#include <TGFileDialog.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TGNumberEntry.h>
#include <TGButtonGroup.h>
#include <TGComboBox.h>
#include <TGWindow.h>

#include "MultiTDCMask.hh"

class MultiTDC : public TGMainFrame {
	private:
		Bool_t debug;

		TGCompositeFrame *firstFrameLine;

		// Electronics
		TGButtonGroup *fElectronics;
		TGCheckButton *cbTDC[2];
		Bool_t isTDC1On;
		Bool_t isTDC2On;

		// Buffer
		TGButtonGroup *fBuffer;
		TGRadioButton *rbBuffer[2];
		UInt_t rmode;

		// Channel Masking
		TGButtonGroup *fChMask;
		TGTextButton *tbTDC[2];
		Bool_t TDC1MaskingWindowIsClosed;
		Bool_t TDC2MaskingWindowIsClosed;
		MultiTDCMask *maskTDC1Channel;
		MultiTDCMask *maskTDC2Channel;
		ULong_t TDC1Mask[4];
		ULong_t TDC2Mask[4];

		TGCompositeFrame *secondFrameLine;
		
		// Gate Mode
		TGButtonGroup *fGateMode;
		TGRadioButton *rbGateMode[2];
		UInt_t gmode;

		// Edge Mode
		TGButtonGroup *fEdgeMode;
		TGRadioButton *rbEdgeMode[2];
		UInt_t emode;

		// Dynamic Range
		TGGroupFrame *fDynamicRange;
		TGComboBox *cbDRList;
		TGLabel *lDRUnit;
		UInt_t dynamicRange;

		// Delay
		TGGroupFrame *fDelay;
		TGNumberEntry *neDelay;
		TGLabel *lDelayUnit;
		Int_t delay;

		TGCompositeFrame *thirdFrameLine;

		// Saving directory and filename
		TGGroupFrame *fSave;
		TGTextEntry *teDirectory;
		TGTextEntry *teFilename;
		TGTextButton *tbChoose;
		TString *stringFilenameWithPath;

		// Taking data as file
		TGButtonGroup *fTakeOrTest;
		TGRadioButton *rbTakeOrTest[2];
		Bool_t isTestMode;

		TGCompositeFrame *fourthFrameLine;

		// Number of Events
		TGGroupFrame *fEvents;
		TGLabel *lEventNumber;
		TGLabel *lSlash;
		TGNumberEntry *neNumEvents;
		Int_t numEvents;

		// Start, Stop, Quit Buttons
		TGTextButton *tbStart;
		TGTextButton *tbStop;
		TGTextButton *tbPlotWindow;

	public:
		MultiTDC(const TGWindow *window, UInt_t width, UInt_t height);
		virtual ~MultiTDC();

		void SetTDC1(Bool_t);	// Set TDC1
		void SetTDC2(Bool_t); // Set TDC2
		void SetBuffer(Int_t buffer);	// Setting Buffer
		void OpenTDC1MaskingWindow(); // Open a Window Setting for Masking of TDC1
		void OpenTDC2MaskingWindow(); // Open a Window Setting for Masking of TDC2
		void SetGateMode(Int_t gatemode);	// Setting Gate Mode
		void SetEdgeMode(Int_t edgeMode);	// Setting Edge Mode
		void SetDynamicRange(Int_t dr);	// Setting Dynamic Range
		void SetDelay(); // Setting Delay
		void SetTakeOrTest(Int_t takeOrTest); // Setting Test Mode
		void SetCurrentEvent(UInt_t eventNum);	// Setting Current Event Number
		void SetNumEvents(); // Setting the Number of Events

		void SetSavingDirectoryAndFilename();	// Choose Saving Directory and Filename

		void StartTDC();	// Start Taking or Testing TDC
		void StopTDC();	// Stop
		void OpenPlotWindow(); // Open Plot Window

		void SetEnabledButtons(Bool_t on);

	ClassDef(MultiTDC, 1)
};

#endif
