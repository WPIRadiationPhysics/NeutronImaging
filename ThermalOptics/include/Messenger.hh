#ifndef Messenger_h
#define Messenger_h 1

#include "globals.hh"
#include "Messenger.hh"

class Messenger {

  public:

    Messenger() {};
    virtual ~Messenger() {};

    // Acquire instance
    static Messenger* GetMessenger()
    { static Messenger the_messenger; return &the_messenger; }

    // Public vars
    G4double fModelHeight;
    G4String fDataDir;
    G4double nFluxTallyDim;

    // Public calls
    void SaveModelHeight(G4double modelHeight) { fModelHeight = modelHeight; }
    G4double GetModelHeight() { return fModelHeight; }
    void SaveDataDir(G4String datadir) { fDataDir = datadir; }
    G4String GetDataDir() { return fDataDir; }
    void SaveTallyDim(G4double nfluxtallydim) { nFluxTallyDim = nfluxtallydim; }
    G4double GetTallyDim() { return nFluxTallyDim; }

  //private:

};

#endif
