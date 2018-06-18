// Marisa Petrusky 06/13/18 Edited Chandan's code to analyze one file at a time and to be compatible with ROOT histogram merger

#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TCanvas.h"
#include <string>
#include <sstream>

using namespace std;

int QuartzData2()
{
	gSystem->Load("liblegolas"); 
	
	gSystem->Load("libpmonitor"); // For Marisa's PC only
	
	// Variables to be plotted
	double integral; // raw signal integral over time
	double intePlastic1,intePlastic2;
	double pedestal,pedPlas1,pedPlas2;
	double charge,chPlas1,chPlas2; // corrected charge integral, pedestal subtracted
	
	// Integration windows of quartz
	double threshold=30.;
	double pedmax1=300.;
	double pedmin1=100.;
	double pedmax2=600.;
	double pedmin2=400.;
	double sigmax=350.;
	double sigmin=300;
    
    // Integration windows of plastics
	double sigPlasMin1=320;
	double sigPlasMax1=430;  
	double pedPlasMin1=30;
	double pedPlasMax1=300;

	//***** Constants (gain subject to change via given PMT) *****//
	double R    = 50.;       // in Ohms
	double q    = 1.602e-19; // in eV (electron charge)
	double gain = 7.765e5;    // PMT gain value (in Coulombs)
  
	TH1D *h_wfm = new TH1D("h_wfm","waveform",1024,-0.5,1023.5);

	TH1F *charge1 = new TH1F("charge1","charge1",2000,-0.5,1999.5); // Histogram of raw charge
	TH1F *ChargeCor = new TH1F("ChargeCor","ChargeCor",2000,-0.5,1999.5); // Histogram of raw charge minus pedestal (Corrected charge)
	TH1F *peds = new TH1F("peds","peds",2000,-0.5,1999.5); // Histogram of pedestal
  
	TH1F *chargePlas1 = new TH1F("Plastic1","Plastic1",3000,-0.5,2999.5); // Histogram of corrected top scint charge
	TH1F *chargePlas2 = new TH1F("Plastic2","Plastic2",3000,-0.5,2999.5); // Histogram of corrected bottom scint charge

	TH2F *TopVsBottom = new TH2F("TopVsBottom","TopVsBottom", 3000,-0.5,2999.5,3000,-0.5,2999.5); // 2D hist of both scint charges
  
	TTree* roottree = new TTree("T","DRS4 Cosmic Data");
  
	roottree->Branch("inte",		&integral,	"inte/D"); // Specifies type
	roottree->Branch("ped",		&pedestal,	"ped/D");
	roottree->Branch("ChargeCor",		&charge,	"ChargeCor/D");
	roottree->Branch("pedPlas1",               &pedPlas1,      "pedPlas1/D");
	roottree->Branch("intePlastic1",         &intePlastic1,        "intePlastic1/D");
	roottree->Branch("chPlas1",               &chPlas1,      "chPlas1/D");
	roottree->Branch("pedPlas2",               &pedPlas2,      "pedPlas2/D");
	roottree->Branch("intePlastic2",         &intePlastic2,        "intePlastic2/D");
	roottree->Branch("chPlas2",               &chPlas2,      "chPlas2/D");
	
	int nofile;
	int evtnumb;
	string myfile;
	string myevent; 
	
		cout << "File number?" << endl; // Input file name
			getline(cin,myfile);
				stringstream(myfile) >> nofile;
	
		cout << "Number of Events" << endl; // Input number of events
			getline(cin,myevent);
				stringstream(myevent) >> evtnumb;
			
	TString infile;
	
	infile=Form("../scint_000000%d-0000.evt",nofile);
	
	poncsopen(infile);
	prun(1); // Dump header data
	
	cout << "Let's begin!" << endl;
	
	for(int ievent=0; ievent<evtnumb -2;ievent++)
	{
		prun(1);
		
		integral=(trace0->Integral(sigmin,sigmax));
		
		//if(integral>150)
		{
			pedestal = (trace0->Integral(pedmin1,pedmax1)+trace0->Integral(pedmin2,pedmax2))*
					(sigmax-sigmin)/(pedmax1+pedmax2-pedmin1-pedmin2);
			
			intePlastic1=(h2->Integral(sigPlasMin1,sigPlasMax1));
			intePlastic2=(h3->Integral(sigPlasMin1,sigPlasMax1));
			
			pedPlas1=(h2->Integral(pedPlasMin1,pedPlasMax1))*(sigPlasMax1-sigPlasMin1)/(pedPlasMax1-pedPlasMin1);
			pedPlas2=(h3->Integral(pedPlasMin1,pedPlasMax1))*(sigPlasMax1-sigPlasMin1)/(pedPlasMax1-pedPlasMin1);
			
			//intePlastic1=-1*intePlastic1; //Commented out because these signals are dynode signals and integral does not need to be flipped
		    //intePlastic2=-1*intePlastic2;
		 
		    //pedPlas1=-1*pedPlas1;
            //pedPlas2=-1*pedPlas2;
            
			charge=(integral-pedestal);
			chPlas1=(intePlastic1-pedPlas1);
			chPlas2=(intePlastic2-pedPlas2);
		 
			charge1->Fill(integral);
			peds->Fill(pedestal);
			ChargeCor->Fill(charge);		 
		 
			chargePlas1->Fill(chPlas1);
			chargePlas2->Fill(chPlas2);
			TopVsBottom->Fill(intePlastic1,intePlastic2);	
			
			/*
			cout<<"ievent "<<ievent<<" plastic1  "<<intePlastic1<<" plastic2  "<<intePlastic2<<
			"  ped1  "<<pedPlas1<< "  ped2  "<<pedPlas2<<" chPlas1 "<<chPlas1<<" chPlas2 "<<chPlas2<<endl;
			cout<<"ievent  "<<ievent<<"  Signal Integral ="<<integral<<" Pedestal : "<< pedestal<<" corrected charge  : "<<charge<<endl;*/
			
			roottree->Fill();	
		}
	}
	
	pclose();
	
	TString dummyno;
	
	dummyno = Form("dummy%d.root",nofile);
	
	/*TCanvas *plot = new TCanvas("plot","plot",800,600);
  plot->Divide(1,3);
  plot->cd(1);
  charge1->Draw();
  
  plot->cd(2);
  peds->Draw();

  plot->cd(3);
  ChargeCor->Draw();

  TCanvas *plot1 = new TCanvas("plot1","Plot1",800,600);
  plot1->Divide(1,3);
  
  plot1->cd(1);
  chargePlas1->Draw();
  
  plot1->cd(2);
  chargePlas2->Draw();

  plot1->cd(3);
  TopVsBottom->Draw("COLZ");*/
	
	TFile* rootfile = new TFile(dummyno, "RECREATE");
		rootfile->cd();
		roottree->Write("T",TObject::kOverwrite);
		rootfile->Close();
	
}
