// -----------
// MultiTDC.cc
// -----------

#include <Riostream.h>
#include <TSystem.h>
#include <TGClient.h>
#include <TGButton.h>
#include <TGFrame.h>
#include <TGDimension.h>

#include <TGFileDialog.h>
#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>
#include <TGTextEntry.h>
#include <TGLabel.h>
#include <TRootEmbeddedCanvas.h>
#include <TGNumberEntry.h>
#include <TGButtonGroup.h>
#include <TApplication.h>

#include <TGComboBox.h>

#include "MultiTDC.hh"
#include "MultiTDCProcess.hh"
#include "MultiTDCMask.hh"
#include "MultiTDCPlot.hh"

ClassImp(MultiTDC)

// - Electronics Group -------------------------------------------------------------------------
void MultiTDC::SetTDC1(Bool_t on)
{
	isTDC1On = ( on ? kTRUE : kFALSE );

	if (debug) {
		cout << "------------------------" << endl;
		cout << "TDC1 is " << ( on ? "on." : "off." ) << endl;
		cout << "------------------------" << endl;
	}
}

void MultiTDC::SetTDC2(Bool_t on)
{
	isTDC2On = ( on ? kTRUE : kFALSE );

	if (debug) {
		cout << "------------------------" << endl;
		cout << "TDC2 is " << ( on ? "on." : "off." ) << endl;
		cout << "------------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Buffer Group -------------------------------------------------------------------------------
void MultiTDC::SetBuffer(Int_t buffer)
{
	if (buffer == 1)
		rmode = 0;
	else if (buffer == 2)
		rmode = 1;

	if (debug) {
		cout << "------------------------" << endl;
		cout << "Use " << ( rmode ? "L2 " : "L1 " ) << "buffer" << endl;
		cout << "------------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Channel Masking Group ----------------------------------------------------------------------
void MultiTDC::OpenTDC1MaskingWindow()
{
	if (TDC1MaskingWindowIsClosed)
		maskTDC1Channel = new MultiTDCMask(gClient -> GetRoot(), this, 380, 385, 1, TDC1Mask, &TDC1MaskingWindowIsClosed);
}

void MultiTDC::OpenTDC2MaskingWindow()
{
	if (TDC2MaskingWindowIsClosed)
		maskTDC2Channel = new MultiTDCMask(gClient -> GetRoot(), this, 380, 385, 2, TDC2Mask, &TDC2MaskingWindowIsClosed);
}
// ----------------------------------------------------------------------------------------------

// - Gate Mode Group ----------------------------------------------------------------------------
void MultiTDC::SetGateMode(Int_t gateMode)
{
	if (gateMode == 1)
		gmode = 0;
	else if (gateMode == 2)
		gmode = 1;

	if (debug) {
		cout << "-------------------------------------" << endl;
		cout << "Gate mode is set to be common " << ( gmode ? "stop." : "start." ) << endl;
		cout << "-------------------------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Edge Mode Group ----------------------------------------------------------------------------
void MultiTDC::SetEdgeMode(Int_t edgeMode)
{
	if (edgeMode == 1)
		emode = 0;
	else if (edgeMode == 2)
		emode = 1;

	if (debug) {
		cout << "------------------------------------------------------------" << endl;
		cout << "Edge mode is set to be " << ( emode ? "both leading and trailing edge mode." :
						"leading edge only mode." ) << endl;
		cout << "------------------------------------------------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Dynamic Range Group ----------------------------------------------------------------------------
void MultiTDC::SetDynamicRange(Int_t dr)
{
	dynamicRange = dr;

	if (debug) {
		cout << "----------------------------------" << endl;
		cout << "Dynamic Range is set to be " << dynamicRange << " us." << endl;
		cout << "----------------------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Delay Group --------------------------------------------------------------------------------
void MultiTDC::SetDelay()
{
	delay = neDelay -> GetNumberEntry() -> GetIntNumber();
	if (!delay) {
		delay = 0;
		neDelay -> SetNumber(0);
	}

	if (debug) {
		cout << "---------------------------" << endl;
		cout << "Delay is set to be " << delay << " ns." << endl;
		cout << "---------------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Take or Test Group -------------------------------------------------------------------------
void MultiTDC::SetTakeOrTest(Int_t takeOrTest)
{
	if (takeOrTest == 1) {
		isTestMode = kFALSE;

		stringFilenameWithPath -> Clear();
		TString check("");
		if (!(check.EqualTo(teDirectory -> GetText()) && check.EqualTo(teFilename -> GetText()))) {
			stringFilenameWithPath -> Append(teDirectory -> GetText());
			stringFilenameWithPath -> Append("/");
			stringFilenameWithPath -> Append(teFilename -> GetText());
		}
		tbChoose -> SetEnabled(kTRUE);
	} else if (takeOrTest == 2) {
		isTestMode = kTRUE;

		stringFilenameWithPath -> Clear();
		stringFilenameWithPath -> Append("./tdc64m.root");
		tbChoose -> SetEnabled(kFALSE);
	}

	if (debug) {
		cout << "------------------" << endl;
		cout << "Test mode is " << ( isTestMode ? "on." : "off." ) << endl;
		cout << "------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Choose Button ------------------------------------------------------------------------------
void MultiTDC::SetSavingDirectoryAndFilename()
{
	TGFileInfo theFileInfo;
	new TGFileDialog(gClient -> GetRoot(), this, kFDSave, &theFileInfo);

	// If cancelled by user, do nothing
	if (theFileInfo.fFilename == NULL)
		return;

	// Get the path and set it to directory TGTextEntry
	TString *fileNameWithPath = new TString(theFileInfo.fFilename);
	TObjArray *decomposedFileNameWithPath = fileNameWithPath -> Tokenize("/");
	UInt_t componentCounts = decomposedFileNameWithPath -> GetEntries();

	teDirectory -> Clear();
	for (Int_t i = 0; i < componentCounts - 1; i++) {
		teDirectory -> AppendText("/");
		TString string = ((TObjString *) decomposedFileNameWithPath -> At(i)) -> GetString();
		teDirectory -> AppendText(string);
	}

	// Get the filename and check it has 'root' for extension name. If not, append one.
	TString fileName = ((TObjString *) decomposedFileNameWithPath -> At(componentCounts - 1)) -> GetString();
	TObjArray *decomposedFileNameWithROOT = fileName.Tokenize(".");

	teFilename -> Clear();
	if ((TObjString *) decomposedFileNameWithROOT -> At(1)) {
		if ((((TObjString *) decomposedFileNameWithROOT -> At(1)) -> GetString()).EqualTo("root", TString::kIgnoreCase))
			teFilename -> SetText(fileName);
		else {
			teFilename -> SetText(fileName);
			teFilename -> AppendText(".root");
		}
	} else {
			teFilename -> SetText(fileName);
			teFilename -> AppendText(".root");
	}

	stringFilenameWithPath -> Clear();
	stringFilenameWithPath -> Append(teDirectory -> GetText());
	stringFilenameWithPath -> Append("/");
	stringFilenameWithPath -> Append(teFilename -> GetText());
	
	delete decomposedFileNameWithROOT;
	delete decomposedFileNameWithPath;
	delete fileNameWithPath;
}
// ----------------------------------------------------------------------------------------------

// - # of Events Group  -------------------------------------------------------------------------
void MultiTDC::SetCurrentEvent(UInt_t eventNum)
{
	lEventNumber -> SetText(Form("%d", eventNum));

	if (debug) {
		cout << "-------------------------------" << endl;
		cout << "Current event number is " << eventNum << endl;
		cout << "-------------------------------" << endl;
	}

	if (eventNum == numEvents)
		StopTDC();
}

void MultiTDC::SetNumEvents()
{
	numEvents = neNumEvents -> GetNumberEntry() -> GetIntNumber();
	neNumEvents -> SetNumber(numEvents);
	if (!numEvents) {
		numEvents = 10000;
		neNumEvents -> SetNumber(10000);
	}

	if (debug) {
		cout << "---------------------" << endl;
		cout << "Take " << numEvents << " events." << endl;
		cout << "---------------------" << endl;
	}
}
// ----------------------------------------------------------------------------------------------

// - Start Button -------------------------------------------------------------------------------
void MultiTDC::StartTDC()
{
	TString check("");
	if (check.EqualTo(stringFilenameWithPath -> Data()))
		return;

	SetNumEvents();
	SetDelay();

	SetEnabledButtons(kFALSE);

	new MultiTDCProcess(this, stringFilenameWithPath -> Data(), gmode, emode, rmode, dynamicRange, delay, numEvents, isTDC1On, isTDC2On, TDC1Mask, TDC2Mask);
}
// ----------------------------------------------------------------------------------------------

// - Stop Button --------------------------------------------------------------------------------
void MultiTDC::StopTDC()
{
	if (tbStart -> GetState())
		SetEnabledButtons(kTRUE);
}
// ----------------------------------------------------------------------------------------------

// - Plot Window Button -------------------------------------------------------------------------
void MultiTDC::OpenPlotWindow()
{
	new MultiTDCPlot(gClient -> GetRoot(), this, 710, 840, stringFilenameWithPath);
}
// ----------------------------------------------------------------------------------------------

// - Activation of Buttons ----------------------------------------------------------------------
void MultiTDC::SetEnabledButtons(Bool_t on)
{
	cbTDC[0] -> SetEnabled(on);
	cbTDC[1] -> SetEnabled(on);
	rbBuffer[0] -> SetEnabled(on);
	rbBuffer[1] -> SetEnabled(on);
	tbTDC[0] -> SetEnabled(on);
	tbTDC[1] -> SetEnabled(on);
	rbGateMode[0] -> SetEnabled(on);
	rbGateMode[1] -> SetEnabled(on);
	rbEdgeMode[0] -> SetEnabled(on);
	rbEdgeMode[1] -> SetEnabled(on);
	cbDRList -> SetEnabled(on);
	neDelay -> SetState(on);
//	teDirectory -> SetState(on);
//	teFilename -> SetState(on);
	if (isTestMode)
		tbChoose -> SetEnabled(kFALSE);
	else
		tbChoose -> SetEnabled(on);
	rbTakeOrTest[0] -> SetEnabled(on);
	rbTakeOrTest[1] -> SetEnabled(on);
	neNumEvents -> SetState(on);
	tbStart -> SetEnabled(on);

	if (!TDC1MaskingWindowIsClosed)
		maskTDC1Channel -> SetEnabledButtons(on);

	if (!TDC2MaskingWindowIsClosed)
		maskTDC2Channel -> SetEnabledButtons(on);
}
// ----------------------------------------------------------------------------------------------

MultiTDC::MultiTDC(const TGWindow *window, UInt_t width, UInt_t height)
:TGMainFrame(window, width, height)
{
	FILE *fp = fopen("DEBUG", "r");
	if (fp) {
		fclose(fp);
		debug = kTRUE;
	} else  
		debug = kFALSE;


// - first frame ---------------------------------------------------------------------------------
	firstFrameLine = new TGHorizontalFrame(this, width, 45, kFixedHeight|kFixedWidth);
	firstFrameLine -> SetLayoutBroken(kTRUE);

	// Electronics Group
	fElectronics = new TGButtonGroup(firstFrameLine, "Electronics", kHorizontalFrame);
		cbTDC[0] = new TGCheckButton(fElectronics, "TDC 1");
		cbTDC[1] = new TGCheckButton(fElectronics, "TDC 2");
		cbTDC[0] -> SetState(kButtonDown, kTRUE);
		cbTDC[0] -> Connect("Toggled(Bool_t)", "MultiTDC", this, "SetTDC1(Bool_t)");
		cbTDC[1] -> Connect("Toggled(Bool_t)", "MultiTDC", this, "SetTDC2(Bool_t)");
	fElectronics -> SetLayoutHints(new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
	fElectronics -> Show();
	fElectronics -> MoveResize(0, 0, 200, 45);
	firstFrameLine -> AddFrame(fElectronics, new TGLayoutHints(kLHintsExpandX, 2, 2, 2, 2));

	// Buffer Group
	fBuffer = new TGButtonGroup(firstFrameLine, "Buffer", kHorizontalFrame);
		rbBuffer[0] = new TGRadioButton(fBuffer, "L1");
		rbBuffer[1] = new TGRadioButton(fBuffer, "L2");
		rbBuffer[0] -> SetState(kButtonDown);
	fBuffer -> Connect("Clicked(Int_t)", "MultiTDC", this, "SetBuffer(Int_t)");
	fBuffer -> SetLayoutHints(new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
	fBuffer -> Show();
	fBuffer -> MoveResize(210, 0, 200, 45);
	firstFrameLine -> AddFrame(fBuffer, new TGLayoutHints(kLHintsExpandX));

	// Channel Masking Group
	fChMask = new TGButtonGroup(firstFrameLine, "Channel Masking", kHorizontalFrame);
		fChMask -> SetLayoutBroken(kTRUE);
		tbTDC[0] = new TGTextButton(fChMask, "TDC 1");
		tbTDC[0] -> MoveResize(15, 19, 80, 20);
		tbTDC[0] -> Connect("Clicked()", "MultiTDC", this, "OpenTDC1MaskingWindow()");
		tbTDC[1] = new TGTextButton(fChMask, "TDC 2");
		tbTDC[1] -> MoveResize(105, 19, 80, 20);
		tbTDC[1] -> Connect("Clicked()", "MultiTDC", this, "OpenTDC2MaskingWindow()");
	fChMask -> SetLayoutHints(new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
	fChMask -> Show();
	fChMask -> MoveResize(420, 0, 200, 45);
	firstFrameLine -> AddFrame(fChMask, new TGLayoutHints(kLHintsExpandX));
	AddFrame(firstFrameLine, new TGLayoutHints(kLHintsCenterX, 10, 10, 10, 0));
// -----------------------------------------------------------------------------------------------

// - second frame --------------------------------------------------------------------------------
	secondFrameLine = new TGHorizontalFrame(this, width, 70, kFixedHeight|kFixedWidth);
	secondFrameLine -> SetLayoutBroken(kTRUE);

	// Gate mode Group
	fGateMode = new TGButtonGroup(secondFrameLine, "Gate Mode");
	fGateMode -> SetLayoutBroken(kTRUE);
		rbGateMode[0] = new TGRadioButton(fGateMode, "Common start");
		rbGateMode[0] -> MoveResize(15, 25, 100, 14);
		rbGateMode[0] -> SetState(kButtonDown);
		rbGateMode[1] = new TGRadioButton(fGateMode, "Common stop");
		rbGateMode[1] -> MoveResize(15, 45, 100, 14);
	fGateMode -> Connect("Clicked(Int_t)", "MultiTDC", this, "SetGateMode(Int_t)");
//	fGateMode -> SetLayoutHints(new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
	fGateMode -> Show();
	fGateMode -> MoveResize(0, 0, 135, 75);
	secondFrameLine -> AddFrame(fGateMode, new TGLayoutHints(kLHintsExpandX));

	// Edge mode Group
	fEdgeMode = new TGButtonGroup(secondFrameLine, "Edge Mode");
	fEdgeMode -> SetLayoutBroken(kTRUE);
		rbEdgeMode[0] = new TGRadioButton(fEdgeMode, "Leading edge only");
		rbEdgeMode[0] -> MoveResize(15, 25, 190, 14);
		rbEdgeMode[0] -> SetState(kButtonDown);
		rbEdgeMode[1] = new TGRadioButton(fEdgeMode, "Both leading and trailing edge");
		rbEdgeMode[1] -> MoveResize(15, 45, 190, 14);
	fEdgeMode -> Connect("Clicked(Int_t)", "MultiTDC", this, "SetEdgeMode(Int_t)");
	fEdgeMode -> SetLayoutHints(new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
	fEdgeMode -> Show();
	fEdgeMode -> MoveResize(145, 0, 230, 75);
	secondFrameLine -> AddFrame(fEdgeMode);

	// Dynamic Range Group
	fDynamicRange = new TGGroupFrame(secondFrameLine, "Dynamic Range");
	fDynamicRange -> SetLayoutBroken(kTRUE);
		cbDRList = new TGComboBox(fDynamicRange);
		cbDRList -> AddEntry("1", 1);
		cbDRList -> AddEntry("2", 2);
		cbDRList -> AddEntry("4", 4);
		cbDRList -> AddEntry("8", 8);
		cbDRList -> AddEntry("16", 16);
		cbDRList -> AddEntry("32", 32);
		cbDRList -> AddEntry("64", 64);
		cbDRList -> MoveResize(30, 30, 45, 20);
		cbDRList -> Select(64);
		cbDRList -> Connect("Selected(Int_t)", "MultiTDC", this, "SetDynamicRange(Int_t)");
	fDynamicRange -> AddFrame(cbDRList);
		lDRUnit = new TGLabel(fDynamicRange);
		lDRUnit -> SetText("us");
		lDRUnit -> MoveResize(75, 30, 20, 20);
	fDynamicRange -> AddFrame(lDRUnit);
	fDynamicRange -> MoveResize(385, 0, 120, 75);
	secondFrameLine -> AddFrame(fDynamicRange);

	// Delay Group
	fDelay = new TGGroupFrame(secondFrameLine, "Delay");
	fDelay -> SetLayoutBroken(kTRUE);
		neDelay = new TGNumberEntry(fDelay, 0);
		neDelay -> MoveResize(25, 30, 45, 20);
		neDelay -> Connect("ValueSet(Long_t)", "MultiTDC", this, "SetDelay()");
	fDelay -> AddFrame(neDelay);
		lDelayUnit = new TGLabel(fDelay);
		lDelayUnit -> SetText("ns");
		lDelayUnit -> MoveResize(70, 30, 20, 20);
	fDelay -> AddFrame(lDelayUnit);
	fDelay -> MoveResize(515, 0, 105, 75);
	secondFrameLine -> AddFrame(fDelay);

	AddFrame(secondFrameLine, new TGLayoutHints(kLHintsCenterX, 10, 10, 10, 0));
// -----------------------------------------------------------------------------------------------

// - third frame ---------------------------------------------------------------------------------
	thirdFrameLine = new TGHorizontalFrame(this, width, 85, kFixedHeight|kFixedWidth);
	thirdFrameLine -> SetLayoutBroken(kTRUE);

	// Saving directory and filename Group
	fSave = new TGGroupFrame(thirdFrameLine, "Saving Directory and Filename");
	fSave -> SetLayoutBroken(kTRUE);
		teDirectory = new TGTextEntry(fSave);
		teDirectory -> MoveResize(12, 25, 400, 20);
		teDirectory -> SetState(kFALSE);
		teFilename = new TGTextEntry(fSave);
		teFilename -> MoveResize(12, 47, 400, 20);
		teFilename -> SetState(kFALSE);
	fSave -> AddFrame(teDirectory);
	fSave -> AddFrame(teFilename);
		tbChoose = new TGTextButton(fSave, "Choose");
		tbChoose -> Connect("Clicked()", "MultiTDC", this, "SetSavingDirectoryAndFilename()");
		tbChoose -> MoveResize(414, 25, 50, 41);
	fSave -> AddFrame(tbChoose);
	fSave -> MoveResize(0, 0, 480, 85);
	thirdFrameLine -> AddFrame(fSave);

	// Taking data as file Group
	fTakeOrTest = new TGButtonGroup(thirdFrameLine, "Taking Data as file");
	fTakeOrTest -> SetLayoutBroken(kTRUE);
		rbTakeOrTest[0] = new TGRadioButton(fTakeOrTest, "Save as file");
		rbTakeOrTest[0] -> MoveResize(15, 25, 100, 20);
		rbTakeOrTest[0] -> SetState(kButtonDown);
		rbTakeOrTest[1] = new TGRadioButton(fTakeOrTest, "Test only");
		rbTakeOrTest[1] -> MoveResize(15, 45, 100, 20);
	fTakeOrTest -> Connect("Clicked(Int_t)", "MultiTDC", this, "SetTakeOrTest(Int_t)");
	fTakeOrTest -> SetLayoutHints(new TGLayoutHints(kLHintsLeft, 2, 2, 2, 2));
	fTakeOrTest -> Show();
	fTakeOrTest -> MoveResize(490, 0, 130, 85);
	thirdFrameLine -> AddFrame(fTakeOrTest, new TGLayoutHints(kLHintsExpandX|kLHintsCenterY));
	AddFrame(thirdFrameLine, new TGLayoutHints(kLHintsCenterX, 10, 10, 10, 0));
// -----------------------------------------------------------------------------------------------

// - fourth frame --------------------------------------------------------------------------------
	fourthFrameLine = new TGHorizontalFrame(this, width, 55, kFixedHeight|kFixedWidth);
	fourthFrameLine -> SetLayoutBroken(kTRUE);

	// Number of events Group
	fEvents = new TGGroupFrame(fourthFrameLine, "# of events");
	fEvents -> SetLayoutBroken(kTRUE);
		lEventNumber = new TGLabel(fEvents, "0");
		lEventNumber -> SetTextJustify(kTextRight);
		lEventNumber -> MoveResize(15, 20, 55, 20);
	fEvents -> AddFrame(lEventNumber);
		lSlash = new TGLabel(fEvents, "/");
		lSlash -> MoveResize(75, 20, 4, 20);
	fEvents -> AddFrame(lSlash);
		neNumEvents = new TGNumberEntry(fEvents, 10000, 5, -1, TGNumberFormat::kNESInteger, TGNumberFormat::kNEAPositive);
		neNumEvents -> MoveResize(83, 20, 80, 20);
		neNumEvents -> Connect("ValueSet(Long_t)", "MultiTDC", this, "SetNumEvents()");
	fEvents -> AddFrame(neNumEvents);
	fEvents -> MoveResize(0, 0, 180, 55);
	fourthFrameLine -> AddFrame(fEvents);
	
	tbStart = new TGTextButton(fourthFrameLine, "Start");
	tbStart -> Connect("Clicked()", "MultiTDC", this, "StartTDC()");
	tbStart -> MoveResize(190, 0, 180, 55);
	fourthFrameLine -> AddFrame(tbStart);

	tbPlotWindow = new TGTextButton(fourthFrameLine, "Plot");
	tbPlotWindow -> Connect("Clicked()", "MultiTDC", this, "OpenPlotWindow()");
	tbPlotWindow -> MoveResize(380, 0, 80, 55);
	fourthFrameLine -> AddFrame(tbPlotWindow);

	tbStop = new TGTextButton(fourthFrameLine, "Stop");
	tbStop -> Connect("Clicked()", "MultiTDC", this, "StopTDC()");
	tbStop -> MoveResize(470, 0, 148, 55);
	fourthFrameLine -> AddFrame(tbStop);
	AddFrame(fourthFrameLine, new TGLayoutHints(kLHintsCenterX, 10, 10, 10, 10));
// -----------------------------------------------------------------------------------------------

	// Setting default values
	isTDC1On = kTRUE;
	isTDC2On = kFALSE;
	rmode = 0;
	gmode = 0;
	emode = 0;
	TDC1MaskingWindowIsClosed = kTRUE;
	TDC2MaskingWindowIsClosed = kTRUE;
	dynamicRange = 64;
	delay = 0;
	numEvents = 10000;
	stringFilenameWithPath = new TString();
	isTestMode = kFALSE;
	for (Int_t group = 0; group < 4; group++) {
		TDC1Mask[group] = 0x0000;
		TDC2Mask[group] = 0x0000;
	}

	Connect("CloseWindow()", "TApplication", gApplication, "Terminate()");
	DontCallClose();

	SetWindowName("Multi TDC Controller");
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();
}

MultiTDC::~MultiTDC()
{
	Cleanup();
}

Int_t main(int argc, char **argv)
{
	TApplication theApp("App", &argc, argv);
	new MultiTDC(gClient -> GetRoot(), 620, 500);
	theApp.Run();

	return 0;
}
