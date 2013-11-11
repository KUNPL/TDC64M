// ---------------
// MultiTDCPlot.cc
// ---------------

#include "MultiTDCPlot.hh"

#include <Riostream.h>
#include <TString.h>
#include <TGButton.h>
#include <TGButtonGroup.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGComboBox.h>
#include <TH1F.h>
#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>

ClassImp(MultiTDCPlot)

MultiTDCPlot::MultiTDCPlot(const TGWindow *window, const TGWindow *main, UInt_t width, UInt_t height, TString *filenameWithPath)
:TGTransientFrame(window, main, width, height, kVerticalFrame),openFilenameWithPath(filenameWithPath)
{
	FILE *fp = fopen("DEBUG", "r");
	if (fp) {
		fclose(fp);
		debug = kTRUE;
	} else
		debug = kFALSE;

//	SetLayoutBroken(kTRUE);
	Connect("CloseWindow()", "MultiTDCPlot", this, "DoClose()");
	DontCallClose();

	bgTDC1 = new TGButtonGroup(this, "TDC 1");
	bgTDC1 -> SetLayoutBroken(kTRUE);
	tbTDC1AllChannel = new TGTextButton(bgTDC1, "All", 1100);
	tbTDC1AllChannel -> MoveResize(10, 20, 50, 40);
		for (Int_t iChannel = 1; iChannel < 65; iChannel++) {
			tbTDC1Channel[iChannel] = new TGTextButton(bgTDC1, Form("%d", iChannel), iChannel + 1000);
			tbTDC1Channel[iChannel] -> MoveResize(60 + ( iChannel < 33 ? iChannel - 1 : iChannel - 33 )*20, ( iChannel < 33 ? 20 : 40), 20, 20);
			tbTDC1Channel[iChannel] -> Connect("Clicked()", "MultiTDCPlot", this, "ChannelButtonClicked()");
		}
	bgTDC1 -> Show();
	bgTDC1 -> Resize(660, 75);
	AddFrame(bgTDC1, new TGLayoutHints(kLHintsTop, 10, 10, 10, 0));

	bgTDC2 = new TGButtonGroup(this, "TDC 2");
	bgTDC2 -> SetLayoutBroken(kTRUE);
	tbTDC2AllChannel = new TGTextButton(bgTDC2, "All", 2100);
	tbTDC2AllChannel -> MoveResize(10, 20, 50, 40);
		for (Int_t iChannel = 1; iChannel < 65; iChannel++) {
			tbTDC2Channel[iChannel] = new TGTextButton(bgTDC2, Form("%d", iChannel), iChannel + 2000);
			tbTDC2Channel[iChannel] -> MoveResize(60 + ( iChannel < 33 ? iChannel - 1 : iChannel - 33 )*20, ( iChannel < 33 ? 20 : 40), 20, 20);
			tbTDC2Channel[iChannel] -> Connect("Clicked()", "MultiTDCPlot", this, "ChannelButtonClicked()");
		}
	bgTDC2 -> Show();
	bgTDC2 -> Resize(660, 75);
	AddFrame(bgTDC2, new TGLayoutHints(kLHintsTop, 10, 10, 0, 0));

	reCanvas = new TRootEmbeddedCanvas("Canvas", this, 660, 660);
	reCanvas -> Resize(660, 660);
	AddFrame(reCanvas, new TGLayoutHints(kLHintsExpandX|kLHintsExpandY, 10, 10, 0, 10));

	channelNumber = 1;
	TDCNumber = 1;
	hist = NULL;

	SetWindowName("Channel Plot");
	MapSubwindows();
	CenterOnParent();
	Resize(GetDefaultSize());
	MapWindow();
}

MultiTDCPlot::~MultiTDCPlot()
{
}

void MultiTDCPlot::ChannelButtonClicked()
{
	TString checking("");
	if (checking.EqualTo(openFilenameWithPath -> Data()))
		return;

	TGButton *aButton = (TGButton *) gTQSender;
	Int_t buttonID = aButton -> WidgetId();

	TDCNumber = buttonID/1000;
	channelNumber = buttonID%1000;

	TFile *openFile = new TFile(openFilenameWithPath -> Data());
	TTree *TDCTree = (TTree *) openFile -> Get(Form("tree_TDC%d", TDCNumber));

	if (TDCTree == NULL)
		return;

	TCanvas *aCanvas = (TCanvas *) reCanvas -> GetCanvas();
	aCanvas -> cd();

	if (hist != NULL)
		delete hist;

	if (hist2 != NULL)
		delete hist2;
	
	aCanvas -> cd();
  if (channelNumber == 100) {
  	hist2 = new TH1F("hist2", "", 64, 0.5, 64.5, 6553, 0, 65536);
  	TDCTree -> Project("hist2", "tdc:ch", "");
  	hist2 -> SetTitle("TDC All Channel");
  	hist2 -> Draw("colz");
  } else {
	  hist = new TH1F("hist", "", 65536, 0, 65536);
  	TDCTree -> Project("hist", "tdc", Form("ch==%d", channelNumber));
  	hist -> SetTitle(Form("TDC %d - Channel %d", TDCNumber, channelNumber));
  	hist -> Draw();
  }
	aCanvas -> Update();

	if (debug) {
		cout << "--------------------------------------------------" << endl;
		cout << "TDC " << TDCNumber << " Channel " << (channelNumber == 100 ?  "All" : channelNumber) << " is plotted!" << endl;
		cout << "--------------------------------------------------" << endl;
	}
}

void MultiTDCPlot::DoClose()
{
	delete this;
}
