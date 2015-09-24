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

    // Public calls
    void SaveModelHeight(G4double modelHeight) { fModelHeight = modelHeight; }
    G4double GetModelHeight() { return fModelHeight; }

  //private:

};

#endif
