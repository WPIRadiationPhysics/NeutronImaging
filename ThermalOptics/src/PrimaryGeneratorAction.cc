#include "PrimaryGeneratorAction.hh"
#include "Messenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
#include "G4UImanager.hh"

#include "Randomize.hh"
//#include <math.h>

PrimaryGeneratorAction::PrimaryGeneratorAction()
 : G4VUserPrimaryGeneratorAction(),
   fParticleGun(0) {

  G4int nofParticles = 1;
  fParticleGun = new G4ParticleGun(nofParticles);

  // Default particle kinematic
  G4ParticleDefinition* particleDefinition 
    = G4ParticleTable::GetParticleTable()->FindParticle("neutron");
  fParticleGun->SetParticleDefinition(particleDefinition);
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0.0,0.0,1.0));
  fParticleGun->SetParticleEnergy(0.025*eV);
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

// This function is called at the begining of event
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {

  // Uniform beam of diameter 1mm; Azimuthal symmetry assumed
  G4double beamDiameter = 1*mm;
  G4double beamR = G4UniformRand()*(beamDiameter/2),
           beamPhi = G4UniformRand()*twopi;
  G4double beamX = beamR*std::cos(beamPhi),
           beamY = beamR*std::sin(beamPhi);

  // Acquire model height via Messenger
  Messenger* messenger = Messenger::GetMessenger();
  G4double worldZLength = messenger->GetModelHeight();

  // Apply position and fire
  fParticleGun->SetParticlePosition(G4ThreeVector(beamX, beamY, -worldZLength/2));
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
