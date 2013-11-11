// ---------------
// MultiTDCMask.hh
// ---------------

#ifndef MULTITDCMASK_H
#define MULTITDCMASK_H 1

#include <TSystem.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGWindow.h>
#include <TGButtonGroup.h>
#include <TGLabel.h>

class MultiTDCMask : public TGTransientFrame {
	private:
		Bool_t debug;

		UInt_t TDC;
		ULong_t *mask;	//!

		Bool_t *TDCMaskingWindowIsClosed;	//!

		TGLabel *lExplanation;
		TGButtonGroup *bgGroups;
		TGCheckButton *cbGroup[4];

		TGButtonGroup *bgGroup1;
		TGButtonGroup *bgGroup2;
		TGButtonGroup *bgGroup3;
		TGButtonGroup *bgGroup4;
		TGCheckButton *cbChannel[64];

	public:
		MultiTDCMask(const TGWindow *window, const TGWindow *main, UInt_t width, UInt_t height, UInt_t TDCNum, ULong_t *TDCMask, Bool_t *check);
		virtual ~MultiTDCMask();

		void DoClose();
		void MaskGroup(Int_t aGroup = -1, Bool_t groupState = kFALSE);
		void MaskChannel(Int_t aChannel = -1, Bool_t channelState = kFALSE);
		void SetDefaultValues();

		void SetEnabledButtons(Bool_t on);

	ClassDef(MultiTDCMask, 1);
};

#endif
