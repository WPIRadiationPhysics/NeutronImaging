#ifndef UImessenger_h
#define UImessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADouble;

class UImessenger : public G4UImessenger {

  public:
    UImessenger(DetectorConstruction* );
    virtual ~UImessenger();
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:

    // Obtain Detector Construction instance
    DetectorConstruction*    fDetectorConstruction;

    // Create cmdDir and cmd objects
    G4UIdirectory* fModelDirectory;
    G4UIcmdWithADouble* fLDCmd;
};

#endif
