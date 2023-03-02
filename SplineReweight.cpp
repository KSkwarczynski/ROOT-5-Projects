#include <iostream>
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>

// ROOT includes
#include "TObjArray.h"
#include "TChain.h"
#include "TFile.h"
#include "TBranch.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"
#include "TH1.h"
#include "TF1.h"
#include "TH2.h"
#include "TGraphErrors.h"
#include "TVectorD.h"
#include "TColor.h"

void SetT2Kstyl();

void SplineReweight() {

    TCanvas *c1 = new TCanvas("c1"," ", 0, 0, 800,630);
    SetT2Kstyl();
    gPad->SetGrid();
    gStyle->SetPalette(51);


    TFile *file = new TFile("Weight.root");
    TTree *tree = (TTree*)file->Get("sample_sum");

    int nentries = tree->GetEntries();

    std::cout<< "\033[1;34mNumber of all events in the tree \033[0m " <<nentries<<std::endl;

    TObjArray *SRCFrac_C = NULL;
    //TObjArray *SRCFrac_O;


    int NFSprot, NFSneut;
    double HMprotonMom, HMneutronMom;

    tree->SetBranchStatus("*", false);

    tree->SetBranchStatus( "NFSprot", true);
    tree->SetBranchAddress("NFSprot", &(NFSprot));

    tree->SetBranchStatus( "NFSneut", true);
    tree->SetBranchAddress("NFSneut", &(NFSneut));

    tree->SetBranchStatus( "HMprotonMom", true);
    tree->SetBranchAddress("HMprotonMom", &(HMprotonMom));

    tree->SetBranchStatus( "HMneutronMom", true);
    tree->SetBranchAddress("HMneutronMom", &(HMneutronMom));

    tree->SetBranchStatus( "SRCFrac_CGraph", true);
    tree->SetBranchAddress("SRCFrac_CGraph", &SRCFrac_C);

    //tree->SetBranchAddress("SRCFrac_OGraph", &SRCFrac_O);


    int NKnots = 4;
    double Knots[4] = {0, 0.3, 0.5, 1};
    Color_t RelevantModesColors[4] = {kRed, kGreen+1, kTeal, kBlue+1};
    TH1F ** Nprotshist = new TH1F*[4];
    TH1F ** Nneutshist = new TH1F*[4];

    TH1F ** ProtMomhist = new TH1F*[4];
    TH1F ** NeutMomhist = new TH1F*[4];

    for(int i = 0; i < NKnots; i++)
    {
       Nprotshist[i] = new TH1F(Form("Nprotshist_knot=%.1f",Knots[i]),Form("Nprotshist_knot=%.1f",Knots[i]), 10, 0, 10);
       Nprotshist[i]->GetXaxis()->SetTitle("N true protons");

       Nneutshist[i] = new TH1F(Form("Nneutshist_knot=%.1f",Knots[i]),Form("Nneutshist_knot=%.1f",Knots[i]), 10, 0, 10);
       Nneutshist[i]->GetXaxis()->SetTitle("N true neutrons");

       ProtMomhist[i] = new TH1F(Form("ProtMomhist_knot=%.1f",Knots[i]),Form("ProtMomhist_knot=%.1f",Knots[i]), 50, 0, 1000);
       ProtMomhist[i]->GetXaxis()->SetTitle("true HM proton  momentum");

       NeutMomhist[i] = new TH1F(Form("NeutMomhist_knot=%.1f",Knots[i]),Form("NeutMomhist_knot=%.1f",Knots[i]), 50, 0, 1000);
       NeutMomhist[i]->GetXaxis()->SetTitle("true HM proton  momentum");

    }

    for (int i = 0; i < nentries; ++i)
    {
        if (i % (1000) == 0) std::cout<<i<<std::endl;
        tree->GetEntry(i);

        //std::cout<<SRCFrac_C->At(0)<<std::endl;

        TGraph *Graph = (TGraph*)(SRCFrac_C->At(0)->Clone());

        double weight = 1.;

        for(int ik = 0; ik < NKnots; ik++)
        {
            weight = Graph->Eval(Knots[ik]);

            Nprotshist[ik]->Fill(NFSprot, weight);
            Nneutshist[ik]->Fill(NFSneut, weight);

            ProtMomhist[ik]->Fill(HMprotonMom, weight);
            NeutMomhist[ik]->Fill(HMneutronMom, weight);
        }
    }


    c1->Print("Reweight.pdf[");

    double Maximum1 = 0;
    double Maximum2 = 0;

    double Maximum3 = 0;
    double Maximum4 = 0;

    for(int i = 0; i < NKnots; i++)
    {
        if(Nprotshist[i]->GetMaximum() > Maximum1) Maximum1 = Nprotshist[i]->GetMaximum();
        Nprotshist[i]->SetLineColorAlpha(RelevantModesColors[i], 1);
        if(Nneutshist[i]->GetMaximum() > Maximum2) Maximum2 = Nneutshist[i]->GetMaximum();
        Nneutshist[i]->SetLineColorAlpha(RelevantModesColors[i], 1);


        if(ProtMomhist[i]->GetMaximum() > Maximum3) Maximum3 = ProtMomhist[i]->GetMaximum();
        ProtMomhist[i]->SetLineColorAlpha(RelevantModesColors[i], 1);

        if(NeutMomhist[i]->GetMaximum() > Maximum4) Maximum4 = NeutMomhist[i]->GetMaximum();
        NeutMomhist[i]->SetLineColorAlpha(RelevantModesColors[i], 1);
    }


    Nprotshist[0]->SetMaximum(Maximum1);
    Nprotshist[0]->Draw("");
    for(int i = 1; i < NKnots; i++)
    {
       Nprotshist[i]->Draw("same");
    }

    TLegend* legend = new TLegend(0.6,0.7,0.9,0.9);
    for(int i = NKnots-1; i >= 0; i--)
    {
       legend->AddEntry(Nprotshist[i], Form("SRCFrac_C=%.1f",Knots[i]),"l");
    }
    legend->SetTextSize(0.035);
    legend->Draw("same");
    c1->Print("Reweight.pdf");

    delete legend;
    legend = NULL;


    Nneutshist[0]->SetMaximum(Maximum2);
    Nneutshist[0]->Draw("");
    for(int i = 1; i < NKnots; i++)
    {
       Nneutshist[i]->Draw("same");
    }

    TLegend* legend = new TLegend(0.6,0.7,0.9,0.9);
    for(int i = NKnots-1; i >= 0; i--)
    {
       legend->AddEntry(Nneutshist[i], Form("SRCFrac_C=%.1f",Knots[i]),"l");
    }
    legend->SetTextSize(0.035);
    legend->Draw("same");
    c1->Print("Reweight.pdf");
    delete legend;
    legend = NULL;


    ProtMomhist[0]->SetMaximum(Maximum3);
    ProtMomhist[0]->Draw("");
    for(int i = 1; i < NKnots; i++)
    {
       ProtMomhist[i]->Draw("same");
    }

    TLegend* legend = new TLegend(0.6,0.7,0.9,0.9);
    for(int i = NKnots-1; i >= 0; i--)
    {
       legend->AddEntry(ProtMomhist[i], Form("SRCFrac_C=%.1f",Knots[i]),"l");
    }
    legend->SetTextSize(0.035);
    legend->Draw("same");
    c1->Print("Reweight.pdf");
    delete legend;
    legend = NULL;


    NeutMomhist[0]->SetMaximum(Maximum4);
    NeutMomhist[0]->Draw("");
    for(int i = 1; i < NKnots; i++)
    {
       NeutMomhist[i]->Draw("same");
    }

    TLegend* legend = new TLegend(0.6,0.7,0.9,0.9);
    for(int i = NKnots-1; i >= 0; i--)
    {
       legend->AddEntry(NeutMomhist[i], Form("SRCFrac_C=%.1f",Knots[i]),"l");
    }
    legend->SetTextSize(0.035);
    legend->Draw("same");
    c1->Print("Reweight.pdf");
    delete legend;
    legend = NULL;

    c1->Print("Reweight.pdf]");
}





void SetT2Kstyl()
{
     // -- WhichStyle --
    // 1 = presentation large fonts
    // 2 = presentation small fonts
    // 3 = publication/paper
    Int_t WhichStyle = 3;

    Int_t FontStyle = 22;
    Float_t FontSizeLabel = 0.035;
    Float_t FontSizeTitle = 0.05;
    Float_t YOffsetTitle = 1.3;

  switch(WhichStyle)
  {
    case 1:
        FontStyle = 42;
        FontSizeLabel = 0.05;
        FontSizeTitle = 0.065;
        YOffsetTitle = 1.19;
        break;
    case 2:
        FontStyle = 42;
        FontSizeLabel = 0.035;
        FontSizeTitle = 0.05;
        YOffsetTitle = 1.6;
        break;
    case 3:
        FontStyle = 132;
        FontSizeLabel = 0.035;
        FontSizeTitle = 0.045;
        YOffsetTitle = 1.6;
        break;
    }
    // use plain black on white colors
    gStyle->SetFrameBorderMode(0);
    gStyle->SetCanvasBorderMode(0);
    gStyle->SetPadBorderMode(0);
    gStyle->SetCanvasBorderSize(0);
    gStyle->SetFrameBorderSize(0);
    gStyle->SetDrawBorder(0);
    gStyle->SetTitleBorderSize(2);
    gStyle->SetStatBorderSize(2);

    gStyle->SetStatX(0.89);
    gStyle->SetStatY(0.89);

    gStyle->SetPadColor(0);
    gStyle->SetCanvasColor(0);
    gStyle->SetStatColor(0);
    gStyle->SetFillColor(0);

    gStyle->SetEndErrorSize(4);
    gStyle->SetStripDecimals(kFALSE);

    //gStyle->SetLegendBorderSize(0); //This option dsables legends borders
    gStyle->SetLegendFont(FontStyle);

    // set the paper & margin sizes
    gStyle->SetPaperSize(20, 26);
    gStyle->SetPadTopMargin(0.1);
    gStyle->SetPadBottomMargin(0.12);
    gStyle->SetPadRightMargin(0.12); // 0.075 -> 0.13 for colz option
    gStyle->SetPadLeftMargin(0.12);//to include both large/small font options

    // Fonts, sizes, offsets
    gStyle->SetTextFont(FontStyle);
    gStyle->SetTextSize(FontSizeTitle);

    gStyle->SetLabelFont(FontStyle, "x");
    gStyle->SetLabelFont(FontStyle, "y");
    gStyle->SetLabelFont(FontStyle, "z");
    gStyle->SetLabelFont(FontStyle, "t");
    gStyle->SetLabelSize(FontSizeLabel, "x");
    gStyle->SetLabelSize(FontSizeLabel, "y");
    gStyle->SetLabelSize(0.030, "z");
    gStyle->SetLabelOffset(0.008, "x");
    gStyle->SetLabelOffset(0.008, "y");
    gStyle->SetLabelOffset(0.006, "z");

    gStyle->SetTitleFont(FontStyle, "x");
    gStyle->SetTitleFont(FontStyle, "y");
    gStyle->SetTitleFont(FontStyle, "z");
    gStyle->SetTitleFont(FontStyle, "t");
    gStyle->SetTitleSize(FontSizeTitle, "y");
    gStyle->SetTitleSize(FontSizeTitle, "x");
    gStyle->SetTitleSize(FontSizeTitle, "z");
    gStyle->SetTitleSize(FontSizeTitle, "t");
    gStyle->SetTitleOffset(1.1, "x");
    gStyle->SetTitleOffset(1.1, "y");
    gStyle->SetTitleOffset(1.1, "z");

    gStyle->SetTitleStyle(0);
    gStyle->SetTitleFontSize(0.045);//0.08
    gStyle->SetTitleFont(FontStyle, "pad");
    gStyle->SetTitleX(0.6);
    gStyle->SetTitleY(0.96);
    //gStyle->SetTitleW(0.8f);

    // use bold lines and markers
    gStyle->SetMarkerStyle(20); //FIXME Coment this for FGD2
    gStyle->SetHistLineWidth( Width_t(2.5) );
    gStyle->SetLineStyleString(2, "[12 12]"); // postscript dashes

    // get rid of X error bars and y error bar caps
    gStyle->SetErrorX(0.001);

    // do not display any of the standard histogram decorations
    //gStyle->SetOptTitle(0); //Set 0 to disable histogram tittle
    gStyle->SetOptStat(0); //Set 0 to disable statystic box
    //gStyle->SetOptFit(0);

    // put tick marks on top and RHS of plots
    gStyle->SetPadTickX(0);
    gStyle->SetPadTickY(0);

    // -- color --
    // functions blue
    //gStyle->SetFuncColor(600-4);
    gStyle->SetFuncColor(2);
    gStyle->SetFuncWidth(2);

    gStyle->SetFillColor(1); // make color fillings (not white)
    // - color setup for 2D -
    // - "cold"/ blue-ish -
    Double_t red[]   = { 0.00, 0.00, 0.00 };
    Double_t green[] = { 1.00, 0.00, 0.00 };
    Double_t blue[]  = { 1.00, 1.00, 0.25 };
    // - "warm" red-ish colors -
    //  Double_t red[]   = {1.00, 1.00, 0.25 };
    //  Double_t green[] = {1.00, 0.00, 0.00 };
    //  Double_t blue[]  = {0.00, 0.00, 0.00 };

    Double_t stops[] = { 0.25, 0.75, 1.00 };
    const Int_t NRGBs = 3;
    const Int_t NCont = 500;

    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

    // - Rainbow -
    gStyle->SetPalette(1);  // use the rainbow color set

    // -- axis --
    gStyle->SetStripDecimals(kFALSE); // don't do 1.0 -> 1
    //  TGaxis::SetMaxDigits(3); // doesn't have an effect
    // no supressed zeroes!
    gStyle->SetHistMinimumZero(kTRUE);

    //gROOT->ForceStyle();
}
