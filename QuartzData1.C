#include "TFile.h"
#include "TTree.h"
#include "TH1D.h"
#include "TString.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "TCanvas.h"

using namespace std;


void QuartzData1(){
  gSystem->Load("liblegolas");
  int nofile=5;
  int evtnub[5]={394,920,523,794,905};
  int filename[5]={1059,1060,1061,1062,1063};
  
  double integral; // raw signal integral over time
  double intePlastic1,intePlastic2;
  double pedestal,pedPlas1,pedPlas2;;
  double charge,chPlas1,chPlas2; // corrected charge integral, pedestal subtracted
  //cout<<"Hi after"<<endl;
  double threshold=30.;
  double pedmax1=300.;
  double pedmin1=100.;
  double pedmax2=600.;
  double pedmin2=400.;
  double sigmax=350.;
  double sigmin=300.;

  double sigPlasMin1=320;
  double sigPlasMax1=430;
  //double sigPlasMin2=340;
  //  double pedPlasMin1=30;
  
  double pedPlasMin1=30;
  double pedPlasMax1=300;

  //double pedPlasMin2=30;
  //double pedPlasMax2=300;

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
  
  TH1F *chargePlas1 = new TH1F("Plastic1","Plastic1",3000,-0.5,2999.5);  
  TH1F *chargePlas2 = new TH1F("Plastic2","Plastic2",3000,-0.5,2999.5);

  TH2F *TopVsBottom = new TH2F("TopVsBottom","TopVsBottom", 3000,-0.5,2999.5,3000,-0.5,2999.5);  

  TTree* roottree = new TTree("T","DRS4 Cosmic Data");
  
  roottree->Branch("inte",		&integral,	"inte/D");
  roottree->Branch("ped",		&pedestal,	"ped/D");
  roottree->Branch("ChargeCor",		&charge,	"pmt/D");
  roottree->Branch("pedPlas1",               &pedPlas1,      "pedPlas1/D");
  roottree->Branch("intePlastic1",         &intePlastic1,        "intePlastic1/D");
  roottree->Branch("chPlas1",               &chPlas1,      "chPlas1/D");
  roottree->Branch("pedPlas2",               &pedPlas2,      "pedPlas2/D");
  roottree->Branch("intePlastic2",         &intePlastic2,        "intePlastic2/D");
  roottree->Branch("chPlas2",               &chPlas2,      "chPlas2/D");


  //roottree->Branch("event",	&event,		"event/I");
  //TString hist_title;
  TString infile;
  int event = 0;
  //for(int ik=0;ik<nofile;ik++)
  for(int ik=0;ik<1;ik++)
  {
	  
	  //int ifile=1011;
	  infile=Form("../scint_000000%d-0000.evt",filename[ik]);
	  //poncsopen("scint_0000001011-0000.evt");
	  poncsopen(infile);
	  prun(1); // dump header data
	  //cout<<"Hi before"<<endl;
	  //TFile *rootfile = TFile::Open("test.root","RECREATE");
	  //cout<<"Hi after"<<endl;
	  //TTree *apv = new TTree("Quartz","Quartz");
	  int nevent = evtnub[ik]; // total event number
	  //for(int ievent=0; ievent<nevent-2;ievent++)
          for(int ievent=0; ievent<200;ievent++)
	  {
		  //event++;
		  prun(1);
		  
		  //hist_title = "h1";//Form("h_0_%d_%d",iapv,ich);
		  //h_wfm = (TH1D*) gDirectory->FindObject(hist_title);
		  //integral = h_wfm->Integral(300,340);
		  integral=(trace0->Integral(sigmin,sigmax))*1.e-9*1.e-3/(R*q*gain);
                 
                 intePlastic1=(h2->Integral(sigPlasMin1,sigPlasMax1));
                 intePlastic2=(h3->Integral(sigPlasMin1,sigPlasMax1));

		 pedPlas1=(h2->Integral(pedPlasMin1,pedPlasMax1))*(sigPlasMax1-sigPlasMin1)/(pedPlasMax1-pedPlasMin1);
                 pedPlas2=(h3->Integral(pedPlasMin1,pedPlasMax1))*(sigPlasMax1-sigPlasMin1)/(pedPlasMax1-pedPlasMin1);

		 //cout<<"ievent "<<ievent<<" plastic1  "<<intePlastic1<<" plastic2  "<<intePlastic2<<
		  //"  ped1  "<<pedPlas1<< "  ped2  "<<pedPlas2<<endl;
		  charge1->Fill(integral);
		  //cout<<"  sum  ="<<integral<<"  integral1 : "<<integral1<<endl;
		  pedestal = (trace0->Integral(pedmin1,pedmax1)+trace0->Integral(pedmin2,pedmax2))*
					(sigmax-sigmin)*1.e-9*1.e-3/(R*q*gain)/(pedmax1+pedmax2-pedmin1-pedmin2);
					
		  //pedestal = (trace0->Integral(pedmin1,pedmax1))*
			//		(sigmax-sigmin)*1.e-9*1.e-3/(R*q*gain)/(pedmax1-pedmin1);
		  charge=(integral-pedestal);//(R*q*gain);
		  peds->Fill(pedestal);

		 intePlastic1=-1*intePlastic1;
		 intePlastic2=-1*intePlastic2;

		 pedPlas1=-1*pedPlas1;
                 pedPlas2=-1*pedPlas2;

		 chPlas1=(intePlastic1-pedPlas1);
                 chPlas2=(intePlastic2-pedPlas2);

		 chargePlas1->Fill(chPlas1);
		 chargePlas2->Fill(chPlas2);
		 TopVsBottom->Fill(intePlastic1,intePlastic2);		  
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

  TCanvas *plot1 = new TCanvas("plot1","Plot1",800,600);
  plot1->Divide(1,3);
  
  plot1->cd(1);
  chargePlas1->Draw();
  
  plot1->cd(2);
  chargePlas2->Draw();

  plot1->cd(3);
  TopVsBottom->Draw("COLZ");

  TFile* rootfile = new TFile("dummy.root","RECREATE");
  rootfile->cd();
  roottree->Write("T",TObject::kOverwrite);
  rootfile->Close();	
}


