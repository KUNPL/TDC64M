// ---------------
// MultiTDCMask.cc
// ---------------

#include "MultiTDCMask.hh"

#include <Riostream.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGLabel.h>
#include <TGButtonGroup.h>

ClassImp(MultiTDCMask)

MultiTDCMask::MultiTDCMask(const TGWindow *window, const TGWindow *main, UInt_t width, UInt_t height, UInt_t TDCNum, ULong_t *TDCMask, Bool_t *checkIsOff)
:TGTransientFrame(window, main, width, height, kFixedWidth|kFixedHeight),TDC(TDCNum),mask(TDCMask),TDCMaskingWindowIsClosed(checkIsOff)
{
	FILE *fp = fopen("DEBUG", "r");
	if (fp) {
		fclose(fp);
		debug = kTRUE;
	} else  
		debug = kFALSE;

	SetLayoutBroken(kTRUE);

	// Make unable to open a window any more
	*TDCMaskingWindowIsClosed = kFALSE;

	Connect("CloseWindow()", "MultiTDCMask", this, "DoClose()");
	DontCallClose();

	lExplanation = new TGLabel(this, "Check channels to mask");
	lExplanation -> SetLayoutBroken(kTRUE);
	lExplanation -> MoveResize(10, 10, 360, 15);
	AddFrame(lExplanation);

	bgGroups = new TGButtonGroup(this, "Channel Group", kHorizontalFrame);
	bgGroups -> SetLayoutBroken(kTRUE);
		cbGroup[0] = new TGCheckButton(bgGroups, "1: 1~16", 0);
		cbGroup[0] -> MoveResize(15, 23, 70, 15);
		cbGroup[1] = new TGCheckButton(bgGroups, "2: 17~32", 1);
		cbGroup[1] -> MoveResize(100, 23, 70, 15);
		cbGroup[2] = new TGCheckButton(bgGroups, "3: 33~48", 2);
		cbGroup[2] -> MoveResize(185, 23, 70, 15);
		cbGroup[3] = new TGCheckButton(bgGroups, "4: 49~64", 3);
		cbGroup[3] -> MoveResize(270, 23, 70, 15);
	bgGroups -> Show();
	bgGroups -> MoveResize(10, 30, 360, 45);
	AddFrame(bgGroups);

	bgGroup1 = new TGButtonGroup(this, "Group 1", kHorizontalFrame);
	bgGroup1 -> SetLayoutBroken(kTRUE);
		for (Int_t channelNum = 0; channelNum < 16; channelNum++) {
			cbChannel[channelNum] = new TGCheckButton(bgGroup1, Form("%d", channelNum+1), channelNum);

			Int_t forLayoutX = channelNum%8;
			Int_t forLayoutY = (channelNum/8)%2;
			cbChannel[channelNum] -> MoveResize(15 + forLayoutX*42, 23 + forLayoutY*20, 35, 15);
		}
	bgGroup1 -> Show();
	bgGroup1 -> MoveResize(10, 80, 360, 70);
	AddFrame(bgGroup1, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 0));

	bgGroup2 = new TGButtonGroup(this, "Group 2", kHorizontalFrame);
	bgGroup2 -> SetLayoutBroken(kTRUE);
		for (Int_t channelNum = 16; channelNum < 32; channelNum++) {
			cbChannel[channelNum] = new TGCheckButton(bgGroup2, Form("%d", channelNum+1), channelNum);

			Int_t forLayoutX = channelNum%8;
			Int_t forLayoutY = (channelNum/8)%2;
			cbChannel[channelNum] -> MoveResize(15 + forLayoutX*42, 23 + forLayoutY*20, 35, 15);
		}
	bgGroup2 -> Show();
	bgGroup2 -> MoveResize(10, 155, 360, 70);
	AddFrame(bgGroup2, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 0));

	bgGroup3 = new TGButtonGroup(this, "Group 3", kHorizontalFrame);
	bgGroup3 -> SetLayoutBroken(kTRUE);
		for (Int_t channelNum = 32; channelNum < 48; channelNum++) {
			cbChannel[channelNum] = new TGCheckButton(bgGroup3, Form("%d", channelNum+1), channelNum);

			Int_t forLayoutX = channelNum%8;
			Int_t forLayoutY = (channelNum/8)%2;
			cbChannel[channelNum] -> MoveResize(15 + forLayoutX*42, 23 + forLayoutY*20, 35, 15);
		}
	bgGroup3 -> Show();
	bgGroup3 -> MoveResize(10, 230, 360, 70);
	AddFrame(bgGroup3, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 0));

	bgGroup4 = new TGButtonGroup(this, "Group 4", kHorizontalFrame);
	bgGroup4 -> SetLayoutBroken(kTRUE);
		for (Int_t channelNum = 48; channelNum < 64; channelNum++) {
			cbChannel[channelNum] = new TGCheckButton(bgGroup4, Form("%d", channelNum+1), channelNum);

			Int_t forLayoutX = channelNum%8;
			Int_t forLayoutY = (channelNum/8)%2;
			cbChannel[channelNum] -> MoveResize(15 + forLayoutX*42, 23 + forLayoutY*20, 35, 15);
		}
	bgGroup4 -> Show();
	bgGroup4 -> MoveResize(10, 305, 360, 70);
	AddFrame(bgGroup4, new TGLayoutHints(kLHintsExpandX, 10, 10, 10, 0));

	for (Int_t groupNum = 0; groupNum < 4; groupNum++)
		cbGroup[groupNum] -> Connect("Clicked()", "MultiTDCMask", this, "MaskGroup()");

	for (Int_t channelNum = 0; channelNum < 64; channelNum++) {
		cbChannel[channelNum] -> Connect("Clicked()", "MultiTDCMask", this, "MaskChannel()");
	}

	SetDefaultValues();

	SetWindowName(Form("Channel Masking of TDC %d", TDC));
	MapSubwindows();
	CenterOnParent();
	Resize(GetDefaultSize());
	MapWindow();
}

MultiTDCMask::~MultiTDCMask()
{
}

void MultiTDCMask::DoClose()
{
	*TDCMaskingWindowIsClosed = kTRUE;

	delete this;
}

void MultiTDCMask::MaskGroup(Int_t aGroup, Bool_t groupState)
{
	if (aGroup == -1) {
		TGButton *aButton = (TGButton *) gTQSender;
		aGroup = aButton -> WidgetId();
		groupState = aButton -> GetState();
	}

	Int_t initialChannelNum = aGroup*16;
	Int_t finalChannelNum = initialChannelNum + 16;
	if (groupState) {
		for (Int_t channelNum = initialChannelNum; channelNum < finalChannelNum; channelNum++)
			cbChannel[channelNum] -> SetState(kButtonDown, kFALSE);
			cbGroup[aGroup] -> SetState(kButtonDown, kFALSE);

		mask[aGroup] = 0xFFFF;
	} else {
		for (Int_t channelNum = initialChannelNum; channelNum < finalChannelNum; channelNum++)
			cbChannel[channelNum] -> SetState(kButtonUp, kFALSE);
			cbGroup[aGroup] -> SetState(kButtonUp, kFALSE);

		mask[aGroup] = 0x0000;
	}

	if (debug) {
		cout << "--------------------------" << endl;
		cout << " Masking status of TDC " << TDC << endl;
		cout << "--------------------------" << endl;
		cout << " Group 1: " << mask[0] << endl;
		cout << " Group 2: " << mask[1] << endl;
		cout << " Group 3: " << mask[2] << endl;
		cout << " Group 4: " << mask[3] << endl;
		cout << "--------------------------" << endl;
	}
}

void MultiTDCMask::MaskChannel(Int_t aChannel, Bool_t channelState)
{
	if (aChannel == -1) {
		TGButton *aButton = (TGButton *) gTQSender;
		aChannel = aButton -> WidgetId();
		channelState = aButton -> GetState();
	}

	Int_t groupNum = aChannel/16;
	Int_t initialChannelNum = groupNum*16;
	Int_t finalChannelNum = initialChannelNum + 16;
	// Connection between group check box and channel check boxes
	for (Int_t channelNum = initialChannelNum; channelNum < finalChannelNum; channelNum++) {
		if (!(cbChannel[channelNum] -> GetState())) {
			cbGroup[groupNum] -> SetState(kButtonUp, kFALSE);
			break;
		} else if (channelNum == finalChannelNum - 1) {
			cbGroup[groupNum] -> SetState(kButtonDown, kTRUE);
			return;
		}
	}
	
	// Find out masking constant
	mask[groupNum] = 0x0000;
	for (Int_t channelNum = initialChannelNum; channelNum < finalChannelNum; channelNum++) {
		mask[groupNum] = mask[groupNum] + ((cbChannel[channelNum] -> GetState()) << (channelNum - groupNum*16));
	}
	
	if (debug) {
		cout << "--------------------------------" << endl;
		cout << "Channel " << aChannel + 1 << " is " << ( channelState ? "masked!" : "unmasked!" ) << endl;
		cout << "--------------------------------" << endl;

		cout << "--------------------------" << endl;
		cout << " Masking status of TDC " << TDC << endl;
		cout << "--------------------------" << endl;
		cout << " Group 1: " << mask[0] << endl;
		cout << " Group 2: " << mask[1] << endl;
		cout << " Group 3: " << mask[2] << endl;
		cout << " Group 4: " << mask[3] << endl;
		cout << "--------------------------" << endl;
	}
}

void MultiTDCMask::SetDefaultValues()
{
	for (UInt_t i = 0; i < 4; i++) {
		if (mask[i] == 0xFFFF) {
			cbGroup[i] -> SetState(kButtonDown, kFALSE);
		}
		
		for (UInt_t j = 0; j < 16; j++) {
			if (((mask[i] >> j) & 0x1))
				cbChannel[j + i*16] -> SetState(kButtonDown, kFALSE);
		}
	}
}

void MultiTDCMask::SetEnabledButtons(Bool_t on)
{
	for (Int_t i = 0; i < 4; i++)
		cbGroup[i] -> SetEnabled(on);

	for (Int_t i = 0; i < 64; i++)
		cbChannel[i] -> SetEnabled(on);
}
