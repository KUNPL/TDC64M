// ---------------
// MultiTDCPlot.hh
// ---------------

#ifndef MULTITDCPLOT_H
#define MULTITDCPLOT_H 1

#include <TString.h>
#include <TGButton.h>
#include <TGWindow.h>
#include <TGButtonGroup.h>
#include <TGFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGComboBox.h>

#include "TH1F.h"

class MultiTDCPlot : public TGTransientFrame {
	private:
		Bool_t debug;
		TString *openFilenameWithPath;
		TGButtonGroup *bgTDC1;
		TGTextButton *tbTDC1AllChannel;
		TGTextButton *tbTDC1Channel[64];
		TGButtonGroup *bgTDC2;
		TGTextButton *tbTDC2AllChannel;
		TGTextButton *tbTDC2Channel[64];
		TGTextButton *tbRefresh;
		TRootEmbeddedCanvas *reCanvas;

		TH1F *hist;
		TH2F *hist2;

		UInt_t TDCNumber;
		UInt_t channelNumber;

	public:
		MultiTDCPlot(const TGWindow *window, const TGWindow *main, UInt_t width, UInt_t height, TString *filenameWithPath);
		virtual ~MultiTDCPlot();

		void ChannelButtonClicked();
		void DoClose();

	ClassDef(MultiTDCPlot, 1);
};

#endif
