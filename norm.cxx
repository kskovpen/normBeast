// NORM BEAST class implementation
#include "norm.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

ClassImp(NORM::beast)

NORM::beast::beast(std::string fin)
{
   gErrorIgnoreLevel = 2000;
   
   _fcom.open(fin.c_str());
   
   if( ! _fcom.is_open() )
     {
	std::cout << "Input file does not exist: " << fin << std::endl;
	exit(1);
     }   
   
   std::string line;
   while( getline(_fcom,line) )
     {
	std::vector<std::string> spl;
	split(spl,line,boost::algorithm::is_any_of(" "));

	if( strcmp(&spl[0][0],"#") == 0 ) continue;	
	else if( strcmp(spl[0].c_str(),"[FILE]") == 0 )
	  {
	     fileInputMC = TFile::Open(spl[1].c_str());
	     if( ! fileInputMC->IsOpen() )
	       {
		  std::cout << "Can not find MC input file: " << spl[1].c_str() << std::endl;
		  exit(1);
	       }	     
	     fileInputData = TFile::Open(spl[2].c_str());
	     if( ! fileInputData->IsOpen() )
	       {
		  std::cout << "Can not find Data input file: " << spl[2].c_str() << std::endl;
		  exit(1);
	       }	     
	  }
	else if( strcmp(spl[0].c_str(),"[TREE]") == 0 )
	  {
	     trInputMC = (TTree*)fileInputMC->Get(spl[1].c_str());
	     trInputData = (TTree*)fileInputData->Get(spl[2].c_str());
	  }
	else if( strcmp(spl[0].c_str(),"[BKG]") == 0 )
	  {
	     nBkg = spl.size()-1;
	     for(int i=1;i<nBkg+1;i++)
	       {		  
		  nameBkg[i-1] = spl[i];
	       }
	  }
	else if( strcmp(spl[0].c_str(),"[VAR]") == 0 )
	  {
	     nVar = spl.size()-1;
	     for(int i=1;i<nVar+1;i++)
	       {		  
		  nameVar[i-1] = spl[i];
	       }
	  }
	else if( strcmp(spl[0].c_str(),"[TYPE]") == 0 )
	  {
	     nVar = spl.size()-1;
	     for(int i=1;i<nVar+1;i++)
	       {		  
		  nameVarType[i-1] = spl[i];
	       }
	  }
	else if( strcmp(spl[0].c_str(),"[HNB]") == 0 )
	  {
	     nVar = spl.size()-1;
	     for(int i=1;i<nVar+1;i++)
	       {		  
		  histNb[i-1] = atoi(spl[i].c_str());
	       }
	  }
	else if( strcmp(spl[0].c_str(),"[HMIN]") == 0 )
	  {
	     nVar = spl.size()-1;
	     for(int i=1;i<nVar+1;i++)
	       {		  
		  histMin[i-1] = atof(spl[i].c_str());
	       }
	  }
	else if( strcmp(spl[0].c_str(),"[HMAX]") == 0 )
	  {
	     nVar = spl.size()-1;
	     for(int i=1;i<nVar+1;i++)
	       {		  
		  histMax[i-1] = atof(spl[i].c_str());
	       }
	  }	
	else if( strcmp(spl[0].c_str(),"[WEIGHT]") == 0 )
	  {
	     weightName = spl[1];
	  }
	else if( strcmp(spl[0].c_str(),"[NITER]") == 0 )
	  {
	     nIter = atoi(spl[1].c_str());
	  }
	else if( strcmp(spl[0].c_str(),"[CHAN]") == 0 )
	  {
	     iChan = atoi(spl[1].c_str());
	  }
     }

   chInputBkg = new TChain("trSel");
   for(int i=0;i<nBkg;i++)
     {
	chInputBkg->Add(nameBkg[i].c_str());
     }   
   nevBkg = chInputBkg->GetEntries();
   
   trInputMC->SetBranchAddress(weightName.c_str(),&weight);
   trInputData->SetBranchAddress(weightName.c_str(),&weight);
   chInputBkg->SetBranchAddress(weightName.c_str(),&weight);

   trInputMC->SetBranchAddress("chan",&chan);
   trInputData->SetBranchAddress("chan",&chan);
   chInputBkg->SetBranchAddress("chan",&chan);
   
   nevMC = trInputMC->GetEntries();
   nevData = trInputData->GetEntries();
   
   int idxF = 0;
   for(int iv=0;iv<nVar;iv++)
     {
	sfPrev[iv] = 1.;
	
	if( nameVarType[iv] == "F" ) 
	  {
	     trInputMC->SetBranchAddress(nameVar[iv].c_str(),&varF[idxF]);
	     trInputData->SetBranchAddress(nameVar[iv].c_str(),&varF[idxF]);
	     chInputBkg->SetBranchAddress(nameVar[iv].c_str(),&varF[idxF]);
	     varNameF[idxF] = nameVar[iv];
	     idxF++;
	  }
	
	std::string histNameMC = nameVar[iv] + "MC";
	histMC[iv] = new TH1D(histNameMC.c_str(),histNameMC.c_str(),histNb[iv],histMin[iv],histMax[iv]);
	histMC[iv]->Sumw2();
	std::string histNameData = nameVar[iv] + "Data";
	histData[iv] = new TH1D(histNameData.c_str(),histNameData.c_str(),histNb[iv],histMin[iv],histMax[iv]);
	histData[iv]->Sumw2();
	std::string histNameBkg = nameVar[iv] + "Bkg";
	histBkg[iv] = new TH1D(histNameBkg.c_str(),histNameBkg.c_str(),histNb[iv],histMin[iv],histMax[iv]);
	histBkg[iv]->Sumw2();
     }

   for(int iv=0;iv<nVar;iv++)
     {   
	for(int i=0;i<nevMC;i++)
	  {
	     trInputMC->GetEntry(i);
	     if( iChan != chan || iChan == -1 ) continue;
	     histMC[iv]->Fill(varF[iv],weight);
	  }
	addbin(histMC[iv]);

	nbins[iv] = histMC[iv]->GetXaxis()->GetNbins();
	for(int ib=1;ib<=nbins[iv];ib++)
	  {	     
	     xmin[iv][ib-1] = histMC[iv]->GetXaxis()->GetBinLowEdge(ib);
	     xmax[iv][ib-1] = histMC[iv]->GetXaxis()->GetBinUpEdge(ib);
	  }	
	
	for(int i=0;i<nevData;i++)
	  {
	     trInputData->GetEntry(i);
	     if( iChan != chan || iChan == -1 ) continue;
	     histData[iv]->Fill(varF[iv],weight);
	  }
	addbin(histData[iv]);

	for(int i=0;i<nevBkg;i++)
	  {
	     chInputBkg->GetEntry(i);
	     if( iChan != chan || iChan == -1 ) continue;
	     histBkg[iv]->Fill(varF[iv],weight);
	  }
	addbin(histBkg[iv]);

	histData[iv]->Add(histBkg[iv],-1.);
     }
   
   double intData = histData[0]->Integral();
   double intMC = histMC[0]->Integral();
   sfOverall = intData/intMC;
}

// Destructor
NORM::beast::~beast()
{
   
}

void NORM::beast::run()
{
   setStyle();
   
   for(int i=0;i<nIter;i++)
     {	
	normalise(i);
     }

   drawEvol();
   writeSF();
}

double NORM::beast::getSF(int ivar,double var,int Iter)
{
   double sfOut = 1.;
   for(int ib=1;ib<=nbins[ivar];ib++)
     {
	if( var >= xmin[ivar][ib-1] && var < xmax[ivar][ib-1] )
	  {
	     sfOut = sf[Iter][ivar][ib-1];
	     break;
	  }	
	else if( var >= xmax[ivar][nbins[ivar]-1] )
	  {
	     sfOut = sf[Iter][ivar][nbins[ivar]-1];
	     break;
	  }	
     }   
   return sfOut;
}

void NORM::beast::draw(TH1D* hMC,TH1D* hData,int iVar,int Flag)
{
   if( Flag == 0 || Flag == 1 )
     {	
	TCanvas *c1 = new TCanvas();
	gStyle->SetOptStat(0);
	c1->Clear();
	c1->SetLogy(0);
	
	hMC->SetFillStyle(0);
	hData->SetFillStyle(0);
	hMC->SetLineColor(kBlue);
	hData->SetLineStyle(9);
	hMC->SetLineWidth(2);
	hData->SetLineColor(kRed);
	hData->SetLineStyle(1);
	hData->SetLineWidth(2);
	hMC->Draw("e1 hist");
	hData->Draw("e1 hist same");
	
	hMC->GetXaxis()->SetTitle("Variable");
	hMC->GetYaxis()->SetTitle("Number of events");
	hMC->SetTitle(nameVar[iVar].c_str());
	
	TLegend *legf = new TLegend(0.70,0.85,0.85,0.68);
	legf->SetFillColor(253);
	legf->SetBorderSize(0);
	legf->AddEntry(hMC,"MC","l");
	legf->AddEntry(hData,"Data","l");
	legf->Draw();
	delete legf;

	std::string pref = "initial";
	if( Flag == 1 ) pref = "final"; 
	std::string pname = "result_"+pref+".eps";
	if( nVar > 0 ) pname = "result_"+pref+".eps(";
	if( iVar == nVar-1 ) pname = "result_"+pref+".eps)";
	c1->Print(pname.c_str());
	c1->Clear();
	
	if( Flag == 1 )
	  {	     
	     TGraph *gr = new TGraph(nIter,grX[iVar],grY[iVar]);
	     gr->Draw("APC");
	     std::string pnameEvol = "evolution.eps";
	     c1->Print(pnameEvol.c_str());
	     c1->Clear();
	  }	
	
	delete c1;
     }   
}

void NORM::beast::drawEvol()
{
   TCanvas *c1 = new TCanvas();
   gStyle->SetOptStat(0);
   c1->Clear();
   c1->SetLogy(0);

   TLegend *legf = new TLegend(0.60,0.85,0.85,0.60);
   legf->SetFillColor(253);
   legf->SetBorderSize(0);
   
   double grMin = 10E+10;
   double grMax = -10E+10;
   TGraph *gr[1000];
   for(int iv=0;iv<nVar;iv++)
     {	
	gr[iv] = new TGraph(nIter,grX[iv],grY[iv]);
	if( iv == 0 ) gr[iv]->Draw("APC");
	else gr[iv]->Draw("PCS");
	legf->AddEntry(gr[iv],varNameF[iv].c_str(),"l");
	double grMinCur = gr[iv]->GetHistogram()->GetMinimum();
	double grMaxCur = gr[iv]->GetHistogram()->GetMaximum();
	if( grMinCur < grMin ) grMin = grMinCur;
	if( grMaxCur > grMax ) grMax = grMaxCur;
	gr[iv]->SetMarkerSize(0);
	gr[iv]->SetLineColor(2+iv);
     }

   gr[0]->GetHistogram()->GetXaxis()->SetTitle("Iteration");
   gr[0]->GetHistogram()->GetYaxis()->SetTitle("#frac{Data-MC}{MC}");

   gr[0]->GetHistogram()->SetMinimum(grMin);
   gr[0]->GetHistogram()->SetMaximum(grMax);
   gr[0]->GetXaxis()->SetLimits(0,nIter);

   c1->Update();
   
   legf->Draw();
   
   std::string pnameEvol = "evolution.eps";
   c1->Print(pnameEvol.c_str());
   c1->Clear();

   delete legf;
   
   for(int iv=0;iv<nVar;iv++)
     {
	delete gr[iv];
     }
   
   delete c1;
}

void NORM::beast::normalise(int Iter)
{
   // measure SF
   for(int iv=0;iv<nVar;iv++)
     {
	TH1D *histDatas = (TH1D*)histData[iv]->Clone("");
	
	double normMC = histMC[iv]->Integral();
	double normData = histDatas->Integral();
	double fc = normData/normMC;
	histMC[iv]->Scale(1./normMC);
	histDatas->Scale(1./normData);
	if( Iter == 0 ) draw(histMC[iv],histDatas,iv,0);

	for(int ib=1;ib<=nbins[iv];ib++)
	  {	     
	     double vMC = histMC[iv]->GetBinContent(ib);
	     double vData = histDatas->GetBinContent(ib);
	     double fs = vData/vMC;
	     if( iv == 0 ) fs *= fc;
	     sf[Iter][iv][ib-1] = (vMC > 0.) ? fs : 0.;
//	     std::cout << Iter << " " << sf[Iter][iv][ib-1] << std::endl;
	  }
	
	delete histDatas;
     }   
   
   for(int iv=0;iv<nVar;iv++)
     {	
	histMC[iv]->Reset();
     }

   // apply SF
   for(int i=0;i<nevMC;i++)
     {
	trInputMC->GetEntry(i);
	if( iChan != chan || iChan == -1 ) continue;
	
	double sfComb = 1.;
	for(int iv=0;iv<nVar;iv++)
	  {
	     for(int it=0;it<=Iter;it++)
	       {		  
		  double sfF = getSF(iv,varF[iv],it);
		  sfComb *= sfF;
	       }	     
	  }	
	
	for(int iv=0;iv<nVar;iv++)
	  {
	     double w = weight*sfComb;
	     histMC[iv]->Fill(varF[iv],w);
	  }
     }

   // check result
   for(int iv=0;iv<nVar;iv++)
     {
	addbin(histMC[iv]);
	TH1D *histMCs = (TH1D*)histMC[iv]->Clone("");
	TH1D *histDatas = (TH1D*)histData[iv]->Clone("");
	histDatas->Scale(1./histDatas->Integral());
	double mcInt = histMCs->Integral();
	histMCs->Scale(1./mcInt);
	double delSum = 0.;
	for(int ib=1;ib<=nbins[iv];ib++)
	  {
	     double mc = histMCs->GetBinContent(ib);
	     double data = histDatas->GetBinContent(ib);
	     double del = fabs(data-mc);
	     delSum += del;
	  }	
	fac[iv] = delSum;
	draw(histMCs,histDatas,iv,1);
	std::cout << iv << " " << fac[iv] << std::endl;
	grX[iv][Iter] = Iter;
	grY[iv][Iter] = fac[iv];
	delete histMCs;
	delete histDatas;
     }
}

void NORM::beast::addbin(TH1D *h)
{   
   Int_t x_nbins = h->GetXaxis()->GetNbins();
   h->SetBinContent(1,h->GetBinContent(0)+h->GetBinContent(1));
   h->SetBinError(1,TMath::Sqrt(pow(h->GetBinError(0),2)+pow(h->GetBinError(1),2)));
   h->SetBinContent(x_nbins,h->GetBinContent(x_nbins)+h->GetBinContent(x_nbins+1));
   h->SetBinError(x_nbins,TMath::Sqrt(pow(h->GetBinError(x_nbins),2)+
				      pow(h->GetBinError(x_nbins+1),2)));

   h->SetBinContent(0,0.);
   h->SetBinError(0,0.);
   h->SetBinContent(x_nbins+1,0.);
   h->SetBinError(x_nbins+1,0.);
}

void NORM::beast::writeSF()
{
   TFile *fout = new TFile("output.root","RECREATE");
   
   for(int iv=0;iv<nVar;iv++)
     {
	std::string hname = "sf_"+varNameF[iv];
	TH1D *h = (TH1D*)histMC[iv]->Clone(hname.c_str());
	h->Reset();
	for(int ib=1;ib<=nbins[iv];ib++)
	  {
	     double sfV = 1.;
	     for(int it=0;it<nIter;it++)
	       {
		  sfV *= sf[it][iv][ib-1];
	       }	     
	     h->SetBinContent(ib,sfV);
	  }       
     }
   
   TH1D *hOverall = new TH1D("sfOverall","sfOverall",1,0.,1.);
   hOverall->SetBinContent(1,sfOverall);
   
   fout->Write();
   fout->Close();
}

void NORM::beast::setStyle()
{
  // use plain black on white colors
  Int_t icol=0; // WHITE
  gStyle->SetFrameBorderMode(icol);
  gStyle->SetFrameFillColor(icol);
  gStyle->SetCanvasBorderMode(icol);
  gStyle->SetCanvasColor(icol);
  gStyle->SetPadBorderMode(icol);
  gStyle->SetPadColor(icol);
  gStyle->SetStatColor(icol);

  // set the paper & margin sizes
  gStyle->SetPaperSize(20,26);

  // set margin sizes
  gStyle->SetPadTopMargin(0.05);
  gStyle->SetPadRightMargin(0.05);
  gStyle->SetPadBottomMargin(0.16);
  gStyle->SetPadLeftMargin(0.16);

  // set title offsets (for axis label)
  gStyle->SetTitleXOffset(1.4);
  gStyle->SetTitleYOffset(1.4);

  // use large fonts
  //Int_t font=72; // Helvetica italics
  Int_t font=42; // Helvetica
  Double_t tsize=0.05;
  gStyle->SetTextFont(font);

  gStyle->SetTextSize(tsize);
  gStyle->SetLabelFont(font,"x");
  gStyle->SetTitleFont(font,"x");
  gStyle->SetLabelFont(font,"y");
  gStyle->SetTitleFont(font,"y");
  gStyle->SetLabelFont(font,"z");
  gStyle->SetTitleFont(font,"z");
  
  gStyle->SetLabelSize(tsize,"x");
  gStyle->SetTitleSize(tsize,"x");
  gStyle->SetLabelSize(tsize,"y");
  gStyle->SetTitleSize(tsize,"y");
  gStyle->SetLabelSize(tsize,"z");
  gStyle->SetTitleSize(tsize,"z");

  // use bold lines and markers
  gStyle->SetMarkerStyle(20);
  gStyle->SetMarkerSize(1.2);
  gStyle->SetHistLineWidth(2.);
  gStyle->SetLineStyleString(2,"[12 12]"); // postscript dashes

  // get rid of X error bars 
  //gStyle->SetErrorX(0.001);
  // get rid of error bar caps
  gStyle->SetEndErrorSize(0.);

  // do not display any of the standard histogram decorations
  gStyle->SetOptTitle(0);
  //gStyle->SetOptStat(1111);
  gStyle->SetOptStat(0);
  //gStyle->SetOptFit(1111);
  gStyle->SetOptFit(0);

  // put tick marks on top and RHS of plots
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);
}
