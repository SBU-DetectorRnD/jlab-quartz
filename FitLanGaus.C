#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;
Double_t langaufun(Double_t *x, Double_t *par);

void FitLanGaus()
{
	gDirectory->DeleteAll();
	int ij;
	double charge;
	double mean;
	double sigma=-10;
	double ampli=-10.;
	double area = -100;
	double fr[2]={-10};
	
	TH1D *pe = new TH1D("pe","pe",200,-0.5,199.5);
	TString infile="dummy.root";
	
	TFile* fh_file = new TFile(infile, "read");
	
	TTree *Tin = (TTree*)fh_file->Get("T");
	
	T->SetBranchAddress("ChargeCor",&charge);
	
	long entries = T->GetEntries();
    cout <<"entries in  "<< infile <<"file  "<<entries-1<<endl;
    //fh_file->cd();
    for(ij=0;ij<entries;ij++)
    //for(ij=0;ij<50;ij++)
    {
		
		T->GetEntry(ij);
		if(charge>20)
		{
			//cout<<"ij  "<<ij<<"  charge  "<<charge<<endl;
			pe->Fill(charge);
		}
	}
	
	//***** Generate Convoluted Landau*Gauss Distribution Fit *****//
	TF1 *langaus = new TF1("langaus",langaufun,20,150,4);
	langaus->SetParNames("lwidth","mpv","integral","gsigma");
	
	mean=pe->GetMean();
	area=pe->Integral(40,150);
	sigma=pe->GetRMS();
	fr[0]=0.5*mean;
	fr[1]=2*mean;
	
	cout<<"mean "<<mean<<"  area  "<<area<<"  sigma  "<<sigma<<endl;
	
	langaus->SetParameters(2.,mean,area,6);
	
	
	pe->Fit("langaus","R0","",fr[0],fr[1]);
	gStyle->SetOptStat(1111);
    gStyle->SetOptFit(111);
	TCanvas *plot = new TCanvas("plot","plot",800,600);
	pe->Draw();
	langaus->Draw("same");
}

Double_t langaufun(Double_t *x, Double_t *par) {

  //Fit parameters:
  //par[0]=Width (scale) parameter of Landau density
  //par[1]=Most Probable (MP, location) parameter of Landau density
  //par[2]=Total area (integral -inf to inf, normalization constant)
  //par[3]=Width (sigma) of convoluted Gaussian function
  //
  //In the Landau distribution (represented by the CERNLIB approximation),
  //the maximum is located at x=-0.22278298 with the location parameter=0.
  //This shift is corrected within this function, so that the actual
  //maximum is identical to the MP parameter.

  // Numeric constants
  Double_t invsq2pi = 0.3989422804014;   // (2 pi)^(-1/2)
  Double_t mpshift  = -0.22278298;       // Landau maximum location

  // Control constants
  Double_t np = 100.0;      // number of convolution steps
  Double_t sc =   5.0;      // convolution extends to +-sc Gaussian sigmas

  // Variables
  Double_t xx;
  Double_t mpc;
  Double_t fland;
  Double_t sum = 0.0;
  Double_t xlow,xupp;
  Double_t step;
  Double_t i;


  // MP shift correction
  mpc = par[1] - mpshift * par[0];

  // Range of convolution integral
  xlow = x[0] - sc * par[3];
  xupp = x[0] + sc * par[3];

  step = (xupp-xlow) / np;

  // Convolution integral of Landau and Gaussian by sum
  for(i=1.0; i<=np/2; i++) {
    xx = xlow + (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0],xx,par[3]);

    xx = xupp - (i-.5) * step;
    fland = TMath::Landau(xx,mpc,par[0]) / par[0];
    sum += fland * TMath::Gaus(x[0],xx,par[3]);
  }

  return (par[2] * step * sum * invsq2pi / par[3]);
}
