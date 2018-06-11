#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TCanvas.h"

using namespace std;


void QuartzData(){
  gSystem->Load("liblegolas");
  int nofile=5;
  int evtnub[5]={394,920,523,61,707};
  int filename[5]={1059,1060,1061,1014,1029};
  
  double integral; // raw signal integral over time
  double pedestal;
  double charge; // corrected charge integral, pedestal subtracted
  //cout<<"Hi after"<<endl;
  double threshold=30.;
  double pedmax1=300.;
  double pedmin1=100.;
  double pedmax2=600.;
  double pedmin2=400.;
  double sigmax=350.;
  double sigmin=300.;
  //***** Constants (gain subject to change via given PMT) *****//
  double R    = 50.;       // in Ohms
  double q    = 1.602e-19; // in eV (electron charge)
  double gain = 7.765e5;    // PMT gain value (in Coulombs)
  
  TH1D *h_wfm = new TH1D("h_wfm","waveform",1024,-0.5,1023.5);
  //TH1D *h_wfm1 = new TH1D("h_wfm1","waveform1",1024,-0.5,1023.5);
  //TH1D *h_first = new TH1D("h_first","h_first",27,-0.5,26.5);
  //TH1D *h_ped = new TH1D("h_ped","pedestal statistic",3000,-0.5,2999.5);
  TH1F *charge1 = new TH1F("charge1","charge1",200,-0.5,199.5);
  TH1F *ChargeCor = new TH1F("ChargeCor","ChargeCor",200,-0.5,199.5);
  TH1F *peds = new TH1F("peds","peds",200,-0.5,199.5);
  TTree* roottree = new TTree("T","DRS4 Cosmic Data");
  roottree->Branch("inte",		&integral,	"inte/D");
  roottree->Branch("ped",		&pedestal,	"ped/D");
  roottree->Branch("ChargeCor",		&charge,	"pmt/D");
  
  //roottree->Branch("event",	&event,		"event/I");
  //TString hist_title;
  TString infile;
  int event = 0;
  //for(int ik=0;ik<nofile;ik++)
  for(int ik=0;ik<1;ik++)
  {
	  
	  //int ifile=1011;
	  infile=Form("scint_000000%d-0000.evt",filename[ik]);
	  //poncsopen("scint_0000001011-0000.evt");
	  poncsopen(infile);
	  prun(1); // dump header data
	  //cout<<"Hi before"<<endl;
	  //TFile *rootfile = TFile::Open("test.root","RECREATE");
	  //cout<<"Hi after"<<endl;
	  //TTree *apv = new TTree("Quartz","Quartz");
	  int nevent = evtnub[ik]; // total event number
	  for(int ievent=0; ievent<nevent-2;ievent++)
	  {
		  //event++;
		  prun(1);
		  
		  //hist_title = "h1";//Form("h_0_%d_%d",iapv,ich);
		  //h_wfm = (TH1D*) gDirectory->FindObject(hist_title);
		  //integral = h_wfm->Integral(300,340);
		  integral=(trace0->Integral(sigmin,sigmax))*1.e-9*1.e-3/(R*q*gain);
		  charge1->Fill(integral);
		  //cout<<"  sum  ="<<integral<<"  integral1 : "<<integral1<<endl;
		  pedestal = (trace0->Integral(pedmin1,pedmax1)+trace0->Integral(pedmin2,pedmax2))*
					(sigmax-sigmin)*1.e-9*1.e-3/(R*q*gain)/(pedmax1+pedmax2-pedmin1-pedmin2);
					
		  //pedestal = (trace0->Integral(pedmin1,pedmax1))*
			//		(sigmax-sigmin)*1.e-9*1.e-3/(R*q*gain)/(pedmax1-pedmin1);
		  charge=(integral-pedestal);//(R*q*gain);
		  peds->Fill(pedestal);
		  //ChargeCor->Fill(charge);
		  //cout<<"  Signal Integral ="<<integral<<" Pedestal : "<< pedestal<<" corrected charge  : "<<charge<<endl;
		  roottree->Fill();
		  if(charge>40)
		  {
			  ChargeCor->Fill(charge);
			  event++;
		  }
	  }
	  pclose();
	  cout<<"ievent  :  "<<ievent<<"   event  "<<event<<endl;
  }
  TCanvas *plot = new TCanvas("plot","plot",800,600);
  plot->Divide(1,3);
  plot->cd(1);
  charge1->Draw();
  plot->cd(2);
  peds->Draw();

  plot->cd(3);
  ChargeCor->Draw();  
  TFile* rootfile = new TFile("dummy.root","RECREATE");
  rootfile->cd();
  roottree->Write("T",TObject::kOverwrite);
  rootfile->Close();	
}


