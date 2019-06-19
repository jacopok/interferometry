#include "MyGUI.h"

ClassImp(MyGUI);

MyGUI::MyGUI() : TGMainFrame(gClient->GetRoot(),10,10,kMainFrame | kVerticalFrame)
{
   SetName("GIMLI");
   SetWindowName("GIMLI");
   SetLayoutBroken(kTRUE);

   // composite frame
   TGCompositeFrame *fMainFrame1322 = new TGCompositeFrame(this,916,621,kVerticalFrame);
   fMainFrame1322->SetName("fMainFrame1322");
   fMainFrame1322->SetLayoutBroken(kTRUE);

   // embedded canvas
   TRootEmbeddedCanvas *bigCanv = new TRootEmbeddedCanvas(0,fMainFrame1322,904,432,kSunkenFrame);
   bigCanv->SetName("bigCanv");
   Int_t wbigCanv = bigCanv->GetCanvasWindowId();
   TCanvas *c123 = new TCanvas("c123", 10, 10, wbigCanv);
   bigCanv->AdoptCanvas(c123);
   fMainFrame1322->AddFrame(bigCanv, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   bigCanv->MoveResize(8,184,904,432);

   // embedded canvas
   TRootEmbeddedCanvas *smallCanv = new TRootEmbeddedCanvas(0,fMainFrame1322,280,176,kSunkenFrame);
   smallCanv->SetName("smallCanv");
   Int_t wsmallCanv = smallCanv->GetCanvasWindowId();
   TCanvas *c124 = new TCanvas("c124", 10, 10, wsmallCanv);
   smallCanv->AdoptCanvas(c124);
   fMainFrame1322->AddFrame(smallCanv, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   smallCanv->MoveResize(632,0,280,176);
   TGTextButton *gradBtn = new TGTextButton(fMainFrame1322,"Find Properties",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   gradBtn->SetTextJustify(36);
   gradBtn->SetMargins(0,0,0,0);
   gradBtn->SetWrapLength(-1);
   gradBtn->Resize(160,24);
   fMainFrame1322->AddFrame(gradBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   gradBtn->MoveResize(16,8,160,24);
   TGTextButton *analBtn = new TGTextButton(fMainFrame1322,"Scan now!",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   analBtn->SetTextJustify(36);
   analBtn->SetMargins(0,0,0,0);
   analBtn->SetWrapLength(-1);
   analBtn->Resize(160,24);
   fMainFrame1322->AddFrame(analBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   analBtn->MoveResize(16,56,160,24);
   TGTextButton *webcamBtn = new TGTextButton(fMainFrame1322,"Open cam",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   webcamBtn->SetTextJustify(36);
   webcamBtn->SetMargins(0,0,0,0);
   webcamBtn->SetWrapLength(-1);
   webcamBtn->Resize(160,24);
   fMainFrame1322->AddFrame(webcamBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   webcamBtn->MoveResize(16,82,160,24);   

   // "Motore" group frame
   TGGroupFrame *fGroupFrame686 = new TGGroupFrame(fMainFrame1322,"Motore");
   fGroupFrame686->SetLayoutBroken(kTRUE);
   TGTextButton *nextBtn = new TGTextButton(fGroupFrame686,">>",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   nextBtn->SetTextJustify(36);
   nextBtn->SetMargins(0,0,0,0);
   nextBtn->SetWrapLength(-1);
   nextBtn->Resize(64,23);
   fGroupFrame686->AddFrame(nextBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   nextBtn->MoveResize(176,24,64,23);
   TGLabel *currLbl = new TGLabel(fGroupFrame686,"fLabel699");
   currLbl->SetTextJustify(36);
   currLbl->SetMargins(0,0,0,0);
   currLbl->SetWrapLength(-1);
   fGroupFrame686->AddFrame(currLbl, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   currLbl->MoveResize(96,24,57,17);
   TGLabel *fLabel690 = new TGLabel(fGroupFrame686,"Start");
   fLabel690->SetTextJustify(36);
   fLabel690->SetMargins(0,0,0,0);
   fLabel690->SetWrapLength(-1);
   fGroupFrame686->AddFrame(fLabel690, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel690->MoveResize(16,64,57,17);
   TGLabel *fLabel691 = new TGLabel(fGroupFrame686,"Step");
   fLabel691->SetTextJustify(36);
   fLabel691->SetMargins(0,0,0,0);
   fLabel691->SetWrapLength(-1);
   fGroupFrame686->AddFrame(fLabel691, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel691->MoveResize(16,88,57,17);
   TGLabel *fLabel692 = new TGLabel(fGroupFrame686,"End");
   fLabel692->SetTextJustify(36);
   fLabel692->SetMargins(0,0,0,0);
   fLabel692->SetWrapLength(-1);
   fGroupFrame686->AddFrame(fLabel692, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel692->MoveResize(16,112,57,17);
   TGNumberEntry *stepNE = new TGNumberEntry(fGroupFrame686, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fGroupFrame686->AddFrame(stepNE, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   stepNE->MoveResize(104,88,59,21);
   TGNumberEntry *endNE = new TGNumberEntry(fGroupFrame686, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fGroupFrame686->AddFrame(endNE, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   endNE->MoveResize(104,112,59,21);
   TGLabel *fLabel844 = new TGLabel(fGroupFrame686,"Go to");
   fLabel844->SetTextJustify(36);
   fLabel844->SetMargins(0,0,0,0);
   fLabel844->SetWrapLength(-1);
   fGroupFrame686->AddFrame(fLabel844, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel844->MoveResize(16,136,57,17);
   TGNumberEntry *gotoNE = new TGNumberEntry(fGroupFrame686, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fGroupFrame686->AddFrame(gotoNE, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   gotoNE->MoveResize(104,136,59,21);
   TGNumberEntry *startNE = new TGNumberEntry(fGroupFrame686, (Double_t) 0,6,-1,(TGNumberFormat::EStyle) 5);
   fGroupFrame686->AddFrame(startNE, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   startNE->MoveResize(104,64,59,21);
   TGTextButton *gotoBtn = new TGTextButton(fGroupFrame686,">>",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   gotoBtn->SetTextJustify(36);
   gotoBtn->SetMargins(0,0,0,0);
   gotoBtn->SetWrapLength(-1);
   gotoBtn->Resize(47,23);
   fGroupFrame686->AddFrame(gotoBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   gotoBtn->MoveResize(176,136,47,23);
   TGTextButton *prevBtn = new TGTextButton(fGroupFrame686,"<<",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   prevBtn->SetTextJustify(36);
   prevBtn->SetMargins(0,0,0,0);
   prevBtn->SetWrapLength(-1);
   prevBtn->Resize(64,24);
   fGroupFrame686->AddFrame(prevBtn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   prevBtn->MoveResize(16,24,64,24);
   TGTextButton *setZero = new TGTextButton(fGroupFrame686,"Zero",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   setZero->SetTextJustify(36);
   setZero->SetMargins(0,0,0,0);
   setZero->SetWrapLength(-1);
   setZero->Resize(47,23);
   fGroupFrame686->AddFrame(setZero, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   setZero->MoveResize(192,48,47,23);

   fGroupFrame686->SetLayoutManager(new TGVerticalLayout(fGroupFrame686));
   fGroupFrame686->Resize(256,176);
   fMainFrame1322->AddFrame(fGroupFrame686, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fGroupFrame686->MoveResize(232,8,256,176);
   TGTextButton *plot1Btn = new TGTextButton(fMainFrame1322,"Plot(1) >",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   plot1Btn->SetTextJustify(36);
   plot1Btn->SetMargins(0,0,0,0);
   plot1Btn->SetWrapLength(-1);
   plot1Btn->Resize(64,35);
   fMainFrame1322->AddFrame(plot1Btn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   plot1Btn->MoveResize(544,56,64,35);
   TGTextButton *plot2Btn = new TGTextButton(fMainFrame1322,"Plot(2) >",-1,TGTextButton::GetDefaultGC()(),TGTextButton::GetDefaultFontStruct(),kRaisedFrame);
   plot2Btn->SetTextJustify(36);
   plot2Btn->SetMargins(0,0,0,0);
   plot2Btn->SetWrapLength(-1);
   plot2Btn->Resize(64,35);
   fMainFrame1322->AddFrame(plot2Btn, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   plot2Btn->MoveResize(544,93,64,35);
   TGLabel *gradLab = new TGLabel(fMainFrame1322,"(...)");
   gradLab->SetTextJustify(36);
   gradLab->SetMargins(0,0,0,0);
   gradLab->SetWrapLength(-1);
   fMainFrame1322->AddFrame(gradLab, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   gradLab->MoveResize(16,38,160,17);

   TGFont *ufont;         // will reflect user font changes
   ufont = gClient->GetFont("-*-helvetica-medium-r-*-*-12-*-*-*-*-*-iso8859-1");

   TGGC   *uGC;           // will reflect user GC changes
   // graphics context changes
   GCValues_t valntry;
   valntry.fMask = kGCForeground | kGCBackground | kGCFillStyle | kGCFont | kGCGraphicsExposures;
   gClient->GetColorByName("#000000",valntry.fForeground);
   gClient->GetColorByName("#e8e8e8",valntry.fBackground);
   valntry.fFillStyle = kFillSolid;
   valntry.fFont = ufont->GetFontHandle();
   valntry.fGraphicsExposures = kFALSE;
   uGC = gClient->GetGC(&valntry, kTRUE);
   TGTextEntry *foldEntry = new TGTextEntry(fMainFrame1322, new TGTextBuffer(14),-1,uGC->GetGC(),ufont->GetFontStruct(),kSunkenFrame | kOwnBackground);
   foldEntry->SetMaxLength(4096);
   foldEntry->SetAlignment(kTextLeft);
   foldEntry->SetText("");
   foldEntry->Resize(160,foldEntry->GetDefaultHeight());
   fMainFrame1322->AddFrame(foldEntry, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   foldEntry->MoveResize(16,152,160,21);
   TGLabel *fLabel1607 = new TGLabel(fMainFrame1322,"Folder:");
   fLabel1607->SetTextJustify(36);
   fLabel1607->SetMargins(0,0,0,0);
   fLabel1607->SetWrapLength(-1);
   fMainFrame1322->AddFrame(fLabel1607, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   fLabel1607->MoveResize(16,128,160,17);
   TGCheckButton *savCheck = new TGCheckButton(fMainFrame1322,"Massive data saving");
   savCheck->SetTextJustify(36);
   savCheck->SetMargins(0,0,0,0);
   savCheck->SetWrapLength(-1);
   fMainFrame1322->AddFrame(savCheck, new TGLayoutHints(kLHintsLeft | kLHintsTop,2,2,2,2));
   savCheck->MoveResize(16,106,160,18);

   AddFrame(fMainFrame1322, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY));
   fMainFrame1322->MoveResize(0,0,916,621);
   SetMWMHints(kMWMDecorAll,kMWMFuncAll,kMWMInputModeless);
   MapSubwindows();
   Resize(GetDefaultSize());
   MapWindow();
   Resize(916,621);


	this->bigCanv = bigCanv;   
	this->smallCanv = smallCanv;

	gradBtn->Connect("Clicked()","MyGUI",this,"click(=\"info\")");
	analBtn->Connect("Clicked()","MyGUI",this,"click(=\"anal\")");
	nextBtn->Connect("Clicked()","MyGUI",this,"click(=\"next\")");
	prevBtn->Connect("Clicked()","MyGUI",this,"click(=\"prev\")");
	setZero->Connect("Clicked()","MyGUI",this,"click(=\"zero\")");
	gotoBtn->Connect("Clicked()","MyGUI",this,"click(=\"goto\")");
	plot1Btn->Connect("Clicked()","MyGUI",this,"click(=\"plot1\")");
	plot2Btn->Connect("Clicked()","MyGUI",this,"click(=\"plot2\")");
	webcamBtn->Connect("Clicked()","MyGUI",this,"click(=\"webcam\")");

	this->foldEntry = foldEntry;

	this->savCheck = savCheck;

	this->gotoNE = gotoNE;
	this->startNE = startNE;
	this->stepNE = stepNE;
	this->endNE = endNE;

	this->currLbl = currLbl;
	this->gradLab = gradLab;

	savCheck->SetState(kButtonDown);
}

void MyGUI::attach(Observer* obs) { this->obs = obs; }

void MyGUI::goToCanv(int canv) {
	if(canv==1) bigCanv->GetCanvas()->cd();
	else if(canv==0) smallCanv->GetCanvas()->cd();
}

void MyGUI::canvUpdate() {
	bigCanv->GetCanvas()->Update();
	smallCanv->GetCanvas()->Update();
	bigCanv->GetCanvas()->Draw();
	smallCanv->GetCanvas()->Draw();
	gSystem->ProcessEvents();
}

void MyGUI::setGradInfo(const char* c) {
	gradLab->SetText(c);
}

void MyGUI::setStepInfo(const char* c) {
	currLbl->SetText(c);
}
int MyGUI::getStart() {
	return startNE->GetNumber();
}
int MyGUI::getStep() {
	return stepNE->GetNumber();
}
int MyGUI::getEnd() {
	return endNE->GetNumber();
}
int MyGUI::getGoTo() {
	return gotoNE->GetNumber();
}
void MyGUI::setFolder(string c) {
	foldEntry->SetText(c.c_str());
}
const char* MyGUI::getFolder() {
	const char* c = foldEntry->GetText();
	if(strlen(c)==0) return "no_name";
	return c;
}
bool MyGUI::massiveSaving() {
	return savCheck->IsOn();
}
void MyGUI::CloseWindow() {
	click("close");
	cout<<"\n\n*****************************\n*                           *\n* See you later! Au revoir! *\n*                           *\n*              Gimli        *\n*                           *\n*****************************\n";
	gApplication->Terminate(0);
}
