// Marisa Petrusky 06/15/18 Merges user defined number of ROOT files and runs ADC code

// How to Use: Create an int array in ROOT with the numbers of the files you wish to merge
// E.g. int a[3] = {1071,1073,1078} for files dummy1071.root, dummy1073.root, and dummy1078.root
// Type .x QuartzDataMerge.X(no files, array name) 
// E.g. .x QuartzDataMerge.X(3,a)

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TCanvas.h"
#include <TChain.h>
#include <string>
#include <sstream>

using namespace std; 

Double_t langaufun(Double_t *x, Double_t *par);

int QuartzDataMerge(int argc, int argv[]) 
{   
	gDirectory->DeleteAll();
	int ij;
	double charge,chPlas1,chPlas2;//peledtal corrected charge
	double ped,pedPlas1,pedPlas2;//normalized pedestal
	double rawCh,rawChPlas1,rawChPlas2;//raw adc charge
	double mean;
	double sigma=-10;
	double ampli=-10.;
	double area = -100;
	double fr[2]={-10};
	
	//***** Constants (gain subject to change via given PMT) *****//
    double R    = 50.;       // in Ohms
    double q    = 1.602e-19; // in eV (electron charge)
	double gain = 7.765e5;    // PMT gain value (in Coulombs)
	
	TH1D *peRaw = new TH1D("peRaw","peRaw",200,-0.5,199.5);//photo electron - raw
	TH1D *pePed = new TH1D("pePed","pePed",200,-0.5,199.5);//photo electron - pedestal
	TH1D *peCorr = new TH1D("peCorr","peCorr",200,-0.5,199.5);//photo electron - pedestal corrected raw
	
	TH1F *charge1 = new TH1F("charge1","charge1",100,-0.5,1499.5);//raw integral
	TH1F *ChargeCor = new TH1F("ChargeCor","ChargeCor",100,-0.5,1499.5);//pedestal corrected charge
	TH1F *peds = new TH1F("peds","peds",100,-0.5,1499.5);//pedestal
  
	TH1F *chargePlas1 = new TH1F("Plastic1","Plastic1",3000,-0.5,2999.5);//pedestal corrected charge on plastic1  
	TH1F *chargePlas2 = new TH1F("Plastic2","Plastic2",3000,-0.5,2999.5);//pedestal corrected charge on plastic2

	//2D histogram of plastic charge
	TH2F *TopVsBottom = new TH2F("TopVsBottom","TopVsBottom", 100,-0.5,2999.5,100,-0.5,2999.5);
	
	//histogram of pe on quartz with gate on plastic charge
	TH1D *peGated = new TH1D("peGated","peGated",200,-0.5,199.5);
	
	TChain ch("T");
			
	for (int i=0;i<argc;i++)
	{	
		TString infile;

		infile=Form("dummy%d.root",argv[i]);
				
		TFile* outfile = new TFile(infile, "read");
				
		ch.Add(infile);
	}
	
	TString title;
	int m;
	
	cout << "What number would you like assigned to this file? The file name will be in the format dummy#.root." << endl;
		cin >> m;
		
		title=Form("dummy%d.root",m);
		
		ch.Merge(title);
		
		TFile* ultimatefile = new TFile(title, "read");
		
		TTree *Tin = (TTree*)ultimatefile->Get("T");

		// For Quartz
		T->SetBranchAddress("ChargeCor",&charge);
		T->SetBranchAddress("inte",&rawCh);
		T->SetBranchAddress("ped",&ped);
	
		//For top plastic
		T->SetBranchAddress("chPlas1",&chPlas1);
		T->SetBranchAddress("pedPlas1",&pedPlas1);
		T->SetBranchAddress("intePlastic1",&rawChPlas1);

		//For bottom plastic
		T->SetBranchAddress("chPlas2",&chPlas2);
		T->SetBranchAddress("pedPlas2",&pedPlas2);
		T->SetBranchAddress("intePlastic2",&rawChPlas2);
		
		long entries = T->GetEntries();
		
		cout <<"entries in  "<< infile <<"file  "<<entries-1<<endl;
    //fh_file->cd();
    for(ij=0;ij<entries;ij++)
    //for(ij=0;ij<50;ij++)
    {
		T->GetEntry(ij);
//		if(charge>20)
		{
			//ADC values
			charge1->Fill(rawCh);
			peds->Fill(ped);
			ChargeCor->Fill(charge);
			
			//ADC values of plasctics
			chargePlas1->Fill(chPlas1);
			chargePlas2->Fill(chPlas2);
			TopVsBottom->Fill(chPlas1,chPlas2);
			
			//PE calculation
			rawCh=rawCh*1.e-9*1.e-3/(R*q*gain);
			ped=ped*1.e-9*1.e-3/(R*q*gain);			
			charge=charge*1.e-9*1.e-3/(R*q*gain);
			
			//Fill PE of quartz
			peRaw->Fill(rawCh);
			pePed->Fill(ped);
			if(charge>10)
			{
				peCorr->Fill(charge);
				if((chPlas1>60 && chPlas1<450) && (chPlas2>60 && chPlas1<400))
				peGated->Fill(charge);
			}
			
			//cout<<"ij  "<<ij<<"  charge  "<<charge<<endl;
			//pe->Fill(charge);
		}
	}
	
	//rawADCs
	TCanvas *plot =new TCanvas("plot","plot",800,600);
	plot->Divide(1,3);
	plot->cd(1);
	charge1->Draw();	

	plot->cd(2);
	peds->Draw();
	
	plot->cd(3);
	ChargeCor->Draw();
	
	//PEl
	TCanvas *plot1 =new TCanvas("plot1","plot1",800,600);
	plot1->Divide(1,3);
	plot1->cd(1);
	peRaw->Draw();	

	plot1->cd(2);
	pePed->Draw();
	
	plot1->cd(3);
	peCorr->Draw();
	peGated->SetLineColor(kRed);
	peGated->Draw("same");


	
	TCanvas *plot2 =new TCanvas("plot2","plot2",800,600);
	plot2->Divide(1,3);
	plot2->cd(1);
	chargePlas1->Draw();	

	plot2->cd(2);
	chargePlas2->Draw();
	
	plot2->cd(3);
	TopVsBottom->Draw("COLZ");


	//***** Generate Convoluted Landau*Gauss Distribution Fit *****//
	TF1 *langaus = new TF1("langaus",langaufun,20,150,4);
	langaus->SetParNames("lwidth","mpv","integral","gsigma");
	
	mean=peCorr->GetMean();
	area=peCorr->Integral(40,150);
	sigma=peCorr->GetRMS();
	fr[0]=0.5*mean;
	fr[1]=2*mean;
	
	cout<<"mean "<<mean<<"  area  "<<area<<"  sigma  "<<sigma<<endl;
	
	langaus->SetParameters(2.,mean,area,6);
	
	
	peCorr->Fit("langaus","R0","",fr[0],fr[1]);
	gStyle->SetOptStat(1111);
    gStyle->SetOptFit(111);
	TCanvas *plot4 = new TCanvas("plot4","plot4",800,600);
	peCorr->Draw();
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
		
	return 0;
}
