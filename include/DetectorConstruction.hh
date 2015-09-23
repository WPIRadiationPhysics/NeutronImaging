#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"

class G4VPhysicalVolume;
class G4GlobalMagFieldMessenger;
class UImessenger;

/// Detector construction class to define materials and geometry.
/// In addition a transverse uniform magnetic field is defined 
/// via G4GlobalMagFieldMessenger class.

class DetectorConstruction : public G4VUserDetectorConstruction {
  public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    
  private:
    // methods
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    
    // magnetic field messenger
    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger; 

    // UI cmd messenger
    UImessenger* fUImessenger;
    
    G4VPhysicalVolume* fDetector;
    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps
};

#endif
