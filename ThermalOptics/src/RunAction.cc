#include "RunAction.hh"
#include "Analysis.hh"
#include "Messenger.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

RunAction::RunAction() : G4UserRunAction() {

  // Acquire analysis manager and Messenger
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  Messenger* messenger = Messenger::GetMessenger();

  // Obtain tally dimension
  G4double tallyDim = 0.25*cm;

  // Create neutron 2D histogram
  analysisManager->CreateH2("nFlux", "nFlux",
                            100, -tallyDim, tallyDim,
                            100, -tallyDim, tallyDim);
}

RunAction::~RunAction() { delete G4AnalysisManager::Instance(); }

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {

  // Obtain data directory for run via Messenger
  Messenger* messenger = Messenger::GetMessenger();
  G4String localDataDir = messenger->GetDataDir(),
           dataFileName = localDataDir + "Imaging";

  // Acquire analysis manager, create data file and open
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  analysisManager->SetFileName(dataFileName);
  analysisManager->OpenFile();
}

void RunAction::EndOfRunAction(const G4Run* /*run*/) {

  // Acquire Analysis Manager
  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // Write data file and close
  analysisManager->Write();
  analysisManager->CloseFile();
}
