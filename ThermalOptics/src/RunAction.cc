#include "RunAction.hh"
#include "Analysis.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction() : G4UserRunAction() {

  // Acquire analysis manager, create neutron 2D histogram
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->CreateH2("nFlux", "nFlux", 100, -2.5*cm, 2.5*cm, 100, -2.5*cm, 2.5*cm);
}

RunAction::~RunAction() { delete G4AnalysisManager::Instance(); }

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {

  // Acquire analysis manager, create data file and open
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetFileName("ImagingData");
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run* /*run*/) {

  // Acquire Analysis Manager, write data file and close
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->Write();
  analysisManager->CloseFile();
}
