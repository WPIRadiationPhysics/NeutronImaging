// *********************************************************************
// To execute this macro under ROOT after your simulation ended, 
//   1 - launch ROOT (usually type 'root' at your machine's prompt)
//   2 - type '.X plot.C' at the ROOT session prompt
// *********************************************************************
{
  // Construct Canvas
  gStyle->SetOptStat(0);
  TCanvas histoCanvas("histoCanvas", "histoCanvas");

  // Var declaration
  TH2F* h2; 
  int* LDs[6] = {1, 25, 50, 75, 100, 200};
  char fluxHistoName[30], fluxHistoFileName[80],
       figTitle[50], pngName[30], syscmd[50];

  // Create analysis directory
  sprintf(syscmd, "mkdir -p analysis");
  system((const char*)syscmd);

  /// histogram constructions ///

  // Model LD iteration
  for ( int LD_i = 0; LD_i < 6; LD_i++ ) {

    // Create LD model output directory
    //sprintf(syscmd, "mkdir -p fluxHistos/LD%d/", LDs[LD_i]);
    //system((const char*)syscmd);
 
    // Set data file
    sprintf(fluxHistoFileName, "data/LD%d.root", LDs[LD_i]);
    TFile f((const char*)fluxHistoFileName);

    // Define Tree names
    sprintf(fluxHistoName, "nFlux");

    // Acquire Deposition Histogram Trees
    h2 = (TH2F*) f->Get((const char*)fluxHistoName);

    // Format and output figure
    sprintf(figTitle, "Neutron Flux [LD = %d]", LDs[LD_i]);
    h2->SetTitle((const char*)figTitle);
    h2->GetXaxis()->SetTitle("x (mm)");
    h2->GetXaxis()->SetRangeUser(-1, 1);
    h2->GetYaxis()->SetTitle("y (mm)");
    h2->GetYaxis()->SetRangeUser(-1, 1);
    h2->Draw("COLZ");

    // Update canvas data
    histoCanvas->Modified();
    histoCanvas->Update();

    // Define output filename and save as png
    sprintf(pngName, "analysis/nFlux-LD%3d.png", LDs[LD_i]);
    histoCanvas->Print((const char*)pngName);
  }
}
