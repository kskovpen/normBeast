// NORM class definition and includes
#ifndef NORM_H
#define NORM_H

#include "TROOT.h"
#include "THStack.h"
#include "Riostream.h"
#include "TMath.h"
#include "TGraphAsymmErrors.h"
#include "TMinuit.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TLine.h"
#include "TH1D.h"
#include "TMatrixD.h"
#include "TMatrixDSym.h"
#include "TDecompLU.h"
#include "TDecompQRH.h"
#include "TDecompChol.h"
#include "TDecompBK.h"

#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <cmath>

namespace NORM
{
   class beast
     {
	
      public:
	beast(std::string fin);
	virtual ~beast();

	// user methods
	void run();
	
      protected:

	double getSF(int ivar,double var,int Iter);
	void draw(TH1D *hMC,TH1D *hData,int iVar,int Flag);
	void drawEvol();
	void normalise(int Iter);
	void addbin(TH1D *h);
	void writeSF();
	void setStyle();
	
	int nVar;
	int nBkg;
	int chan;
	int iChan;
	double sfOverall;
	std::string nameVar[1000];
	std::string nameVarType[1000];
	std::string nameBkg[100];
	std::ifstream _fcom;
	TFile *fileInputMC;
	TTree *trInputMC;
	TFile *fileInputData;
	TTree *trInputData;
	TChain *chInputBkg;
	float varF[1000];
	std::string varNameF[1000];
	int histNb[1000];
	double histMin[1000];
	double histMax[1000];
	TH1D *histMC[1000];
	TH1D *histData[1000];
	TH1D *histBkg[1000];
	std::string weightName;
	float weight;
	double sf[1000][1000][1000];
	double sfPrev[1000];
	double xmin[1000][1000];
	double xmax[1000][1000];
	int nevMC;
	int nevData;
	int nevBkg;
	double fac[1000];
	double sfGlob;
	int nbins[1000];
	int nIter;
	double grX[1000][10000];
	double grY[1000][10000];
	
      ClassDef(NORM::beast,1)
     };
}

#endif
