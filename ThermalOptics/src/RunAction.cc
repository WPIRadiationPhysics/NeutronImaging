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
  G4double meshDim = 1*mm;
  G4int meshResolution = 100;

  // Create neutron 2D histogram
  analysisManager->CreateH2("nFlux", "nFlux",
                            meshResolution*2, -meshDim, meshDim,
                            meshResolution*2, -meshDim, meshDim);
}

RunAction::~RunAction() { delete G4AnalysisManager::Instance(); }

void RunAction::BeginOfRunAction(const G4Run* /*run*/) {

  // Obtain vars via Messenger
  Messenger* messenger = Messenger::GetMessenger();
  G4double LDratio = messenger->GetLDRatio();
  G4String localDataDir = messenger->GetDataDir();

  // Construct data file path
  // (FIX: Decimal-valued LDratio mistakes LDX.Y for file LDX w/ extension Y, as a result omits ".root")
  G4String dataFileName; std::ostringstream dataFileNameStream;
  dataFileNameStream.str(""); dataFileNameStream << localDataDir << "LD" << LDratio;
  dataFileName = dataFileNameStream.str();

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
