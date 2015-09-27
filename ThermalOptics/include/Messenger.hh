#ifndef Messenger_h
#define Messenger_h 1

#include "globals.hh"
#include "Messenger.hh"

#include "G4SystemOfUnits.hh"

class Messenger {

  public:

    Messenger() {};
    virtual ~Messenger() {};

    // Acquire instance
    static Messenger* GetMessenger()
    { static Messenger the_messenger; return &the_messenger; }

    // Public vars
    G4double fModelHeight;
    G4double fMeshDim;
    G4String fDataDir;
    G4double nFluxTallyDim;
    G4double fLDratio;

    // Public calls
    void SaveLDRatio(G4double ldratio) { fLDratio = ldratio; }
    G4double GetLDRatio() { return fLDratio; }
    void SaveModelHeight(G4double modelheight) { fModelHeight = modelheight; }
    G4double GetModelHeight() { return fModelHeight; }
    void SaveMeshDim(G4double meshdim) { fMeshDim = meshdim; }
    G4double GetMeshDim() { return fMeshDim; }
    void SaveDataDir(G4String datadir) { fDataDir = datadir; }
    G4String GetDataDir() { return fDataDir; }

  //private:

};

#endif
