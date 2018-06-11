void draw()
{
	gROOT->SetStyle("Plain");
    //gStyle->SetOptStat(0);

    TChain *T = new TChain("T");
    T->Add("dummy.root");
    TCanvas *c =  new TCanvas();
    c->Divide(1,3);
	//cout<<T->GetEntries()<<endl;
    c->cd(1);
    gPad->SetLogy();
    T->Draw("inte >> h1(200, -0.5, 199.5 )", "");
    //h1->SetMinimum(1e11);
    h1->SetTitle("Integrated charge (without pedestal)");
    h1->GetXaxis()->SetTitle("PE");
    h1->GetXaxis()->CenterTitle();

	c->cd(2);
	gPad->SetLogy();
    T->SetLineColor(kBlue);
    T->Draw("ped>>h2(200,-0.5,199.5)", "");
    
    c->cd(3);
    gPad->SetLogy();
    T->SetLineColor(kRed);
    T->Draw("ChargeCor>>h3(200,-0.5,199.5)", "");
    T->SetLineColor(kBlue);
    T->Draw("ChargeCor", "ChargeCor>40", "same");
    
}

