#include "SteppingAction.hh"
#include "Messenger.hh"
#include "Analysis.hh"

#include "G4SteppingManager.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"

// Initialize Step Procedure
SteppingAction::SteppingAction( const DetectorConstruction* detectorConstruction,
                               EventAction* eventAction)
                               : G4UserSteppingAction(),
                                 fDetConstruction(detectorConstruction),
                                 fEventAction(eventAction) {}
SteppingAction::~SteppingAction() {}

// Step Procedure (for every step...)
void SteppingAction::UserSteppingAction(const G4Step* step) {

  // If end of track
  if ( step->GetTrack()->GetTrackStatus() != fAlive ) {

    // Acquire particle name
    G4String stepParticle = step->GetTrack()->GetDefinition()->GetParticleName();

    // Get step position
    G4ThreeVector stepXYZ = step->GetPostStepPoint()->GetPosition();
    G4double stepX = stepXYZ[0], stepY = stepXYZ[1], stepZ = stepXYZ[2];

    // Acquire model height via Messenger
    Messenger* messenger = Messenger::GetMessenger();
    G4double worldZLength = messenger->GetModelHeight();

    // Acquire Analysis Manager
    G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

    // If neutron makes it through collimator, append xy coordinates to 2Dhistogram
    if ( stepParticle == "neutron" ) { if ( stepZ >= worldZLength/2 ) {
      G4int histoID = analysisManager->GetH2Id("nFlux");
      analysisManager->FillH2(histoID, stepX, stepY);
    }}
  }
}
