{
///////WCZYTANIE DRZEWA///////
    TFile *f1= TFile::Open("25August_2_MCR0_hadrons_0pt8Gev_0pt0T_Beam___all.root");
	TTree *FEB;
/////FEBnumber SECLECT FEB WHICH YOU WANT TO OPEN////
	int FEBnumber=1;
//////////kanalek USTAWIA KTORY KANAL WYPLOC W HISTOGRAMIE AMPLITUDY OD KANALY///////
    int kanalek=2;
///////////////////////////////////////
///////////////////////////////////////
	f1->GetObject( Form("FEB_%d",FEBnumber),FEB);
    //creating vector<double> to set branch adress//
	vector<double>  amplituda;
	vector<double> *amplitude= &amplituda;
    vector<double>  amplitudaLG;
	vector<double> *amplitudeLG= &amplitudaLG;
    vector<double>  kanal;
	vector<double> *channel= &kanal;
    vector<double>  czas;
    vector<double> *time= &czas;
///////////setting branches/////////////////////
  	FEB->SetBranchAddress( Form("FEB_%d_hitAmpl",FEBnumber), &amplitude);
    FEB->SetBranchAddress( Form("FEB_%d_hitLGAmpl",FEBnumber), &amplitudeLG);
  	FEB->SetBranchAddress( Form("FEB_%d_hitsChannel",FEBnumber), &channel);
    FEB->SetBranchAddress( Form("FEB_%d_hitTimeDif",FEBnumber), &time);

    int Eventy = FEB->GetEntries();
/////////////////////////////////////////////////////////////////////////
    //creating histogram for AmplitudexChannel
    TH2F *AmplChaHist=new TH2F( Form("AmplChaHist_FEB_%d",FEBnumber), Form("Amplitude HG and Channel histogram for FEB_%d",FEBnumber), 1001, 0, 1000, 101, 0, 100);
    AmplChaHist->GetXaxis()->SetTitle("Amplitude HG");
    AmplChaHist->GetYaxis()->SetTitle("Channel");
////////////////////////////////////////////////////////////////////////
    //creating histogram for ToTxChannel
    TH2F *AmplTimeHist=new TH2F( Form("AmplTimeHist_FEB_%d",FEBnumber), Form("Amplitude HG and ToT histogram for FEB_%d",FEBnumber), 801, 0, 800, 51, 0, 50);
    AmplTimeHist->GetXaxis()->SetTitle("Amplitude HG");
    AmplTimeHist->GetYaxis()->SetTitle("ToT");
///////////////////////////CREATING HISTOGRAM FOR AMPLITUDE WITH GIVEN CHANNEL///////////////
    TH2F *AmplLGTimeaHist=new TH2F( Form("AmplLGTimeHist_FEB_%d",FEBnumber), Form("Amplitude LG and ToT histogram for channel %d FEB_%d",kanalek ,FEBnumber), 1001, 0, 1000, 41, 0, 40);
    AmplLGTimeaHist->GetXaxis()->SetTitle("Amplitude LG");
    AmplLGTimeaHist->GetYaxis()->SetTitle("ToT");
//////////////////////////////////////////////////////////////////////////////////////////////
    //creating histogram for AmplitudeLGxChannel
    TH1F *AmplHist = new TH1F( Form("AmplHist_FEB_%d",FEBnumber), Form("Amplitude HG histogram for channel %d and FEB_%d",kanalek, FEBnumber), 5001, 0, 5000);
    AmplHist->GetXaxis()->SetTitle("Amplitude HG");
/////////////////////////////////////////////////////////////////////////////
    //CREATING HG AMPLITUDE V LG AMPLITUDE
    TH2F *AmplLGAmplHist=new TH2F( Form("AmplLGAmplHist_FEB_%d",FEBnumber), Form("Amplitude LG and Amplitude HG histogram for channel %d and FEB_%d",kanalek, FEBnumber), 4001, 0, 4000, 4001, 0, 4000);
    AmplLGAmplHist->GetXaxis()->SetTitle("Amplitude HG");
    AmplLGAmplHist->GetYaxis()->SetTitle("Amplitude LG");
////////////////////////////////////////////////////////////////////

    FEB->GetEntry(0);
    double rozmiar = channel.size();
//////////////////////////////////////

    int counter=0;
    for(int i=0; i<rozmiar ;i++){
        if(time.at(i)<30){
        counter++;
                        }
        AmplChaHist->Fill(amplitude.at(i),channel.at(i));
        AmplTimeHist->Fill(amplitude.at(i),time.at(i));


        if(channel.at(i)==kanalek){
                AmplLGTimeaHist->Fill(amplitudeLG.at(i),time.at(i));
                AmplHist->Fill(amplitude.at(i));
                AmplLGAmplHist->Fill(amplitude.at(i),amplitudeLG.at(i));
                                    }
    }
////////LICZNIK EVENTOW KTORE SA PONIZEJ/POWYZEJ DANEJ WARTOSC DO FAJNYCH RYSUNKOW, GLOWNIE DLA AmplTimeHist/////////
    cout<<counter<<endl;
    float ulamek=counter/rozmiar*100;
    cout<<"percentage above/below set value "<<ulamek<<"%"<<endl;
///////ZROBIC FAJNA LINIE W POWER POINT POKAZUJACA PROCENT NAD LUB POD DANA WARTOSCIA//////////
    TCanvas *c1 = new TCanvas("c1","c1");
    gROOT->SetStyle("Plain");
    gStyle->SetOptLogy(0);
    gStyle->SetOptStat(1110);
    c1->SetLogz();

//////////////WYBOR KTORY HISTOGRAM NARYSOWAC///////////
/////////////DRAW APLITUDE CHANNEL HISTOGRAM//////////
    //AmplChaHist->Draw("COLZ L");
    //AmplChaHist->SetMaximum(2000);
/////////////DRAW AMPLITUDE TOT HISTOGRAM/////////////
    //AmplTimeHist->Draw("COLZ");
/////////DRAW APLITUDE LG CHANNEL HISTOGRAM//////////
    AmplLGTimeaHist->Draw("COLZ L");
///////////DRAW AMPLITUDE FOR A GIVVEN CHANNEL//////////
    //AmplHist->Draw();
    //AmplHist->SetMaximum(200);
////////DRAW HG AND LG AMPLITUDE HISTOGRAM/////////////
    //AmplLGAmplHist->Draw("COLZ");

}
