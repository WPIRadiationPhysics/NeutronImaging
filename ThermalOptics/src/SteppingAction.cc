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

    // If neutron makes it to 1mmx1mm mesh, opposite source
    if ( stepParticle == "neutron" &&  stepZ >= worldZLength/2 ) {
    if ( abs(stepX) <= 1*mm && abs(stepY) <= 1*mm ) {

      // Append xy coordinates in cm to 2Dhistogram
      G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
      G4int histoID = analysisManager->GetH1Id("nFlux1D");
      analysisManager->FillH1(histoID, stepY);
      histoID = analysisManager->GetH2Id("nFlux2D");
      analysisManager->FillH2(histoID, stepX, stepY);
    }}
  }
}
