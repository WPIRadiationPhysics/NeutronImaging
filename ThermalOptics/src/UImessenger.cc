#include "UImessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADouble.hh"

UImessenger::UImessenger(DetectorConstruction* Det)
 : G4UImessenger(),
   fDetectorConstruction(Det) {

  fModelDirectory = new G4UIdirectory("/model/");
  fModelDirectory->SetGuidance("Optical model settings");

  fLDCmd = new G4UIcmdWithADouble("/model/setLD", this);
  fLDCmd->SetGuidance("Length to Diameter ratio of collimator");
  fLDCmd->SetParameterName("choice", false);
  fLDCmd->AvailableForStates(G4State_PreInit, G4State_Idle);
}

UImessenger::~UImessenger() {
  delete fLDCmd;
  delete fModelDirectory;
}

void UImessenger::SetNewValue(G4UIcommand* command, G4String newValue) {

  if( command == fLDCmd ) {
    fDetectorConstruction->ModelLDConfiguration(std::atof(newValue));
  }
}
