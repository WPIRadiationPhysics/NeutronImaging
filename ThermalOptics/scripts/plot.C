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
  TH1F* h1; TH2F* h2; 
  int* LDs[6] = {1, 25, 50, 75, 100, 200};
  char fluxHistoName[30], fluxHistoFileName[80],
       figTitle[50], pngName[30], syscmd[50];
  // ROOT color codes - 1: black, 2: red, 3: green,
  //             4: blue, 5: yellow, 6: violet, 7: cyan
  int* lineColors[6] = {1, 3, 6, 7, 4, 2}; int lc_i;

  // Create analysis directory
  sprintf(syscmd, "rm -rf analysis; mkdir -p analysis");
  system((const char*)syscmd);

  // Create 1D/2D figure output directory
  sprintf(syscmd, "mkdir -p analysis/nFlux1D/"); system((const char*)syscmd);
  sprintf(syscmd, "mkdir -p analysis/nFlux2D/"); system((const char*)syscmd);

  /// Histogram Constructions ///

  // Model LD iteration
  for ( int LD_i = 0; LD_i < 6; LD_i++ ) {

    // Set data file
    sprintf(fluxHistoFileName, "data/LD%d.root", LDs[LD_i]);
    TFile f((const char*)fluxHistoFileName);

    // 1D Histogram

      // Acquire Deposition Histogram Trees
      sprintf(fluxHistoName, "nFlux1D");
      h1 = (TH1F*) f->Get((const char*)fluxHistoName);

      // Format and output figure
      sprintf(figTitle, "Neutron Flux [LD = %d]", LDs[LD_i]);
      h1->SetTitle((const char*)figTitle);
      h1->GetXaxis()->SetTitle("y (mm)");
      h1->GetYaxis()->SetTitle("Neutron Fluence (n/mm^2)");
      lc_i = lineColors[LD_i]; h1->SetLineColor(lc_i);
      h1->Draw("HIST C");

      // Update canvas data
      histoCanvas->Modified();
      histoCanvas->Update();

      // Define output filename and save as png
      sprintf(pngName, "analysis/nFlux1D/LD%3d.png", LDs[LD_i]);
      histoCanvas->Print((const char*)pngName);

    // 2D Histogram

      // Acquire Deposition Histogram Trees
      sprintf(fluxHistoName, "nFlux2D");
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
      sprintf(pngName, "analysis/nFlux2D/LD%3d.png", LDs[LD_i]);
      histoCanvas->Print((const char*)pngName);
  }
}
