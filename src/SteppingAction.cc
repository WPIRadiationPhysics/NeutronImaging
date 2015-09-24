#include "SteppingAction.hh"
#include "G4SteppingManager.hh"
#include "G4Step.hh"

// Initialize Step Procedure
SteppingAction::SteppingAction( const DetectorConstruction* detectorConstruction,
                               EventAction* eventAction)
                               : G4UserSteppingAction(),
                                 fDetConstruction(detectorConstruction),
                                 fEventAction(eventAction) {}
SteppingAction::~SteppingAction() {}

// Step Procedure (for every step...)
void SteppingAction::UserSteppingAction(const G4Step* /*step*/) {}
